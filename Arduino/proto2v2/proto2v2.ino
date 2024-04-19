#include "proto2v2.h"
#include <Servo.h>
//#include "mbed.h"
//#include <mbed_mktime.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

void setup() {
    Init_Serial();           // Initialize serial communication
    delay(500);
    setupLoadCells();        // Initialize load cells
    delay(500);
    Init_Buttons();          // Initialize button
    delay(500);
    Init_Servo();            // Initialize servo motors
    delay(500);
    tareOperations();        // Null or zero out cell data while nothing is on them
    delay(500);
}

void loop() {
    // The following detects if button is pressed, and if it is, it will trigger a food dispense
    // There is a cooldown between presses to take effect
    if ((checkButton()) && (currentTime() >= foodCooldown)) {    // Check if button is pressed and cooldown is met
        dispenseFood();                                          // Dispense the food
        foodCooldown = cooldown(1);                              // We will dispense, set the cooldown
    }

    // The following reads the scales in cycles and uses that information to track dietary trends
    if (currentTime() >= readingCooldown) {          // Readings will only run every 200ms
     // Get a current reading of food and water scales
        readFoodScale(foodReading);
        readWaterScale(waterReading);

        // Debugging
        Serial.print("Your current food: ");
        Serial.println(foodReading);

        Serial.print("Your current water: ");
        Serial.println(waterReading);

        Serial.print("Your tempFood: ");
        Serial.println(tempFood);

        Serial.print("Your tempWater: ");
        Serial.println(tempWater);
        Serial.println("***************************");

        // Check if food and water was consumed and update if so
        foodOperations();                            // Check and update if pet ate
        waterOperations();                           // Check and update if pet drank

        // Debugging
        Serial.print("Your foodEaten: ");
        Serial.println(foodEaten);

        Serial.print("Your waterDrank: ");
        Serial.println(waterDrank);
        Serial.println("***************************");

        // Set new temp values as being current values for future comparison
        tempFood = foodReading;
        tempWater = waterReading;

        // Update cooldown
        readingCooldown = cooldown(1);             // Reset cooldown after each reading
    }

    // The following resets data captures after the day (24 hours) is over
    if (currentTime() >= foodEatenCooldown) {        // If it is time to reset FoodEaten monitor
        foodEatenCooldown = cooldown(86400);         // Set cooldown to 24 hours
        resetFoodEaten();                            // Sets foodEaten back to zero
    }

    // The following will frequently print out pet's daily consumption progress
    /*if (currentTime() >= consumptionCooldown) {
        
        Serial.print(dispenseCount);
        Serial.print(",");
        Serial.print(foodEaten);
        Serial.print(",");
        Serial.println(waterDrank);

        consumptionCooldown = cooldown(2);
    }*/
}

// Runs the motors to dispense food
void dispenseFood(void) {
    dispenseCount++;
    myservo.write(180);       // Set servo to half-speed, counterclockwise
    delay(2000);              // Run for 2 seconds
    myservo.write(90);        // Stop servo
}

// Reads the scales and populates Load Cell A and B
void readFoodScale(float &reading) {
    LoadCell_1.update();                      // This built-in function updates the value of LoadCell_1 and returns TRUE if value is new  
    reading = LoadCell_1.getData();           // This built-in function gets the float value produced by load cell
}

void readWaterScale(float &reading) {
  LoadCell_2.update();
  reading = LoadCell_2.getData();
}

// Prints the value that was just weighed (value)
void printScale (float value) {
    Serial.print("Load cell output val: ");
    Serial.println(value);
}

// Checks if food was eaten and updates monitors
void foodOperations(void) {
    if (didPetEat(tempFood, foodReading)) foodUpdate(tempFood - foodReading); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Checks if food was eaten and updates monitors
void waterOperations(void) {
    if (didPetDrink(tempWater, waterReading)) waterUpdate(tempWater - waterReading); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Did the pet eat? Only returns true for significant scale changes
bool didPetEat(float first, float second) {
    if ((first > 1) && (second > 1)) {  // We don't want to check whent the scales fluctuate while unused 
        if ((((first - second) / first) * 100.0) > 5) {                    // Check if the percentage increase exceeds the threshold of 10%
            return true;                                                     // Second is significantly larger than first
        }
    }
    return false;
}

// Did the pet drink? Only returns true for significant scale changes
bool didPetDrink(float first, float second) {
    if ((first > 10) && (second > 10)) {  // We don't want to check whent the scales fluctuate while unused 
        if (((((first - second) / first) * 100.0) > 10) || ((first - second) >= 5)) {                    // Check if the percentage increase exceeds the threshold of 10%
            return true;                                                     // Second is significantly larger than first
        }
    }
    return false;
}

// Add to the total amount eaten
void foodUpdate(float amountEaten) {
    foodEaten += amountEaten;          // Add the amount eaten to foodEaten monitor
}

// Add to the total amount drank
void waterUpdate(float amountDrank) {
    waterDrank += amountDrank;          // Add the amount eaten to foodEaten monitor
}

// Reset foodEaten value (probably after the day is over or something)
void resetFoodEaten(void) {
    foodEaten = RESET;
}

// Reset waterEaten value (probably after the day is over or something)
void resetWaterDrank(void) {
    waterDrank = RESET;
}

// Check if the pushbutton is pressed
bool checkButton (void) {
    // LOW indicates a press
    if (digitalRead(buttonPin) == LOW) return true;
    else return false;
}

// Returns current timestamp in seconds
unsigned long currentTime(void) {
    return millis()/1000;
}

// Returns current time + delay for use as a cooldown
unsigned long cooldown (unsigned long delay) {
    return millis()/1000 + delay;
}

// Used to reset ReadingA and ReadingB
void resetReadings(void) {
    
}

// Initializing Servo Motors
void Init_Servo (void) {
    myservo.attach(7); //servo connected to pin 9
    myservo.write(90); //90 is stopped
    //0 is full speed clockwise
    //90 is stopped
    //180 is full speed counterclockwise
}

// Initializing Buttons
void Init_Buttons (void) {
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    ButtonState = 0;  
}

void Init_Serial (void) {
    Serial.begin(9600); delay(10);
    //Serial.println();
    //Serial.println("Starting...");
}

// This function must be called in the setup phase of main file
void setupLoadCells(void) {
    float calibrationValue_1; // calibration value load cell 1
    float calibrationValue_2; // calibration value load cell 2

    calibrationValue_1 = 195.8; // uncomment this if you want to set this value in the sketch
    calibrationValue_2 = 17.05; // uncomment this if you want to set this value in the sketch
    #if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
    #endif
    //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
    //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom

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
        //Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
    }
    if (LoadCell_2.getTareTimeoutFlag()) {
        //Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
    }
    LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
    LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
    //Serial.println("Startup is complete");
}

// Used to zero or null out the scale reading if nothing is on it
void tareOperations (void) {
  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  // Used to check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    //Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    //Serial.println("Tare load cell 2 complete");
  }
}  
