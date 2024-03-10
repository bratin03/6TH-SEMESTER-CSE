"""
Group Number: 29
Student Name: Bratin Mondal
Roll Number: 21CS10016
Project Title: Song Popularity using Neural Networks
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


class NeuralNetwork:

    def __init__(self,
                 num_hidden,
                 num_neurons_per_layer,
                 activation_func_hidden,
                 num_neurons_out_layer=1,
                 activation_func_output="linear",
                 opt_algo="SGD",
                 loss_func="mean_squared_error",
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

    def DataLoader(self):
        """
        Module Name: DataLoader
        Purpose: To load the training and testing datasets
        Input: None
        Output: None
        Functionality:
            1. Load the training and testing datasets
            2. Split the datasets into features and labels
            3. Split the datasets into batches of size 32
        """
        self.train_data = pd.read_csv(
            '../DATASET/train.csv', header=None)
        self.test_data = pd.read_csv(
            '../DATASET/test.csv', header=None)

        self.train_data_feats = self.train_data.iloc[:, :-1].to_numpy()
        self.train_data_labels = self.train_data.iloc[:, -1:].to_numpy()
        self.test_data_feats = self.test_data.iloc[:, :-1].to_numpy()
        self.test_data_labels = self.test_data.iloc[:, -1:].to_numpy()

        self.train_feat_batch = self.dfSplit(self.train_data_feats)
        self.train_label_batch = self.dfSplit(self.train_data_labels)

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
            (forward_list[-1]).dot(layer_weights[-1]) + layer_bias[-1])
        return forward_list
    
    def backwardProp(self, layer_weights, layer_bias, act_right, label):
        """
        Module Name: backwardProp
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
        label_copy = label/100 # Normalizing the label
        delta = 2*(act_right[-1]-label_copy)/label_copy.shape[0]

        for i in range(len(layer_weights)-1, 0, -1):
            delta_next = delta.dot(
                layer_weights[i].T)*self.derivative_act_func_output(act_right[i])
            layer_weights[i] = layer_weights[i] - \
                self.learning_rate * ((act_right[i].T).dot(delta))
            layer_bias[i] = layer_bias[i] - self.learning_rate * \
                np.sum(delta, axis=0, keepdims=True)
            delta = delta_next
        return layer_weights, layer_bias

    
    def train(self):
        self.test_accuracy = list()
        self.train_accuracy = list()
        layer_weights = self.WeightInitialiser(
            13, self.num_neurons_per_layer, self.num_neurons_out_layer)
        layer_bias = self.BiasInitialiser(
            self.num_neurons_per_layer, self.num_neurons_out_layer)

        train_prediction, test_prediction = self.predict(
            layer_weights, layer_bias)
        print(
            f"Inital Train Accuracy: {self.curAccu(train_prediction, self.train_data_labels)}")
        print(
            f"Inital Test Accuracy: {self.curAccu(test_prediction, self.test_data_labels)}")
        self.train_accuracy.append(self.curAccu(
            train_prediction, self.train_data_labels))
        self.test_accuracy.append(self.curAccu(
            test_prediction, self.test_data_labels))

        for cur_epoch in range(self.num_epochs):
            for feat_batch, label_batch in zip(self.train_feat_batch, self.train_label_batch):
                act_right = self.ForwardPass(
                    feat_batch, layer_weights, layer_bias)
                layer_weights, layer_bias = self.backwardProp(
                    layer_weights, layer_bias, act_right, label_batch)

            if not (cur_epoch % 10):
                train_prediction, test_prediction = self.predict(
                    layer_weights, layer_bias)
                self.train_accuracy.append(self.curAccu(
                    train_prediction, self.train_data_labels))
                self.test_accuracy.append(self.curAccu(
                    test_prediction, self.test_data_labels))
                print(
                    f"Train Accuracy: {self.train_accuracy[-1]} Test Accuracy: {self.test_accuracy[-1]}")

        train_prediction, test_prediction = self.predict(
            layer_weights, layer_bias)
        unique_values = np.unique(train_prediction)
        print(f"Unique values in train prediction: {unique_values}")
        unique_values = np.unique(test_prediction)
        print(f"Unique values in test prediction: {unique_values}")

    def fit(self):
        self.DataLoader()
        self.train()

    def showAccu(self, part):
        print(f"Part 1 Specification {part} :")
        print("Train Accuracy = ", self.train_accuracy[-1])
        print("Test Accuracy = ", self.test_accuracy[-1])

    def plot(self, part):
        iterations = [x for x in range(0, 200, 10)]
        plt.plot(iterations, self.train_accuracy[1:], label="Train")
        plt.plot(iterations, self.test_accuracy[1:], label="Test")
        plt.xlabel('Cost')
        plt.ylabel('Accuracy')
        plt.legend()
        plt.title(f"Part {part} Accuracy Vs Iterations")
        plt.show()
        plt.clf()



    def preProcess(self):
        pass

    def predict(self, layer_weights, layer_bias):
        prediction_train = self.ForwardPass(
            self.train_data_feats, layer_weights, layer_bias)
        pred_train2 = prediction_train[-1]*100
        pred_train2 = np.round(pred_train2)
        for i in range(len(pred_train2)):
            if pred_train2[i] > 100:
                pred_train2[i] = 100
            elif pred_train2[i] < 0:
                pred_train2[i] = 0

        prediction_test = self.ForwardPass(
            self.test_data_feats, layer_weights, layer_bias)
        pred_test2 = prediction_test[-1]*100
        pred_test2 = np.round(pred_test2)
        for i in range(len(pred_test2)):
            if pred_test2[i] > 100:
                pred_test2[i] = 100
            elif pred_test2[i] < 0:
                pred_test2[i] = 0
        return pred_train2, pred_test2


    
    @staticmethod
    def curAccu(predicted, orig):
        iter_shape = predicted.shape[0]
        count = 0
        for i in range(iter_shape):
            if np.array_equal(predicted[i], orig[i]):
                count += 1
        count *= 100
        return count/iter_shape

    @staticmethod
    def dfSplit(df):
        df_list = []
        numChunks = len(df) // 32 + 1
        for i in range(numChunks):
            df_list.append(df[i*32:(i+1)*32])
        return df_list

    @staticmethod
    def gradSig(x):
        return (1/(1+np.exp(-x))) * (1 - (1/(1+np.exp(-x))))

    @staticmethod
    def sig(x):
        return (1/(1+np.exp(-x)))

    @staticmethod
    def reluDerivative(x):
        x[x <= 0] = 0
        x[x > 0] = 1
        return x

    @staticmethod
    def relu(x):
        x[x <= 0] = 0
        return x


# nn1 = NeuralNetwork(
#     num_hidden=1,
#     num_neurons_per_layer=(32, ),
#     activation_func_hidden='sigmoid',
#     num_neurons_out_layer=1,
#     activation_func_output="linear",
#     opt_algo="SGD",
#     loss_func="mean_squared_error",
#     learning_rate=0.01,
#     num_epochs=200)

# nn1.DataLoader()
# print(nn1.train_feat_batch.__len__())
# print(nn1.train_feat_batch[0].shape)

# nn1.train()
# print(nn1.layer_weights)

# nn1.fit()
# nn1.showAccu("1A")
# nn1.plot("1A")

# print('\n')

nn2 = NeuralNetwork(
    num_hidden=2,
    num_neurons_per_layer=(64, 32),
    activation_func_hidden='relu',
    num_neurons_out_layer=1,
    activation_func_output="linear",
    opt_algo="SGD",
    loss_func="categorial_cross_entropy_loss",
    learning_rate=0.01,
    num_epochs=200)

nn2.DataLoader()
nn2.train()
