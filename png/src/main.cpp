#include <M5Unified.h>
#include <PNGdec.h>
#include <SPIFFS.h>

M5GFX display;
LGFX_Sprite sprite(&display);

PNG png;
File pngFile;

int16_t w, h, xOffset, yOffset;

void *myOpen(const char *filename, int32_t *size) {
  Serial.printf("Opening file: %s\n", filename);
  pngFile = SPIFFS.open(filename, "r");
  return &pngFile;
}

void myClose(void *handle) {
  Serial.println("Closing file");
  if (pngFile)
    pngFile.close();
}

int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  Serial.printf("Reading %d bytes\n", length);
  if (!pngFile)
    return 0;
  return pngFile.read(buffer, length);
}

int32_t mySeek(PNGFILE *handle, int32_t position) {
  Serial.printf("Seeking to %d\n", position);
  if (!pngFile)
    return 0;
  return pngFile.seek(position);
}

void PNGDraw(PNGDRAW *pDraw) {
  Serial.printf("Draw pos = 0,%d. size = %d x 1\n", pDraw->y, pDraw->iWidth);
  uint16_t usPixels[320];
  uint8_t usMask[320];

  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0x00000000);
  png.getAlphaMask(pDraw, usMask, 1);
  sprite.drawPixel(xOffset, yOffset + pDraw->y, TFT_BLACK);
  // gfx->draw16bitRGBBitmapWithMask(xOffset, yOffset + pDraw->y, usPixels,
  // usMask,
  //                                 pDraw->iWidth, 1);
}

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

  w = 200;
  h = 200;

  int rc = png.open("/image.png", myOpen, myClose, myRead, mySeek, PNGDraw);
  uint32_t dt = millis();
  if (rc == PNG_SUCCESS) {
    int16_t pw = png.getWidth();
    int16_t ph = png.getHeight();

    xOffset = (w - pw) / 2;
    yOffset = (h - ph) / 2;

    rc = png.decode(NULL, 0);

    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n",
                  png.getWidth(), png.getHeight(), png.getBpp(),
                  png.getPixelType());
    png.close();
    Serial.print(millis() - dt);
    Serial.println("ms");
  } else {
    Serial.println("png.open() failed!");
  }

  // スプライトをディスプレイにプッシュ
  sprite.pushSprite(0, 0);
}

void loop() {}
