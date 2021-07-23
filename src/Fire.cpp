#include "Fire.h"
#include "Streaming.h"
#include "Arduino.h"
using namespace std;

Fire::Fire(int totalLEDs, CRGB *currentLED)
{
  this -> totalLEDs  = totalLEDs;
  this -> currentLED = currentLED;
}

//////////////////////////////////////////////////////////////////////////////
//
//  ######                
//  #     # #    # #    # 
//  #     # #    # ##   # 
//  ######  #    # # #  # 
//  #   #   #    # #  # # 
//  #    #  #    # #   ## 
//  #     #  ####  #    # 
//
//////////////////////////////////////////////////////////////////////////////
void Fire::run(int Cooling, int Sparkling, int SpeedDelay, bool direction)
{
  byte led[totalLEDs];
  int cooldown;
  
  for(int i = 0; i < totalLEDs; i++) // Step 1.  Cool down every cell a little
  {
    cooldown = random(0, ((Cooling * 10) / totalLEDs) + 2);
    
    if(cooldown > led[i]) led[i] = 0;
    else                  led[i] = led[i] - cooldown;
  }
  
  for(int k = totalLEDs - 1; k >= 2; k--)  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  {
    led[k] = (led[k - 1] + led[k - 2] + led[k - 2]) / 3;
  }
  
  if(random(255) < Sparkling) // Step 3.  Randomly ignite new 'sparks' near the bottom
  {
    int y = random(7);
    led[y] = led[y] + random(160,255);
  }

  for(int j = 0; j < totalLEDs; j++) // Step 4.  Convert led to LED colors
  {
    setPixelHeatColor(j, led[j], direction);
  }

  FastLED.show();
  delay(SpeedDelay);
}

void Fire::setPixelHeatColor(int pixel, byte temperature, bool direction) 
{
  byte heatLevel = round((temperature / 255.0) * 191); // Scale 'heat' down from 0-255 to 0-191
  // byte heatLevel = map(temperature, 0, 255, 0, 191); // Scale 'heat' down from 0-255 to 0-191
    
  // calculate ramp up from
  byte heatRamp = heatLevel & 0x3F; // 0..63
  heatRamp <<= 2; // scale up to 0..252
  
  // figure out which third of the spectrum we're in:
  if(heatLevel > 0x80)      setPixel(direction, pixel, 255, 255, heatRamp); // Hotest  (Yellow to white)
  else if(heatLevel > 0x40) setPixel(direction, pixel, 255, heatRamp, 0);   // Middle  (Red -> Orange -> Yellow)
  else                      setPixel(direction, pixel, heatRamp, 0, 0);     // Coldest (Black to Red)
}

void Fire::setPixel(bool direction, int pixel, byte red, byte green, byte blue) 
{
  if(!direction)
  {
    currentLED[totalLEDs - pixel].r = red;
    currentLED[totalLEDs - pixel].g = green;
    currentLED[totalLEDs - pixel].b = blue;
  }
  else
  {
    currentLED[pixel].r = red;
    currentLED[pixel].g = green;
    currentLED[pixel].b = blue;
  }
}
