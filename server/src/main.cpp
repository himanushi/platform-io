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
      "/check-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        if (index == 0) {
          request->_tempObject = new String();
        }

        if (request->_tempObject) {
          ((String *)request->_tempObject)->concat((char *)data, len);
        }

        if (index + len == total) {
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, *(String *)request->_tempObject);
          String ssid = doc["wifi_ssid"];
          String password = doc["wifi_password"];

          WiFi.begin(ssid.c_str(), password.c_str());
          // while でやると Task watchdog got triggered. The following tasks did
          // not reset the watchdog in time が発生するため、一旦4秒待つ。
          delay(4000);

          if (WiFi.status() == WL_CONNECTED) {
            request->send(200, "application/json",
                          "{\"result\":\"Connected!!!\"}");
          } else {
            request->send(400, "application/json",
                          "{\"result\":\"Failed to connect.\"}");
          }

          delete (String *)request->_tempObject;
        }
      });
  server.on(
      "/settings", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        if (!index) {
          request->_tempFile = SPIFFS.open("/setting_values.json", "w");
        }
        if (request->_tempFile) {
          request->_tempFile.write(data, len);
        }
        if (index + len == total) {
          request->_tempFile.close();
        }
        request->send(200, "application/json", "{\"result\":\"Success!!\"}");
      });

  server.begin();
}

void loop() {}
