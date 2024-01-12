#include <Arduino.h> 
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncEventSource.h>

#define LED_PIN 2

// Count down for testing the software serial

const char *ssid = "Module";
const char *pass = "un2trois4";
const char *mDNSName = "smartOra-reader";

unsigned long timerDelay = 1000;
unsigned long lastTime = 0;

uint32_t count = 0;

// Creating server
AsyncWebServer server(1234);
AsyncEventSource events("/events");


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>BME280 WEB SERVER (SSE)</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('counter', function(e) {
  console.log("counter", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
}
</script>
</body>
</html>)rawliteral";

void setup()
{

  Serial.begin(4800);
  while (!Serial){}

  // Connecting to Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  // Confirm if the Wifi is connected
  Serial.println(WiFi.localIP());

  // Giving name to the localIP (apparently that's called DNS)
  if (MDNS.begin(mDNSName)){
    Serial.println("mDNS Started, Access at: http://" + String(mDNSName) + ".local");
  }
  
  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200,"text/html",index_html);
  });

  /*server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200,"text/plain","Hello");
  });*/
  // When the client is connected to the event
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!",NULL,millis(),1000);
  });

  server.addHandler(&events);
  server.begin();

}

void loop()
{
  MDNS.update();
  if(Serial.available()){
    String data = Serial.readStringUntil('\r');
    //const char * uid = (Serial.readStringUntil('\n')).c_str();
    events.send(data.c_str(),"counter",millis());
  }
}