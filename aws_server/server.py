from flask import Flask
from flask import request
from datetime import date, datetime


app = Flask(__name__)

@app.route("/")
def process_input():
    # Values sent
    soilMoisture = request.args.get("soil")
    temperature = request.args.get("temp")
    humidity = request.args.get("hum")
    uva = request.args.get("uva")
    uvb = request.args.get("uvb")

    with open('data.txt', 'a') as f:
        f.write(datetime.now(), "\n")
        f.write(soilMoisture, "\n")
        f.write(temperature, "\n")
        f.write(humidity, "\n")
        f.write(uva, "\n")
        f.write(uvb, "\n")

    return "Data received", 200
    

