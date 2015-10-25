Pebble.addEventListener('appmessage', function(e) {
  var text = e.payload[0xabbababe];
});

Pebble.addEventListener('ready', function() {
  console.log("ready!");
});
