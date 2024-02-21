#include <M5Unified.h>
#include <WebServer.h>
#include <WiFi.h>

const char *ssid = "M5StackAP";
const char *password = "123456789";

WebServer server(80);

void setup() {
  M5.begin();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, []() {
    server.send(200, "application/json", "{\"message\": \"Hello World\"}");
  });

  server.begin();
}

void loop() { server.handleClient(); }
