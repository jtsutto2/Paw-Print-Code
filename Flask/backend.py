from flask import Flask, request, jsonify, render_template
import csv

# Assuming you have a separate module for specific calculations
import calculation_module

app = Flask(__name__)

# Variables to store user input data
user_data = {
    'pet_type': None,
    'weight': None,
    # Add more as needed
}

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

# API route for processing and returning calculated data
@app.route('/api/data', methods=['GET'])
def get_processed_data():
    # Example function call to process data; implement your logic in calculation_module
    processed_data = calculation_module.process_data(user_data, read_csv_data())
    
    # Return processed data as JSON
    return jsonify(processed_data)

@app.route('/submit_pet_info', methods=['POST'])
def submit_pet_info():
    data = request.get_json()
    # Assuming you want to update your user_data with this JSON data
    # Remember, user_data might need to be more dynamic depending on your application's requirements
    user_data['pet_type'] = data.get('petName')  # You might need to adjust the key based on your JS
    user_data['weight'] = data.get('petCurrentWeight')  # Adjust the key as necessary

    # Process your data here
    # For example, call a function from calculation_module to process this data

    # For demonstration, let's just return the received JSON
    return jsonify({'status': 'success', 'receivedData': data})

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

# Placeholder for calculation_module functionality
# Implement this module based on your project's specific requirements
# This could include functions for processing data, performing calculations based on user inputs and CSV data, etc.

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
