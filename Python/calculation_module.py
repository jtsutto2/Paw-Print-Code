from datetime import datetime
import time

def calculation():
  petName = 'Kirby'
  targetWeight = 80
  petWeight = 70;
  dietFactor = targetWeight/petWeight #returns correction factor in diet relative to target weight
  #if targetWeight < petWeight then dietFactor < 1 to indicate less food

  foodGoal = petWeight * dietFactor * 27  # daily food goal adjusted for dietary needs
  print(petName + " needs: " + str(foodGoal) + "g of food")
  waterGoal = petWeight * 30  # water should be based on current weight not target
  print(petName + " needs: " + str(waterGoal) + "mL of water")
  
  foodEaten = 0
  waterDrank = 0

  while True:
      consumptionProgress(foodEaten, waterDrank, foodGoal, waterGoal, petName)
      foodEaten += 10
      waterDrank += 10
      time.sleep(5)


def consumptionProgress(foodEaten, waterDrank, foodGoal, waterGoal, petName):
  foodProgress = (foodEaten / foodGoal) * 100; #create percent of goal
  waterProgress = (waterDrank / waterGoal) * 100;
  print(petName + " has eaten {:.2f}% of their daily food goal as of".format(foodProgress))
  print(current_time())
  print(petName + " has drank {:.2f}% of their daily water goal as of".format(waterProgress))
  print(current_time())

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
  
  return

def current_time():
  return datetime.now().strftime('%m-%d-%Y %H:%M:%S') #creates string with MM/DD/YYYY and HR:Min:sec

def current_date():
  return datetime.now().strftime('%m-%d-%Y') #creates string with MM/DD/YYYY
  
calculation()
