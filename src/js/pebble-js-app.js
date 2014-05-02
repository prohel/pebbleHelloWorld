Pebble.addEventListener("appmessage",
 function(e) {
   if (e.payload.temp_msg == "switch" || e.payload.temp_msg == "pause") {
     //those 2 commands are to be sent to our server
     sendToServer1(e.payload.temp_msg);
   } else {
     //otherwise it is the tomorrow feature with request to send to the service provider server
     sendToServer2(e.payload.temp_msg);
   }
 }
);

var connected = false;

//Taking care of sending request and getting answer from our server
function sendToServer1(param) {

var req = new XMLHttpRequest();
var ipAddress = "158.130.62.240"; // Hard coded IP address
var port = "3001"; // Same port specified as argument to server
var url = "http://" + ipAddress + ":" + port + "/" + param;
var method = "GET";
var async = true;

req.onload = function(e) {
               //by default, message is error
  var msg = "error in connection with server";
  if (req.readyState==4 && req.status==200) {
              connected = true;
               var response = JSON.parse(req.responseText);
               if (response && response.minion) {
                 //if we get a temperature we update our variable accordingly
                 if (response.temperature) {
                     msg = "Latest: " + response.temperature + "\n";
                  }
                 if (response.high) {
                     msg += "H: " + response.high + "\n";
                  }
                 if (response.low) {
                     msg += "L: " + response.low + "\n";
                 }
                 if (response.avg) {
                     msg += "A: " + response.avg + "\n";
                  }
                 
               } else {
                 //if we get an error, we send it for display
                 if (response && response.error) {
                         msg = response.error;
                 } else {
                   msg += "";
                 }
               }
  
               // sends message back to pebble
              Pebble.sendAppMessage({ "0": msg, 
                                       "1": response.minion? response.minion : ""
                                      });
  } else {
    Pebble.sendAppMessage({ "0": msg});
  }
};

       req.open(method, url, async);
       req.send(null);
  if (!connected) {
    Pebble.sendAppMessage({ "0": "error in connection with server"});
  }
  connected = false;
}

//Request and answer from the weather forecast API
function sendToServer2(param) {

var req = new XMLHttpRequest();
var url = "http://api.worldweatheronline.com/free/v1/weather.ashx?q=19102&format=json&num_of_days=2&key=tmdg2ckm22e838m79gjsx4rg";
var method = "GET";
var async = true;

req.onload = function(e) {
               // see what came back
  var msg = "";
               var response = JSON.parse(req.responseText);
               if (response) {
                 if (response.data.weather[1]) {
                   msg += response.data.weather[1].date + "\n";
                   msg += "Min:" + response.data.weather[1].tempMinC + "\n";
                   msg += "Max:" + response.data.weather[1].tempMaxC + "\n";
                 } 
               } else msg += "error";
               // sends message back to pebble
  Pebble.sendAppMessage({ "0": msg});
};

       req.open(method, url, async);
       req.send(null);
}