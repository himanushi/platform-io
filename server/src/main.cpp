#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <M5Unified.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <map>
#include <string>

M5GFX display;
AsyncWebServer server(80);

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);
  WiFi.softAP("M5StackAP", "123456789");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  M5.Lcd.println(IP);

  if (!SPIFFS.begin()) {
    M5.Lcd.println("SPIFFS Mount Failed");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/van.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/van.min.js", "application/javascript");
  });
  server.on("/pico.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/pico.min.css", "text/css");
  });
  server.on(
      "/setting_options.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/setting_options.json", "application/json");
      });
  server.on("/setting_values.json", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              request->send(SPIFFS, "/setting_values.json", "application/json");
            });

  server.on(
      "/settings", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, (const char *)data);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          request->send(400, "application/json", "{\"result\":\"error\"}");
          return;
        }

        JsonObject obj = doc.as<JsonObject>();
        for (JsonPair kv : obj) {
          const char *key = kv.key().c_str();
          const char *value = kv.value().as<const char *>();
        }

        request->send(200, "application/json", "{\"result\":\"success\"}");
      });

  server.begin();
}

void loop() {}
