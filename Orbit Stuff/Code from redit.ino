#include "FastLED.h"

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NUM_LEDS 280
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define DATA_PIN 53

//#define CLK_PIN 4
#define VOLTS 5
#define MAX_MA 50000

#define FRAMES_PER_SECOND 15
#define LEAD_COUNT 10
#define TRAIL_LENGTH 200
#define TRAIL_DIM_BY 60
#define MIN_DELAY 1500
#define MAX_DELAY 3000
#define DRIP_HUE1 233
#define DRIP_HUE_SKEW 0
#define DRIP_SAT 127
#define MIRROR true
#define MIRROR_INV_COLOR true
#define STOP_SEGMENT 280
#define MAX_BRIGHTNESS 255

static float pulseSpeed = 0.4;  // Larger value gives faster pulse.

uint8_t hueA = 30;      // Start hue at valueMin.
uint8_t satA = 100;     // Start saturation at valueMin.
float valueMin = 60.0;  // Pulse minimum value (Should be less then valueMax).

uint8_t hueB = 35;       // End hue at valueMax.
uint8_t satB = 200;      // End saturation at valueMax.
float valueMax = 200.0;  // Pulse maximum value (Should be larger then valueMin).

uint8_t hue = hueA;                                       // Do Not Edit
uint8_t sat = satA;                                       // Do Not Edit
float val = valueMin;                                     // Do Not Edit
uint8_t hueDelta = hueA - hueB;                           // Do Not Edit
static float delta = (valueMax - valueMin) / 2.35040238;  // Do Not Edit
uint8_t hueChase = 0;

byte led_pos = 0;
byte inv_led_pos = NUM_LEDS - 1;

// TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
// Colors are chosen from a palette; a few palettes are provided.
//
// This December 2015 implementation improves on the December 2014 version
// in several ways:
// - smoother fading, compatible with any colors and any palettes
// - easier control of twinkle speed and twinkle density
// - supports an optional 'background color'
// - takes even less RAM: zero RAM overhead per pixel
// - illustrates a couple of interesting techniques (uh oh...)
//
// The idea behind this (new) implementation is that there's one
// basic, repeating pattern that each pixel follows like a waveform:
// The brightness rises from 0..255 and then falls back down to 0.
// The brightness at any given point in time can be determined as
// as a function of time, for example:
// brightness = sine( time ); // a sine wave of brightness over time
//
// So the way this implementation works is that every pixel follows
// the exact same wave function over time. In this particular case,
// I chose a sawtooth triangle wave (triwave8) rather than a sine wave,
// but the idea is the same: brightness = triwave8( time ).
//
// Of course, if all the pixels used the exact same wave form, and
// if they all used the exact same 'clock' for their 'time base', all
// the pixels would brighten and dim at once -- which does not look
// like twinkling at all.
//
// So to achieve random-looking twinkling, each pixel is given a
// slightly different 'clock' signal. Some of the clocks run faster,
// some run slower, and each 'clock' also has a random offset from zero.
// The net result is that the 'clocks' for all the pixels are always out
// of sync from each other, producing a nice random distribution
// of twinkles.
//
// The 'clock speed adjustment' and 'time offset' for each pixel
// are generated randomly. One (normal) approach to implementing that
// would be to randomly generate the clock parameters for each pixel
// at startup, and store them in some arrays. However, that consumes
// a great deal of precious RAM, and it turns out to be totally
// unnessary! If the random number generate is 'seeded' with the
// same starting value every time, it will generate the same sequence
// of values every time. So the clock adjustment parameters for each
// pixel are 'stored' in a pseudo-random number generator! The PRNG
// is reset, and then the first numbers out of it are the clock
// adjustment parameters for the first pixel, the second numbers out
// of it are the parameters for the second pixel, and so on.
// In this way, we can 'store' a stable sequence of thousands of
// random clock adjustment parameters in literally two bytes of RAM.
//
// There's a little bit of fixed-point math involved in applying the
// clock speed adjustments, which are expressed in eighths. Each pixel's
// clock speed ranges from 8/8ths of the system clock (i.e. 1x) to
// 23/8ths of the system clock (i.e. nearly 3x).
//
// On a basic Arduino Uno or Leonardo, this code can twinkle 300+ pixels
// smoothly at over 50 updates per seond.
//
// -Mark Kriegsman, December 2015

