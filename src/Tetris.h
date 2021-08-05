#ifndef Tetris_H
#define Tetris_H
#include "FastLED.h"

class Tetris {
 public:
  Tetris(int totalLEDs, CRGB *currentLED, int interval);

  void begin();
  void run();
  void run(int wait);
  void reset();
  void changeSpeed(int interval);  // Time to change to next speed
  void setInterval(int interval);

  int getInterval();

  uint32_t colours[24] = {  // https://www.w3schools.com/colors/colors_picker.asp
      0xff4000, 0xff8000, 0xffbf00, 0xffff00, 0xbfff00, 0x80ff00,
      0x40ff00, 0x00ff00, 0x00ff40, 0x00ff80, 0x00ffbf, 0x00ffff,
      0x00bfff, 0x0080ff, 0x0040ff, 0x0000ff, 0x4000ff, 0x8000ff,
      0xbf00ff, 0xff00ff, 0xff00bf, 0xff0080, 0xff0040, 0xff0000};

 private:
  CRGB *currentLED;  // LED strip object

  uint32_t *finalColours;

  bool full;

  int initialInterval;

  int currentLEDNum;

  bool slowDown;

  int totalLEDs;
  int totalSteps;
  int currentStep;

  // Timing Stuff
  long currentMillis;
  long lastMillis;
  int interval;

  long speedChangeCurrentMillis;
  long speedChangeLastMillis;
};
#endif