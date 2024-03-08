#include <M5Unified.h>
#include <SPIFFS.h>

void setup() {
  M5.begin();
  SPIFFS.begin(true);
  Serial.begin(115200);
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    Serial.println("Recording started...");
    // 録音開始の処理をここに実装
  }

  if (M5.BtnA.wasReleased()) {
    Serial.println("Recording stopped. Saving to SPIFFS...");
    // 録音停止およびSPIFFSへの保存の処理をここに実装
  }
}
