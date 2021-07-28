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
// Frameworks
#include <PubSubClient.h>

#include "ColourCycle.h"
#include "FastLED.h"
#include "Streaming.h"

// Wifi Manager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

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

// Button

// Effects
Fire fire(totalLEDs, leds);
Test test(totalLEDs, leds, 50);
Meteor meteor(totalLEDs, leds, 50);
Crisscross crissCross(totalLEDs, leds, 50);
ColourCycle colourCycle(totalLEDs, leds, 50);
ColourFade colourFade(totalLEDs, leds, 50);
Rainbow rainbow(totalLEDs, leds, 50);

WiFiClient espClient;
PubSubClient mqtt(espClient);

WiFiManager wifiManager;

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

const char* disconnectMsg = "Buttons Disconnected";

// const char* mqttServerIP = "192.168.1.46";
const char* mqttServerIP = "mqtt.kavanet.io";

const char* nodeName = "Astrapi";

int mode = 7;

long connectionTimeout = (2 * 1000);
// long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

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

  wifiManager.autoConnect("Astrapi");
  startMQTT();

  FastLED.addLeds<NEOPIXEL, dataPin>(leds, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  pinMode(connectionLED, OUTPUT);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  Serial << "\n|** " << nodeName << " **|" << endl;
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
  handleMQTT();

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

int pos = 0;
bool clear = false;
void upDown() {
  // FastLED.clear();  // clear all pixel data
  // FastLED.show();
  // for(int i = 0 )

  if (!clear) {
    leds[pos] = 0xff0000;
    if (pos < totalLEDs) {
      pos += 1;
    } else {
      clear = true;
      pos = 0;
    }
  } else {
    leds[pos] = 0x000000;
    if (pos < totalLEDs) {
      pos += 1;
    } else {
      clear = false;
      pos = 0;
    }
  }

  FastLED.show();
}
