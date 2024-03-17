import serial
from datetime import datetime
import csv
import time
import os

# Define the path to the CSV file
csv_file_path = 'data_log.csv'

# Check if the file exists and is empty to decide whether to add headers later
file_exists = os.path.isfile(csv_file_path) and os.path.getsize(csv_file_path) > 0

# Open the CSV file to append
with open(csv_file_path, mode='a', newline='') as logging:
    writer = csv.writer(logging, delimiter=",", quoting=csv.QUOTE_MINIMAL)

    # If the file didn't exist or was empty, write the headers
    if not file_exists:
        writer.writerow(["Time (EST)", "Dispense Count (#)", "Food Eaten (g)", "Water Drank (mL)"])

    # Open the serial port connected to Arduino
    ser = serial.Serial('/dev/ttyACM0')
    ser.flushInput()

    # Tell the Arduino to start sending data
    ser.write(bytes('x', 'utf-8'))

    # Wait to skip initial unwanted output
    time.sleep(5)

    while True:
        ser_bytes = ser.readline()
        
        # Convert received bytes to text format, trimming the newline
        decoded_bytes = ser_bytes.decode("utf-8").strip()
        
        # Retrieve current time
        current_time = datetime.now().strftime('%H:%M:%S')
        
        # Exit loop if Arduino sends "stop"
        if decoded_bytes == "stop":
            break
        
        # Split the decoded bytes by commas and prepend the time
        data = [current_time] + decoded_bytes.split(',')
        
        # Write the data to CSV, ensuring no additional spaces
        writer.writerow(data)

    ser.close()
print("logging finished")
