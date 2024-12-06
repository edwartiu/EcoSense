import numpy as np
import pandas as pd
from statsmodels.tsa.arima.model import ARIMA 
from scipy.spatial import distance
from sklearn.model_selection import train_test_split

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

# Calculates distances between all points
dist_matrix = distance.cdist(coordinates, coordinates, 'euclidean')
print("Distance Matrix:\n", dist_matrix)

# Loads our dataset
df = pd.read_csv('/home/masudal/EcoSense/data/enviornmental_data.csv')

# Checks if the DataFrame is loaded correctly
print("DataFrame head:\n", df.head())

X = df.drop(columns=['temperature']).values # Features
y = df['temperature'].values # Target variable

# Splits the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Adds bias term to features
X_train_bias = np.c_[np.ones(X_train.shape[0]), X_train]

# Implements linear regression
def linear_regression(X, y):
    X = np.array(X)
    y = np.array(y)
    return np.linalg.inv(X.T @ X) @ X.T @ y

# Fit model
# coefficients = linear_regression(X_train_bias, y_train)

# print("Coefficients:", coefficients)


