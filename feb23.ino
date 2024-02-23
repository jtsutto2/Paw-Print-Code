#include "feb23.h"
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
    Init_Load_Cells();       // Initialize load cells
    delay(500);
    Init_Buttons();          // Initialize button
    delay(500);
    Init_Servo();            // Initialize servo motors
    delay(500);
}

void loop() {
    if (checkButton()) {                      // Check if button is pressed
        if (currentTime() >= foodCooldown) {  // Check if dispensing is still on cooldown
            foodCooldown = cooldown(0.5);    // We will dispense, set the cooldown
            dispenseFood();                   // Dispense the food
        }
        // Dispenser is still on cooldown
        else Serial.println("Food has been dispensed less than 1 second ago.");
    }
    // Currently, scale readings are only performed while button is unpressed
    else if (currentTime() >= readingCooldown) {
        readingCooldown = cooldown(0.5);     // Only initiate readings every 500ms
        readScales();                        // Read and update scale values
        foodOperations();                    // Check if pet ate
    }
    if (currentTime() >= foodEatenCooldown) {        // If it is time to reset FoodEaten monitor
        foodEatenCooldown = cooldown(864000000);     // Set cooldown to 24 hours
        resetFoodEaten();                            // Sets foodEaten back to zero
    }

    // Troubleshooting:
    if (currentTime() >= myCooldown) {
        myCooldown = cooldown(1); 

        Serial.print("Your pet has eaten ");
        Serial.print(foodEaten);                   // Prints value of foodEaten
        Serial.print(" total grams of food.\n");

        if (flag1) Serial.println("A difference was detected");
        else Serial.println("No difference detected");
    }
}

// Runs the motors to dispense food
void dispenseFood(void) {
    myservo.write(135);       // Set servo to half-speed, counterclockwise
    delay(2000);              // Run for 2 seconds
    myservo.write(90);        // Stop servo
}

// Checks if food was eaten and updates monitors
void foodOperations(void) {
    if (didPetEat(firstReadingA, secondReadingA)) foodUpdate(secondReadingA - firstReadingA); // If there is a decrease in scale weight (food was eaten), make necessary updates
    if (didPetEat(firstReadingB, secondReadingB)) foodUpdate(secondReadingB - firstReadingB); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Prints the value that was just weighed (value)
void printScale (float value) {
    Serial.print("Load cell output val: ");
    Serial.println(value);
}

// Did the pet eat? Only returns true for significant scale changes
bool didPetEat(float first, float second) {
    if ((((second - first) / first) * 100.0) > 50) { // Check if the percentage increase exceeds the threshold of 10%
        flag1 = true;
        return true;                                 // Second is significantly larger than first
    }
    else return false;
}

// Add to the total amount eaten
void foodUpdate(float amountEaten) {
    foodEaten += amountEaten;          // Add the amount eaten to foodEaten monitor
}

// Reset foodEaten value (probably after the day is over or something)
void resetFoodEaten(void) {
    foodEaten = RESET;
}

// This function must be called in the setup phase of main file
void setupLoadCells(void) {
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

void Init_Serial (void) {
    Serial.begin(9600); delay(10);
    Serial.println();
    Serial.println("Starting...");
}

void Init_Load_Cells (void) {
    LoadCell_1.begin();
    //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
    //float calibrationValue_1; // calibration value (see example file "Calibration.ino")
    float calibrationValue_1 = 696.0;
    #if defined(ESP8266)|| defined(ESP32)
    EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
    #endif
    EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the calibration value from eeprom
    unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    LoadCell_1.start(stabilizingtime, _tare);
    if (LoadCell_1.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
        while (1);
    }
    else {
        LoadCell_1.setCalFactor(calibrationValue_1); // set calibration value (float)
        Serial.println("Startup is complete");
    }
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

// Check if the pushbutton is pressed
bool checkButton (void) {
    // LOW indicates a press
    if (digitalRead(buttonPin) == LOW) return true;
    else return false;
}

// Simply returns current timestamp in seconds
unsigned long currentTime(void) {
    return millis()/1000;
}

// Simply returns current time + delay for use as a cooldown
unsigned long cooldown (unsigned long delay) {
    return millis()/1000 + delay;
}

// Reads the scales and populates Load Cell A and B
void readScales(void) {

    Serial.println("hello");

    LoadCell_1.update(); // This built-in function updates the value of LoadCell_1 and returns TRUE if value is new
    LoadCell_2.update();
      
    firstReadingA = LoadCell_1.getData(); // This built-in function gets the float value produced by load cell
    printScale(firstReadingA);

    firstReadingB = LoadCell_2.getData();
    printScale(firstReadingB);

    delay(500); // 500ms delay between readings
      
    LoadCell_1.update(); 
    LoadCell_2.update(); 

    secondReadingA = LoadCell_1.getData();
    //printScale(secondReadingA);

    secondReadingB = LoadCell_2.getData();
    //printScale(secondReadingB);

    tareOperations();
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

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }
}
