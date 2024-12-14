import csv
import os
import pandas as pd
import matplotlib.pyplot as plt
from flask import Flask,  request, redirect, url_for
from datetime import date, datetime

app = Flask(__name__)

UPLOAD_FOLDER = 'static/graphs'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

@app.route("/")
def process_input():
    # Values sent
    soilMoisture = request.args.get("soil")
    temperature = request.args.get("temp")
    humidity = request.args.get("hum")
    uva = request.args.get("uva")
    uvb = request.args.get("uvb")

    with open('data.csv', 'a') as file:
        writer = csv.writer(file)
        writer.writerow([str(datetime.now()), soilMoisture, temperature,
                        humidity, uva, uvb])
  
    return "Data received"


@app.route("/example")
def display_example():
    data = pd.read_csv('data12-12.csv')
    data['time'] = pd.to_datetime(data['time'], format='%H:%M')
    
    plt.figure(figsize=(10, 6))
    plt.plot(data['time'], data['soil_moisture'], label='Soil Moisture', color='brown')
    plt.xlabel('Time')
    plt.ylabel('Soil Moisture')
    plt.title('Soil Moisture Over Time') 
    plt.ylim(0, max(data['soil_moisture']) * 1.1)
    plt.legend()
    plt.grid(True)
    soil_moisture_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'soil_moisture_plot.png')
    plt.savefig(soil_moisture_plot_path)
    plt.close()

    
    plt.figure(figsize=(10, 6))
    plt.plot(data['time'], data['uva'], label='UVA Rays', color='brown')
    plt.xlabel('Time')
    plt.ylabel('Values')
    plt.title('UVA Rays') 
    plt.ylim(0, max(data['uva']) * 1.1)
    plt.legend()
    plt.grid(True)
    uva_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'uva_plot.png')
    plt.savefig(uva_plot_path)
    plt.close()

    plt.figure(figsize=(10, 6))
    plt.plot(data['time'], data['temperature'], label='Temperature')
    plt.plot(data['time'], data['humidity'], label='Humidity')
    plt.plot(data['time'], data['uvb'], label='UVB')
    plt.xlabel('Time')
    plt.ylabel('Values')
    plt.title('Sensor Data Over Time (Excluding Soil Moisture)')
    plt.legend()
    plt.grid(True)
    sensor_data_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'sensor_data_plot.png')
    plt.savefig(sensor_data_plot_path)
    plt.close()

    return f'<h3>Soil Moisture</h3><img src="/{UPLOAD_FOLDER}/soil_moisture_plot.png" alt="Soil Moisture Plot"> \
            <h3>Other Sensor Data</h3><img src="/{UPLOAD_FOLDER}/uva_plot.png" alt="UVA Plot">  \
            <h3>Other Sensor Data</h3><img src="/{UPLOAD_FOLDER}/sensor_data_plot.png" alt="Sensor Data Plot">'

