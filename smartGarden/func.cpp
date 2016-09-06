// This file DEFINES the global functions
#include "Arduino.h"
#include "global.hpp"
#include "func.hpp"
#include "tft.hpp"

void initPorts(void) {
  // Setup pins:
  pinMode(manualStart_button, INPUT_PULLUP);
  pinMode(okLED, OUTPUT);
  pinMode(irrigation, OUTPUT);
  pinMode(activateSensors_pin, OUTPUT);
  // Turn off all outputs:
  digitalWrite(okLED, LOW);
  digitalWrite(irrigation, LOW);
  digitalWrite(activateSensors_pin, LOW);
}

void connectToWiFi(void) {     // Waiting for incoming serial data (30 seconds)
  wifi_screen();
  for(int i=1; i<=10; i++) {   // should be i<=30 
    print_fillRect(139,159,30,30,BLUE);
    printint_display(i, 2, WHITE, 140, 160);
    delay(1000);
    if (Serial.available() > 0) {
      delay(100);
      receivedString = Serial.readStringUntil('\n');
      wiFi = true;
      break;
    }
  }
  if (wiFi == false) {
  print_display("No internet connection.", 2, WHITE, 20, 200); 
  delay(3000);
  }
  else {
  print_display("Successfully connected.", 2, WHITE, 20, 200); 
  delay(3000);
  }
}

void askForManualProg(void) {
  printStatus_display("Press button to irrigate..", 2);
  int k = 0;
  while(k < 1000)
  {
    delay(10);
    if (digitalRead(manualStart_button) == HIGH)
    {
      startIrrigation(manualProgram);
      k=1000;
    }
    else k++;
  }
}

void readMoistureSensors(void) {
  delay(3000);
  print_fillRect(6 ,203, 308, 33, BLUE);
  print_display("Reading sensors...", 2, YELLOW, 7, 210);
  delay(2000);
  digitalWrite(activateSensors_pin, HIGH);
  delay(1000);
  readSensor(1);
  readSensor(2);
  readSensor(3);
  digitalWrite(activateSensors_pin, LOW);
  delay(2000);
}

void readSensor(int sensor) {
  switch(sensor) {
    case 1:
      moistSensor_1_val = analogRead(moistSensor_1_pin);
      print_display("Sensor", 2, WHITE, 20, 88);
      print_display("#1", 2, WHITE, 38, 105);
      delay(2000);
      if (moistSensor_1_val <= 450)
      {
        print_fillRect(15, 135, 80, 40, BLUE);
        print_display("DRY", 3, WHITE, 25, 140); 
      }
      else
      {
        print_fillRect(15, 135, 80, 40, BLUE);
        print_display("WET", 3, WHITE, 25, 140);
      }
      print_fillRect(15+30, 165, 50, 30, BLUE);
      printint_display(moistSensor_1_val, 3, WHITE, 22+30, 170);
      break;
      
    case 2:
      moistSensor_2_val = analogRead(moistSensor_2_pin);
      print_display("Sensor", 2, WHITE, 20+110, 88);
      print_display("#2", 2, WHITE, 38+110, 105);
      delay(2000);
      if (moistSensor_2_val <=450)
      {
        print_fillRect(15+110, 135, 80, 40, BLUE);
        print_display("DRY", 3, WHITE, 25+110, 140); 
      }
      else
      {
        print_fillRect(15+110, 135, 80, 40, BLUE);
        print_display("WET", 3, WHITE, 25+110, 140);
      }
      print_fillRect(15+125, 165, 50, 30, BLUE);
      printint_display(moistSensor_2_val, 3, WHITE, 22+125, 170);     
      break;
      
    case 3:
      moistSensor_3_val = analogRead(moistSensor_3_pin);
      print_display("Sensor", 2, WHITE, 20+100+110, 88);
      print_display("#3", 2, WHITE, 38+100+110, 105);
      delay(2000);
      if (moistSensor_3_val <=450)
      {
        print_fillRect(15+100+120, 135, 60, 40, BLUE);
        print_display("DRY", 3, WHITE, 25+100+120, 140); 
      }
      else
      {
        print_fillRect(15+100+120, 135, 60, 40, BLUE);
        print_display("WET", 3, WHITE, 25+100+120, 140);
      }
      print_fillRect(15+110+130, 165, 50, 30, BLUE);
      printint_display(moistSensor_3_val, 3, WHITE, 22+110+130, 170);
      break;
  }
}

int countDrySensors(void) {
  int dry_sensors = 0;
  if (moistSensor_1_val <= 450) dry_sensors++;
  if (moistSensor_2_val <= 450) dry_sensors++;
  if (moistSensor_3_val <= 450) dry_sensors++;
  return dry_sensors;
}

bool getForecast(void) {
  printStatus_display("Gathering forecast...", 2);
  delay(2000);
  if (wiFi == true) {
    date = receivedString.substring(0,16);
    currentTime = receivedString.substring(17,26);
    currentTemp = receivedString.substring(27,30);
    rainChanceToday_string = receivedString.substring(31,34);
    rainChanceTomorrow_string = receivedString.substring(35,38);
    if (rainChanceTomorrow_string.toInt() > 90) return true;
  }
  else {
    printStatus_display("No internet connection.", 2);
    delay(2000);
    return false;
  }
}

void startIrrigation(int program) {
  if (program == manualProgram) {       // Manual program
    printStatus_display("Manual irrigation chosen.", 2);
    delay(2000);
  }
  printStatus_display("Starting irrigation...", 2);
  delay(5000);
  digitalWrite(irrigation, HIGH);
  printStatus_display("Irrigating right now.", 2);
  for(int i=0; i<10; i++)  blinkLED(okLED, 500); // 10 secs (10*2*500=10000ms) blinking
  digitalWrite(irrigation, LOW);
  digitalWrite(okLED, HIGH);		// Perform test if soil is ok
  printStatus_display("Finished. 10 secs till next check", 2);
  delay(10000);
}

void autoProgram(void)
{
  printStatus_display("Automatic program chosen", 2);
  delay(3000);
  readMoistureSensors();
  getForecast();
  if (countDrySensors() >= 1)
    if (getForecast() == false)  startIrrigation(autoIrrigation);
    else 
    {
      printStatus_display("Soil is dry, rain tomorrow - waiting for rain", 1);
      digitalWrite(okLED, HIGH);
      delay(10000);
    }
  else  
  {
  print_fillRect(6 ,203, 308, 33, BLUE);
  print_display("Soil is wet.", 2, YELLOW, 7, 205); 
  print_display("No need to irrigate", 2, YELLOW, 7, 220); 
  digitalWrite(okLED, HIGH); 
  delay(10000);
  }
}

void blinkLED(int pin, int freq) {
  digitalWrite(pin, HIGH);
  delay(freq);
  digitalWrite(pin, LOW);
  delay(freq);
}
