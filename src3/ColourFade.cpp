#include "ColourFade.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

ColourFade::ColourFade(int totalLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->currentLED = currentLED;
  this->interval = interval;
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
void ColourFade::run() {
  run(interval);
}

void ColourFade::run(int wait) {
  currentMillis = millis();

  if (currentMillis - lastMillis >= wait) {
    lastMillis = currentMillis;

    if (newRed - oldRed != 0 || newGreen - oldGreen != 0 || newBlue - oldBlue != 0) {
      oldRed = upDown(newRed, oldRed);
      oldGreen = upDown(newGreen, oldGreen);
      oldBlue = upDown(newBlue, oldBlue);
    }

    else if (newRed - oldRed == 0 && newGreen - oldGreen == 0 && newBlue - oldBlue == 0) {
      chooseNewColour();
    }

    for (int i = 0; i < totalLEDs; i++) {
      currentLED[i].setRGB(oldRed, oldGreen, oldBlue);
    }
    FastLED.show();
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
void ColourFade::changeSpeed(int interval) {
  run();

  speedChangeCurrentMillis = millis();

  if (speedChangeCurrentMillis - speedChangeLastMillis >= interval) {
    speedChangeLastMillis = speedChangeCurrentMillis;

    if (slowDown) {
      if (getInterval() >= 3)
        setInterval(getInterval() - 1);
      else
        slowDown = false;
    }

    else if (!slowDown) {
      if (getInterval() <= initialInterval)
        setInterval(getInterval() + 1);
      else
        slowDown = true;
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
void ColourFade::setInterval(int interval) {
  this->interval = interval;
}

int ColourFade::getInterval() {
  return interval;
}

void ColourFade::reset() {
  oldRed = 0;
  oldGreen = 0;
  oldBlue = 0;
}

void ColourFade::chooseNewColour()  // need to include the rndom seed here
{
  randomSeed(analogRead(0));

  uint32_t newColour = colours[random(0, 24)];  // Pick a random colour from the table

  newRed = (newColour & 0xff0000) >> 16;
  newGreen = (newColour & 0x00ff00) >> 8;
  newBlue = (newColour & 0x0000ff);
}

int ColourFade::upDown(int newVal, int oldVal) {
  if (newVal - oldVal > 0)
    return oldVal + 1;
  else if (newVal - oldVal < 0)
    return oldVal - 1;
  else if (newVal - oldVal == 0)
    return newVal;
  // return 0;  // Removes warning, check this *NB*
}