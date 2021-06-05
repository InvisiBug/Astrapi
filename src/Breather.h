// #ifndef RAINBOW_H
// #define RAINBOW_H
// #include "FastLED.h"

// class Rainbow
// {
// 	public:
// 		Rainbow(int totalLEDs, CRGB *currentLED, int interval); // Constructor

//     void run();
//     void run(int wait);
//     void reset();
//     void begin();
//     void changeSpeed(int interval); // Time to change to next speed
//     void setInterval(int interval);
    
//     int  getInterval();

// 	private:
//     CRGB *currentLED; // LED strip object

//     int initialInterval;

//     int colour;

//     bool slowDown;

//     int totalLEDs; 
//     int totalSteps;
//     int currentStep;

//     // Timing Stuff
//     long currentMillis;
//     long lastMillis;
//     int  interval;

//     long speedChangeCurrentMillis;
//     long speedChangeLastMillis;
//     int  speedChangeInterval;
// };
// #endif