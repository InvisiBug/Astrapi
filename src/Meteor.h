#ifndef METEOR_H
#define METEOR_H
#include "FastLED.h"

class Meteor {
 public:
  Meteor(int totalLEDs, CRGB *currentLED, int interval);  // Constructor

  void run();
  void run(int wait);
  void reset();
  void changeSpeed(int interval);  // Time to change to next speed
  void setInterval(int interval);

  int getInterval();

 private:
  CRGB *currentLED;  // LED strip object

  int initialInterval;

  int Position;

  bool slowDown;

  int totalLEDs;
  int totalSteps;
  int currentStep;

  bool clear;
  int pos = 0;

  // Timing Stuff
  long currentMillis;
  long lastMillis;
  int interval;

  long speedChangeCurrentMillis;
  long speedChangeLastMillis;

  int a = 0;
  int b = 0;
};
#endif