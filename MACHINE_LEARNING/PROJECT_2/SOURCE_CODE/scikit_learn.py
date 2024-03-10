from sklearn.neural_network import MLPRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
import numpy as np

# Load the training and testing datasets
train = np.loadtxt("../DATASET/train.csv", delimiter=",")
test = np.loadtxt("../DATASET/test.csv", delimiter=",")
X_train, y_train = train[:, :-1], train[:, -1]
X_test, y_test = test[:, :-1], test[:, -1]


mlp = MLPRegressor(hidden_layer_sizes=(32), activation='logistic', solver='sgd', learning_rate='constant',
                   learning_rate_init=0.01, max_iter=200, random_state=23)
mlp.fit(X_train, y_train)

y_pred = mlp.predict(X_test)
y_pred_rounded = np.round(y_pred)
total = len(y_test)
correct = 0
for i in range(total):
    if y_pred_rounded[i] == y_test[i]:
        correct += 1

print("Accuracy: ", correct/total)

