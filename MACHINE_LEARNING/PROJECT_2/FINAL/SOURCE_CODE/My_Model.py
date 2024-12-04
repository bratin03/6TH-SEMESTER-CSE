"""
Group Number: 29
Student Name: Bratin Mondal
Roll Number: 21CS10016
Project Title: Song Popularity using Neural Networks
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from Preprocess import Preprocess

class NeuralNetwork_My:
    """
    Class Name: NeuralNetwork_My
    Purpose: To create a neural network model
    Input: num_hidden (int) -> Number of hidden layers
           num_neurons_per_layer (tuple) -> Number of neurons in each hidden layer
           activation_func_hidden (str) -> Activation function for the hidden layers
           num_neurons_out_layer (int) -> Number of neurons in the output layer
           activation_func_output (str) -> Activation function for the output layer
           opt_algo (str) -> Optimization algorithm
           loss_func (str) -> Loss function
           learning_rate (float) -> Learning rate
           num_epochs (int) -> Number of epochs
    Functionality:
        1. Preprocess the input dataset
        2. Load the training and testing datasets
        3. Initialize the weights and biases of the dense layers
        4. Perform the forward pass
        5. Perform the backward propagation
        6. Calculate the accuracy
        7. Print the accuracy
        8. Plot the accuracy vs epochs graph for training and testing datasets
    """
    def __init__(self,
                 num_hidden,
                 num_neurons_per_layer,
                 activation_func_hidden,
                 num_neurons_out_layer=101,
                 activation_func_output="linear",
                 opt_algo="SGD",
                 loss_func="categorical_crossentropy",
                 learning_rate=0.01,
                 num_epochs=200):
        self.num_hidden = num_hidden
        self.num_neurons_per_layer = num_neurons_per_layer
        self.activation_func_hidden = activation_func_hidden
        if activation_func_hidden == 'sigmoid':
            self.activation_func_output = self.sig
            self.derivative_act_func_output = self.gradSig
        elif activation_func_hidden == 'relu':
            self.activation_func_output = self.relu
            self.derivative_act_func_output = self.reluDerivative
        self.num_neurons_out_layer = num_neurons_out_layer
        self.opt_algo = opt_algo
        self.loss_func = loss_func
        self.learning_rate = learning_rate
        self.num_epochs = num_epochs

    def Preprocess(self):
        """
        Module Name: Preprocess
        Purpose: To preprocess the input dataset and split it into training and testing datasets
        Input: None
        Output: None
        Functionality:
            1. Read the input dataset
            2. Drop the first column and remove duplicates
            3. Normalize the dataset
            4. Split the dataset into training and testing datasets
            5. Save the training and testing datasets
            6. Normalize the labels
        """
        Preprocess()
        self.train_data = pd.read_csv(
            '../DATASET/train.csv', header=None)
        self.test_data = pd.read_csv(
            '../DATASET/test.csv', header=None)
        self.train_data_feats = self.train_data.iloc[:, :-1].to_numpy()
        self.train_data_labels = self.train_data.iloc[:, -1:].to_numpy()
        self.test_data_feats = self.test_data.iloc[:, :-1].to_numpy()
        self.test_data_labels = self.test_data.iloc[:, -1:].to_numpy()
        self.train_data_labels = self.train_data_labels.astype(float)
        self.test_data_labels = self.test_data_labels.astype(float)

        self.train_data_labels = self.convert_to_one_hot(self.train_data_labels)
        self.test_data_labels = self.convert_to_one_hot(self.test_data_labels)

    def DataLoader(self):
        """
        Module Name: DataLoader
        Purpose: To load the training ancd testing datasets
        Input: None
        Output: None
        Functionality:
            1. Load the training and testing datasets
            2. Split the datasets into features and labels
            3. Split the datasets into batches of size 32
        """
        self.Preprocess()
        self.train_feat_batch = self.BatchSplit(self.train_data_feats)
        self.train_label_batch = self.BatchSplit(self.train_data_labels)

    @staticmethod
    def WeightInitialiser(data_dim, layers, num_out_layers):
        """
        Module Name: WeightInitialiser
        Purpose: To initialize the weights of the dense layers
        Input: data_dim (int) -> Number of features
               layers (tuple) -> Number of neurons in each hidden layer
               num_out_layers (int) -> Number of neurons in the output layer
        Output: weights (list) -> List of weights for each layer
        Functionality:
            1. Initialize the weights of the input layer
            2. Initialize the weights of the hidden layers
            3. Initialize the weights of the output layer
        """
        weights = list()
        for layer in range(len(layers)):
            if not layer:
                rand_mat = np.random.rand(data_dim, layers[layer])
            else:
                rand_mat = np.random.rand(layers[layer-1], layers[layer])
            weights.append((2 * rand_mat) - 1)
        rand_mat = np.random.rand(layers[-1], num_out_layers)
        weights.append((2 * rand_mat) - 1)
        return weights

    @staticmethod
    def BiasInitialiser(layers, num_out_layers):
        """
        Module Name: BiasInitialiser
        Purpose: To initialize the biases of the dense layers
        Input: layers (tuple) -> Number of neurons in each hidden layer
               num_out_layers (int) -> Number of neurons in the output layer
        Output: bias (list) -> List of biases for each layer
        Functionality:
            1. Initialize the biases of the hidden layers
            2. Initialize the biases of the output layer
        """
        bias = list()
        for layer in range(len(layers)):
            bias.append(np.zeros((1, layers[layer])))
        bias.append(np.zeros((1, num_out_layers)))
        return bias

    def ForwardPass(self, features, layer_weights, layer_bias):
        """
        Module Name: ForwardPass
        Purpose: To perform the forward pass of the neural network
        Input: features (np.array) -> Input features
               layer_weights (list) -> List of weights for each layer
               layer_bias (list) -> List of biases for each layer
        Output: forward_list (list) -> List of outputs of each layer
        Functionality:
            1. Perform the forward pass of the input layer
            2. Perform the forward pass of the hidden layers
            3. Perform the forward pass of the output layer
        """
        forward_list = list()
        forward_list.append(features)
        for layer_weight, layer_bia in zip(layer_weights[:-1], layer_bias[:-1]):
            forward_list.append(self.activation_func_output(
                (forward_list[-1]).dot(layer_weight) + layer_bia))

        forward_list.append(
            self.softMax((forward_list[-1]).dot(layer_weights[-1]) + layer_bias[-1]))
        return forward_list

    def BackPropagation(self, layer_weights, layer_bias, act_right, label):
        """
        Module Name: BackPropagation
        Purpose: To perform the backward propagation of the neural network
        Input: layer_weights (list) -> List of weights for each layer
               layer_bias (list) -> List of biases for each layer
               act_right (list) -> List of outputs of each layer
               label (np.array) -> True labels
        Output: layer_weights (list) -> Updated list of weights for each layer
                layer_bias (list) -> Updated list of biases for each layer
        Functionality:
            1. Perform the backward propagation of the output layer
            2. Perform the backward propagation of the hidden layers
        """
        delta =act_right[-1]
        delta[label==1] -= 1

        for i in range(len(layer_weights)-1, 0, -1):
            delta_next = delta.dot(
                layer_weights[i].T)*self.derivative_act_func_output(act_right[i])
            layer_weights[i] = layer_weights[i] - \
                self.learning_rate * ((act_right[i].T).dot(delta))
            layer_bias[i] = layer_bias[i] - self.learning_rate * \
                np.sum(delta, axis=0, keepdims=True)
            delta = delta_next
        return layer_weights, layer_bias

    def Training(self):
        """
        Module Name: Training
        Purpose: To train the neural network
        Input: None
        Output: None
        Functionality:
            1. Initialize the weights and biases of the dense layers
            2. Perform the forward pass
            3. Perform the backward propagation
            4. Calculate the accuracy
            5. Print the accuracy
            6. Plot the accuracy vs epochs graph for training and testing datasets
        """
        self.test_accuracy = list()
        self.train_accuracy = list()
        layer_weights = self.WeightInitialiser(
            13, self.num_neurons_per_layer, self.num_neurons_out_layer)
        layer_bias = self.BiasInitialiser(
            self.num_neurons_per_layer, self.num_neurons_out_layer)

        for cur_epoch in range(1, self.num_epochs+1):
            for feat_batch, label_batch in zip(self.train_feat_batch, self.train_label_batch):
                act_right = self.ForwardPass(
                    feat_batch, layer_weights, layer_bias)
                layer_weights, layer_bias = self.BackPropagation(
                    layer_weights, layer_bias, act_right, label_batch)

            if not (cur_epoch % 10):
                train_prediction, test_prediction = self.Predict(
                    layer_weights, layer_bias)
                self.train_accuracy.append(self.curAccu(
                    train_prediction, self.train_data_labels))
                self.test_accuracy.append(self.curAccu(
                    test_prediction, self.test_data_labels))
                print(
                    f"Epoch {cur_epoch} : Train Accuracy = {self.train_accuracy[-1]} % Test Accuracy = {self.test_accuracy[-1]} %")
        epochs = range(10, self.num_epochs+1, 10)
        plt.plot(epochs, self.train_accuracy, label='Train Accuracy')
        plt.plot(epochs, self.test_accuracy, label='Test Accuracy')
        plt.xlabel('Epochs')
        plt.ylabel('Accuracy')
        plt.legend()
        plt.title('Accuracy vs Epochs')
        plt.grid(True)
        file_name="Model_improved_layer_"
        for i in range(len(self.num_neurons_per_layer)):
            file_name+=str(self.num_neurons_per_layer[i])+"_"
        file_name+=str(self.num_neurons_out_layer)
        plt.savefig(f"../OUTPUT/{file_name}.png")
        plt.show()


        print(f"Final Train Accuracy = {self.train_accuracy[-1]} %")
        print(f"Final Test Accuracy = {self.test_accuracy[-1]} %")


    def convert_to_one_hot(self, labels):
        """
        Module Name: convert_to_one_hot
        Purpose: To convert the labels to one-hot encoding
        Input: labels (np.array) -> True labels
        Output: one_hot_labels (np.array) -> One-hot encoded labels
        Functionality: This function converts the labels to one-hot encoding
        """
        num_classes = 101 
        one_hot_labels = np.zeros((labels.shape[0], num_classes))
        for i in range(labels.shape[0]):
            label = int(labels[i][0])  
            one_hot_labels[i][label] = 1
        return one_hot_labels
    
    def softMax(self, z_mat):
        """
        Module Name: softMax
        Purpose: To calculate the softmax function
        Input: z_mat (np.array) -> Input to the softmax function
        Output: (np.array) -> Output of the softmax function
        Functionality: This function calculates the output of the softmax function
        """
        z_exp=np.exp(z_mat)
        interim_sum=np.sum(z_exp,axis=1,keepdims=True)
        return z_exp/interim_sum

    def fit(self):
        """
        Module Name: fit
        Purpose: To fit the neural network
        Input: None
        Output: None
        Functionality:
            1. Load the training and testing datasets
            2. Split the datasets into batches of size 32
            3. Train the neural network
        """
        self.DataLoader()
        self.Training()

    def Predict(self, layer_weights, layer_bias):
        """
        Module Name: Predict
        Purpose: To predict the labels
        Input: layer_weights (list) -> List of weights for each layer
               layer_bias (list) -> List of biases for each layer
        Output: pred_train2 (np.array) -> Predicted labels for the training dataset
                pred_test2 (np.array) -> Predicted labels for the testing dataset
        Functionality:
            1. Perform the forward pass
            2. Find the predicted labels
            3. Clip the predicted labels
        """
        prediction_train = self.ForwardPass(
            self.train_data_feats, layer_weights, layer_bias)
        prediction_test = self.ForwardPass(
            self.test_data_feats, layer_weights, layer_bias)
        shape = prediction_train[-1].shape
        pred_train2 = np.zeros(shape)
        pred_train2[np.arange(shape[0]),np.argmax(prediction_train[-1], axis=-1)] = 1
        shape = prediction_test[-1].shape
        pred_test2 = np.zeros(shape)
        pred_test2[np.arange(shape[0]),np.argmax(prediction_test[-1], axis=-1)] = 1
        return pred_train2, pred_test2

    @staticmethod
    def curAccu(predicted, orig):
        """
        Module Name: curAccu
        Input: predicted (np.array) -> Predicted labels
                orig (np.array) -> True labels 
        Output: count (float) -> Accuracy of the model
        Functionality: This function calculates the accuracy of the model
        """
        iter_shape = predicted.shape[0]
        count = 0
        for i in range(iter_shape):
            if np.array_equal(predicted[i], orig[i]):
                count += 1
        count *= 100
        return count/iter_shape

    @staticmethod
    def BatchSplit(df):
        """
        Module Name: BatchSplit
        Purpose: To split the dataset into batches of size 32
        Input: df (pd.DataFrame) -> Input dataset
        Output: df_list (list) -> List of batches of size 32
        Functionality: This function splits the dataset into batches of size 32
        """
        df_list = []
        numChunks = len(df) // 32 + 1
        for i in range(numChunks):
            df_list.append(df[i*32:(i+1)*32])
        return df_list

    @staticmethod
    def gradSig(x):
        """
        Module Name: gradSig
        Purpose: To calculate the gradient of the sigmoid function
        Input: x (np.array) -> Input to the sigmoid function
        Output: (np.array) -> Gradient of the sigmoid function
        Functionality: This function calculates the gradient of the sigmoid function
        """
        return (1/(1+np.exp(-x))) * (1 - (1/(1+np.exp(-x))))

    @staticmethod
    def sig(x):
        """
        Module Name: sig
        Purpose: To calculate the sigmoid function
        Input: x (np.array) -> Input to the sigmoid function
        Output: (np.array) -> Output of the sigmoid function
        Functionality: This function calculates the output of the sigmoid function
        """
        return (1/(1+np.exp(-x)))

    @staticmethod
    def reluDerivative(x):
        """
        Module Name: reluDerivative
        Purpose: To calculate the gradient of the ReLU function
        Input: x (np.array) -> Input to the ReLU function
        Output: (np.array) -> Gradient of the ReLU function
        Functionality: This function calculates the gradient of the ReLU function
        """
        x[x <= 0] = 0
        x[x > 0] = 1
        return x

    @staticmethod
    def relu(x):
        """
        Module Name: relu
        Purpose: To calculate the ReLU function
        Input: x (np.array) -> Input to the ReLU function
        Output: (np.array) -> Output of the ReLU function
        Functionality: This function calculates the output of the ReLU function
        """
        x[x <= 0] = 0
        return x

if __name__ == "__main__":
    print("\n\n")
    """
    Model 1: MLP with 1 hidden layer of 32 neurons and output layer of 101 neurons using categorical cross entropy
    """
    print("\nMLP Model 1")
    print("-----------")
    print("Hidden Layer Sizes: (32)")
    print("Activation Function: Sigmoid")
    print("Solver: Stochastic Gradient Descent")
    print("Learning Rate: Constant")
    print("Learning Rate Initial: 0.01")
    print("Max Iterations: 200")
    print("Batch Size: 32")
    print("\n")
    nn3=NeuralNetwork_My(num_hidden=1,num_neurons_per_layer=[32],activation_func_hidden='sigmoid',num_neurons_out_layer=101,activation_func_output='softmax',opt_algo='SGD',loss_func='categorical_crossentropy',learning_rate=0.01,num_epochs=200)
    nn3.fit()

    print("\n\n")
    """
    Model 2: MLP with 2 hidden layers of 64 and 32 neurons and output layer of 101 neurons using categorical cross entropy
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
    nn4=NeuralNetwork_My(num_hidden=2,num_neurons_per_layer=[64,32],activation_func_hidden='relu',num_neurons_out_layer=101,activation_func_output='softmax',opt_algo='SGD',loss_func='categorical_crossentropy',learning_rate=0.01,num_epochs=200)
    nn4.fit()

