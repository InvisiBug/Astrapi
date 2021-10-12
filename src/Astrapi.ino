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

#include "FastLED.h"
#include "Streaming.h"
#include "WiFi.h"

// Effects
// #include "Bolt.h"
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
#define totalLEDs 60
// #define totalLEDs 60 // LEDs in the cloud

#define LED_BUILTIN 2  // ESP32, nothing required for ESP8266
#define connectionLED LED_BUILTIN

#define dataPin 4  // ESP32
// #define dataPin D6  // ESP8266

#define OFF LOW
#define ON HIGH

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
WiFiClient espClient;
PubSubClient mqtt(espClient);

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
// Bolt bolt(totalLEDs, currentLED, 100);
Rain rain(totalLEDs, currentLED, 100);

TaskHandle_t Task1;
TaskHandle_t Task2;

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
int LEDBrightness = 50;  // As a percentage (saved as a dynamic variable to let us change later)

const char* wifiSsid = "I Don't Mind";
const char* wifiPassword = "Have2Biscuits";

const char* nodeName = "Astrapi";

const char* disconnectMsg = "Astrapi Disconnected";

const char* mqttServerIP = "mqtt.kavanet.io";

// Wifi Params
bool WiFiConnected = false;
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

int testRand = 25;

int mode = 8;

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
  xTaskCreatePinnedToCore(core1Loop, "Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);

  xTaskCreatePinnedToCore(core2Loop, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500);

  disableCore0WDT();  // This prevents the WDT taking out an idle core
  disableCore1WDT();  // the wifi code was triggering the WDT

  // LEDs
  FastLED.addLeds<NEOPIXEL, dataPin>(currentLED, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(1);

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  // Wireless comms
  startWifi();
  startMQTT();

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
void core1Loop(void* pvParameters) {
  for (;;) {
    handleMQTT();
    handleWiFi();
    // delay(500); // * Add this back if WDT issues come back
  }
}

void core2Loop(void* pvParameters) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      switch (mode) {
        case 0:  // Off
          FastLED.clear();
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
        case 2:  // Bolt
          // bolt.begin();  // Turns off all LEDs
          // bolt.run();

          rain.begin();

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
          rainbow.run();
          break;

        case 8:
          EVERY_N_MILLISECONDS(20) {
            pacifica_loop();
            FastLED.show();
            delay(5);
          }
          break;
      }
    }
  }
}

void loop() {}

CRGBPalette16 pacifica_palette_1 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50};
CRGBPalette16 pacifica_palette_2 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
CRGBPalette16 pacifica_palette_3 =
    {0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
     0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF};

void pacifica_loop() {
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  // Clear out the LED array to a dim background blue-green
  fill_solid(currentLED, totalLEDs, CRGB(2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer(pacifica_palette_1, sCIStart1, beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130), 0 - beat16(301));
  pacifica_one_layer(pacifica_palette_2, sCIStart2, beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80), beat16(401));
  pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, beatsin8(9, 10, 38), 0 - beat16(503));
  pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, beatsin8(8, 10, 28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer(CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff) {
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for (uint16_t i = 0; i < totalLEDs; i++) {
    waveangle += 250;
    uint16_t s16 = sin16(waveangle) + 32768;
    uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16(ci) + 32768;
    uint8_t sindex8 = scale16(sindex16, 240);
    CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
    currentLED[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps() {
  uint8_t basethreshold = beatsin8(9, 55, 65);
  uint8_t wave = beat8(7);

  for (uint16_t i = 0; i < totalLEDs; i++) {
    uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = currentLED[i].getAverageLight();
    if (l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8(overage, overage);
      currentLED[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors() {
  for (uint16_t i = 0; i < totalLEDs; i++) {
    currentLED[i].blue = scale8(currentLED[i].blue, 145);
    currentLED[i].green = scale8(currentLED[i].green, 200);
    currentLED[i] |= CRGB(2, 5, 7);
  }
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