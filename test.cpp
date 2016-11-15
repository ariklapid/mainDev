void Setup::checkFailures(void) {
  // Declare objects.
  Display display;
  Main main;
  // use EEPROM registers to determine faliures
  minutesPassed = EEPROM.read(0);
  noWater = EEPROM.read(1);
  display.checkFailures(); // Prints: Checking previous failures.
  // 
  if((minutesPassed != 180)||(minutesPassed != 0)) {
    display.print("Previous irrigation was not finished.",2, WHITE,25,100);
	Serial.println("Previous irrigation was not finished.");
    int minutesLeft = 180 - minutesPassed;
    delay(3000);
    display.printInt(minutesLeft,2,WHITE,30,120);
    display.print(" minutes left. Press",2,WHITE,40,120);
    display.print(" button to stop recovery",2, WHITE,35,135);
    if(main.manualInput(60) == false) {   // Wait for user's decision - (60 seconds).
      main.irrigateMinutes(minutesLeft);  // If button was pressed - we skip this if.
    }
}
  else if(noWater == true) {
    display.print("Something is wrong with the water/valve.",2,WHITE,40,120);
    display.print("Fix the problem and press the button.",2,WHITE,43,140);
    while(true) {
      // System is blocked until user presses the button.
      if(main.manualInput(60) == true) break;
    }
    display.print("Button pressed. Assuming fix has been done.",2,WHITE,10,200);
  } 
  else {
    display.print("Succesfully passed",3,WHITE,40,80);
    display.print("itialization test.",3,WHITE,40,100);
  }
}
