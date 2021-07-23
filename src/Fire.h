#ifndef FIRE_H
#define FIRE_H
#include "FastLED.h"

class Fire
{
  public:
    Fire(int totalLEDs, CRGB *currentLED);

    void run(int Cooling, int Sparkling, int SpeedDelay, bool direction);
    void setPixelHeatColor(int Pixel, byte temperature, bool direction);

    void setPixel(bool direction, int Pixel, byte red, byte green, byte blue);
    int setColour(int r, int g, int b);

    
  private:
    CRGB *currentLED; // LED strip object
    int totalLEDs; 

    int buffer[55];
};
#endif