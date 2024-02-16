#include "PawPrintHeader.h"
#include "mbed.h"
#include <mbed_mktime.h>


void ScaleValue(void) {
  int ScaleLast, ScaleCurrent, FoodEaten, FoodReset;
  int FoodMax = 9999;
  int FoodUpdate = FALSE;

  
  ScaleLast = analogRead(FoodPin); //read scale
  Serial.println("Food level is being read: ");
  Serial.println(ScaleLast); //output value
  Serial.println("getLocalTime()"); //print current time
  delay(500); //wait 500ms
  ScaleCurrent = analogRead(FoodPin); //read again
  Serial.println("Food level is being read: ");
  Serial.println(ScaleCurrent); //output value

  if (FoodReset == TRUE) { //if reset command is given
    FoodEaten = 0; //reset FoodEaten
  }
  else if (FoodEaten >= FoodMax) {
    Serial.println("ERROR: FoodEaten > FoodMax");
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
