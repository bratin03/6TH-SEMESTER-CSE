
import numpy as np
from sklearn import neural_network
from sklearn import model_selection
from sklearn.model_selection import cross_val_score
from sklearn.metrics import classification_report, confusion_matrix

def data_loader():

    # load test and train datasets
    X_train = np.load('./data/X_train.npy')
    y_train = np.load('./data/y_train.npy')
    X_test = np.load('./data/X_test.npy')
    y_test = np.load('./data/y_test.npy')
    return X_train.T, X_test.T, y_train.T, y_test.T

def sklearn_MLP_model(X_train,y_train,X_test,y_test,d):
	
	# Implement a Multi Layer Perceptron Model using sklearn library
    clf = neural_network.MLPClassifier(hidden_layer_sizes=(d['hidden_layer_sizes']), batch_size=d['batch_size'], activation=d['activation'], solver=d['solver'], learning_rate_init = d['learning_rate_init'], max_iter=d['max_iter'], random_state=d['random_state'])
    clf.fit(X_train.T,y_train.T)
    y_pred = clf.predict_proba(X_train.T)
    a = np.argmax(y_pred,axis = 1)
    b = np.argmax(y_train.T,axis = 1)
    train_acc = (a == b).mean()*100
    y_pred = clf.predict_proba(X_test.T)
    a = np.argmax(y_pred,axis = 1)
    b = np.argmax(y_test.T,axis = 1)
    test_acc = (a == b).mean()*100
    return clf,train_acc,test_acc


print("Part 2 Specification 1A:")
# NN1
X_train, X_test, y_train, y_test = data_loader()
d1 = {'activation': 'logistic', 'batch_size': 32, 'learning_rate_init': 0.01, 'hidden_layer_sizes': (32,), 'max_iter': 200, 'random_state': 42, 'solver': 'sgd'}
clf,train_acc,test_acc = sklearn_MLP_model(X_train,y_train,X_test,y_test,d1)
print("Final Training Accuracy = "+str(train_acc)+"%")
print("Final Training Accuracy = "+str(test_acc)+"%")

print("\n")

print("Part 2 Specification 1B:")
# NN2
X_train, X_test, y_train, y_test = data_loader()
d2 = {'activation': 'relu', 'batch_size': 32, 'learning_rate_init': 0.01, 'hidden_layer_sizes': (64,32,), 'max_iter': 200, 'random_state': 42, 'solver': 'sgd'}
clf,train_acc,test_acc = sklearn_MLP_model(X_train,y_train,X_test,y_test,d2)
print("Final Training Accuracy = "+str(train_acc)+"%")
print("Final Training Accuracy = "+str(test_acc)+"%")

