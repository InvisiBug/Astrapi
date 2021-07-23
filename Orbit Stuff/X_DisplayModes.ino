////////////////////////////////////////////////////////////////////////
//
// ######
// #     #   ##   ##### ##### ###### #####  #    #  ####
// #     #  #  #    #     #   #      #    # ##   # #
// ######  #    #   #     #   #####  #    # # #  #  ####
// #       ######   #     #   #      #####  #  # #      #
// #       #    #   #     #   #      #   #  #   ## #    #
// #       #    #   #     #   ###### #    # #    #  ####
//
//////////////////////////////////////////////////////////////////////////////
void picturesPattern()
{
  showPicture = true;
  if(!patternLongPressActive)     pictures1();
  else if(patternLongPressActive) pictures2();
}

void rainbowPattern()
{
  if(!patternLongPressActive)     rainbow.run();
  else if(patternLongPressActive) rainbow.run(2);
}

void colourCyclePattern()
{
  if(!patternLongPressActive)     colourCycle.run();
  else if(patternLongPressActive) colourCycle.run(2);
}

void colourFadePattern()
{
  if(!patternLongPressActive)     colourFade.run();
  else if(patternLongPressActive) colourFade.run(5); 
}

void crissCrossPattern()
{
  if(!patternLongPressActive)     crissCross.run();
  else if(patternLongPressActive) crissCross.run(10);
}

void firePattern()
{
  if(!patternLongPressActive)     fire.run(55, 120, 20, false);
  else if(patternLongPressActive) fire.run(55, 120, 20, true);
}

////////////////////////////////////////////////////////////////////////
//
// #######
// #     # #####  ##### #  ####  #    #  ####
// #     # #    #   #   # #    # ##   # #
// #     # #    #   #   # #    # # #  #  ####
// #     # #####    #   # #    # #  # #      #
// #     # #        #   # #    # #   ## #    #
// ####### #        #   #  ####  #    #  ####
//
//////////////////////////////////////////////////////////////////////////////
void changeBrightnessOption()
{
  if(!optionsLongPressActive)     changeBrightness.showBrightness();
  else if(optionsLongPressActive) changeBrightness.run(); 
}

void singleColour()
{
  if(!optionsLongPressActive)     setAll(0xff3000);
  else if(optionsLongPressActive) setAll(0x00ffff);
}