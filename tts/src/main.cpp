#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <M5Unified.h>

AudioFileSourceSD *file;
AudioGeneratorMP3 *mp3;
AudioOutputI2S *out;

void setup() {
  Serial.begin(115200);
  if (!SD.begin(4)) {
    Serial.println("SDカードの初期化に失敗しました");
    return;
  }

  file = new AudioFileSourceSD("/tts.mp3");
  mp3 = new AudioGeneratorMP3();
  out = new AudioOutputI2S();

  out->begin();
  mp3->begin(file, out);
}

void loop() {
  if (mp3->isRunning()) {
    if (!mp3->loop())
      mp3->stop();
  } else {
    Serial.println("MP3の再生が終了しました");
    delay(1000);
  }
}
