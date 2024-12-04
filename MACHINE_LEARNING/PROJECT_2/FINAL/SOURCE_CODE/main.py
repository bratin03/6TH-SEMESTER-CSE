"""
Group Number: 29
Student Name: Bratin Mondal
Roll Number: 21CS10016
Project Title: Song Popularity using Neural Networks
"""

from Model import NeuralNetwork
from My_Model import NeuralNetwork_My

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
nn1=NeuralNetwork(num_hidden=1,num_neurons_per_layer=[32],activation_func_hidden='sigmoid',num_neurons_out_layer=1,activation_func_output='linear',opt_algo='SGD',loss_func='mean_squared_error',learning_rate=0.01,num_epochs=200)
nn1.fit()

print("\n\n")
"""
Model 2: MLP with 2 hidden layers of 64 and 32 neurons
"""
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
nn2=NeuralNetwork(num_hidden=2,num_neurons_per_layer=[64,32],activation_func_hidden='relu',num_neurons_out_layer=1,activation_func_output='linear',opt_algo='SGD',loss_func='mean_squared_error',learning_rate=0.01,num_epochs=200)
nn2.fit()

