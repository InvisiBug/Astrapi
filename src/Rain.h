#ifndef RAIN_H
#define RAIN_H
#include "FastLED.h"

class Rain {
 public:
  Rain(int totalLEDs, CRGB *currentLED, int interval);

  void run();
  void run(int wait);
  void begin();

 private:
  CRGB *currentLED;  // LED strip object

  // https://www.w3schools.com/colors/colors_picker.asp
  uint32_t colours[8] = {0x00ffbf, 0x00ffff, 0x00bfff, 0x0080ff, 0x0040ff, 0x0000ff, 0x4000ff, 0x8000ff};

  int totalLEDs;
  int stripLEDs;
  int cloudLEDs;

  int cloudDelay = 50;
  long cloudCurrentMillis;
  long cloudLastMillis;

  long rainCurrentMillis;
  long rainLastMillis;

  int currentLEDNum = 0;
  int currentCloudLEDPos;
  int currentRainLEDPos = 0;

  int interval;
  int totalRaindrops;
  int *rainDrop;
  uint32_t *raindropColours;
};
#endif