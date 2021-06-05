// #include "Breather.h"
// #include "Streaming.h"
// #include "Arduino.h"
// using namespace std;

// Breather::Breather(int totalLEDs, CRGB *currentLED, int interval)
// {
//   this -> totalLEDs  = totalLEDs;
//   this -> currentLED = currentLED;
//   this -> interval   = interval;

//   colour = random(255);
// }

// void Breather::begin()
// {
//   initialInterval = interval;
// }

// //////////////////////////////////////////////////////////////////////////////
// //
// //  ######                
// //  #     # #    # #    # 
// //  #     # #    # ##   # 
// //  ######  #    # # #  # 
// //  #   #   #    # #  # # 
// //  #    #  #    # #   ## 
// //  #     #  ####  #    # 
// //
// //////////////////////////////////////////////////////////////////////////////
// void Breather::run()
// {
//   currentMillis = millis();

//   if(currentMillis - lastMillis >= interval)
//   {
//     lastMillis = currentMillis;

//     if((currentStep < totalSteps))
//     {
//       for(int i = 0; i < totalLEDs; i ++)
//       {
//         currentLED[i].setHue(currentStep - i * (255 / totalLEDs));
//       }

//       FastLED.show();
//       currentStep ++;
//     }
//     else currentStep = 0;
//   }
// }

// //////////////////////////////////////////////////////////////////////////////
// //
// // ###                                                 
// //  #  #    # ##### ###### #####  #    #   ##   #      
// //  #  ##   #   #   #      #    # #    #  #  #  #      
// //  #  # #  #   #   #####  #    # #    # #    # #      
// //  #  #  # #   #   #      #####  #    # ###### #      
// //  #  #   ##   #   #      #   #   #  #  #    # #      
// // ### #    #   #   ###### #    #   ##   #    # ######
// //
// //////////////////////////////////////////////////////////////////////////////
