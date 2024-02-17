#include "PawPrintHeader.h"

//GLOBAL VARIABLES
int FoodEaten;
int WaterDrank;
int ButtonState;
int cooldownTime;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  pinMode(directionPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);
  
}

void loop() {
  
  if (ButtonState == LOW) { //check for button press
    if (millis() >= cooldownTime) { //if 60s have passed since last food
      DispenseFood(); //dispense function
    }
    else {
      Serial.println("Food has been dispensed less than 60 seconds ago."); //triggers if cooldown hasn't been met
    }
    else {
      switch((millis() + 200) % 200) {
        case 0: ScaleValue(); //triggers on 0.2s, 0.4s, etc.
        break;
        case 100: WaterValue(); //triggers on 0.3s, 0.5s, etc.
        break;
        default: break; //on all other values do nothing
      }
    }
  }
}
