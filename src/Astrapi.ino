// Dual core ESP32 tutorial =>https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
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
#include <WiFiClient.h>
#include <Wire.h>

#include "FastLED.h"
#include "Streaming.h"
// #include "WiFi.h"

// Effects
#include "Bolt.h"
#include "ColourCycle.h"
#include "ColourFade.h"
#include "Crisscross.h"  // Has some odd flickering
#include "Fire.h"
#include "Meteor.h"
#include "Rain.h"
#include "Rainbow.h"
#include "Test.h"
#include "Tetris.h"

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
#define cloudLEDs 60
#define stripLEDs 135
#define totalLEDs stripLEDs + cloudLEDs
// #define totalLEDs 60 // LEDs in the cloud
// #define LED_BUILTIN 2 // ESP32, nothing required for ESP8266
#define connectionLED LED_BUILTIN

// #define dataPin 4 // ESP32
#define dataPin D6  // ESP8266

#define OFF LOW
#define ON HIGH

// #define NONE 0
// #define BOLT 1
// #define RAIN 2

#define cloudStart 136
#define cloudFinish 135 + 60

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
// WiFiClient espClient;
// PubSubClient mqtt(espClient);

// LED Strip
CRGB currentLED[totalLEDs];

Fire fire(totalLEDs, currentLED);
Test test(totalLEDs, currentLED, 50);
Meteor meteor(totalLEDs, currentLED, 50);
Crisscross crissCross(totalLEDs, currentLED, 50);
ColourCycle colourCycle(totalLEDs, currentLED, 50);
ColourFade colourFade(totalLEDs, currentLED, 50);
Rainbow rainbow(totalLEDs, currentLED, 50);
Tetris tetris(totalLEDs, currentLED, 100);
Bolt bolt(totalLEDs, stripLEDs, cloudLEDs, currentLED, 100);
Rain rain(totalLEDs, stripLEDs, cloudLEDs, currentLED, 100);

// TaskHandle_t Task1;
// TaskHandle_t Task2;

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
int LEDBrightness = 25;  // As a percentage (saved as a dynamic variable to let us change later)

const char* wifiSsid = "I Don't Mind";
const char* wifiPassword = "Have2Biscuits";

const char* nodeName = "Astrapi";

// const char* disconnectMsg = "Astrapi Disconnected";

const char* mqttServerIP = "mqtt.kavanet.io";

// Wifi Params
bool WiFiConnected = false;
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

int testRand = 25;

