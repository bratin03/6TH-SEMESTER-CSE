import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def Data_loader():

    # load test and train datasets
    X_train = np.load('./data/X_train.npy')
    y_train = np.load('./data/y_train.npy')
    X_test = np.load('./data/X_test.npy')
    y_test = np.load('./data/y_test.npy')
    
    return X_train.T, X_test.T, y_train.T, y_test.T


def Weight_initialiser(layer_dims):
    
    weights = {} 			   # dictionary of weights
    L = len(layer_dims)            # number of layers in the network

    for l in range(1, L):
        weights['W' + str(l)] = np.random.uniform(-1,1,(layer_dims[l], layer_dims[l - 1]))
        weights['b' + str(l)] = np.ones((layer_dims[l], 1))

    return weights


def lin_fwd(A, W, b):

	# single pass linear layer
    Z = np.dot(W, A) + b
    
    param_dict = (A, W, b)
    
    return Z, param_dict



def sigmoid(s):
    return 1/(1 + np.exp(-s)), s

def relu(s):
    return np.maximum(s,0), s

def softmax(s):
    exps = np.exp(s)
    return exps/np.sum(exps, axis=0, keepdims=True), s




def lin_activation_fwd(A_prev, W, b, activation):
    
    # single pass activation layer
    if activation == "sigmoid":
        Z, linear_dict = lin_fwd(A_prev, W, b)
        A, activation_dict = sigmoid(Z)
        
    elif activation == "relu":
        Z, linear_dict = lin_fwd(A_prev, W, b)
        A, activation_dict = relu(Z)
     
    elif activation == "softmax":
        Z, linear_dict = lin_fwd(A_prev, W, b)
        A, activation_dict = softmax(Z)
    
    cache = (linear_dict, activation_dict)

    return A, cache



def Forward_pass(X, weights,activate):

    # Forward propagation function
    caches = []
    A = X
    L = len(weights) // 2                  # number of layers in the neural network
    
    for l in range(1, L):
        A_prev = A 
        
        A, cache = lin_activation_fwd(A_prev, weights['W' + str(l)], weights['b' + str(l)], activation=activate)
        caches.append(cache)
      
    AL, cache = lin_activation_fwd(A, weights['W' + str(L)], weights['b' + str(L)], activation='softmax')
    caches.append(cache)
            
    return AL, caches


def Cost_Calculator(AL, Y):
    
    m = Y.shape[1]
    
    cost = (1 / m) * np.array(Y,dtype=np.int64).T[:]*-np.log(AL).T[:]
   
    return np.sum(np.sum(cost,axis = 1))


def ReLU_backprop(dA, dictionary):

    # single pass backpropagation for relu activation layer

    Z = dictionary
    dZ = np.array(dA, copy=True) 

    dZ[Z <= 0] = 0
    
    return dZ

def Sigmoid_backprop(dA, dictionary):
    
    # single pass backpropagation for sigmoid activation layer

    Z = dictionary
    
    s = 1/(1+np.exp(-Z))
    dZ = dA * s * (1-s)
    
    return dZ

def Softmax_backprop(pred, real):  

	# single pass backpropagation for sigmoid activation layer

    n_samples = real.shape[1]
    res = pred - real
    
    return res


def Linear_backprop(dZ, dictionary):

	# single pass backpropagation for linear layer

    A_prev, W, b = dictionary
    m = A_prev.shape[1]

    dW = np.dot(dZ, dictionary[0].T) / m
    db = np.squeeze(np.sum(dZ, axis=1, keepdims=True)) / m
    dA_prev = np.dot(dictionary[1].T, dZ)
   
    return dA_prev, dW, db


def lin_activation_bwd(dA, dictionary_cache, activation):

	# single pass backpropagation for activation layer
    
    linear_dictionary_cache, activation_dictionary_cache = dictionary_cache
    
    if activation == "relu":
        dZ = ReLU_backprop(dA, activation_dictionary_cache)
    
    elif activation == "sigmoid":
        dZ = Sigmoid_backprop(dA, activation_dictionary_cache)
    
    dA_prev, dW, db = Linear_backprop(dZ, linear_dictionary_cache)
    
    return dA_prev, dW, db


def Backpropagation(AL, Y, dictionary_cache, activation):

	# Backpropagation Function
 
    grads = {}
    L = len(dictionary_cache) # the number of layers
    m = AL.shape[1]
    Y = Y.reshape(AL.shape) 
    
    dAL = Softmax_backprop(AL,Y)  
    
    current_cache = dictionary_cache[-1]
    grads["dA" + str(L)], grads["dW" + str(L)], grads["db" + str(L)] = Linear_backprop(dAL, current_cache[0])
    
    for l in reversed(range(L-1)):
        current_cache = dictionary_cache[l]
        if activation == 'sigmoid':
            dA_prev_temp, dW_temp, db_temp = Linear_backprop(Sigmoid_backprop(grads["dA" + str(l + 2)], current_cache[1]), current_cache[0])
        elif activation == 'relu':
            dA_prev_temp, dW_temp, db_temp = Linear_backprop(ReLU_backprop(grads["dA" + str(l + 2)], current_cache[1]), current_cache[0])
        grads["dA" + str(l + 1)] = dA_prev_temp
        grads["dW" + str(l + 1)] = dW_temp
        grads["db" + str(l + 1)] = db_temp

    return grads


