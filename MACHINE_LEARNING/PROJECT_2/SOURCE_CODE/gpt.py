import pandas as pd
import numpy as np

# Sigmoid activation function
def sigmoid(x):
    return 1 / (1 + np.exp(-x))

# ReLU activation function
def relu(x):
    return np.maximum(0, x)

# Linear activation function
def linear(x):
    return x

# Mean Squared Error loss function
def mse_loss(y_true, y_pred):
    return np.mean((y_true - y_pred)**2)

class NeuralNetwork:
    def __init__(self, input_dim, hidden_dims, output_dim, activations):
        self.input_dim = input_dim
        self.hidden_dims = hidden_dims
        self.output_dim = output_dim
        self.activations = activations
        
        self.weights = []
        self.biases = []
        
        # Initialize weights and biases
        dims = [input_dim] + hidden_dims + [output_dim]
        for i in range(len(dims) - 1):
            self.weights.append(np.random.uniform(-1, 1, (dims[i], dims[i+1])))
            self.biases.append(np.zeros((1, dims[i+1])))
        
    def forward(self, X):
        output = X
        for i in range(len(self.weights)):
            output = np.dot(output, self.weights[i]) + self.biases[i]
            output = self.activations[i](output)
            print(output)
        return output
    
    def backward(self, X, y_true, learning_rate):
        # Forward pass
        layer_outputs = [X]
        for i in range(len(self.weights)):
            layer_outputs.append(np.dot(layer_outputs[-1], self.weights[i]) + self.biases[i])
            layer_outputs[-1] = self.activations[i](layer_outputs[-1])

        # Backpropagation
        error = y_true - layer_outputs[-1]
        deltas = [error * (layer_outputs[-1] * (1 - layer_outputs[-1]))]
        for i in range(len(self.weights) - 1, 0, -1):
            deltas.append(np.dot(deltas[-1], self.weights[i].T) * (layer_outputs[i] * (1 - layer_outputs[i])))
        deltas.reverse()

        # Update weights and biases
        for i in range(len(self.weights)):
            self.weights[i] += learning_rate * np.dot(layer_outputs[i].T, deltas[i])
            self.biases[i] += learning_rate * np.sum(deltas[i], axis=0, keepdims=True)

    
    def train(self, X_train, y_train, learning_rate, epochs, batch_size):
        for epoch in range(epochs):
            for i in range(0, len(X_train), batch_size):
                X_batch = X_train[i:i+batch_size]
                y_batch = y_train[i:i+batch_size]
                
                # Forward pass
                predictions = self.forward(X_batch)
                
                # Backpropagation
                self.backward(X_batch, y_batch, learning_rate)
                
                # Compute and print loss
                loss = mse_loss(y_batch, predictions)
                print(f'Epoch {epoch+1}/{epochs}, Batch {i//batch_size+1}/{len(X_train)//batch_size}, Loss: {loss}')
                
    def predict(self, X_test):
        return self.forward(X_test)

# Preprocess data and split into train and test sets
# Assuming X_train, y_train, X_test, y_test are prepared



# Load CSV file
data = pd.read_csv('../DATASET/song.csv')

# Extract features and target variable
X = data.drop(['song_name', 'song_popularity'], axis=1).values
y = data['song_popularity'].values

# Normalize features manually
X_normalized = (X - np.min(X, axis=0)) / (np.max(X, axis=0) - np.min(X, axis=0))

# Split data into train and test sets manually
split_ratio = 0.8
split_index = int(split_ratio * len(X_normalized))
X_train, X_test = X_normalized[:split_index], X_normalized[split_index:]
y_train, y_test = y[:split_index], y[split_index:]

# Train the neural network (using Specification 1)
nn_spec1 = NeuralNetwork(input_dim=X_train.shape[1], hidden_dims=[32], output_dim=1, activations=[sigmoid, linear])
nn_spec1.train(X_train, y_train, learning_rate=0.01, epochs=200, batch_size=32)

# Alternatively, you can train using Specification 2
# nn_spec2 = NeuralNetwork(input_dim=X_train.shape[1], hidden_dims=[64, 32], output_dim=1, activations=[relu, relu, linear])
# nn_spec2.train(X_train, y_train, learning_rate=0.01, epochs=200, batch_size=32)

