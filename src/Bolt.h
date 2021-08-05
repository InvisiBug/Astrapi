#ifndef BOLT_H
#define BOLT_H
#include "FastLED.h"

class Bolt {
 public:
  Bolt(int totalLEDs, int stripLEDs, int cloudLEDs, CRGB *currentLED, int interval);

  void run();
  void run(int wait);
  void begin();

  int getInterval();

 private:
  CRGB *currentLED;  // LED strip object

  uint32_t colours[8] = {  // https://www.w3schools.com/colors/colors_picker.asp
      0x00ffbf, 0x00ffff, 0x00bfff, 0x0080ff, 0x0040ff, 0x0000ff, 0x4000ff, 0x8000ff};

  int totalLEDs;
  int interval;

  int stripLEDs = 135;
};
#endif