CRGBArray<NUM_LEDS> leds;

// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).
// 4, 5, and 6 are recommended, default is 4.
#define TWINKLE_SPEED 4

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).
// Default is 5.
#define TWINKLE_DENSITY 2

// How often to change color palettes.
#define SECONDS_PER_PALETTE 30
// Also: toward the bottom of the file is an array
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black;
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1

#define BRIGHTNESS 10
CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

void setup() {
  delay(3000);  //safety startup delay
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);

  chooseNextColorPalette(gTargetPalette);
  randomSeed(analogRead(0));
  FastLED.setBrightness(MAX_BRIGHTNESS);
}

extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;
byte trail_pos = 0;
byte lead_pos = 0;
CHSV color;
CHSV alt_color;

void loop() {
  for (int i = 0; i < 100; i++) {
    EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
      chooseNextColorPalette(gTargetPalette);
    }

    EVERY_N_MILLISECONDS(10) {
      nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
    }

    drawTwinkles(leds);

    FastLED.show();
  }

  // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay
  meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
  meteorRain(0xB7, 0x00, 0xFE, 10, 64, true, 30);

  // theatherChase - Color (red, green, blue), speed delay
  //theaterChase(0xff,0,0,150);
  //theaterChase(0xff,0,0,150);

  for (int b = 0; b < 500; b++) {
    FastLED.show();

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    fadeToBlackBy(leds, NUM_LEDS, TRAIL_DIM_BY);

    if (trail_pos == 0) {
      lead_pos++;
      if (lead_pos >= LEAD_COUNT) {
        color = CHSV(DRIP_HUE1 + (DRIP_HUE_SKEW * led_pos), DRIP_SAT, 255);
        alt_color = CHSV(DRIP_HUE1 + (DRIP_HUE_SKEW * led_pos) + 127, DRIP_SAT, 255);
        leds[led_pos] = color;
        if (MIRROR) {
          if (MIRROR_INV_COLOR)
            leds[inv_led_pos] = alt_color;
          else
            leds[inv_led_pos] = color;
        }
        inv_led_pos--;
        led_pos++;
      } else {
        CHSV color_1 = CHSV(DRIP_HUE1 + LEAD_COUNT - lead_pos, DRIP_SAT, map(lead_pos, 1, LEAD_COUNT, 1, 255));
        CHSV color_2 = CHSV(DRIP_HUE1 + LEAD_COUNT - lead_pos, DRIP_SAT, map(lead_pos, 1, LEAD_COUNT, 1, 64));
        CHSV alt_color_1 = CHSV(DRIP_HUE1 + LEAD_COUNT - lead_pos + 127, DRIP_SAT, map(lead_pos, 1, LEAD_COUNT, 1, 255));
        CHSV alt_color_2 = CHSV(DRIP_HUE1 + LEAD_COUNT - lead_pos + 127, DRIP_SAT, map(lead_pos, 1, LEAD_COUNT, 1, 64));

        if (MIRROR) {
          leds[inv_led_pos - 1] = color_2;
          leds[inv_led_pos] = color_1;
          if (MIRROR_INV_COLOR) {
            leds[inv_led_pos - 1] = alt_color_2;
            leds[inv_led_pos] = alt_color_1;
          }
        }
        leds[led_pos] = color_1;
        leds[led_pos + 1] = color_2;
      }
    }

    if (led_pos >= STOP_SEGMENT - 1) {
      trail_pos++;
      if (trail_pos <= TRAIL_LENGTH * 0.50) {
        leds[led_pos] = color;
        if (MIRROR) {
          leds[inv_led_pos] = color;
          if (MIRROR_INV_COLOR) {
            leds[inv_led_pos] = alt_color;
          }
        }
      }
      if (trail_pos >= TRAIL_LENGTH) {
        leds[led_pos] = CHSV(0, 0, 0);
        if (MIRROR) leds[inv_led_pos] = CHSV(0, 0, 0);
        FastLED.delay(random(MIN_DELAY, MAX_DELAY));
        trail_pos = 0;
        led_pos = 0;
        lead_pos = 0;
        inv_led_pos = NUM_LEDS - 1;
      }
    }
  }

  for (int c = 0; c < 1000; c++) {
    EVERY_N_SECONDS(7) {
      gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
      gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];
    }

    EVERY_N_MILLISECONDS(40) {
      nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 16);
    }

    colorwaves(leds, NUM_LEDS, gCurrentPalette);

    FastLED.show();
    FastLED.delay(20);
  }
}

