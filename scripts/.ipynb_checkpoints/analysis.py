import numpy as np
import pandas as pd
from statsmodels.tsa.arima.model import ARIMA 
# from scipy.spatial import distance

# Loads time series data into a Numpy array
data = np.array(pd.read_csv('/home/masudal/EcoSense/data/enviornmental_data.csv')['temperature'])

# Fit ARIMA model
model = ARIMA(data, order=(1, 1, 1))
results = model.fit()

# Makes predictions
forecast = results.forecast(steps=30)