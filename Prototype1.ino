#include "PawPrintHeader.h"
#include <Servo.h>
//#include "mbed.h"
//#include <mbed_mktime.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

/***************************/
// Function Declarations //
//*************************//
void printScale (float value);

/***************************/
// Global Variables //
//*************************//
int WaterDrank, WaterReset;
int ButtonState;         // variable for reading the pushbutton status
float cooldownTime, readingCooldown, currentTime;

//pins:
const int HX711_dout_1 = 4; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 5; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 6; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 7; //mcu > HX711 no 2 sck pin
float firstReadingA, firstReadingB, secondReadingA, secondReadingB;



//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
unsigned long t = 0;

float FoodEaten = 0; // Sum of all weight unloaded (food consumed) from food scale

//Servo Objects and fields
Servo myservo;
int speed;

void setup() {
  //Load Cell Side
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell_1.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  //float calibrationValue_1; // calibration value (see example file "Calibration.ino")
  float calibrationValue_1 = 696.0; // uncomment this if you want to set the calibration value in the sketch
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

//Motor Side

   // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  ButtonState = 0;  

  myservo.attach(7); //servo connected to pin 9
  myservo.write(90); //90 is stopped
  //0 is full speed clockwise
  //90 is stopped
  //180 is full speed counterclockwise
  
}

void loop() {
  ButtonState = digitalRead(buttonPin); //check button

  // check if the pushbutton is pressed.
  // if it is, the buttonState is LOW:
  if (ButtonState == LOW) { //check for button press
    if (millis()/1000 >= cooldownTime) { //if 60s have passed since last food
      DispenseFood(); //dispense function
    }
    else {
      Serial.println("Food has been dispensed less than 60 seconds ago."); //triggers if cooldown hasn't been met
    }
  } else { //if button isn't pressed, read scales
      currentTime = millis()/1000;
      if(currentTime >= readingCooldown) {
        food(); //triggers on 0.2s, 0.4s, etc.
      } 
      else if ((millis()/100 + 2) % 2 >= 1) {
        //WaterValue(); //triggers on 0.3s, 0.5s, etc.
        }
      }
}

void DispenseFood() {
  
  //motor control
  myservo.write(135); //half speed counterclockwise
  delay(2000); //run for 2 seconds
  myservo.write(90); //stop servo

  cooldownTime = millis()/1000 + 0.5; //set 60 second cooldown
}

// This is the function called from main for all food-related operations
void food(void) {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0; //increase value to slow down serial print activity    

    /*float firstReadingA = 0.0;
    float firstReadingB = 0.0;
    float secondReadingA = 0.0;
    float secondReadingB = 0.0;*/

  // Reading the scales and populating Load Cell A + B:
    
      LoadCell_1.update(); // This built-in function updates the value of LoadCell_1 and returns TRUE if value is new
      
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

      t = millis();
    

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

  readingCooldown = millis()/1000 + 500;

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
  
  //Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
}

// Did the pet eat?
bool didPetEat(float first, float second) {
    // Calculate the percentage increase
    float percentageIncrease = ((second - first) / first) * 100.0;

    Serial.println("Difference is: ");
    Serial.println(second - first);

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
  
  //Serial.println("Your pet has eaten ");
  //Serial.println(FoodEaten);                 // Outputs value of Food Eaten
  //Serial.println("total grams of food.");

  //Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
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
