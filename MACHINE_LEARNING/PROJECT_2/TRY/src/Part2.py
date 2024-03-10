from sklearn.neural_network import MLPClassifier
import numpy as np
import pandas as pd

# Define MLPClassifier with specified parameters
mlp = MLPClassifier(hidden_layer_sizes=(32,), activation='logistic', solver='sgd', 
                    alpha=0.0, batch_size='auto', learning_rate='constant', 
                    learning_rate_init=0.01, max_iter=200, shuffle=True, 
                    random_state=None, tol=1e-4, verbose=False, 
                    early_stopping=False, validation_fraction=0.1, 
                    n_iter_no_change=10)

# Train the classifier

pandas_data = pd.read_csv('../data/song.csv', header=None)
pandas_data.drop(pandas_data.columns[0], axis=1, inplace=True)
pandas_data.drop(pandas_data.index[0], inplace=True)
pandas_data.columns = range(len(pandas_data.columns))

test=pandas_data.sample(frac=0.2, random_state=200)
train=pandas_data.drop(test.index)

X_train = train.pop(0)
# print(X_train)
y_train = train.pop(13)
# print(y_train)
X_test = test.pop(0)
# print(X_test)
y_test = test.pop(13)
# print(y_test)
mlp.fit(X_train.to_numpy().reshape(-1, 1), y_train.to_numpy())

# Test the classifier
print(f"Training set score: {mlp.score(X_train.to_numpy().reshape(-1, 1), y_train.to_numpy())}")
print(f"Test set score: {mlp.score(X_test.to_numpy().reshape(-1, 1), y_test.to_numpy())}")
# print(f"Predictions: {mlp.predict(X_test.to_numpy().reshape(-1, 1))}")
# print(f"Actual: {y_test.to_numpy()}")
# print(f"Predictions: {mlp.predict(X_train.to_numpy().reshape(-1, 1))}")
# print(f"Actual: {y_train.to_numpy()}")
# print(f"Predictions: {mlp.predict(X_train.to_numpy().reshape(-1, 1))}")



