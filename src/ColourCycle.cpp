#include "ColourCycle.h"
#include "Streaming.h"
#include "Arduino.h"
using namespace std;

ColourCycle::ColourCycle(int totalLEDs, CRGB *currentLED, int interval)
{
  this -> totalLEDs  = totalLEDs;
  this -> currentLED = currentLED;
  this -> interval   = interval;

  currentStep = 0;
  totalSteps = 255;
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
void ColourCycle::run()
{
  run(interval);
}

void ColourCycle::run(int wait)
{
  currentMillis = millis();

  if(currentMillis - lastMillis >= wait)
  {
    lastMillis = currentMillis;

    if((currentStep < totalSteps))
    {
      for(int i = 0; i < totalLEDs; i ++)
      {
        currentLED[i].setHue(currentStep); 
      }

      FastLED.show();
      currentStep ++;
    }
    else
    {
      currentStep = 0;
    }
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
void ColourCycle::changeSpeed(int interval)
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
void ColourCycle::setInterval(int interval)
{
  this -> interval = interval;
}

int ColourCycle::getInterval()
{
  return interval;
}

void ColourCycle::reset()
{
  currentStep = 0;
}