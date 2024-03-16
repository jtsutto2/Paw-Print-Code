import datetime
import matplotlib as mp

def main():

  print("Enter your pets name and then press enter:")
  petName = input()
  
  print("Please enter " + petName + "'s weight in pounds")
  petWeight = input()

  print("What is " + petName + "'s target weight?")
  targetWeight = input()
  dietfactor = petWeight/targetWeight #returns correction factor in diet relative to target weight

  foodGoal = petWeight * dietFactor * 27 #daily food goal adjusted for dietary needs
  print(petName + " needs: " + foodGoal + "g of food")
  waterGoal = petWeight * 30 #water should be based on current weight not target 
  print(petName + " needs: " + waterGoal + "mL of water")

  consumptionProgress()
  
  return

def consumptionProgress():
  foodProgress = (foodEaten / foodGoal) * 100;
  waterProgress = (waterDrank / waterGoal) * 100;
  print(petName + " has eaten " + foodProgress + "% of their daily food goal as of")
  print current_time()
  print(petName + " has drank " + waterProgress + "% of their daily water goal as of")
  print current_time()

  goalDict = { #creates a dictionary with the date: true/false format
    'date': 'true/false'
  }

  if foodProgress >= 100:
    foodGoalMet = current_time()
    goalDict[foodGoalMet] = 'true' #adds todays date with value true
  else:
    foodGoalMet = current_time()
    goalDict[foodGoalMet] = 'false' #adds todays date with value false

  if waterProgress >= 100:
    waterGoalMet = current_time()
    goalDict[waterGoalMet] = 'true' #adds todays date with value true
  else:
    waterGoalMet = current_time()
    goalDict[waterGoalMet] = 'false' #adds todays date with value false

  print("Would you like to see your goals to date?")
  yesorno = input()
  if yesorno.casefold() == 'yes': #not case sensitive
    print(goalDict) #prints dates and if goal was met or not, will update to graph later
  else if yesorno.casefold() == 'no':
    continue
  
  return

def current_time():
  return datetime.now().strftime('%m-%d-%Y %H:%M:%S') #creates string with MM/DD/YY and HR:Min:sec
