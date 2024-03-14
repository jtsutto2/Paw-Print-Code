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
    setupLoadCells();        // Initialize load cells
    delay(500);
    Init_Buttons();          // Initialize button
    delay(500);
    Init_Servo();            // Initialize servo motors
    delay(500);
    tareOperations();        // Null or zero out cell data while nothing is on them
    delay(500);

    Serial.println("All components initialized.");
    Serial.println("Startup is complete.\n");

    Serial.println("Please enter your pet's weight in pounds:");
    while (1) {
        // Check if data is available on the serial port
        if (Serial.available() > 0) {
            petWeight = Serial.parseFloat();                  // Parse the input as a float

            // Print the received weight for confirmation
            Serial.print("Received dog weight: ");
            Serial.print(petWeight);
            Serial.println(" lb.");

            dailyFoodAmount = howMuchToEat(petWeight);        // Calculate total grams pet should eat per day
            dailyWaterAmount = howMuchToDrink(petWeight);     // Calculate total grams pet should drink every day

            // Print the calculate daily food amount
            Serial.print("Your furry friend needs: ");
            Serial.print(dailyFoodAmount);
            Serial.print(" grams of food and ");
            Serial.print(dailyWaterAmount);
            Serial.println(" milliliters of water per day.\n");
            break;
        }
    }
}

void loop() {
    // The following detects if button is pressed, and if it is, it will trigger a food dispense
    // There is a cooldown between presses to take effect
    if (checkButton()) {                      // Check if button is pressed
        if (currentTime() >= foodCooldown) {  // Check if dispensing is still on cooldown
            foodCooldown = cooldown(1);       // We will dispense, set the cooldown
            dispenseFood();                   // Dispense the food
        }
        // Dispenser is still on cooldown
        else Serial.println("Food has been dispensed less than 1 second ago.\n");
    }

    // The following reads the scales in cycles and uses that information to track dietary trends
    if (currentTime() >= readingCooldown) {          // Readings will only run every 200ms
        if (isFirstReading) {                        // Alternates between first and second readings every 200ms
            readFoodScale(firstReadingA);            // Populates firstReadingA as initial food reading
            readWaterScale(firstReadingB);           // Populates firstReadingB as initial water reading

            //printScale(firstReadingB);
        }
        else {                                       // This only executes if first readings have already been taken
            readFoodScale(secondReadingA);
            readWaterScale(secondReadingB);
        }

        isFirstReading = !isFirstReading;            // Mechanism by which readings alternate (every 200ms it is one or the other)

        foodOperations();                            // Check and update if pet ate
        waterOperations();                           // Check and update if pet drank

        readingCooldown = cooldown(0.2);             // Reset cooldown after each reading

        cycleCount++;                                // We must reset our readings every 2 cycles to avoid comparing an old 2nd reading with a new 1st reading

        if (cycleCount >= 2) {                       
            isFirstReading = true;                   // Might be redundant
            cycleCount = RESET;                      // Reset cycle count

            resetReadings();                         // Sets all readings to 0
        }
    }

    // The following resets data captures after the day (24 hours) is over
    if (currentTime() >= foodEatenCooldown) {        // If it is time to reset FoodEaten monitor
        foodEatenCooldown = cooldown(86400);         // Set cooldown to 24 hours
        resetFoodEaten();                            // Sets foodEaten back to zero
    }

    // The following will frequently print out pet's daily consumption progress
    if (currentTime() >= consumptionCooldown) {
        consumptionProgress();

        Serial.print("Your pet has reached ");
        Serial.print(foodProgress);
        Serial.print("% of its daily food intake goal and ");
        Serial.print(waterProgress);
        Serial.println("% of its daily water intake goal.\n");

        consumptionCooldown = cooldown(10);
    }

    // Troubleshooting:
    if (petAte) {
        Serial.println("A difference in food was detected");
        Serial.print("Your pet has eaten ");
        Serial.print(foodEaten);                      // Prints value of foodEaten
        Serial.println(" total grams of food.\n");

        petAte = false;
    }

    if (petDrank) {
        Serial.println("A difference in water was detected");
        Serial.print("Your pet has drank ");
        Serial.print(waterDrank);                      // Prints value of waterDrank
        Serial.println(" total grams of water.\n");

        petDrank = false;
    }
}

// Runs the motors to dispense food
void dispenseFood(void) {
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
    if (didPetEat(firstReadingA, secondReadingA)) foodUpdate(firstReadingA - secondReadingA); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Checks if food was eaten and updates monitors
void waterOperations(void) {
    if (didPetDrink(firstReadingB, secondReadingB)) waterUpdate(firstReadingB - secondReadingB); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Did the pet eat? Only returns true for significant scale changes
bool didPetEat(float first, float second) {
  if ((first > 1) && (second > 1)) {  // We don't want to check whent the scales fluctuate while unused 
    if ((((first - second) / first) * 100.0) > 5) {                    // Check if the percentage increase exceeds the threshold of 10%
        petAte = true;
        return true;                                                     // Second is significantly larger than first
    }
    else return false;
  }
  else return false;
}

// Did the pet drink? Only returns true for significant scale changes
bool didPetDrink(float first, float second) {
    if ((first > 10) && (second > 10)) {  // We don't want to check whent the scales fluctuate while unused 
      if (((((first - second) / first) * 100.0) > 10) || ((first - second) >= 5)) {                    // Check if the percentage increase exceeds the threshold of 10%
          petDrank = true;
          return true;                                                     // Second is significantly larger than first
      }
      else return false;
    }
    else return false;
}

// Add to the total amount eaten
void foodUpdate(float amountEaten) {
  //Serial.println(amountEaten);
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
    firstReadingA = RESET;
    secondReadingA = RESET;
    firstReadingB = RESET;
    secondReadingB = RESET;
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
        //Serial.println("Startup is complete");
    }
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
        Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
    }
    if (LoadCell_2.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
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

// Used to calculate how much food in grams a pet should eat daily based on bodyweight in pounds
float howMuchToEat(float weight) {
    // ROUGH APPROXIMATION: A pet should eat around 27grams per 1 pound of bodyweight daily
    return weight * 27;
}

// Used to calculate how much water in milliliters a pet should drink daily based on bodyweight in pounds
float howMuchToDrink(float weight) {
    // ROUGH APPROXIMATION: A pet should drink around 30milliliters per 1 pound of bodyweight daily
    return weight * 30;
}

float consumptionProgress(void) {
    foodProgress = (foodEaten / dailyFoodAmount) * 100;
    waterProgress = (waterDrank / dailyFoodAmount) * 100;
}
