"""
Group Number: 29
Student Name: Bratin Mondal
Roll Number: 21CS10016
Project Title: Song Popularity using Neural Networks
"""

from sklearn.neural_network import MLPClassifier
from sklearn.metrics import accuracy_score
import numpy as np
import matplotlib.pyplot as plt

"""
Load the training and testing datasets
"""
train = np.loadtxt("../DATASET/train.csv", delimiter=",")
test = np.loadtxt("../DATASET/test.csv", delimiter=",")
X_train, y_train = train[:, :-1], train[:, -1]
X_test, y_test = test[:, :-1], test[:, -1]

"""
Function name : get_accuracy
Input : y_true (list) -> True labels
        y_pred (list) -> Predicted labels   
Output : accuracy (float) -> Accuracy of the model
Functionality : This function calculates the accuracy of the model
"""


def get_accuracy(y_true, y_pred):
    assert len(y_true) == len(y_pred)
    return accuracy_score(y_true, y_pred) * 100


"""
Model 1: MLP with 1 hidden layer of 32 neurons
"""
print("MLP Model 1")
print("-----------")
print("Hidden Layer Sizes: (32)")
print("Activation Function: Sigmoid")
print("Solver: Stochastic Gradient Descent")
print("Learning Rate: Constant")
print("Learning Rate Initial: 0.01")
print("Max Iterations: 200")
print("Batch Size: 32")
print("\n")

# Create the MLP model
mlp1 = MLPClassifier(hidden_layer_sizes=(32), activation='logistic', solver='sgd', learning_rate='constant',
                     learning_rate_init=0.01, max_iter=200, batch_size=32, verbose=False)
# Lists to store the accuracies
train_accuracies = []
test_accuracies = []
# Train the model
for epoch in range(1, 201):
    mlp1.partial_fit(X_train, y_train, classes=np.unique(y_train))
    if epoch % 10 == 0:
        # Predict the labels
        # Calculate the accuracy
        # Append the accuracy to the lists
        # Print the accuracy
        # This is done every 10 epochs
        train_pred1 = mlp1.predict(X_train)
        test_pred1 = mlp1.predict(X_test)
        train_accuracy = get_accuracy(y_train, train_pred1)
        test_accuracy = get_accuracy(y_test, test_pred1)
        train_accuracies.append(train_accuracy)
        test_accuracies.append(test_accuracy)
        print(
            f"Epoch {epoch} - Train accuracy: {train_accuracy} %, Test accuracy: {test_accuracy} %")

epochs = range(10, 201, 10)
# Plot the accuracies
plt.plot(epochs, train_accuracies, label='Train Accuracy')
plt.xlabel('Epochs')
plt.ylabel('Train Accuracy (%)')
plt.title('Train Accuracy vs Epochs')
plt.legend()
plt.grid(True)
plt.savefig('../OUTPUT/scikit_learn_model_1_train.png')
plt.show()

plt.plot(epochs, test_accuracies, label='Test Accuracy')
plt.xlabel('Epochs')
plt.ylabel('Accuracy (%)')
plt.title('Accuracy vs Epochs')
plt.legend()
plt.grid(True)
plt.savefig('../OUTPUT/scikit_learn_model_1_test.png')
plt.show()
# Print the final accuracies
print(f"Final Train accuracy: {train_accuracies[-1]} %")
print(f"Final Test accuracy: {test_accuracies[-1]} %")

print("\n")

# Model 2: MLP with 2 hidden layers of 64 and 32 neurons
print("\nMLP Model 2")
print("-----------")
print("Hidden Layer Sizes: (64, 32)")
print("Activation Function: ReLU")
print("Solver: Stochastic Gradient Descent")
print("Learning Rate: Constant")
print("Learning Rate Initial: 0.01")
print("Max Iterations: 200")
print("Batch Size: 32")
print("\n")

# Create the MLP model
mlp2 = MLPClassifier(hidden_layer_sizes=(64, 32), activation='relu', solver='sgd', learning_rate='constant',
                     learning_rate_init=0.01, max_iter=200, batch_size=32, verbose=False)
# Lists to store the accuracies
train_accuracies = []
test_accuracies = []

# Train the model
for epoch in range(1, 201):
    mlp2.partial_fit(X_train, y_train, classes=np.unique(y_train))
    if epoch % 10 == 0:
        # Predict the labels
        # Calculate the accuracy
        # Append the accuracy to the lists
        # Print the accuracy
        # This is done every 10 epochs
        train_pred2 = mlp2.predict(X_train)
        test_pred2 = mlp2.predict(X_test)
        train_accuracy = get_accuracy(y_train, train_pred2)
        test_accuracy = get_accuracy(y_test, test_pred2)
        train_accuracies.append(train_accuracy)
        test_accuracies.append(test_accuracy)
        print(
            f"Epoch {epoch} - Train accuracy: {train_accuracy} %, Test accuracy: {test_accuracy} %")

epochs = range(10, 201, 10)
# Plot the accuracies
plt.plot(epochs, train_accuracies, label='Train Accuracy')
plt.xlabel('Epochs')
plt.ylabel('Train Accuracy (%)')
plt.title('Train Accuracy vs Epochs')
plt.legend()
plt.grid(True)
plt.savefig('../OUTPUT/scikit_learn_model_2_train.png')
plt.show()

plt.plot(epochs, test_accuracies, label='Test Accuracy')
plt.xlabel('Epochs')
plt.ylabel('Accuracy (%)')
plt.title('Accuracy vs Epochs')
plt.legend()
plt.grid(True)
plt.savefig('../OUTPUT/scikit_learn_model_2_test.png')
plt.show()
# Print the final accuracies
print(f"Final Train accuracy: {train_accuracies[-1]} %")
print(f"Final Test accuracy: {test_accuracies[-1]} %")