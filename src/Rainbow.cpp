#include "Rainbow.h"
#include "Streaming.h"
#include "Arduino.h"
using namespace std;

Rainbow::Rainbow(int totalLEDs, CRGB *currentLED, int interval)
{
  this -> totalLEDs  = totalLEDs;
  this -> currentLED = currentLED;
  this -> interval   = interval;

  totalSteps = 255;

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
void Rainbow::run()
{
  run(interval);
}

void Rainbow::run(int wait)
{
  currentMillis = millis();

  if(currentMillis - lastMillis >= wait)
  {
    lastMillis = currentMillis;

    if((currentStep < totalSteps))
    {
      for(int i = 0; i < totalLEDs; i ++)
      {
        currentLED[i].setHue(currentStep - i * (255 / totalLEDs));
      }

      FastLED.show();
      currentStep ++;
    }
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
void Rainbow::changeSpeed(int interval)
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
void Rainbow::setInterval(int interval)
{
  this -> interval = interval;
}

int Rainbow::getInterval()
{
  return interval;
}

void Rainbow::reset()
{
  currentStep = 0;
}