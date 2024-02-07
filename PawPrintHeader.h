#include "RTC.h"

//values
#define TRUE 1
#define FALSE 0

RTCTime currentTime; //current time variable
  
//sensor (analog???) pins
const int FoodPin 9; //analog GPIO pin 9
const int WaterPin 10; //analog GPIO pin 10

//motor pins
const int directionPin = 12; //Digital GPIO D6
const int pwmPin = 3; //AREF for PWM
const int brakePin = 9; //Digital GPIO D9

//function defines
void ScaleValue(void);
void RunButton(void);
void WaterValue(void);
void PictureProcess(void);
