import datetime
import matplotlib as mp

def main():

  print("Enter your pets name and then press enter:")
  petName = input()
  
  print("Please enter " + petName + "'s weight in pounds")
  petWeight = input()

  #maybe add an option to manually enter a diet based on vet reccomendation

  print("What is " + petName + "'s target weight?")
  targetWeight = input()
  dietfactor = targetWeight/petWeight #returns correction factor in diet relative to target weight

  foodGoal = petWeight * dietFactor * 27 #daily food goal adjusted for dietary needs
  print(petName + " needs: " + foodGoal + "g of food")
  waterGoal = petWeight * 30 #water should be based on current weight not target 
  print(petName + " needs: " + waterGoal + "mL of water")

  consumptionProgress()
  
  return

def consumptionProgress():
  foodProgress = (foodEaten / foodGoal) * 100; #create percent of goal
  waterProgress = (waterDrank / waterGoal) * 100;
  print(petName + " has eaten " + foodProgress + "% of their daily food goal as of")
  print current_time()
  print(petName + " has drank " + waterProgress + "% of their daily water goal as of")
  print current_time()

  goalDict1 = { #creates a dictionary with the date: true/false format
    'date': 'true/false'
  } #this one is for food

  goalDict2 = { #creates a dictionary with the date: true/false format
    'date': 'true/false'
  } #this one is for water

  if foodProgress >= 100:
    foodGoalMet = current_date()
    goalDict1[foodGoalMet] = 'true' #adds todays date with value true
  else:
    foodGoalMet = current_date()
    goalDict1[foodGoalMet] = 'false' #adds todays date with value false

  if waterProgress >= 100:
    waterGoalMet = current_date()
    goalDict2[waterGoalMet] = 'true' #adds todays date with value true
  else:
    waterGoalMet = current_date()
    goalDict2[waterGoalMet] = 'false' #adds todays date with value false

  print("Would you like to see your goals to date?")
  yesorno = input()
  if yesorno.casefold() == 'yes': #not case sensitive
    print(goalDict) #prints dates and if goal was met or not, will update to graph later
  else if yesorno.casefold() == 'no':
    continue
  
  return

def current_time():
  return datetime.now().strftime('%m-%d-%Y %H:%M:%S') #creates string with MM/DD/YYYY and HR:Min:sec

def current_date():
  return datetime.now().strftime('%m-%d-%Y') #creates string with MM/DD/YYYY
