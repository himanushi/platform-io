#include <M5Unified.h>

#include "setting-server.h"

M5GFX display;

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);

  server_init();
}

void loop() { server_handle(); }
