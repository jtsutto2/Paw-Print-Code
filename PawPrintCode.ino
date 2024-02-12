#include "PawPrintHeader.h"
#include "mbed.h"
#include <mbed_mktime.h>

void setup() { //runs once at beginning

  pinMode(directionPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);

  Serial.begin(9600); //baud rate 9600
  Serial.println("<Arduino is ready>");

  RTCset();
}

void loop() { //runs constantly

  //Keya do this part
  if (UserPromptNeeded == TRUE) {
    Serial.println("Please answer all questions by typing your answer and hitting Enter/Return to submit.");
    //send first question
    //wait for response
    //accept response and save to variable
    //ask other questions and wait for responses
    //do math with variables to tell them what health needs their pet has
    UserPromptNeeded = 0; //ends this section
  }
  
  if (millis() % 2000 == 0) { //poll touch sensor
      //AskUserButton();
      //RunButton(); //dispense food  
  }

  if (millis() % 3000 == 0) {
    //WaterValue(); //run water scale
  }

  if (millis() % 3500 == 0) { //poll device Scale 
    //ScaleValue(); //run scale
  }


}

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

void RunButton(void) {
  int ButtonPressed;

  if (ButtonPressed == TRUE) {
  //motor control
  digitalWrite(directionPin, HIGH); //set direction to high
  digitalWrite(brakePin, LOW); //release brake
  analogWrite(pwmPin, 50); //set pwm to 50% duty cycle (medium speed)

  delay(2000); //run for 2 seconds

  digitalWrite(brakePin, HIGH); //activate brake
  analogWrite(pwmPin, 0); //set pwm to 0% (off)
  delay(500); //cooldowm 0.5 seconds

  Serial.println("getLocalTime()"); //print current time
  
  //take picture

  PictureProcess(); //reconstruct and send picture
  }
}

void PictureProcess(void) {
  //code to reconstruct picture

  //send serial out
}

void AskUserButton(void) {
  Serial.println("Would you like to run the motor? 1 for yes, 2 for no");

  while (Serial.available() == 0) {
  }

  int menuChoice = Serial.parseInt();

  switch (menuChoice) {
    case 1:
      //run Button
      RunButton();
      break;

    case 2:
      //Do nothing      
      break;
    
    default:
    break;
}
}

//LOOK INTO BATTERY BACKUP FOR RTC
void RTCset()  // Set cpu RTC
{
  /* FIX THIS LATER
  // return the time if a valid sync message is received on the serial port.
  //have this only run once and prompt user for it
  while(Serial.available() >=  TIME_MSG_LEN){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if(c == TIME_HEADER) { //check for T at beginning      
      int pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if(c >= '0' && c <= '9') {   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
  */
  tm t; //fix this to set to pctime
  t.tm_sec = (0);            // 0-59
  t.tm_min = (30);           // 0-59
  t.tm_hour = (11);          // 0-23
  t.tm_mday = (13);          // 1-31
  t.tm_mon = (1);           // 0-11  "0" = Jan, -1
  t.tm_year = ((24) + 100);  // year since 1900,  current year + 100 + 1900 = correct year
  set_time(mktime(&t));      // set RTC clock
    }

 
String getLocalTime() {
  char buffer[32];
  tm t;
  _rtc_localtime(time(NULL), &t, RTC_4_YEAR_LEAP_YEAR_SUPPORT);
  strftime(buffer, 32, "%Y-%m-%d %k:%M:%S", &t);
  return String(buffer);
}
