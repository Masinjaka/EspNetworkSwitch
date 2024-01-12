#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncEventSource.h>


AsyncWebServer server(1223);

const char* apSSID = "SmartTek Module";
const char* apPASS = "00001111";

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <body>
  <button onclick="sendRequest()">send</button>
  <script>
    function sendRequest() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/credentials?ssid=Awkward&password=uUuUuUuU", true);
      xhttp.send();
    }
  </script>
  </body>
  </html>
)rawliteral";

String ssid;
String pass;
bool isConnected;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  WiFi.softAP(apSSID,apPASS);
  Serial.println(WiFi.softAPIP());

  //
  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  //
  server.on("/credentials",HTTP_GET,[](AsyncWebServerRequest *request){
    String requette;
    if(request->hasParam("ssid") && request->hasParam("password")){
      ssid= request->getParam("ssid")->value();
      pass = requette + request->getParam("password")->value();
      Serial.println(requette);
    }
  });

  server.begin();
}

void loop() {
  //
  if(ssid!=null && pass != null){
    //Stop Hotspot
    //Connect to the Wifi
  }
  if(Serial.available()){
    // if(isConnected){MQTT}
  }

}