// This function loops over each pixel, calculates the
// adjusted 'clock' that this pixel should use, and calls
// "CalculateOneTwinkle" on each pixel. It then displays
// either the twinkle color of the background color,
// whichever is brighter.
void drawTwinkles(CRGBSet& L) {
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;

  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if ((AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1])) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if (bglight > 64) {
      bg.nscale8_video(16);  // very bright, so scale to 1/16th
    } else if (bglight > 16) {
      bg.nscale8_video(64);  // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video(86);  // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor;  // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();

  for (CRGB& pixel : L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    uint16_t myclockoffset16 = PRNG16;          // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8;  // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color,
      // use the new color.
      pixel = c;
    } else if (deltabright > 0) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend(bg, c, deltabright * 8);
    } else {
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}

// This function takes a time in pseudo-milliseconds,
// figures out brightness = f( time ), and also hue = f( time )
// The 'low digits' of the millisecond time are used as
// input to the brightness wave function.
// The 'high digits' are used to select a color, so that the color
// does not change over the course of the fade-in, fade-out
// of one cycle of the brightness wave function.
// The 'high digits' are also used to determine whether this pixel
// should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt) {
  uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0) {
    c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    if (COOL_LIKE_INCANDESCENT == 1) {
      coolLikeIncandescent(c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
// / \
// / \
// / \
// / \
//

uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB& c, uint8_t phase) {
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
     CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green};

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
    {Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Red};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
    {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
    {CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
     HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight};

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
    {0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0xE0F0FF};

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
    {C9_Red, C9_Orange, C9_Red, C9_Orange,
     C9_Orange, C9_Red, C9_Orange, C9_Red,
     C9_Green, C9_Green, C9_Green, C9_Green,
     C9_Blue, C9_Blue, C9_Blue,
     C9_White};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
    {
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
    //&RetroC9_p,
    //&BlueWhite_p,
    //&RainbowColors_p,
    &FairyLight_p,
    //&RedGreenWhite_p,
    //&PartyColors_p,
    //&RedWhite_p,
    &Snow_p,
    //&Holly_p,
    &Ice_p};

// Advance to the next color palette in the list (above).
void chooseNextColorPalette(CRGBPalette16& pal) {
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1;
  whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j = 0; j < 50; j++) {  //do 10 cycles of chasing

    for (int q = 0; q < 7; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 7) {
        setPixel(i + q, red, green, blue);  //turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < NUM_LEDS; i = i + 7) {
        setPixel(i + q, 0, 0, 0);  //turn every third pixel off
      }
    }
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {
    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((i - j < NUM_LEDS) && (i - j >= 0)) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

// Apply LED color changes
void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void colorwaves(CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette) {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;  //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if (h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8(index, 240);

    CRGB newcolor = ColorFromPalette(palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;

    nblend(ledarray[pixelnumber], newcolor, 128);
  }
}

// Alternate rendering function just scrolls the current palette
// across the defined LED strip.
void palettetest(CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette) {
  static uint8_t startindex = 0;
  startindex--;
  fill_palette(ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
}

// Gradient Color Palette definitions for 33 different cpt-city color palettes.
// 956 bytes of PROGMEM for all of the palettes together,
// +618 bytes of PROGMEM for gradient palette code (AVR).
// 1,494 bytes total for all 34 color palettes and associated code.

// Gradient palette "ib_jul01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/xmas/tn/ib_jul01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE(ib_jul01_gp){
    0, 194, 1, 1,
    94, 1, 29, 18,
    132, 57, 131, 28,
    255, 113, 1, 1};

// Gradient palette "es_vintage_57_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/vintage/tn/es_vintage_57.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_vintage_57_gp){
    0, 2, 1, 1,
    53, 18, 1, 0,
    104, 69, 29, 1,
    153, 167, 135, 10,
    255, 46, 56, 4};

// Gradient palette "es_vintage_01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/vintage/tn/es_vintage_01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_vintage_01_gp){
    0, 4, 1, 1,
    51, 16, 0, 1,
    76, 97, 104, 3,
    101, 255, 131, 19,
    127, 67, 9, 4,
    153, 16, 0, 1,
    229, 4, 1, 1,
    255, 4, 1, 1};

// Gradient palette "es_rivendell_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/rivendell/tn/es_rivendell_15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_rivendell_15_gp){
    0, 1, 14, 5,
    101, 16, 36, 14,
    165, 56, 68, 30,
    242, 150, 156, 99,
    255, 150, 156, 99};

// Gradient palette "rgi_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/rgi/tn/rgi_15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE(rgi_15_gp){
    0, 4, 1, 31,
    31, 55, 1, 16,
    63, 197, 3, 7,
    95, 59, 2, 17,
    127, 6, 2, 34,
    159, 39, 6, 33,
    191, 112, 13, 32,
    223, 56, 9, 35,
    255, 22, 6, 38};

