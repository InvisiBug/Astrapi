#include "ChangeBrightness.h"
#include "Streaming.h"
#include "Arduino.h"
using namespace std;

ChangeBrightness::ChangeBrightness(int totalLEDs, CRGB *currentLED, int interval)
{
  this -> totalLEDs  = totalLEDs;
  this -> currentLED = currentLED;
  this -> interval   = interval;

  up = true;
}

void ChangeBrightness::begin(int themeColour)
{
  this -> themeColour = themeColour;
  initialInterval     = interval;
  brightness          = map(FastLED.getBrightness(), 0, 255, 0, 55);
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
void ChangeBrightness::run()
{
  if(up) // Increase the brightness
  {
    currentMillis = millis();

    if(currentMillis - lastMillis >= interval)
    {
      lastMillis = currentMillis;

      if(brightness <= totalLEDs - 1) // look in to this -1
      {
        for(int i = 0; i < brightness; i ++)
        {
          currentLED[i] = themeColour;
        }        
        FastLED.show();
        // Serial << "Up :" << up << "\t Brightness :" << brightness << "\t Brightness Value : "  << map(brightness, 0, totalLEDs, 0, 255) << endl;

        FastLED.setBrightness(map(brightness, 0, totalLEDs, 0, 255)); 
        brightness ++;
      }
      else
      {
        up = false;
      }
    }

  }

  else // decrease the brightness
  {
    currentMillis = millis();
    
    if(currentMillis - lastMillis >= interval)
    {
      lastMillis = currentMillis;

      if(brightness > 0)  // No point turning off (2)
      {
        for(int i = 0; i < brightness; i ++)
        {
          currentLED[i] = themeColour;
        }

        for(int j = totalLEDs - 1; j > brightness; j --)
        {
          currentLED[j] = 0x000000;
        }        
        FastLED.show();

        // Serial << "Up :" << up << "\t Brightness :" << brightness << "\t Brightness Value : "  << map(brightness, 0, totalLEDs, 0, 255) << endl;

        FastLED.setBrightness(map(brightness, 0, totalLEDs, 0, 255));
        brightness --;
      }
      else 
      {
        up = true;
      }
    }
  }  
}

void ChangeBrightness::showBrightness()
{
  for(int k = 0; k <= brightness; k ++)
  {
    currentLED[k] = themeColour;
  }
  FastLED.show();
}

int ChangeBrightness::getBrightness()
{
  return brightness;
}

void ChangeBrightness::setInterval(int interval)
{
  this -> interval = interval;
}

void ChangeBrightness::setInitialBrightness(int brightness)
{
  this -> initialBrightness = brightness;
}

void ChangeBrightness::setBrightness()
{
  // brightness can go up to 56, this - 1 prevents that
  FastLED.setBrightness(map(brightness - 1, 0, totalLEDs, 0, 255)); 
}

void ChangeBrightness::setBrightness(int brightness)
{
  FastLED.setBrightness(brightness); 
}