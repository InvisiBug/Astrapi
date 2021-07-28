#include "Meteor.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

Meteor::Meteor(int totalLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->currentLED = currentLED;
  this->interval = interval;

  currentStep = 0;
  totalSteps = 255;
  clear = false;
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
void Meteor::run() {
  run(interval);
}

void Meteor::run(int wait) {
  currentMillis = millis();

  if (currentMillis - lastMillis >= wait) {
    lastMillis = currentMillis;

    // for (int j = 0; j < totalLEDs * 2; j++) {
    // int Position = 0;

    int num1 = 127;  // 127
    int num2 = 128;  // 128
    int num3 = 255;  // 255

    if (a < totalLEDs * 2) {
      Position++;

      for (int b = 0; b < totalLEDs; b++) {
        currentLED[b].setRGB(((sin(b + Position) * num1 + num2) / num3) * 0xff, ((sin(b + Position) * num1 + num2) / num3) * 0xff, ((sin(b + Position) * num1 + num2) / num3) * 0xff);
      }

      FastLED.show();
      a++;
    } else {
      a = 0;
    }

    // for (int j = 0; j < totalLEDs * 2; j++) {
    //   Position++;  // = 0; //Position + Rate;
    //   for (int i = 0; i < totalLEDs; i++) {
    //     // sine wave, 3 offset waves make a rainbow!
    //     //float level = sin(i+Position) * 127 + 128;
    //     //setPixel(i,level,0,0);
    //     //float level = sin(i+Position) * 127 + 128;
    //     // currentLED[i].setRGB(sin(i + Position) * 127 + 128) / 255) * 1, ((sin(i + Position) * 127 + 128) / 255) * 1, ((sin(i + Position) * 127 + 128) / 255) * 1);
    //     currentLED[i].setRGB(((sin(i + Position) * 127 + 128) / 255) * 0xff, ((sin(i + Position) * 127 + 128) / 255) * 0xff, ((sin(i + Position) * 127 + 128) / 255) * 0xff);
    //   }

    //   FastLED.show();
    // }
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
void Meteor::changeSpeed(int interval) {
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
void Meteor::setInterval(int interval) {
  this->interval = interval;
}

int Meteor::getInterval() {
  return interval;
}

void Meteor::reset() {
  currentStep = 0;
}