int mode = 6;

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

  // System architecture
  // xTaskCreatePinnedToCore(core1Loop, "Task1", 10000, NULL, 1, &Task1, 0);
  // delay(500);

  // xTaskCreatePinnedToCore(core2Loop, "Task2", 10000, NULL, 1, &Task2, 1);
  // delay(500);

  // disableCore0WDT();  // This prevents the WDT taking out an idle core
  // disableCore1WDT();  // the wifi code was triggering the WDT

  // LEDs
  FastLED.addLeds<NEOPIXEL, dataPin>(currentLED, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  // Wireless comms
  // startWifi();
  // startMQTT();

  tetris.begin();
  rain.begin();

  // On-board status led (Used for wifi and MQTT indication)
  pinMode(connectionLED, OUTPUT);

  Serial << "\n|** " << nodeName << " **|" << endl;
  delay(100);
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
// void core1Loop(void* pvParameters) {
//   for (;;) {
//     handleMQTT();
//     handleWiFi();
//     // delay(500); // * Add this back if WDT issues come back
//   }
// }

// void core2Loop(void* pvParameters) {
//   for (;;) {
//     if (WiFi.status() == WL_CONNECTED) {
//       switch (mode) {
//         case 0:
//           FastLED.clear();  // clear all pixel data
//           FastLED.show();
//           break;
//         case 1:
//           test.run(50);
//           break;
//         case 2:
//           colourCycle.run();
//           break;
//         case 3:
//           crissCross.run(50);
//           break;
//         case 4:
//           colourFade.run();
//           break;
//         case 5:
//           fire.run(55, 120, 20, true);
//           break;
//         case 6:
//           rainbow.run();
//           break;
//         case 7:
//           meteor.run();
//           break;
//       }
//     }
//   }
// }

void loop() {
  switch (mode) {
    case 0:  // Off
      for (int i = 0; i < totalLEDs; i++) {
        currentLED[i] = 0x000000;
      }
      FastLED.show();
      delay(5);
      break;
    case 1:  // Rain
      rain.run();
      testRand = random(0, 1000);
      // if (testRand < 2) {
      //   mode = 2;
      // }
      break;
    case 2:          // Bolt
      bolt.begin();  // Turns off all LEDs
      bolt.run();

      mode = 1;
      break;
    case 4:
      colourFade.run();
      break;
    case 3:
      fire.run(55, 120, 20, true);
      break;
    case 5:
      colourCycle.run();
      break;
    case 6:
      meteorRain(0xff, 0xff, 0x00, 10, 64, true, 30);
      break;
    case 7:
      tetris.run();
      break;
  }

  // rain.run();
  // rainbow.run();

  // for (int i = stripLEDs; i < totalLEDs; i++) {
  //   currentLED[i] = 0x0040ff;
  // }
  // // FastLED.show();
  // delay(2); // This delay prevents the cloud flickering (Dont have time to figure out why)

  // for (int i = 0; i < stripLEDs; i++) {
  //   currentLED[i] = 0x000000;
  // }
  // FastLED.show();

  // bolt.run();

  // uint32_t colours[8] = {// https://www.w3schools.com/colors/colors_picker.asp
  //                        0x00ffbf, 0x00ffff, 0x00bfff, 0x0080ff, 0x0040ff, 0x0000ff, 0x4000ff};

  // for (int i = cloudStart; i < cloudFinish; i++) {
  //   currentLED[i] = colours[random(0, sizeof(colours))];
  //   FastLED.show();
  //   delay(50);
  // }

  // if (random(0, 10) < 5) {  // Random lightning flash
  //   for (int i = cloudStart; i < cloudFinish; i++) {
  //     currentLED[i] = 0xffffff;
  //   }
  //   FastLED.show();
  //   delay(25);

  //   for (int i = cloudStart; i < cloudFinish; i++) {
  //     currentLED[i] = colours[random(0, sizeof(colours))];

  //     // currentLED[totalLEDs - i] = 0x00ff00;
  //   }
  //   FastLED.show();
  // }

  // if (mode == 1) {
  //   rain.run();
  //   testRand = random(0, 100);
  //   if (testRand < 10) {
  //     mode = 2;
  //   } else {
  //     mode = 1;
  //   }
  // } else if (mode == 2) {
  //   bolt.begin();  // Sets leds to off
  //   bolt.run();

  //   // When bolt has finished
  //   testRand = random(0, 100);
  //   if (testRand < 10) {
  //     mode = 2;
  //   } else {
  //     mode = 1;
  //   }
  // }
  // if (mode == 1) {
  //   bolt.run(50);
  //   mode = 2;  // Change mode once the bolt animation has finished
  // } else if (mode == 2) {
  //   fire.run(55, 120, 20, true);
  // }
  // tetris.run(50);
  // colourFade.run();
  // rainbow.run();
  // meteorRain(0xff, 0xff, 0x00, 10, 64, true, 30);
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  for (int i = 0; i < totalLEDs; i++) {
    currentLED[i] = 0x000000;
  }

  // for (int i = 0; i < totalLEDs + totalLEDs; i++) {
  for (int i = 0; i < totalLEDs + totalLEDs; i++) {
    // fade brightness all LEDs one step
    for (int j = 0; j < totalLEDs; j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((i - j < totalLEDs) && (i - j >= 0)) {
        currentLED[i - j].setRGB(red, green, blue);
        Serial << i - j << endl;
      }
    }

    FastLED.show();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
  currentLED[ledNo].fadeToBlackBy(fadeValue);
}