import numpy as np

def main():

  print("Enter your pets name and then press enter:")
  petName = input()
  
  print("Please enter " + petName + "'s weight in pounds")
  petWeight = input()

  print("Is your pet underweight, at weight, or overweight. Please enter only one")
  dietString = input()
  if dietString.casefold() == 'underweight': #not case sensitive
    dietFactor = 1.25 #needs more food if underweight
  else if dietString.casefold() == 'at weight':
    dietFactor = 1.0 #keep food amount normal
  else if dietString.casefold() == 'overweight':
    dietFactor 0.75 #needs less food if overweight
  else:
    print("Error: please enter only underweight, at weight, or overweight")

  foodGoal = petWeight * dietFactor * 27
  print("Your furry friend needs: " + foodGoal + "g of food")
  waterGoal = petWeight * dietFactor * 30
  print("Your furry friend needs: " + waterGoal + "mL of water")

  
  
  return

def consumptionProgress:
  foodProgress = (foodEaten / foodGoal) * 100;
  waterProgress = (waterDrank / waterGoal) * 100;
  print()