// Gradient palette "retro2_16_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ma/retro2/tn/retro2_16.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE(retro2_16_gp){
    0, 188, 135, 1,
    255, 46, 7, 1};

// Gradient palette "Analogous_1_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/red/tn/Analogous_1.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(Analogous_1_gp){
    0, 3, 0, 255,
    63, 23, 0, 255,
    127, 67, 0, 255,
    191, 142, 0, 45,
    255, 255, 0, 0};

// Gradient palette "es_pinksplash_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/pink_splash/tn/es_pinksplash_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_pinksplash_08_gp){
    0, 126, 11, 255,
    127, 197, 1, 22,
    175, 210, 157, 172,
    221, 157, 3, 112,
    255, 157, 3, 112};

// Gradient palette "es_pinksplash_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/pink_splash/tn/es_pinksplash_07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_pinksplash_07_gp){
    0, 229, 1, 1,
    61, 242, 4, 63,
    101, 255, 12, 255,
    127, 249, 81, 252,
    153, 255, 11, 235,
    193, 244, 5, 68,
    255, 232, 1, 5};

// Gradient palette "Coral_reef_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/other/tn/Coral_reef.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(Coral_reef_gp){
    0, 40, 199, 197,
    50, 10, 152, 155,
    96, 1, 111, 120,
    96, 43, 127, 162,
    139, 10, 73, 111,
    255, 1, 34, 71};

// Gradient palette "es_ocean_breeze_068_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/ocean_breeze/tn/es_ocean_breeze_068.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_ocean_breeze_068_gp){
    0, 100, 156, 153,
    51, 1, 99, 137,
    101, 1, 68, 84,
    104, 35, 142, 168,
    178, 0, 63, 117,
    255, 1, 10, 10};

// Gradient palette "es_ocean_breeze_036_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/ocean_breeze/tn/es_ocean_breeze_036.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_ocean_breeze_036_gp){
    0, 1, 6, 7,
    89, 1, 99, 111,
    153, 144, 209, 255,
    255, 0, 73, 82};

// Gradient palette "departure_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/mjf/tn/departure.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 88 bytes of program space.

DEFINE_GRADIENT_PALETTE(departure_gp){
    0, 8, 3, 0,
    42, 23, 7, 0,
    63, 75, 38, 6,
    84, 169, 99, 38,
    106, 213, 169, 119,
    116, 255, 255, 255,
    138, 135, 255, 138,
    148, 22, 255, 24,
    170, 0, 255, 0,
    191, 0, 136, 0,
    212, 0, 55, 0,
    255, 0, 55, 0};

// Gradient palette "es_landscape_64_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/landscape/tn/es_landscape_64.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_landscape_64_gp){
    0, 0, 0, 0,
    37, 2, 25, 1,
    76, 15, 115, 5,
    127, 79, 213, 1,
    128, 126, 211, 47,
    130, 188, 209, 247,
    153, 144, 182, 205,
    204, 59, 117, 250,
    255, 1, 37, 192};

// Gradient palette "es_landscape_33_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/landscape/tn/es_landscape_33.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_landscape_33_gp){
    0, 1, 5, 0,
    19, 32, 23, 1,
    38, 161, 55, 1,
    63, 229, 144, 1,
    66, 39, 142, 74,
    255, 1, 4, 1};

