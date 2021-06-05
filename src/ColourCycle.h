#ifndef COLOYRCYCLE_H
#define COLOYRCYCLE_H
#include "FastLED.h"

class ColourCycle
{
  public:
    ColourCycle(int totalLEDs, CRGB *currentLED, int interval); // Constructor

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