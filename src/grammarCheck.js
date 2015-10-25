var SERVER_IP = '10.121.251.137:8070';

function fetchCorrections(text) {
  var req = new XMLHttpRequest();
  req.open('GET', "http://" + SERVER_IP + "/api/?sentence=" + text, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
      } else {
        console.log("Error", req.status);
      }
    }
  }
  req.send();
}

Pebble.addEventListener('appmessage', function(e) {
  var text = e.payload[0xabbababe];
  fetchCorrections(text);
});

Pebble.addEventListener('ready', function() {
  console.log("ready!");
});