/*
This effect displays the colour wheel along the length of the LEDs and moves the starting 
position according to a time dalay
*/

#ifndef RAINBOW_H
#define RAINBOW_H
#include "FastLED.h"

class Rainbow
{
  public:
    Rainbow(int totalLEDs, CRGB *currentLED, int interval); // Constructor

    void run(); // Run using the intiial interval as the delay
    void run(int wait); // Run using an argument as the delay
    void reset();
    void changeSpeed(int interval); // Not used but left in for later
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