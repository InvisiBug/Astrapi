// Notes
/*
• On startup the total leds, the strip object and the speed at which the brightness changes is given to the constructor
• When the begin function is called, the constructed interval is saved to "initialInterval" for later use
  The current led brightness is retrieved from the fast led methods and saved for later use
• When the run function is called a few different things happen
  If the brightness is to be increased then the strip indicates the current brightness value, waits a while
  then increases and indicates the level again
  if the brightness is to be decreased the opposite is true
  As the brightness level changes it is written directly to the strip so there is no need to 
  have a seperate function to hande the writing
• The show brightness function indicates the current level on the leds
• The get brightness function returns the current level
*/

#ifndef CHANGEBRIGHTNESS_H
#define CHANGEBRIGHTNESS_H
#include "FastLED.h"

class ChangeBrightness
{
	public:
		ChangeBrightness(int totalLEDs, CRGB *currentLED, int interval);

    void begin(int colour);
    void run();

    void setBrightness();
    void setBrightness(int brightness);
    void showBrightness();   

    void setInterval(int interval);
    void setInitialBrightness(int brightness);

    int  getBrightness();
  
  private:
    CRGB *currentLED; // LED strip object

    int initialInterval;
    int initialBrightness;

    int themeColour;

    int totalLEDs;
    int currentTime;

    bool up;

    // Timing Stuff
    long currentMillis;
    long lastMillis;
    int  interval;

    int brightness;
};
#endif