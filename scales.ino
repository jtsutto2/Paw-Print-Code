#include "PawPrintHeader.h"
#include "mbed.h"
#include <mbed_mktime.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

/***************************/
// Global Variables //
//*************************//

//pins:
const int HX711_dout_1 = 4; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 5; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 6; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 7; //mcu > HX711 no 2 sck pin

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
unsigned long t = 0;

float FoodEaten = 0; // Sum of all weight unloaded (food consumed) from food scale

/***************************/
// Scale-Related Functions //
//*************************//

//This is the function called from main for all food-related operations
void food(void) {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0; //increase value to slow down serial print activity    

    float firstReadingA = 0.0;
    float firstReadingB = 0.0;
    float secondReadingA = 0.0;
    float secondReadingB = 0.0;

  // Reading the scales and populating Load Cell A + B:
    if (millis() > t + serialPrintInterval) { // Only runs at certain time intervals
      LoadCell_1.update(); // This built-in function updates the value of LoadCell and returns TRUE if value is new
      firstReadingA = LoadCell_1.getData(); // This built-in function gets the float value produced by load cell
      printScale(firstReadingA);

      LoadCell_2.update();
      firstReadingB = LoadCell_2.getData();
      printScale(firstReadingB);

      delay(500); // 500ms delay between readings
      
      LoadCell_1.update(); 
      secondReadingA = LoadCell_1.getData();
      printScale(secondReadingA);

      LoadCell_2.update(); 
      secondReadingB = LoadCell_2.getData();
      printScale(secondReadingB);

      newDataReady = 0;
      t = millis();
    }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }

  if (didPetEat(firstReadingA, secondReadingA)) foodUpdate(secondReadingA - firstReadingA); // If there is a decrease in scale weight (food was eaten), make necessary updates
  if (didPetEat(firstReadingB, secondReadingB)) foodUpdate(secondReadingB - firstReadingB); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Reads and returns value from scale
//int readScale(void) {
//  return analogRead(FoodPin);                 // Read scale and store value, must make sure of FoodPin 
//} 

// Prints the value that was just weighed (value)
void printScale (float value) {
  Serial.print("Load_cell output val: ");
  Serial.print(value);
  
  Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
}

// Did the pet eat?
bool didPetEat(float first, float second) {
    // Calculate the percentage increase
    float percentageIncrease = ((second - first) / first) * 100.0;

    // Check if the percentage increase exceeds the threshold
    if (percentageIncrease > 10) { //set the threshhold to 5%
        return true;  // Second is significantly larger than first
    } else {
        return false; // Second is not significantly larger than first
    }
}

// Add to the total amount eaten
void foodUpdate(float amountEaten) {
  FoodEaten += amountEaten;                  // Update the amount of Food Eaten
  
  Serial.println("Your pet has eaten ");
  Serial.println(FoodEaten);                 // Outputs value of Food Eaten
  Serial.println("total grams of food.");

  Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
}

// Reset FoodEaten value (probably after the day is over or something)
void resetFoodEaten(void) {
  FoodEaten = 0;
}

// This function must be called in the setup phase of main file
void setupLoadCells(void) {
    Serial.begin(57600); delay(10);
    Serial.println();
    Serial.println("Starting...");

    float calibrationValue_1; // calibration value load cell 1
    float calibrationValue_2; // calibration value load cell 2

    calibrationValue_1 = 696.0; // uncomment this if you want to set this value in the sketch
    calibrationValue_2 = 733.0; // uncomment this if you want to set this value in the sketch
    #if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
    #endif
    EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
    EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom

    LoadCell_1.begin();
    LoadCell_2.begin();
    //LoadCell_1.setReverseOutput();
    //LoadCell_2.setReverseOutput();
    unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    byte loadcell_1_rdy = 0;
    byte loadcell_2_rdy = 0;
    while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { //run startup, stabilization and tare, both modules simultaniously
        if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
        if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
    }
    if (LoadCell_1.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
    }
    if (LoadCell_2.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
    }
    LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
    LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
    Serial.println("Startup is complete");
}
