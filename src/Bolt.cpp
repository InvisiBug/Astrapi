#include "Bolt.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

Bolt::Bolt(int totalLEDs, int stripLEDs, int cloudLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->currentLED = currentLED;
  this->interval = interval;

  Serial << totalLEDs << endl;

  // for (int i = 0; i < totalLEDs; i++) {
  //   finalColours[i] = colours[random(0, 24)];
  // }
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
void Bolt::run() {
  run(interval);
}

void Bolt::begin() {
  for (int i = 0; i < totalLEDs; i++) {
    currentLED[i] = 0x000000;
  }
  FastLED.show();
}

void Bolt::run(int wait) {
  for (int i = 135; i < 195; i++) {
    currentLED[i] = colours[random(0, 8)];
  }
  FastLED.show();
  delay(5);

  // Feelers moving down to ground
  for (int i = stripLEDs; i > -1; i--) {  // "i > -1" as it needs to illuminate the 0'th LED
    currentLED[i] = 0xffffff;
    if (i < totalLEDs) {
      currentLED[i + 1] = 0x000000;
    }
    FastLED.show();
    delay(30);
  }

  // Bolt following
  for (int i = stripLEDs; i > -1; i--) {  // "i > -1" as it needs to illuminate the 0'th LED
    currentLED[i] = 0xffffff;
    FastLED.show();
    delay(1);
  }

  int flashingDelay = 100;
  // Flashing light a few times
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < stripLEDs; j++) {
      currentLED[j] = 0xffffff;
    }
    FastLED.show();
    delay(flashingDelay);

    for (int k = 0; k < stripLEDs; k++) {
      currentLED[k] = 0x000000;
    }
    FastLED.show();
    delay(flashingDelay);
  }

  delay(1000);
}