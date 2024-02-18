#include "PawPrintHeader.h"
#include <Servo.h>

//GLOBAL VARIABLES
int FoodEaten, FoodReset;
int WaterDrank, WaterReset;
int ButtonState = 0;         // variable for reading the pushbutton status
int cooldownTime;

//Servo Objects and fields
Servo myservo;
int speed;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  myservo.attach(9); //servo connected to pin 9
  myservo.write(90); //90 is stopped
  //0 is full speed clockwise
  //90 is stopped
  //180 is full speed counterclockwise
  
}

void loop() {
  
  if (ButtonState == LOW) { //check for button press
    if (millis() >= cooldownTime) { //if 60s have passed since last food
      DispenseFood(); //dispense function
    }
    else {
      Serial.println("Food has been dispensed less than 60 seconds ago."); //triggers if cooldown hasn't been met
    }
  } else { //if button isn't pressed, read scales
      if((millis() + 200) % 200 < 100) {
        food(); //triggers on 0.2s, 0.4s, etc.
      } 
      else if ((millis() + 200) % 200 >= 100) {
        WaterValue(); //triggers on 0.3s, 0.5s, etc.
        }
      }
}

void DispenseFood() {
  
  //motor control
  myservo.write(135); //half speed counterclockwise
  delay(2000); //run for 2 seconds
  myservo.write(90); //stop servo

  cooldownTime = millis() + 60000; //set 60 second cooldown
    
}
