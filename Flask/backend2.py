from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/submit_pet_info', methods=['POST'])
def submit_pet_info():
    # Process pet information received from the frontend
    pet_name = request.form['petName']
    # Process other fields similarly

    # Example: Generate some dummy data for visualization
    processed_data = {
        'pet1': {'age': 5},
        'pet2': {'age': 3},
        'pet3': {'age': 7}
    }

    return jsonify(processed_data)

if __name__ == '__main__':
    app.run(debug=True)
