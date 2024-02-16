#include "PawPrintHeader.h"
#include "mbed.h"
#include <mbed_mktime.h>

void WaterValue(void) {
  int WaterLast, WaterCurrent, WaterDrank, WaterReset;
  int WaterMax = 9999;
  int WaterUpdate = FALSE;

  WaterLast = analogRead(WaterPin); //read water level
  Serial.println("Water level is being read");
  Serial.println("getLocalTime()"); //print current time
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
