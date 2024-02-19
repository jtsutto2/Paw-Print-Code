#include "PawPrintHeader.h"
#include "mbed.h"
#include <mbed_mktime.h>

/***************************/
// Global Variables //
//*************************//

int FoodEaten = 0; // Sum of all weight unloaded (food consumed) from food scale

/***************************/
// Scale-Related Functions //
//*************************//

//This is the function called from main for all food-related operations
void food(void) {
  int firstReading = readScale();            // Read the initial value of the scale
  printScale(firstReading);                  // Display reading on console
  
  delay(500);                                // Wait 500ms
  
  int secondReading = readScale();           // Read again for the second value
  printScale(secondReading);                 // Display reading on console

  if (didPetEat(firstReading, secondReading)) foodUpdate(secondReading - firstReading); // If there is a decrease in scale weight (food was eaten), make necessary updates
}

// Reads and returns value from scale
int readScale(void) {
  return analogRead(FoodPin);                 // Read scale and store value, must make sure of FoodPin 
}

// Prints the value that was just weighed (value)
void printScale (int value) {
  Serial.println("Food level: ");
  Serial.println(value);                     // Output value
  
  Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
}

// Did the pet eat?
bool didPetEat(int first, int second) {
  if (second > first) return TRUE;
  else return FALSE;
}

// Add to the total amount eaten
void foodUpdate(int amountEaten) {
  FoodEaten += amountEaten;                  // Update the amount of Food Eaten
  
  Serial.println("Your pet has eaten ");
  Serial.println(FoodEaten);                 // Outputs value of Food Eaten
  Serial.println("total grams of food.");

  Serial.println("getLocalTime()");          // Print current timestamp for troubleshooting purposes
}

// Reset FoodEaten value (probably after the day is over or something)
void resetFoodEaten(void) {
  FoodEaten = 0;
}

void setupEEPROM(void) {

}
