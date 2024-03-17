import serial
from datetime import datetime
import csv
import os
import time

# Define the path to the CSV file
csv_file_path = '/home/pawprint/Documents/csv_output/data_log.csv'

# Check if the file already exists and has content
file_exists = os.path.isfile(csv_file_path) and os.path.getsize(csv_file_path) > 0

#Open a csv file and set it up to receive comma delimited input
logging = open('data_log.csv',mode='a')
writer = csv.writer(logging, delimiter=",", escapechar="\\", quoting=csv.QUOTE_NONE)

# Write the headers only if the file doesn't exist already
if not file_exists:
    writer.writerow(["Time (EST)", "Dispense Count (#)", "Food Eaten (g)", "Water Drank (mL)"])

#Open a serial port that is connected to an Arduino (below is Linux, Windows and Mac would be "COM4" or similar)
#No timeout specified; program will wait until all serial data is received from Arduino
#Port description will vary according to operating system. Linux will be in the form /dev/ttyXXXX
#Windows and MAC will be COMX
ser = serial.Serial('/dev/ttyACM0')
ser.flushInput()

#Write out a single character encoded in utf-8; this is defalt encoding for Arduino serial comms
#This character tells the Arduino to start sending data
ser.write(bytes('x', 'utf-8'))

# Wait for 5 seconds to skip initial unwanted output
time.sleep(5)

while True:
    #Read in data from Serial until \n (new line) received
    ser_bytes = ser.readline()
    print(ser_bytes)
    
    #Convert received bytes to text format
    decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
    print(decoded_bytes)
    
    #Retreive current time
    c = datetime.now()
    current_time = c.strftime('%H:%M:%S')
    print(current_time)
    
    #If Arduino has sent a string "stop", exit loop
    if (decoded_bytes == "stop"):
         break
    
    #Write received data to CSV file

    # Split decoded_bytes into a list
    data_parts = decoded_bytes.split(',')

    # Combine current_time with data_parts into a single list
    row_data = [current_time] + data_parts

    # Write the combined list to the CSV, resulting in each part being a separate column
    writer.writerow(row_data)
            
# Close port and CSV file to exit
ser.close()
logging.close()