////////////////////////////////////////////////////////////////////////
//
// ###                                 #####
//  #  #    #   ##    ####  ######    #     # ###### #    # ###### #####    ##   #####  ####  #####
//  #  ##  ##  #  #  #    # #         #       #      ##   # #      #    #  #  #    #   #    # #    #
//  #  # ## # #    # #      #####     #  #### #####  # #  # #####  #    # #    #   #   #    # #    #
//  #  #    # ###### #  ### #         #     # #      #  # # #      #####  ######   #   #    # #####
//  #  #    # #    # #    # #         #     # #      #   ## #      #   #  #    #   #   #    # #   #
// ### #    # #    #  ####  ######     #####  ###### #    # ###### #    # #    #   #    ####  #    #
//
////////////////////////////////////////////////////////////////////////
void drawImage(unsigned long time, const char picture[][totalLEDs][3], int pictureWidth) 
{
  int currentTime = millis(); // Get the current time (Unix Time)
  int column; // This might need to be declared outside this function
  
  while((millis() < currentTime + (time * 1000)) && showPicture )
  {
    if(column < pictureWidth)
    {
      for(int row = totalLEDs; row >= 0; row --)
      {
        currentLED[row].setRGB(
          picture[column] [row] [redVal],   // Red   (0)
          picture[column] [row] [greenVal], // Green (1)
          picture[column] [row] [blueVal]); // Blue  (2)
      }      
      FastLED.show();
      column ++;
      delayMicroseconds(40); // How wide the image is (800 used to be good but now 40 seems better, figure out why) *NB*
    }
    
    else column = 0;
    delayMicroseconds(1000); // Gap between images (1000 seems good)   
    button.tick(); // This is here to prevent the strip being updated after a mode change
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # #  ####  ##### #    # #####  ######  ####
//  #     # # #    #   #   #    # #    # #      #
//  ######  # #        #   #    # #    # #####   ####
//  #       # #        #   #    # #####  #           #
//  #       # #    #   #   #    # #   #  #      #    #
//  #       #  ####    #    ####  #    # ######  ####
//
//////////////////////////////////////////////////////////////////////////////
void pictures1()
{
  // drawPicture(10, Inkeds);  
  drawPicture(10, squares);
  drawPicture(10, Space);
  drawPicture(10, AX);
  drawPicture(10, X);
  drawPicture(10, BB);
  drawPicture(10, BY);  
  drawPicture(10, P);

  // drawPicture(1, BW); // This breaks the poi for some reason, figure out why (Sets doubleClickActive to 3) *NB*
  
  // drawPicture(10, AL);

  // drawPicture(10, BC);
  // drawPicture(10, AE);
}

void pictures2()
{
  // drawPicture(15, ruby);
  // drawPicture()
  // drawPicture(10, TestImage);
}

//////////////////////////////////////////////////////////////////////////////
//
//  #####
// #     # #   #  ####  ##### ###### #    #
// #        # #  #        #   #      ##  ##
//  #####    #    ####    #   #####  # ## #
//       #   #        #   #   #      #    #
// #     #   #   #    #   #   #      #    #
//  #####    #    ####    #   ###### #    #
//
//////////////////////////////////////////////////////////////////////////////
void resetAll()
{
  rainbow.reset();
  crissCross.reset();
  colourCycle.reset();
  colourFade.reset();

  showPicture = false;
  off(10);
}

void off(int wait)
{
  setAll(0x000000);
  delay(wait);  
}

void flash(int wait)
{
  int totalFlashes = 2;
  for(int i = 0; i < totalFlashes; i ++)
  {
    setAll(themeColour);
    delay(wait);  
    off(wait); 
  }
}

void setAll(int themeColour)
{
  for(int i = 0; i < totalLEDs; i ++)
  {
    currentLED[i] = themeColour;
  }
  FastLED.show();
}

void printStatus()
{
  Serial << "Click Active : " << clickActive;
  Serial << " Long Press Active : " << longPressActive;
  Serial << " Double Click Active :" << doubleClickActive << endl;
}