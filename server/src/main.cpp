#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <M5Unified.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <map>
#include <string>

M5GFX display;
AsyncWebServer server(80);

const char *settingsJson = R"json(
[
  {
    "category": "Wi-Fi",
    "settings": [
      { "value": "", "type": "text", "label": "SSID", "key": "wifi_ssid" },
      {
        "value": "",
        "type": "password",
        "label": "Password",
        "key": "wifi_password"
      }
    ]
  },
  {
    "category": "OpenAI API",
    "settings": [
      {
        "value": "",
        "type": "password",
        "label": "API Key",
        "key": "openai_api_key"
      },
      {
        "value": "gpt-3.5-turbo",
        "label": "Model",
        "type": "select",
        "options": [
          { "label": "GPT-3.5", "value": "gpt-3.5" },
          { "label": "GPT-3.5 Turbo", "value": "gpt-3.5-turbo" }
        ],
        "key": "openai_model"
      }
    ]
  }
]
)json";

std::map<std::string, std::string> settings = {{"ssid", ""}, {"password", ""}};

String getSettingsAsJson() {
  DynamicJsonDocument doc(1024);
  JsonObject obj = doc.to<JsonObject>();

  for (const auto &setting : settings) {
    obj[setting.first] = setting.second;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

void updateSetting(const std::string &key, const std::string &value) {
  settings[key] = value;
}

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
    request->send(SPIFFS, "/settings/index.html", "text/html");
  });
  server.on("/van.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings/van.min.js", "application/javascript");
  });
  server.on("/pico.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings/pico.min.css", "text/css");
  });
  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings/settings.json", "application/json");
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

          settings[key] = value;
        }

        request->send(200, "application/json", "{\"result\":\"success\"}");

        for (const auto &setting : settings) {
          Serial.print(setting.first.c_str());
          Serial.print(": ");
          Serial.println(setting.second.c_str());
        }
      });

  server.begin();
}

void loop() {}
