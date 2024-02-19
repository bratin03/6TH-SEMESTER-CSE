"""
Group Number: 23

Group Members:
    Bratin Mondal 
    21CS10016

    Kapil Bhanwariwal
    23BM6JP20

------------------------

Project Number: 1

Project Title: Medical Insurance Approval using Decision Tree based Learning Model
"""

# Import the required libraries
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier, export_graphviz
from sklearn.metrics import accuracy_score, precision_recall_fscore_support
from graphviz import Source

# Load the training data
train_data = pd.read_csv('mc_train_formatted.csv')

# Load the test data
test_data = pd.read_csv('mc_test_formatted.csv')

test_data.rename(columns={'ACC_D': 'ACC-H'}, inplace=True)

X_train = train_data.drop('Class', axis=1)
y_train = train_data['Class'].astype(str)  # Convert class labels to string

X_test = test_data.drop('Class', axis=1)
y_test = test_data['Class'].astype(str)  # Convert class labels to string

# Initialize the DecisionTreeClassifiers with different criteria
decision_tree_gini = DecisionTreeClassifier(criterion='gini', random_state=42)
decision_tree_entropy = DecisionTreeClassifier(criterion='entropy', random_state=42)

# Train the decision trees on the training data
decision_tree_gini.fit(X_train, y_train)
decision_tree_entropy.fit(X_train, y_train)

# Make predictions on the test set for both trees
y_pred_gini = decision_tree_gini.predict(X_test)
y_pred_entropy = decision_tree_entropy.predict(X_test)

# Evaluate the accuracy of the models
accuracy_gini = accuracy_score(y_test, y_pred_gini)
accuracy_entropy = accuracy_score(y_test, y_pred_entropy)

# Display the accuracy of the models
print(f"Accuracy (Gini): {accuracy_gini:.4f}")
print(f"Accuracy (Entropy): {accuracy_entropy:.4f}")

# Calculate precision, recall, f1-score, and support for each class
metrics_gini = precision_recall_fscore_support(y_test, y_pred_gini, average=None)
metrics_entropy = precision_recall_fscore_support(y_test, y_pred_entropy, average=None)

# Display metrics for each class
classes = decision_tree_gini.classes_.astype(str)  # Convert class labels to string
for i, class_name in enumerate(classes):
    print(f"\nMetrics for class {class_name} (Gini):")
    print(f"Precision: {metrics_gini[0][i]:.4f}")
    print(f"Recall: {metrics_gini[1][i]:.4f}")
    print(f"F1-score: {metrics_gini[2][i]:.4f}")
    print(f"Support: {metrics_gini[3][i]}")

    print(f"\nMetrics for class {class_name} (Entropy):")
    print(f"Precision: {metrics_entropy[0][i]:.4f}")
    print(f"Recall: {metrics_entropy[1][i]:.4f}")
    print(f"F1-score: {metrics_entropy[2][i]:.4f}")
    print(f"Support: {metrics_entropy[3][i]}")

# Visualize the decision trees using graphviz
dot_data_gini = export_graphviz(decision_tree_gini, out_file=None, 
                                feature_names=X_train.columns.tolist(),  
                                class_names=classes,  
                                filled=True, rounded=True, special_characters=True)

graph_gini = Source(dot_data_gini)
graph_gini.render("decision_tree_visualization_gini", format='png', cleanup=True)

dot_data_entropy = export_graphviz(decision_tree_entropy, out_file=None, 
                                   feature_names=X_train.columns.tolist(),  
                                   class_names=classes,  
                                   filled=True, rounded=True, special_characters=True)

graph_entropy = Source(dot_data_entropy)
graph_entropy.render("decision_tree_visualization_entropy", format='png', cleanup=True)

graph_gini.view()
graph_entropy.view()
