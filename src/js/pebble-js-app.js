Pebble.addEventListener("appmessage",
 function(e) {
   sendToServer(e.payload.hello_msg);
    
 }
);


function sendToServer(param) {

var req = new XMLHttpRequest();
var ipAddress = "158.130.62.244"; // Hard coded IP address
var port = "3001"; // Same port specified as argument to server
var url = "http://" + ipAddress + ":" + port + "/" + param;
var method = "GET";
var async = true;

req.onload = function(e) {
               // see what came back
  var msg = "Temp stats:\n\n";
               var response = JSON.parse(req.responseText);
               if (response) {
                 if (response.temperature) {
                       msg += response.temperature + "\n";
                  }
                 if (response.high) {
                     msg += response.high + "\n";
                  }
                 if (response.low) {
                       msg += response.low + "\n";
                 }
                 if (response.avg) {
                       msg += response.avg + "\n";
                  }
                 
               } else msg += "error";
               // sends message back to pebble
               Pebble.sendAppMessage({ "0": msg });
}

       req.open(method, url, async);
       req.send(null);
}