// Gradient palette "rainbowsherbet_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ma/icecream/tn/rainbowsherbet.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(rainbowsherbet_gp){
    0, 255, 33, 4,
    43, 255, 68, 25,
    86, 255, 7, 25,
    127, 255, 82, 103,
    170, 255, 255, 242,
    209, 42, 255, 22,
    255, 87, 255, 65};

// Gradient palette "gr65_hult_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/hult/tn/gr65_hult.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(gr65_hult_gp){
    0, 247, 176, 247,
    48, 255, 136, 255,
    89, 220, 29, 226,
    160, 7, 82, 178,
    216, 1, 124, 109,
    255, 1, 124, 109};

// Gradient palette "gr64_hult_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/hult/tn/gr64_hult.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE(gr64_hult_gp){
    0, 1, 124, 109,
    66, 1, 93, 79,
    104, 52, 65, 1,
    130, 115, 127, 1,
    150, 52, 65, 1,
    201, 1, 86, 72,
    239, 0, 55, 45,
    255, 0, 55, 45};

// Gradient palette "GMT_drywet_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_drywet.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(GMT_drywet_gp){
    0, 47, 30, 2,
    42, 213, 147, 24,
    84, 103, 219, 52,
    127, 3, 219, 207,
    170, 1, 48, 214,
    212, 1, 1, 111,
    255, 1, 7, 33};

// Gradient palette "ib15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/general/tn/ib15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(ib15_gp){
    0, 113, 91, 147,
    72, 157, 88, 78,
    89, 208, 85, 33,
    107, 255, 29, 11,
    141, 137, 31, 39,
    255, 59, 33, 89};

// Gradient palette "Fuschia_7_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/fuschia/tn/Fuschia-7.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(Fuschia_7_gp){
    0, 43, 3, 153,
    63, 100, 4, 103,
    127, 188, 5, 66,
    191, 161, 11, 115,
    255, 135, 20, 182};

// Gradient palette "es_emerald_dragon_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_emerald_dragon_08_gp){
    0, 97, 255, 1,
    101, 47, 133, 1,
    178, 13, 43, 1,
    255, 2, 10, 1};

// Gradient palette "lava_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/neota/elem/tn/lava.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE(lava_gp){
    0, 0, 0, 0,
    46, 18, 0, 0,
    96, 113, 0, 0,
    108, 142, 3, 1,
    119, 175, 17, 1,
    146, 213, 44, 2,
    174, 255, 82, 4,
    188, 255, 115, 4,
    202, 255, 156, 4,
    218, 255, 203, 4,
    234, 255, 255, 4,
    244, 255, 255, 71,
    255, 255, 255, 255};

// Gradient palette "fire_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/neota/elem/tn/fire.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(fire_gp){
    0, 1, 1, 0,
    76, 32, 5, 0,
    146, 192, 24, 0,
    197, 220, 105, 5,
    240, 252, 255, 31,
    250, 252, 255, 111,
    255, 255, 255, 255};

// Gradient palette "Colorfull_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Colorfull.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 44 bytes of program space.

DEFINE_GRADIENT_PALETTE(Colorfull_gp){
    0, 10, 85, 5,
    25, 29, 109, 18,
    60, 59, 138, 42,
    93, 83, 99, 52,
    106, 110, 66, 64,
    109, 123, 49, 65,
    113, 139, 35, 66,
    116, 192, 117, 98,
    124, 255, 255, 137,
    168, 100, 180, 155,
    255, 22, 121, 174};

// Gradient palette "Magenta_Evening_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Magenta_Evening.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(Magenta_Evening_gp){
    0, 71, 27, 39,
    31, 130, 11, 51,
    63, 213, 2, 64,
    70, 232, 1, 66,
    76, 252, 1, 69,
    108, 123, 2, 51,
    255, 46, 9, 35};

// Gradient palette "Pink_Purple_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Pink_Purple.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 44 bytes of program space.

