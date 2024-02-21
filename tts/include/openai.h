#ifndef OPENAI_H
#define OPENAI_H

#include <HTTPClient.h>
#include <SD.h>

#include "credentials.h"

extern const char *API_KEY;

const char *openai_endpoint = "https://api.openai.com/v1";

void textToSpeech(const String &content) {
  HTTPClient http;
  http.begin(String(openai_endpoint) + "/audio/speech");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(API_KEY));
  http.setTimeout(60000);

  String postData =
      "{\"model\": \"tts-1\", \"voice\": \"alloy\", \"input\": \"" + content +
      "\"}";
  int httpResponseCode = http.POST(postData);

  String response = http.getString();

  if (httpResponseCode > 0) {
    File file = SD.open("/tts.mp3", FILE_WRITE);
    if (file) {
      http.writeToStream(&file);
      file.close();
      Serial.println("Saved: /tts.mp3");
    } else {
      Serial.println("Failed to open file on SD card");
    }
  } else {
    Serial.println("TextToSpeech でエラー発生");
    Serial.println(httpResponseCode);
  }

  http.end();
}

#endif
