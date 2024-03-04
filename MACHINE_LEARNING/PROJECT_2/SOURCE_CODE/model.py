import pandas as pd
import numpy as np
import random
import time


def read_csv_drop_column(csv_file):
    # Read the CSV file into a pandas DataFrame
    data = pd.read_csv(csv_file)

    # Drop the "song_name" column
    data = data.drop(columns=['song_name'])

    # Separate features (X) and target variable (y)
    X = data.drop(columns=['song_popularity']).values
    y = data['song_popularity'].values

    return X, y


def normalize_attributes(X):
    # Custom normalization function to scale features between 0 and 1
    normalized_X = (X - X.min()) / (X.max() - X.min())
    # print(f"X.min():\n{X.min()}")
    # print(f"X.max():\n{X.max()}")
    return normalized_X, X.min(), X.max()


def standardize_attributes(X):
    # Custom standardization function to scale features with a mean of 0 and standard deviation of 1
    standardized_X = (X - X.mean()) / X.std()
    # print(f"X.mean():\n{X.mean()}")
    # print(f"X.std():\n{X.std()}")
    return standardized_X, X.mean(), X.std()


class NeuralNetwork:
    def __init__(self, input_size, hidden_sizes, output_size, learning_rate):
        # Initialize weights and biases for each layer
        self.weights = []
        self.biases = []
        layer_sizes = [input_size] + hidden_sizes + [output_size]

        for i in range(len(layer_sizes) - 1):
            self.weights.append(
                np.random.uniform(-1, 1, (layer_sizes[i], layer_sizes[i+1])))
            self.biases.append(np.zeros((1, layer_sizes[i+1])))

        # Set learning rate
        self.learning_rate = learning_rate

    def sigmoid(self, x):
        return 1 / (1 + np.exp(-x))

    def sigmoid_derivative(self, x):
        return x * (1 - x)

    def relu(self, x):
        return np.maximum(0, x)

    def relu_derivative(self, x):
        return np.where(x > 0, 1, 0)

    def mean_squared_error_loss(self, y_true, y_pred):
        return np.mean(np.square(y_true - y_pred))

    def forward(self, X):
        # Forward pass
        self.activations = [X]

        for i in range(len(self.weights)):
            layer_input = np.dot(
                self.activations[-1], self.weights[i]) + self.biases[i]
            if i == len(self.weights) - 1:
                layer_output = layer_input  # Output layer uses linear activation
            else:
                # Hidden layers use sigmoid activation
                layer_output = self.sigmoid(layer_input)
            self.activations.append(layer_output)

        return self.activations[-1]

    def backward(self, X, y_true, y_pred):
        # Backward pass
        output_error = y_pred - y_true
        output_delta = output_error

        deltas = [output_delta]

        # Backpropagate through hidden layers in reverse order
        for i in range(len(self.weights) - 1, 0, -1):
            error = np.dot(deltas[-1], self.weights[i].T)
            delta = error * self.sigmoid_derivative(self.activations[i])
            deltas.append(delta)

        deltas.reverse()

        # Update weights and biases
        for i in range(len(self.weights)):
            self.weights[i] -= self.learning_rate * \
                np.dot(self.activations[i].T, deltas[i])
            self.biases[i] -= self.learning_rate * \
                np.sum(deltas[i], axis=0, keepdims=True)

    def train(self, X_train, y_train, epochs, batch_size):
        for epoch in range(epochs):
            for i in range(0, len(X_train), batch_size):
                X_batch = X_train[i:i+batch_size]
                y_batch = y_train[i:i+batch_size]

                # Forward pass
                y_pred = self.forward(X_batch)

                # Backward pass
                self.backward(X_batch, y_batch, y_pred)

            # Calculate loss
            loss = self.mean_squared_error_loss(y_train, self.forward(X_train))
            print(f'Epoch {epoch+1}/{epochs}, Loss: {loss:.4f}')

    def predict(self, X_test):
        # Perform prediction
        return self.forward(X_test)


# Preprocess module
def preprocess(X, y, test_size=0.2, random_state=None):

    shuffled_indices = np.random.permutation(len(X))
    X_shuffled = X[shuffled_indices]
    y_shuffled = y[shuffled_indices]

    # Split dataset into train and test sets
    split_index = int(len(X) * (1 - test_size))
    X_train, X_test = X_shuffled[:split_index], X_shuffled[split_index:]
    y_train, y_test = y_shuffled[:split_index], y_shuffled[split_index:]

    return X_train, X_test, y_train, y_test


# Data loader module
def create_mini_batches(X, y, batch_size=32):
    mini_batches = []
    data = np.hstack((X, y.reshape(-1, 1)))
    np.random.shuffle(data)
    n_minibatches = data.shape[0] // batch_size
    for i in range(n_minibatches):
        mini_batch = data[i * batch_size:(i + 1)*batch_size, :]
        X_mini = mini_batch[:, :-1]
        y_mini = mini_batch[:, -1].reshape((-1, 1))
        mini_batches.append((X_mini, y_mini))
    if data.shape[0] % batch_size != 0:
        mini_batch = data[n_minibatches * batch_size:data.shape[0], :]
        X_mini = mini_batch[:, :-1]
        y_mini = mini_batch[:, -1].reshape((-1, 1))
        mini_batches.append((X_mini, y_mini))
    return mini_batches


# Example usage:
random.seed=time.time()
csv_file = '../DATASET/song.csv'
X, y = read_csv_drop_column(csv_file)
# print("Features (X):")
# print(X.head())
# print("\nTarget variable (y):")
# print(y.head())
# print("\nNormalized features (X):")
normalized_X, X_min, X_max = normalize_attributes(X)
standardized_X, X_mean, X_std = standardize_attributes(X)
print("\nNeuralNetwork class:")
input_size = X.shape[1]
hidden_size = [32]
output_size = 1
learning_rate = 0.01
epochs = 200
batch_size = 32

model_specification = NeuralNetwork(
    input_size, hidden_size, output_size, learning_rate)

X_train, X_test, y_train, y_test = preprocess(X, y, test_size=0.2, random_state=42)
model_specification.train(X_train, y_train, epochs, batch_size)