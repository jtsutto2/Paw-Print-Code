import datetime
import matplotlib as mp

def main():

  print("Enter your pets name and then press enter:")
  petName = input()
  
  print("Please enter " + petName + "'s weight in pounds. Ex. 25.7")
  petWeight = input()

  #maybe add an option to manually enter a diet based on vet reccomendation

  print("What is " + petName + "'s target weight in pounds? Ex. 20.0")
  targetWeight = input()
  dietfactor = targetWeight/petWeight #returns correction factor in diet relative to target weight
  #if targetWeight < petWeight then dietFactor < 1 to indicate less food

  foodGoal = petWeight * dietFactor * 27 #daily food goal adjusted for dietary needs
  print(petName + " needs: " + foodGoal + "g of food")
  waterGoal = petWeight * 30 #water should be based on current weight not target 
  print(petName + " needs: " + waterGoal + "mL of water")

  while True:
    if consumptionTrigger % 1000 == 0: #makes this run less often
      consumptionProgress()
    else:
      consumptionTrigger += 1 #increase count
  
  return

def consumptionProgress():
  foodProgress = (foodEaten / foodGoal) * 100; #create percent of goal
  waterProgress = (waterDrank / waterGoal) * 100;
  print(petName + " has eaten " + foodProgress + "% of their daily food goal as of")
  print current_time()
  print(petName + " has drank " + waterProgress + "% of their daily water goal as of")
  print current_time()

  goalDict1 = { #creates a dictionary with the date: true/false format
    'Date MM/DD/YYYY': 'Goal met? (true/false)'
  } #this one is for food

  goalDict2 = { #creates a dictionary with the date: true/false format
    'Date MM/DD/YYYY': 'Goal met? (true/false)'
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
    #does not allow duplicates so will update for todays date if one already exists

  print("Would you like to see your goals to date?")
  yesorno = input()
  if yesorno.casefold() == 'yes': #not case sensitive
    print(goalDict1) #prints dates and if goal was met or not, will update to graph later
    print(goalDict2)
  else if yesorno.casefold() == 'no':
    continue
  
  return

def current_time():
  return datetime.now().strftime('%m-%d-%Y %H:%M:%S') #creates string with MM/DD/YYYY and HR:Min:sec

def current_date():
  return datetime.now().strftime('%m-%d-%Y') #creates string with MM/DD/YYYY
