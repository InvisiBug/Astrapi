#include "Tetris.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

Tetris::Tetris(int totalLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->currentLED = currentLED;
  this->initialInterval = interval;

  Serial << totalLEDs << endl;

  // totalSteps = 55;
  this->totalSteps = totalLEDs;  // 0 is a step so there are actually only 54 but this is taken care of later

  slowDown = true;

  // for (int i = 0; i < totalLEDs; i++) {
  //   finalColours[i] = colours[random(0, 24)];
  // }

  finalColours = new uint32_t[totalLEDs];

  currentStep = totalSteps;

  full = false;
}

void Tetris::begin() {
  for (int i = 0; i < totalLEDs; i++) {
    finalColours[i] = colours[random(0, 24)];
  }
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
void Tetris::run() {
  run(interval);
}

void Tetris::run(int wait) {
  if (currentLEDNum == totalLEDs) {
    Serial << "Finished" << endl;
    // Need to make all the leds shift down by one here
  } else if (!currentLED[currentLEDNum]) {  // if the lowest LED isnt lit
    currentLED[currentStep] = finalColours[currentLEDNum];

    Serial << "Here\t" << currentLEDNum << endl;

  } else {
    currentStep = totalLEDs;
    currentLEDNum++;

    Serial << "There" << endl;
  }

  FastLED.show();

  currentMillis = millis();
  if (currentMillis - lastMillis >= wait) {
    lastMillis = currentMillis;

    currentLED[currentStep] = 0x000000;  // Turn off leds

    FastLED.show();

    if (currentStep > 0) {
      currentStep--;
    } else {
      currentStep = totalLEDs;
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
void Tetris::changeSpeed(int interval) {
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
void Tetris::setInterval(int interval) {
  this->interval = interval;
}

int Tetris::getInterval() {
  return interval;
}

void Tetris::reset() {
  currentStep = 0;
}