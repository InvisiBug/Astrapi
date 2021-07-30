////////////////////////////////////////////////////////////////////////
//  ###
//   #  #    #  ####  #      #    # #####  ######  ####
//   #  ##   # #    # #      #    # #    # #      #
//   #  # #  # #      #      #    # #    # #####   ####
//   #  #  # # #      #      #    # #    # #           #
//   #  #   ## #    # #      #    # #    # #      #    #
//  ### #    #  ####  ######  ####  #####  ######  ####
//
////////////////////////////////////////////////////////////////////////

#include <Wire.h>

#include "ColourCycle.h"
#include "FastLED.h"
#include "Streaming.h"

// Effects
#include "ColourCycle.h"
#include "ColourFade.h"
#include "Crisscross.h"  // Has some odd flickering
#include "Fire.h"
#include "Meteor.h"
#include "Rainbow.h"
#include "Test.h"

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # ###### ###### # #    # # ##### #  ####  #    #  ####
//  #     # #      #      # ##   # #   #   # #    # ##   # #
//  #     # #####  #####  # # #  # #   #   # #    # # #  #  ####
//  #     # #      #      # #  # # #   #   # #    # #  # #      #
//  #     # #      #      # #   ## #   #   # #    # #   ## #    #
//  ######  ###### #      # #    # #   #   #  ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
#define totalLEDs 55
#define connectionLED LED_BUILTIN

#define dataPin D6

#define OFF HIGH
#define ON LOW

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #####  #    #   ##   #####  ######
//  #     #  #  #  #    # #    # #    #  #  #  #    # #
//  ####### #    # #    # #    # #    # #    # #    # #####
//  #     # ###### #####  #    # # ## # ###### #####  #
//  #     # #    # #   #  #    # ##  ## #    # #   #  #
//  #     # #    # #    # #####  #    # #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
// LED Strip
CRGB leds[totalLEDs];

// Effects
Fire fire(totalLEDs, leds);
Test test(totalLEDs, leds, 50);
Meteor meteor(totalLEDs, leds, 50);
Crisscross crissCross(totalLEDs, leds, 50);
ColourCycle colourCycle(totalLEDs, leds, 50);
ColourFade colourFade(totalLEDs, leds, 50);
Rainbow rainbow(totalLEDs, leds, 50);

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #   ##   #####  #      ######  ####
//  #     #  #  #  #    # #  #  #  #    # #      #      #
//  #     # #    # #    # # #    # #####  #      #####   ####
//   #   #  ###### #####  # ###### #    # #      #           #
//    # #   #    # #   #  # #    # #    # #      #      #    #
//     #    #    # #    # # #    # #####  ###### ######  ####
//
////////////////////////////////////////////////////////////////////////
int LEDBrightness = 100;  // As a percentage (saved as a dynamic variable to let us change later)
int mode = 0;

////////////////////////////////////////////////////////////////////////
//
//  ######                                                #####
//  #     # #####   ####   ####  #####    ##   #    #    #     # #####   ##   #####  ##### #    # #####
//  #     # #    # #    # #    # #    #  #  #  ##  ##    #         #    #  #  #    #   #   #    # #    #
//  ######  #    # #    # #      #    # #    # # ## #     #####    #   #    # #    #   #   #    # #    #
//  #       #####  #    # #  ### #####  ###### #    #          #   #   ###### #####    #   #    # #####
//  #       #   #  #    # #    # #   #  #    # #    #    #     #   #   #    # #   #    #   #    # #
//  #       #    #  ####   ####  #    # #    # #    #     #####    #   #    # #    #   #    ####  #
//
////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, dataPin>(leds, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  pinMode(connectionLED, OUTPUT);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  Wire.begin(1);                 // Join I2C Bus with address 1
  Wire.onReceive(receiveEvent);  // Data received event

  digitalWrite(connectionLED, OFF);
  Serial << "Astrapi LED Board Ready" << endl;
  delay(1000);
}

///////////////////////////////////////////////////////////////////////
//
//  #     #                    ######
//  ##   ##   ##   # #    #    #     # #####   ####   ####  #####    ##   #    #
//  # # # #  #  #  # ##   #    #     # #    # #    # #    # #    #  #  #  ##  ##
//  #  #  # #    # # # #  #    ######  #    # #    # #      #    # #    # # ## #
//  #     # ###### # #  # #    #       #####  #    # #  ### #####  ###### #    #
//  #     # #    # # #   ##    #       #   #  #    # #    # #   #  #    # #    #
//  #     # #    # # #    #    #       #    #  ####   ####  #    # #    # #    #
//
//////////////////////////////////////////////////////////////////////
void loop() {
  switch (mode) {
    case 0:
      FastLED.clear();  // clear all pixel data
      FastLED.show();
      break;
    case 1:
      test.run(50);
      break;
    case 2:
      colourCycle.run();
      break;
    case 3:
      crissCross.run(50);
      break;
    case 4:
      colourFade.run();
      break;
    case 5:

      fire.run(55, 120, 20, true);
      break;
    case 6:
      rainbow.run();
      break;
    case 7:
      meteor.run();
      break;
  }
}

void receiveEvent(int howMany)  // This is the function for the I2C communication, this runs every time the processor detects new incoming data
{
  Serial << Wire.read() << endl;
}