def params_update(weights, grads, learning_rate = 0.01):
    
    #Update weights using gradient descent
      
    L = len(weights) // 2 # number of layers in the neural network

    for l in range(L):
        weights["W" + str(l + 1)] = weights["W" + str(l + 1)] - learning_rate * (grads["dW" + str(l + 1)])
        weights["b" + str(l + 1)] = weights["b" + str(l + 1)] - learning_rate * (grads["db" + str(l + 1)].reshape(-1,1))
   
    return weights


def create_random_mini_batches(X, Y, mini_batch_size = 64, seed = 0):
    
    # create random minibatches for Mini-Batch SGD 

    np.random.seed(seed)            
    m = X.shape[1]                  
    mini_batches = []
        
    perm = list(np.random.permutation(m))
    shuffled_X = X[:, perm]
    shuffled_Y = Y[:, perm].reshape((3,m))

    num_full_minibatches = int(np.math.floor(m/mini_batch_size)) # number of mini batches of size mini_batch_size in your partitionning
    for k in range(0, num_full_minibatches):
        mini_batch_X = shuffled_X[:,k * mini_batch_size:(k + 1) * mini_batch_size]
        mini_batch_Y = shuffled_Y[:,k * mini_batch_size:(k + 1) * mini_batch_size]
        mini_batch = (mini_batch_X, mini_batch_Y)
        mini_batches.append(mini_batch)
    
    if m % mini_batch_size != 0:
        end = m - mini_batch_size * int(np.math.floor(m/mini_batch_size))
        mini_batch_X = shuffled_X[:,num_full_minibatches * mini_batch_size:]
        mini_batch_Y = shuffled_Y[:,num_full_minibatches * mini_batch_size:]
        mini_batch = (mini_batch_X, mini_batch_Y)
        mini_batches.append(mini_batch)
    
    return mini_batches


def Predict(X,Y,parameters,activation):
    AL, catch = Forward_pass(X, parameters, activation)
    a = np.argmax(AL.T,axis = 1)
    b = np.argmax(Y.T,axis = 1)
    
    return (a == b).mean()*100


def Training(X, Y, X_t, Y_t, fname, layers_dims, activation, learning_rate=0.01, mini_batch_size=32, n_epochs=200):
    
    # Mini Batch Stochastic Gradient Descent Optimizer

    L = len(layers_dims)             # number of layers in the neural networks
    costs = []                       
    t = 0                           
    seed = 10 
    
    train_acc = []
    test_acc = []
    
    # Initialize weights
    weights = Weight_initialiser(layers_dims)

    # Optimization loop
    for i in range(n_epochs+1):
        
        # Define the random minibatches. We increment the seed to reshuffle differently the dataset after each epoch
        seed = seed + 1
        minibatches = create_random_mini_batches(X, Y, mini_batch_size, seed)

        for minibatch in minibatches:

            # Select a minibatch
            (mini_X, mini_Y) = minibatch

            # Forward propagation
            a3, caches = Forward_pass(mini_X, weights, activation)

            # Compute cost
            cost = Cost_Calculator(a3, mini_Y)

            # Backward propagation
            grads = Backpropagation(a3, mini_Y, caches, activation)

            # Update weights
            weights = params_update(weights, grads, learning_rate)
            
        # Store the cost every 10th epoch
        if i % 10 == 0:
            costs.append(cost)
            train_acc.append(Predict(X, Y, weights, activation))
            test_acc.append(Predict(X_t, Y_t, weights, activation))
                
    plt.figure()
    plt.plot(np.arange(0,21),train_acc,label='Train')
    plt.plot(np.arange(0,21),test_acc,label='Test')
    plt.ylabel('Predict')
    plt.xlabel('Epochs (per 10)')
    plt.title("Learning rate = " + str(learning_rate))
    plt.legend()
    plt.savefig(fname+'.jpg')
    plt.show()

    return costs, train_acc, test_acc, weights


print("Part 1A:")
# NN 1
layer_dims = [7,32,3]
X_train, X_test, y_train, y_test = Data_loader()
costs, train_acc, test_acc, parameters = Training(X_train, y_train, X_test, y_test, 'NN1a', layer_dims, activation = 'sigmoid')
acc = Predict(X_train, y_train, parameters, 'sigmoid')
print("Final Training Accuracy = "+str(acc)+"%")
acc = Predict(X_test, y_test, parameters, 'sigmoid')
print("Final Test Accuracy = "+str(acc)+"%")

print("\n")

print("Part 1B:")
# NN 2
layer_dims = [7,64,32,3]
X_train, X_test, y_train, y_test = Data_loader()
costs, train_acc, test_acc, parameters = Training(X_train, y_train, X_test, y_test, 'NN1b', layer_dims, activation = 'relu')
acc = Predict(X_train, y_train, parameters, 'relu')
print("Final Training Accuracy = "+str(acc)+"%")
acc = Predict(X_test, y_test, parameters, 'relu')
print("Final Test Accuracy = "+str(acc)+"%")

