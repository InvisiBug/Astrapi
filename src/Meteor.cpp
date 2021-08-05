#include "Meteor.h"

#include "Arduino.h"
#include "Streaming.h"
using namespace std;

Meteor::Meteor(int totalLEDs, CRGB *currentLED, int interval) {
  this->totalLEDs = totalLEDs;
  this->currentLED = currentLED;
  this->interval = interval;

  currentStep = 0;
  totalSteps = totalLEDs;
  clear = false;

  currentLocation = totalLEDs - 1;
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
  wait = 50;
  currentMillis = millis();

  int red = 101;
  int green = 191;
  int blue = 214;

  // currentLED[currentLocation].setRGB(255, 255, 0);

  FastLED.show();

  if (currentMillis - lastMillis >= wait) {
    lastMillis = currentMillis;

    // * Loop from here every 500 mil

    currentLED[currentLocation].setRGB(red, green, blue);
    if (currentLocation < totalLEDs - 1) {
      currentLED[currentLocation].setRGB(red / 2, green / 2, blue / 2);
    }
    if (currentLocation < totalLEDs - 2) {
      currentLED[currentLocation + 2].setRGB(0, 0, 0);
    }

    FastLED.show();

    Serial << currentLocation << endl;

    if (currentLocation > 0) {
      currentLocation--;
    } else {
      currentLocation = totalLEDs - 1;
    }

    // if (currentStep < totalSteps - 1) {
    //   currentStep++;
    // } else {
    //   currentStep = 0;
  }

  // int num1 = 127;  // 127
  // int num2 = 128;  // 128
  // int num3 = 255;  // 255

  // if (a < totalLEDs * 2) {
  //   Position++;

  //   for (int b = 0; b < totalLEDs; b++) {
  //     currentLED[b].setRGB(((sin(b + Position) * num1 + num2) / num3) * 0xff, ((sin(b + Position) * num1 + num2) / num3) * 0xff, ((sin(b + Position) * num1 + num2) / num3) * 0xff);
  //   }

  //   FastLED.show();
  //   a++;
  // } else {
  //   a = 0;
  // }
}

// void Meteor::fadeToBlack(int ledNo, byte fadeValue) {
//  #ifdef ADAFRUIT_NEOPIXEL_H
//     // NeoPixel
//     uint32_t oldColor;
//     uint8_t r, g, b;
//     int value;

//     oldColor = strip.getPixelColor(ledNo);
//     r = (oldColor & 0x00ff0000UL) >> 16;
//     g = (oldColor & 0x0000ff00UL) >> 8;
//     b = (oldColor & 0x000000ffUL);

//     r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
//     g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
//     b=(b<=10)? 0 : (int) b-(b*fadeValue/256);

//     strip.setPixelColor(ledNo, r,g,b);
//  #endif
// //  #ifndef ADAFRUIT_NEOPIXEL_H
// //    // FastLED
// //    leds[ledNo].fadeToBlackBy( fadeValue );
// //  #endif
// }