DEFINE_GRADIENT_PALETTE(Pink_Purple_gp){
    0, 19, 2, 39,
    25, 26, 4, 45,
    51, 33, 6, 52,
    76, 68, 62, 125,
    102, 118, 187, 240,
    109, 163, 215, 247,
    114, 217, 244, 255,
    122, 159, 149, 221,
    149, 113, 78, 188,
    183, 128, 57, 155,
    255, 146, 40, 123};

// Gradient palette "Sunset_Real_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Sunset_Real.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(Sunset_Real_gp){
    0, 120, 0, 0,
    22, 179, 22, 0,
    51, 255, 104, 0,
    85, 167, 22, 18,
    135, 100, 0, 103,
    198, 16, 0, 130,
    255, 0, 0, 160};

// Gradient palette "es_autumn_19_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/autumn/tn/es_autumn_19.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_autumn_19_gp){
    0, 26, 1, 1,
    51, 67, 4, 1,
    84, 118, 14, 1,
    104, 137, 152, 52,
    112, 113, 65, 1,
    122, 133, 149, 59,
    124, 137, 152, 52,
    135, 113, 65, 1,
    142, 139, 154, 46,
    163, 113, 13, 1,
    204, 55, 3, 1,
    249, 17, 1, 1,
    255, 17, 1, 1};

// Gradient palette "BlacK_Blue_Magenta_White_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Blue_Magenta_White.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(BlacK_Blue_Magenta_White_gp){
    0, 0, 0, 0,
    42, 0, 0, 45,
    84, 0, 0, 255,
    127, 42, 0, 255,
    170, 255, 0, 255,
    212, 255, 55, 255,
    255, 255, 255, 255};

// Gradient palette "BlacK_Magenta_Red_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Magenta_Red.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(BlacK_Magenta_Red_gp){
    0, 0, 0, 0,
    63, 42, 0, 45,
    127, 255, 0, 255,
    191, 255, 0, 45,
    255, 255, 0, 0};

// Gradient palette "BlacK_Red_Magenta_Yellow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/BlacK_Red_Magenta_Yellow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(BlacK_Red_Magenta_Yellow_gp){
    0, 0, 0, 0,
    42, 42, 0, 0,
    84, 255, 0, 0,
    127, 255, 0, 45,
    170, 255, 0, 255,
    212, 255, 55, 45,
    255, 255, 255, 0};

// Gradient palette "Blue_Cyan_Yellow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/basic/tn/Blue_Cyan_Yellow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(Blue_Cyan_Yellow_gp){
    0, 0, 0, 255,
    63, 0, 55, 255,
    127, 0, 255, 255,
    191, 42, 255, 45,
    255, 255, 255, 0};

// Single array of defined cpt-city color palettes.
// This will let us programmatically choose one based on
// a number, rather than having to activate each explicitly
// by name every time.
// Since it is const, this array could also be moved
// into PROGMEM to save SRAM, but for simplicity of illustration
// we'll keep it in a regular SRAM array.
//
// This list of color palettes acts as a "playlist"; you can
// add or delete, or re-arrange as you wish.
const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
    Sunset_Real_gp,
    es_rivendell_15_gp,
    es_ocean_breeze_036_gp,
    rgi_15_gp,
    retro2_16_gp,
    Analogous_1_gp,
    es_pinksplash_08_gp,
    Coral_reef_gp,
    es_ocean_breeze_068_gp,
    es_pinksplash_07_gp,
    es_vintage_01_gp,
    departure_gp,
    es_landscape_64_gp,
    es_landscape_33_gp,
    rainbowsherbet_gp,
    gr65_hult_gp,
    gr64_hult_gp,
    GMT_drywet_gp,
    ib_jul01_gp,
    es_vintage_57_gp,
    ib15_gp,
    Fuschia_7_gp,
    es_emerald_dragon_08_gp,
    lava_gp,
    fire_gp,
    Colorfull_gp,
    Magenta_Evening_gp,
    Pink_Purple_gp,
    es_autumn_19_gp,
    BlacK_Blue_Magenta_White_gp,
    BlacK_Magenta_Red_gp,
    BlacK_Red_Magenta_Yellow_gp,
    Blue_Cyan_Yellow_gp};

// Count of how many cpt-city gradients are defined:
const uint8_t gGradientPaletteCount =
    sizeof(gGradientPalettes) / sizeof(TProgmemRGBGradientPalettePtr);