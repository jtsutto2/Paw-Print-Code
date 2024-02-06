#include "PawPrintHeader.h"

void setup() { //runs once at beginning

  int ScalesReady = FALSE;
  int ButtonReady = FALSE;

  pinMode(directionPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);

  Serial.begin(9600); //baud rate 9600
  Serial.println("<Arduino is ready>");

}

void loop() { //runs constantly

  //Keya do this part
  if (UserPromptNeeded == TRUE) {
    //send first question
    //wait for response
    //accept response and save to variable
    //ask other questions and wait for responses
    //do math with variables to tell them what health needs their pet has
    UserPromptNeeded = FALSE; //ends this section
  }
  
  if (ScalesReady == TRUE) { //poll device Scale
    ScaleValue(); //run scale
    WaterValue(); //run other scale
  }

  if (ButtonReady == TRUE) { //poll touch sensor
    RunButton(); //dispense food
  }

  if (ScalesReady == FALSE) { //wait 2s then let system check scale
    delay(2000); 
    ScalesReady = TRUE;
  }

  if (ButtonReady == FALSE) {
    //do something
  }

}

void ScaleValue(void) {
  int ScaleLast, ScaleCurrent, FoodEaten;
  int FoodMax = 9999;
  int FoodUpdate = FALSE;

  ScaleLast = analogRead(FoodPin); //read scale
  Serial.println("Food level is being read: ");
  Serial.println(ScaleLast); //output value
  delay(500); //wait 500ms
  ScaleCurrent = analogRead(FoodPin); //read again
  Serial.println("Food level is being read: ");
  Serial.println(ScaleCurrent); //output value

  if (FoodEaten >= FoodMax) {
    FoodEaten = 0; //empty amount of food eaten (implies refill of container)
  if (ScaleLast != ScaleCurrent) {
    FoodEaten += ScaleCurrent - ScaleLast; //update the amount of Food Eaten
    FoodUpdate = TRUE;
    }
  }

  if (FoodUpdate == TRUE) {
    Serial.println("Your pet has eaten ");
    Serial.println(FoodEaten); //outputs value of Food Eaten
    Serial.println("total grams of food.");
    FoodUpdate = FALSE;
  }

}

void WaterValue(void) {
  int WaterLast, WaterCurrent, WaterDrank;
  int WaterMax = 9999;
  int WaterUpdate = FALSE;

  WaterLast = analogRead(WaterPin); //read water level
  Serial.println("Water level is being read");
  delay(500); //wait 500ms
  WaterCurrent = analogRead(WaterPin); //read again
  Serial.println("Water level is being read");

  if (WaterDrank >= WaterMax) {
    WaterDrank = 0; //empty amount of water drank (implies refill of container)
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

void RunButton(void) {
  //motor control
  digitalWrite(directionPin, HIGH); //set direction to high
  digitalWrite(brakePin, LOW); //release brake
  analogWrite(pwmPin, 50); //set pwm to 50% duty cycle (medium speed)

  delay(2000); //run for 2 seconds

  digitalWrite(brakePin, HIGH); //activate brake
  analogWrite(pwmPin, 0); //set pwm to 0% (off)
  delay(500); //cooldowm 0.5 seconds

  //take picture

  PictureProcess(); //reconscruct and send picture
}

void PictureProcess(void) {
  //code to reconstruct picture

  //send serial out
}
