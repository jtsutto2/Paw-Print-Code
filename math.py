import datetime
import matplotlib as mp

def main():

  print("Enter your pets name and then press enter:")
  petName = input()
  
  print("Please enter " + petName + "'s weight in pounds")
  petWeight = input()

  print("What is " + petName + "'s target weight?")
  targetWeight = input()
  dietfactor = petWeight/targetWeight

  foodGoal = petWeight * dietFactor * 27 #daily food goal adjusted for dietary needs
  print("Your furry friend needs: " + foodGoal + "g of food")
  waterGoal = petWeight * 30 #water should be based on current weight not target 
  print("Your furry friend needs: " + waterGoal + "mL of water")

  consumptionProgress()
  
  return

def consumptionProgress():
  foodProgress = (foodEaten / foodGoal) * 100;
  waterProgress = (waterDrank / waterGoal) * 100;
  print(petName + " has eaten " + foodProgress + "% of their daily food goal as of")
  print current_time()
  print(petName + " has drank " + waterProgress + "% of their daily water goal as of")
  print current_time()

  if foodProgress >= 100:
    foodGoalMet = 1

  if waterProgress >= 100:
    waterGoalMet = 1

    
  return

def current_time():
  return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
