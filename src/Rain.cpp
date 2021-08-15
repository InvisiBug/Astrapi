#include "Rain.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

Rain::Rain(int totalLEDs, int stripLEDs, int cloudLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->stripLEDs = stripLEDs;
  this->cloudLEDs = cloudLEDs;
  this->currentLED = currentLED;
  this->interval = interval;

  this->currentCloudLEDPos = 0;

  totalRaindrops = 20;

  rainDrop = new int[totalRaindrops];
  raindropColours = new uint32_t[totalRaindrops];
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
void Rain::run() {
  run(interval);
}

void Rain::begin() {
  for (int i = 0; i < totalRaindrops; i++) {  // Give each raindrop a random location
    rainDrop[i] = stripLEDs + random(0, stripLEDs);
    raindropColours[i] = colours[random(0, 8)];
  }
}

void Rain::run(int wait) {
  FastLED.show();
  delay(5);

  {  // Cloud
    cloudCurrentMillis = millis();
    if (cloudCurrentMillis - cloudLastMillis >= 50) {
      cloudLastMillis = cloudCurrentMillis;

      currentLED[currentCloudLEDPos] = colours[random(0, 8)];
      delay(2);

      if (currentCloudLEDPos < totalLEDs) {  // Basically a for loop but its non blocking
        currentCloudLEDPos++;
      } else {  // Random flashing effect every time the full cloud cycle has run
        currentCloudLEDPos = 0;
        if (random(0, 10) < 5) {  // Random lightning flash
          for (int i = stripLEDs; i < totalLEDs; i++) {
            currentLED[i] = 0xffffff;
          }
          FastLED.show();
          delay(25);

          for (int i = stripLEDs; i < totalLEDs; i++) {  // Reset cloud to random colours
            currentLED[i] = colours[random(0, 8)];
          }
          FastLED.show();
          delay(5);
        }
      }
    }

    // {  // Rain
    //   rainCurrentMillis = millis();
    //   if (rainCurrentMillis - rainLastMillis >= 20) {
    //     rainLastMillis = rainCurrentMillis;

    //     for (int i = 0; i < stripLEDs; i++) {  // Turn off everything
    //       currentLED[i] = 0x000000;
    //     }

    //     for (int i = 0; i < totalRaindrops; i++) {
    //       if (rainDrop[i] <= stripLEDs) {  // Only show the raindrops that are on the strip
    //         currentLED[rainDrop[i]] = raindropColours[i];
    //       }

    //       if (rainDrop[i] > 1) {
    //         rainDrop[i]--;
    //       } else {
    //         rainDrop[i] = stripLEDs + random(0, 50);
    //         raindropColours[i] = colours[random(0, 8)];
    //       }
    //     }

    //     if (currentRainLEDPos < stripLEDs) {  // Basically a for loop but its non blocking
    //       currentRainLEDPos++;
    //     } else {
    //       currentRainLEDPos = 0;
    //     }
    //   }
    // }
  }
}