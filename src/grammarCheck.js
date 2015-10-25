var SERVER_IP = 'alang.herokuapp.com';

console.log('loaded Javascript!');

function fetchCorrections(text) {
  var url = "http://" + SERVER_IP + "/api?sentence=" + encodeURIComponent(text);
  console.log("About to request:",  url);
  var req = new XMLHttpRequest();
  req.open('GET', url);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log("got a response!", req.responseText);
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
  console.log('got an app message!');
  fetchCorrections(text);
});

Pebble.addEventListener('ready', function() {
  console.log("Ready!");
});