#ifndef CRISSCROSS_H
#define CRISSCROSS_H
#include "FastLED.h"

class Crisscross
{
  public:
    Crisscross(int totalLEDs, CRGB *currentLED, int interval);

    void run();
    void run(int wait);
    void reset();
    void changeSpeed(int interval); // Time to change to next speed
    void setInterval(int interval);

    int  getInterval();
    
  private:
    CRGB *currentLED; // LED strip object

    int initialInterval;

    bool slowDown;

    int totalLEDs; 
    int totalSteps;
    int currentStep;

    // Timing Stuff
    long currentMillis;
    long lastMillis;
    int  interval;

    long speedChangeCurrentMillis;
    long speedChangeLastMillis;
};
#endif