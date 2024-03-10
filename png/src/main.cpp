#include <M5Unified.h>
#include <SPIFFS.h>

M5GFX display;
LGFX_Sprite sprite(&display);

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    M5.Lcd.println("SPIFFS Mount Failed");
    return;
  }

  // Reset
  sprite.setColorDepth(1);
  sprite.createSprite(200, 200);
  sprite.clear(TFT_WHITE);
  sprite.pushSprite(0, 0);

  // draw png
  sprite.drawPixel(x, y, color);
  sprite.pushSprite(0, 0);
}

void loop() {}
