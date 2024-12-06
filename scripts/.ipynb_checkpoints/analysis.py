import numpy as np
import pandas as pd
# from statsmodels.tsa.arima.model import ARIMA 

# Loads time series data into a Numpy array
data = np.array(pd.read_csv('/home/masudal/EcoSense/data/enviornmental_data.csv')['temperature'])