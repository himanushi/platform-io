#include <ArduinoOTA.h>
#include <M5Unified.h>
#include <WiFi.h>

void setup() {
  M5.begin();
  M5.Lcd.println("Hello, M5Stack!");
  // WiFi接続
  WiFi.begin("fold", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  // OTA設定
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // 更新開始の処理
    M5.Lcd.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    // 更新終了の処理
    M5.Lcd.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // 更新進行中の処理
    M5.Lcd.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    M5.Lcd.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      M5.Lcd.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      M5.Lcd.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      M5.Lcd.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      M5.Lcd.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      M5.Lcd.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  M5.Lcd.println("OTA Ready");
  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.localIP());
}

void loop() { ArduinoOTA.handle(); }
