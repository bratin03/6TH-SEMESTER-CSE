import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


class NeuralNetwork:
    def __init__(self,
                 num_hidden,
                 num_neurons_per_layer,
                 activation_func_hidden,
                 num_neurons_out_layer=1,
                 activation_func_output="Linear",
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

        def fit(self):
            self.dataLoader()

        def dataLoader(self):
            self.train_data = pd.read_csv(
                '../data/training_data_prep.csv', header=None)
            self.test_data = pd.read_csv(
                '../data/test_data_prep.csv', header=None)

            self.train_data_feats = self.train_data.iloc[:, :-1].to_numpy()
            self.train_data_labels = self.train_data.iloc[:, -1].to_numpy()
            self.test_data_feats = self.test_data.iloc[:, :-1].to_numpy()
            self.test_data_labels = self.test_data.iloc[:, -1].to_numpy()

            self.train_feat_batch= self.dfSplit(self.train_data_feats)
            self.train_label_batch= self.dfSplit(self.train_data_labels)
            self.test_feat_batch= self.dfSplit(self.test_data_feats)
            self.test_label_batch= self.dfSplit(self.test_data_labels)

        @staticmethod
        def dfSplit(df):
            df_list = []
            numChunks = len(df)//32+1
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
            x[x<=0] = 0
            x[x>0] = 1
            return x
        
        @staticmethod
        def relu(x):
            x[x<=0] = 0
            return x


nn= NeuralNetwork(num_hidden=1,num_neurons_per_layer=32,activation_func_hidden='sigmoid',num_neurons_out_layer=1,activation_func_output="Linear",opt_algo="SGD",loss_func="mean_squared_error",learning_rate=0.01,num_epochs=200)