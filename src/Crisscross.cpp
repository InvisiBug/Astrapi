#include "Crisscross.h"
#include "Streaming.h"
#include "Arduino.h"
using namespace std;

Crisscross::Crisscross(int totalLEDs, CRGB *currentLED, int interval)
{
  this -> totalLEDs       = totalLEDs;
  this -> currentLED      = currentLED;
  this -> initialInterval = interval;

  totalSteps = 55; // 0 is a step so there are actually only 54 but this is taken care of later

  slowDown = true;
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
void Crisscross::run()
{
  run(interval);
}

void Crisscross::run(int wait)
{
  currentMillis = millis();

  // Multiply first to remove rounding in an int 
  currentLED[currentStep].setHue((255 * (currentStep + 1)) / totalLEDs); // turn on leds
  currentLED[totalLEDs - currentStep - 1].setHue(255 - ((255 * (currentStep + 1)) / 55));

  FastLED.show();

  if(currentMillis - lastMillis >= wait)
  {
    lastMillis = currentMillis;

    currentLED[currentStep] = 0x000000; // Turn off leds
    currentLED[totalLEDs - currentStep - 1] = 0x000000;
    FastLED.show();

    // this - 1 prevents the light going off the end of the stick
    if(currentStep < totalSteps - 1) currentStep ++;    
    else currentStep = 0;
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  #####                                         #####                              
// #     # #    #   ##   #    #  ####  ######    #     # #####  ###### ###### #####  
// #       #    #  #  #  ##   # #    # #         #       #    # #      #      #    # 
// #       ###### #    # # #  # #      #####      #####  #    # #####  #####  #    # 
// #       #    # ###### #  # # #  ### #               # #####  #      #      #    # 
// #     # #    # #    # #   ## #    # #         #     # #      #      #      #    # 
//  #####  #    # #    # #    #  ####  ######     #####  #      ###### ###### ##### 
//
//////////////////////////////////////////////////////////////////////////////
void Crisscross::changeSpeed(int interval)
{
  run();

  speedChangeCurrentMillis = millis();

  if(speedChangeCurrentMillis - speedChangeLastMillis >= interval)
  {
    speedChangeLastMillis = speedChangeCurrentMillis;

    if(slowDown)
    {
      if(getInterval() >= 3) setInterval(getInterval() - 1);
      else slowDown = false;
    }

    else if(!slowDown)
    {
      if(getInterval() <= initialInterval) setInterval(getInterval() + 1);
      else slowDown = true;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//
// ###                                                 
//  #  #    # ##### ###### #####  #    #   ##   #      
//  #  ##   #   #   #      #    # #    #  #  #  #      
//  #  # #  #   #   #####  #    # #    # #    # #      
//  #  #  # #   #   #      #####  #    # ###### #      
//  #  #   ##   #   #      #   #   #  #  #    # #      
// ### #    #   #   ###### #    #   ##   #    # ######
//
//////////////////////////////////////////////////////////////////////////////
void Crisscross::setInterval(int interval)
{
  this -> interval = interval;
}

int Crisscross::getInterval()
{
  return interval;
}

void Crisscross::reset()
{
  currentStep = 0;
}