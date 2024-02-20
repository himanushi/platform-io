#ifndef OPENAI_H
#define OPENAI_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5Unified.h>

#include "credentials.h"

extern const char *API_KEY;
const char *openai_endpoint = "https://api.openai.com/v1";

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

#endif
