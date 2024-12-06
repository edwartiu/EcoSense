import numpy as np
import pandas as pd
from statsmodels.tsa.arima.model import ARIMA 
from scipy.spatial import distance

# Loads time series data into a Numpy array
data = np.array(pd.read_csv('/home/masudal/EcoSense/data/enviornmental_data.csv')['temperature'])

# Fit ARIMA model
model = ARIMA(data, order=(1, 1, 1))
results = model.fit()

# Makes predictions
forecast = results.forecast(steps=30)

# Creates a 2D array of latitude and longtitude coordinates
lat1 = 33.6846  # Example latitude for Irvine
lon1 = 117.8265 # Example longtitude for Irvine
lat2 = 34.0522 # Example latitude for Los Angeles
lon2 = -118.2437 # Example longtitude for Los Angeles
coordinates = np.array([[lat1, lon1], [lat2, lon2]])

# Calculate distances between all points
dist_matrix = distance.cdist(coordinates, coordinates, 'euclidean')
