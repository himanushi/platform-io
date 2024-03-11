#include <M5Unified.h>
#include <SPIFFS.h>

M5GFX display;
LGFX_Sprite sprite(&display);

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Reset
  sprite.createSprite(200, 200);
  sprite.fillSprite(TFT_WHITE);

  sprite.drawPngFile(SPIFFS, "/image.png", 0, 0);

  sprite.pushSprite(0, 0);
}

void loop() {}
