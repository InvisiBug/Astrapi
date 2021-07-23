////////////////////////////////////////////////////////////////////////
//
// ######
// #     # #    # ##### #####  ####  #    #
// #     # #    #   #     #   #    # ##   #
// ######  #    #   #     #   #    # # #  #
// #     # #    #   #     #   #    # #  # #
// #     # #    #   #     #   #    # #   ##
// ######   ####    #     #    ####  #    #
//
////////////////////////////////////////////////////////////////////////
// *NB* redo everythign about this section, it's terrible
void click()
{
  Serial << "Click" << endl;

  if(menu == patterns) // Pattern Menu
  {
    if(!patternLongPressActive) // Advance through pattern menu
    {
      resetAll();
      if(pattern < maxPatterns) pattern ++;
      else pattern = 1;
    }      
  }

  else if(menu == options) // Options Menu
  {
    if(!optionsLongPressActive)
    {
      resetAll();
      if(option < maxOptions) option ++;
      else option = 1;
    }
  }
}

void longPress()
{
  Serial << "Long Press" << endl;

  showPicture = false;
  if(menu == patterns)
  { 
    patternLongPressActive = !patternLongPressActive;
    off(150);
    flash(100);
  }

  else if(menu == options)
  {
    optionsLongPressActive = !optionsLongPressActive;
    off(150);
    flash(100);
  }
}

void doubleClick()
{  
  Serial << "Double Click" << endl;

  if(menu == patterns)     menu = options;
  else if(menu == options) menu = patterns;

  option = 1; // Always go back to brightness option
  optionsLongPressActive = false;

  off(0);
  showPicture = false; // Releases the code from the image generation function
}