#include <M5GFX.h>
#include <M5Unified.h>

#include "credentials.h"
#include "openai.h"

M5GFX display;

extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

void setup() {
  M5.begin();
  display.begin();
  display.setFont(&fonts::lgfxJapanGothic_16);

  display.print("Wifi connecting...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    display.print(".");
    delay(500);
  }
  // OpenAI API の DNS サーバーを指定
  IPAddress dns(104, 18, 6, 192);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, dns);

  display.fillScreen(TFT_BLACK);
  display.setCursor(0, 0);
  display.println("100文字程度の雑学をお話ししてください。");
  display.println("");

  display.print(completions("100文字程度の雑学をお話ししてください。"));
}

void loop() {}
