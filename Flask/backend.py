from flask import Flask, request, jsonify, render_template
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

# Function to read and parse data_log.csv
def read_csv_data():
    csv_data = []
    # Use the absolute path to the CSV file
    csv_file_path = '/home/pawprint/Documents/csv_output/data_log.csv'
    with open(csv_file_path, mode='r') as csv_file:
        csv_reader = csv.DictReader(csv_file)
        for row in csv_reader:
            csv_data.append(row)
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
