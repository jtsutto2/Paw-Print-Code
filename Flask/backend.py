from flask import Flask, request, jsonify, render_template
from datetime import datetime
import csv
import sys
import os

# Expand the '~' to the full path of the home directory
module_path = os.path.expanduser('~/Documents/GitHub/Paw-Print-Code/modules')
sys.path.append(module_path)

from calculation_module import dailyGoal

# Handling CORS in Flask
from flask_cors import CORS

# Assuming you have a separate module for specific calculations
#import calculation_module

app = Flask(__name__)
CORS(app)

# Global dictionary to store user data
users = {}
newUser = None

# Variables to store user input data
user_data = {
    'pet_type': None,
    'weight': None,
    # Add more as needed
}

# API route for processing and returning calculated data
@app.route('/api/data', methods=['GET'])
def get_processed_data():
    # Example function call to process data; implement your logic in calculation_module
    processed_data = calculation_module.process_data(user_data, read_csv_data())
    
    # Return processed data as JSON
    return jsonify(processed_data)

# Route that receives REGISTRATION data from frontend
@app.route('/register', methods=['POST'])
def register():
    if request.form:
        global newUser
        newUser = request.form.get('username')
        username = request.form.get('username')
        password = request.form.get('password')  # Remember, storing passwords like this is not secure
        pet_name = request.form.get('petName')
        pet_current_weight = request.form.get('petCurrentWeight')
        pet_target_weight = request.form.get('petTargetWeight')

        food_goal, water_goal = dailyGoal(pet_current_weight, pet_target_weight)

        # Store user data
        users[username] = {
            'password': password,
            'pet_name': pet_name,
            'pet_current_weight': pet_current_weight,
            'pet_target_weight': pet_target_weight,
            'food_goal': food_goal,
            'water_goal': water_goal
        }

        print("Received registration data:")
        print("Username:", username)
        print("Password:", password)
        print("Pet Name:", pet_name)
        print("Pet Current Weight:", pet_current_weight)
        print("Pet Target Weight:", pet_target_weight)

        print("Calculated goal data:")
        print("Daily Food Intake:", food_goal)
        print("Daily Water Intake:", water_goal)

        print("Sending response with 200 status")

        return jsonify({
            'username': username,
            'password': password,
            'pet_name': pet_name,
            'pet_current_weight': pet_current_weight,
            'pet_target_weight': pet_target_weight
        }), 200
    else:
        return jsonify({'error': 'No form data received'}), 400
    
@app.route('/chart_endpoint')
def chart_data():
    try:
        pet_data = read_csv_data()
        return jsonify(petData=pet_data)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# Route that handles login
@app.route('/login_endpoint', methods=['POST'])
def login():
    username = request.form.get('authUsername')
    password = request.form.get('authPassword')

    # Check if username exists and password matches
    if username in users and users[username]['password'] == password:
        print("Sending response with 200 status")
        return jsonify({'message': 'Login successful'}), 200
    else:
        return jsonify({'error': 'Invalid username or password'}), 401

@app.route('/calculate_intake', methods=['GET'])
def calculate_intake():
        foodGoal = users[newUser]['food_goal']
        waterGoal = users[newUser]['water_goal']
        message = (f"Your furry friend should consume {foodGoal:.2f} grams of food and {waterGoal:.2f} mL of water everyday to stay healthy and meet your goals!")
        return jsonify(message=message), 200

def read_csv_data():
    csv_data = []
    csv_file_path = '/home/pawprint/Documents/csv_output/data_log.csv'

    try:
        with open(csv_file_path, mode='r') as csv_file:
            csv_reader = csv.DictReader(csv_file)
            initial_time = None  # To store the first timestamp

            for row in csv_reader:
                # Parse the current timestamp
                current_time = datetime.strptime(row['Time (EST)'], '%H:%M:%S')

                if initial_time is None:
                    initial_time = current_time
                    base_seconds = initial_time.hour * 3600 + initial_time.minute * 60 + initial_time.second

                current_seconds = current_time.hour * 3600 + current_time.minute * 60 + current_time.second
                seconds_since_first = current_seconds - base_seconds

                csv_data.append({
                    'timeInSeconds': int(seconds_since_first),
                    'foodEaten': float(row['foodEaten']),
                    'waterDrank': float(row['waterDrank'])
                })

    except FileNotFoundError:
        print("Error: File not found. Please check the file path.")
        return None
    except PermissionError:
        print("Error: Permission denied when trying to read the file.")
        return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None

    return csv_data

# Route for the main page and handling user input form
@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        # Processing form data and updating user_data dictionary
        user_data['pet_type'] = request.form.get('petType')
        user_data['weight'] = request.form.get('weight')
        # Process and store additional form inputs as needed
        
        # Optionally, redirect to another page after POST
        return render_template('success.html')  # Assuming you have a template for successful form submission
    return render_template('index.html')  # Show the input form
# Placeholder for calculation_module functionality
# Implement this module based on your project's specific requirements
# This could include functions for processing data, performing calculations based on user inputs and CSV data, etc.

if __name__ == '__main__':
    app.run(debug=True, threaded=True, host='0.0.0.0', port=5000)
