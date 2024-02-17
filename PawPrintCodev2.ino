#include "PawPrintHeader.h"

//GLOBAL VARIABLES
int FoodEaten, FoodReset;
int WaterDrank, WaterReset;
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
    
    else { //if button isn't pressed, read scales
      if((millis() + 200) % 200 < 100) {
        food(); //triggers on 0.2s, 0.4s, etc.
      } 
      else if ((millis() + 200) % 200 >= 100) {
        WaterValue(); //triggers on 0.3s, 0.5s, etc.
        }
      }
    }
}

void DispenseFood() {
  //motor control
  digitalWrite(directionPin, HIGH); //set direction to high
  digitalWrite(brakePin, LOW); //release brake
  analogWrite(pwmPin, 50); //set pwm to 50% duty cycle (medium speed)

  delay(2000); //run for 2 seconds

  digitalWrite(brakePin, HIGH); //activate brake
  analogWrite(pwmPin, 0); //set pwm to 0% (off)
  delay(500); //cooldowm 0.5 seconds
}

void WaterValue(void) {
  int WaterLast, WaterCurrent,
  int WaterMax = 9999;
  int WaterUpdate = FALSE;

  WaterLast = analogRead(WaterPin); //read water level
  Serial.println("Water level is being read");
  //Serial.println("getLocalTime()"); //print current time
  delay(500); //wait 500ms
  WaterCurrent = analogRead(WaterPin); //read again
  Serial.println("Water level is being read");

  if (WaterReset == TRUE) { //if reset command is given
    WaterDrank = 0; //reset water
  }
  else if (WaterDrank >= WaterMax) {
    Serial.println("ERROR: WaterDrank > WaterMax");
  if (WaterLast != WaterCurrent) {
    WaterDrank += WaterCurrent - WaterLast; //update the amount of water drank
    WaterUpdate = TRUE;
    }
  }

  if (WaterUpdate == TRUE) {
    Serial.println("Your pet has eaten ");
    Serial.println(WaterDrank); //outputs value of water drank
    Serial.println("total grams of food.");
    WaterUpdate = FALSE;
  }
}
