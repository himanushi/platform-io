#ifndef OPENAI_H
#define OPENAI_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5Unified.h>

#include "credentials.h"

extern const char *API_KEY;
const char *openai_endpoint = "https://api.openai.com/v1";

extern M5GFX display;

String completions(const String &content) {
  HTTPClient http;
  http.begin(String(openai_endpoint) + "/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(API_KEY));
  http.setTimeout(60000);

  String postData =
      "{\"model\": \"gpt-4-turbo-preview\", \"messages\": [{\"role\": "
      "\"user\", \"content\": \"" +
      content + "\"}]}";

  int httpResponseCode = http.POST(postData);
  String text = "";

  String response = http.getString();
  Serial.println(response);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);

  if (httpResponseCode > 0) {
    if (doc["choices"].isNull()) {
      JsonObject error = doc["error"];
      Serial.println(error["code"].as<String>());
    } else {
      JsonArray choices = doc["choices"];
      for (JsonObject choice : choices) {
        text = choice["message"]["content"].as<String>();
      }
    }
  } else {
    Serial.println(httpResponseCode);
  }

  http.end();

  return text;
}

void streamCompletions(const String &content) {
  HTTPClient http;
  http.begin(String(openai_endpoint) + "/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(API_KEY));
  http.setTimeout(60000);

  String postData = "{\"stream\": true, \"model\": \"gpt-3.5-turbo\", "
                    "\"messages\": [{\"role\": \"user\", \"content\": \"" +
                    content + "\"}]}";

  int httpResponseCode = http.POST(postData);

  if (httpResponseCode > 0) {

    WiFiClient *stream = http.getStreamPtr();
    String buffer = "";
    bool dataPrefixDetected = false;
    String line;

    while (stream->available()) {
      delay(15);
      line = stream->readStringUntil('\n');
      if (line.startsWith("data:")) {
        String jsonStr = line.substring(5);
        jsonStr.trim();

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, jsonStr);
        if (!error) {
          JsonArray choices = doc["choices"];
          for (JsonObject choice : choices) {
            const char *content = choice["delta"]["content"].as<const char *>();
            Serial.println(content);
            display.print(content);
          }
        } else {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        }
      }
    }
  } else {
    Serial.print("HTTP Request failed: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

#endif
