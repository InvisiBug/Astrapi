////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh 
// 
//  Project Orbit
//  Orbit V3.ino (Object Orientated)
//  2018
//  *********
//
// Add version history
// 10 / 07 / 2017 V1.0 Basic functionality
// Some time later V1.1 Image display improvements
// A lot of time later V2.0 Button and patterns added
////////////////////////////////////////////////////////////////////////
//  Notes
//
//  VERY IMPORTANT, when testing high intensity patterns
//  make sure to leave a long delay in the startup function 
//  to allow time for a reupload when the battery cant supply enough power causing a mcu crash
//
//  The LED strips use the hardware SPI pins so there is no need to specify the pin numbers
//  Strip Data    11
//  Strip Clock   13
//
//  Button  24
//
//  Search for *NB* to find things that need fixing
//
////////////////////////////////////////////////////////////////////////
//
// Software
//    #      ##   #####   ####  ###### #####  ####
//    #     #  #  #    # #    # #        #   #
//    #    #    # #    # #      #####    #    ####
//    #    ###### #####  #  ### #        #        #
//    #    #    # #   #  #    # #        #   #    #
//    #    #    # #    #  ####  ######   #    ####
//
////////////////////////////////////////////////////////////////////////
// Determine whether the current methods are fast enough for proper pattern displaying
// Add accelerometer / gyro combo
// -> Calculate spin rate and adjust picture to compensate
// -> Calculate spin direction and adjust picture to match

////////////////////////////////////////////////////////////////////////
//
// #######           ######             #####
//    #     ####     #     #  ####     #     #  ####   ####  #    #
//    #    #    #    #     # #    #    #       #    # #    # ##   #
//    #    #    #    #     # #    #     #####  #    # #    # # #  #
//    #    #    #    #     # #    #          # #    # #    # #  # #
//    #    #    #    #     # #    #    #     # #    # #    # #   ##
//    #     ####     ######   ####      #####   ####   ####  #    #
//
////////////////////////////////////////////////////////////////////////
// *NB*
// Battery level meter
// Rotation sensor
// Make the poi remeber the states

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
#include "FastLED.h"
#include "Streaming.h"
#include "OneButton.h"

// Effects
#include "Fire.h"
#include "Rainbow.h"
#include "Crisscross.h"
#include "ColourFade.h"
#include "ColourCycle.h"
#include "ChangeBrightness.h"

//Images
#include "Images.h"
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
// #define totalLEDs 55 // Needs to be defined in Images.h <- look in to this later *NB*
#define powerLED  13 // This is the same as the spi clock (may cause issues)
#define buttonPin 24

#define redVal   0
#define greenVal 1
#define blueVal  2

#define data  11
#define clock 13

#define OFF LOW
#define ON  HIGH

// Image displaying macros
#define width(array) sizeof(array) / sizeof(array[0]) 
#define drawPicture(time, picture) drawImage(time, picture, sizeof(picture) / sizeof(picture[0]))

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
CRGB currentLED[totalLEDs];

// Button
OneButton button(buttonPin, true);

// Effects
Fire                         fire(totalLEDs, currentLED);
Rainbow                   rainbow(totalLEDs, currentLED, 20);
ColourFade             colourFade(totalLEDs, currentLED, 10);
Crisscross             crissCross(totalLEDs, currentLED, 20);
ColourCycle           colourCycle(totalLEDs, currentLED, 20);
ChangeBrightness changeBrightness(totalLEDs, currentLED, 40);

////////////////////////////////////////////////////////////////////////
//
// ######
// #     # #####   ####  #####  ####  ##### #   # #####  ######  ####
// #     # #    # #    #   #   #    #   #    # #  #    # #      #
// ######  #    # #    #   #   #    #   #     #   #    # #####   ####
// #       #####  #    #   #   #    #   #     #   #####  #           #
// #       #   #  #    #   #   #    #   #     #   #      #      #    #
// #       #    #  ####    #    ####    #     #   #      ######  ####
//
////////////////////////////////////////////////////////////////////////
// System prototypes
void resetAll();
void printStatus();
void setAll(int themeColour);

// Button Prototypes
void click();
void longPress();
void doubleClick();

// Image Prototypes
void pictures1();
void pictures2();
void drawImage(unsigned long time, const char picture[][totalLEDs][3], int pictureWidth);

// Status Change Indicator
void off(int wait);
void flash(int wait);

// Pattern Functions
void picturesPattern();
void rainbowPattern();
void colourCyclePattern();
void colourFadePattern();
void crissCrossPattern();
void firePattern();

// Options Menu Functions (There will be more)
void changeBrightnessOption();
void singleColour();

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
int LEDBrightness = 25; // As a percentage (saved as a dynamic variable to let us change later)

int themeColour = 0x20845; 

int maxOptions  = 2;
int maxPatterns = 5; // Used as a variable here while testing, will move to a define later to save memory

int option  = 1; // The current option to be changed
int pattern = 1; // The current pattern being displayed (Maybe rename the variable)

// Image Generation Variables (Don't remove)
bool showPicture = true; // Used for images
// int  currentTime; // This might be needed *NB*

// Button navigation
bool clickActive       = false; // False
bool longPressActive   = false; // False
bool doubleClickActive = false; // False

bool patternLongPressActive  = false;
bool optionsLongPressActive  = false;

// bool menu = patterns;

enum PatternMenu {patterns, options};
PatternMenu menu;

// int pattern[maxPatterns]; // *NB* Make it remember the pattern state (Fast or slow speed)

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
void setup()
{
  Serial.begin(115200);

	// Power LED (Might not use), The led is on the same pin as the spi clock
  pinMode(powerLED, OUTPUT);
  digitalWrite(powerLED, OFF);

	// Set LED Strip Parameters
  // FastLED.addLeds<APA102, BGR >(currentLED, totalLEDs);
  FastLED.addLeds< APA102, data, clock, BGR, DATA_RATE_MHZ( 24 ) >(currentLED, totalLEDs); // May not need to specify the data rate *NB*
  
  FastLED.setBrightness( LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither( 1 );

  // Set Button Parameters
  button.setDebounceTicks( 20 ); // 20
  button.setClickTicks( 350 );   // 350
  button.setPressTicks( 250 );   // 250

  // Setup Button Functions
  button.attachLongPressStart(longPress);
  button.attachDoubleClick(doubleClick);
  button.attachClick(click);

  // Initialize Effects
	changeBrightness.begin(themeColour);
	colourCycle.begin();
  colourFade.begin();
  crissCross.begin();
  rainbow.begin();
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
void loop()
{
  button.tick();

  if(menu == patterns) // Pattern Menu
  {
    if(pattern == 1)      picturesPattern();
    else if(pattern == 2) rainbowPattern();
    else if(pattern == 3) colourCyclePattern();
    else if(pattern == 4) colourFadePattern();
    else if(pattern == 5) firePattern();
  }

  else if(menu == options) // Options Menu
  {
    if(option == 1)      changeBrightnessOption();
    else if(option == 2) singleColour();
  }
}