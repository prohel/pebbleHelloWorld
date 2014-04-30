#include <pebble.h>
#include <stdio.h>
static char msg[100];
static bool celsius = true;
static Window *window;
static TextLayer *hello_layer;
GBitmap *future_bitmap;
static char minion[10];
BitmapLayer *future_layer;
int pause = 0;
//static int temp;
/* This is called when the select button is clicked */
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (celsius == 0) {
    text_layer_set_text(hello_layer, "switching to Celsius");
  } else {
    text_layer_set_text(hello_layer, "switching to Fahrenheit");
  }
  celsius = 1 - celsius;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  int key = 0;
  Tuplet value = TupletCString(key, "switch");
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

void select_click_down_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  int key = 0;
  Tuplet value = TupletCString(key, "tmrw");
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

void pause_handler(ClickRecognizerRef recognizer, void *context) {
  if (pause) {
    text_layer_set_text(hello_layer, "Leaving standby mode");
  } else {
    text_layer_set_text(hello_layer, "Switching to standby mode");
  }
  pause = 1 - pause;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  int key = 0;
  Tuplet value = TupletCString(key, "pause");
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

void init_minion_handler(ClickRecognizerRef recognizer, void *context) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  if (strcmp(minion, "hot") == 0) {
   future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_HOT_MINION);
  } else {
    future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MINION);
  }
  future_layer = bitmap_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, bounds.size.h } });
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(future_layer));
}

void destroy_minion_handler(ClickRecognizerRef recognizer, void *context) {
  //Destroy GBitmaps
  gbitmap_destroy(future_bitmap);
 
  //Destroy BitmapLayers
  bitmap_layer_destroy(future_layer);
}

/* this registers the appropriate function to the appropriate button */
void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 0, init_minion_handler, destroy_minion_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 3000, pause_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, select_click_down_handler);
}


void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  //text_layer_set_text(hello_layer, "Out sent!");
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  text_layer_set_text(hello_layer, "Error out!");
}


void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  //text_layer_set_text(hello_layer, "Message received!");
  
  
  // looks for key #0 in the incoming message
  int key = 0;
  int key2 = 1;
  Tuple *text_tuple = dict_find(received, key);
  Tuple *text_tuple2 = dict_find(received, key2);
  if (text_tuple) {
    if (text_tuple->value) {
      // put it in this global variable
      strcpy(msg, text_tuple->value->cstring);
    }
    else strcpy(msg, "no value!");
    
    text_layer_set_text(hello_layer, msg);
  }
  else {
    text_layer_set_text(hello_layer, "no message!");
  }
  if (text_tuple2) {
    if (text_tuple2->value) {
       //put it in this global variable
       strcpy(minion, text_tuple2->value->cstring);
    }
 }


}

void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  text_layer_set_text(hello_layer, "Error in!");
}

static void window_load(Window *window) {
 Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
 hello_layer = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, bounds.size.h } });
 DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  int key = 0;
  Tuplet value = TupletCString(key, "hello?");
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
 text_layer_set_text_alignment(hello_layer, GTextAlignmentCenter);
 layer_add_child(window_layer, text_layer_get_layer(hello_layer));
}

static void window_unload(Window *window) {
 text_layer_destroy(hello_layer);
}

static void init(void) {
 window = window_create();
 window_set_window_handlers(window, (WindowHandlers) {
   .load = window_load,
   .unload = window_unload,
 });
 
 // need this for adding the listener
 window_set_click_config_provider(window, config_provider);
 // for registering AppMessage handlers
 app_message_register_inbox_received(in_received_handler);
 app_message_register_inbox_dropped(in_dropped_handler);
 app_message_register_outbox_sent(out_sent_handler);
 app_message_register_outbox_failed(out_failed_handler);
 const uint32_t inbound_size = 64;
 const uint32_t outbound_size = 64;
 app_message_open(inbound_size, outbound_size);
 
 const bool animated = true;
 window_stack_push(window, animated);
}

static void deinit(void) {
 window_destroy(window);
  //Destroy GBitmaps
gbitmap_destroy(future_bitmap);
 
//Destroy BitmapLayers
bitmap_layer_destroy(future_layer);
}

int main(void) {
 init();
 app_event_loop();
 deinit();
}