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

# Importing required libraries
from helper import *
import graphviz
class node:
    """
    Class Name: node
    Purpose: This class is used to create a node for the decision tree
    """
    def __init__(self, attr, split, entropy, is_leaf, class_counts=None):
        """
        method Name: __init__
        Input: attr, split, entropy, is_leaf, class_counts
        Output: None
        Purpose: Initialize the node with the given parameters
        """
        self.attr = attr
        self.split = split
        self.entropy = entropy
        self.is_leaf = is_leaf
        self.left = None
        self.right = None
        self.class_counts = class_counts if class_counts is not None else [
            0, 0, 0, 0]

    def clone(self):
        """
        method Name: clone
        Input: None
        Output: node
        Purpose: Return a clone of the current node
        """
        new_node = node(self.attr, self.split, self.entropy, self.is_leaf, self.class_counts)
        if not self.is_leaf:
            new_node.left = self.left.clone() if self.left else None
            new_node.right = self.right.clone() if self.right else None
        return new_node

    def set_class_counts(self, class_counts):
        """
        method Name: set_class_counts
        Input: class_counts
        Output: None
        Purpose: Set the class_counts of the node
        """
        self.class_counts = class_counts

    def node_count(self):
        """
        method Name: node_count
        Input: None
        Output: int
        Purpose: Return the number of nodes in the tree
        """
        if self == None:
            return 0
        if self.is_leaf:
            return 1
        else:
            return 1 + self.left.node_count() + self.right.node_count()
        
    def tree_height(self):
        """
        method Name: tree_height
        Input: None
        Output: int
        Purpose: Return the height of the tree
        """
        if self == None:
            return 0
        if self.is_leaf:
            return 1
        else:
            return 1 + max(self.left.tree_height(), self.right.tree_height())

    def remove_children(self):
        """
        method Name: remove_children
        Input: None
        Output: None
        Purpose: Remove the children of the node
        """
        self.left = None
        self.right = None
        self.is_leaf = True
        self.attr = 'Class'
        self.split = self.class_counts.index(max(self.class_counts))

    def restore_children(self, store_left, store_right, store_attr, store_split):
        """
        method Name: restore_children
        Input: store_left, store_right, store_attr, store_split
        Output: None
        Purpose: Restore the children of the node
        """
        self.left = store_left
        self.right = store_right
        self.is_leaf = False
        self.attr = store_attr
        self.split = store_split


def Decision_tree(samples, attribute_list, current_height, maximum_height, criterion='entropy'):
    """
    Function Name: Decision_tree
    Input: List of Dictionaries, List of Attributes, Current Height, Maximum Height, Criterion
    Output: node
    Purpose: Create a decision tree based on the given samples and attributes
    """
    if current_height == maximum_height or len(samples) == 0:
        return None
    if len(samples) == 1 or calculate_entropy([row['Class'] for row in samples]) == 0:
        class_counts = [0, 0, 0, 0]
        for row in samples:
            class_counts[row['Class']] += 1
        class_value = class_counts.index(max(class_counts))

        return node('Class', class_value, 0, True, class_counts)

    attribute_select, split_select, entropy = select_attribute(
        samples, attribute_list, criterion)
    samples_left = [
        row for row in samples if row[attribute_select] <= split_select]
    samples_right = [
        row for row in samples if row[attribute_select] > split_select]

    parent = node(attribute_select, split_select, entropy, False)

    parent.left = Decision_tree(
        samples_left, attribute_list, current_height+1, maximum_height)
    parent.right = Decision_tree(
        samples_right, attribute_list, current_height+1, maximum_height)

    class_values = [i['Class'] for i in samples]
    parent.set_class_counts([class_values.count(0), class_values.count(
        1), class_values.count(2), class_values.count(3)])

    if parent.left == None and parent.right == None:
        parent.is_leaf = True
        max_class_value = max(set(class_values), key=class_values.count)
        parent.split = max_class_value
        parent.attr = 'Class'

    return parent


def prune_tree(self, Decision_tree_Root, validation_set):
    """
    Function Name: prune_tree
    Input: node, Decision_tree_Root, validation_set
    Output: None
    Purpose: Prune the decision tree based on the given validation set
    """
    if self.left == None and self.right == None:
        return
    if self.left != None:
        prune_tree(self.left, Decision_tree_Root, validation_set)
    if self.right != None:
        prune_tree(self.right, Decision_tree_Root, validation_set)

    current_error = calculate_batch_error(Decision_tree_Root, validation_set)
    store_left = self.left
    store_right = self.right
    store_attr = self.attr
    store_split = self.split
    self.remove_children()
    new_error = calculate_batch_error(Decision_tree_Root, validation_set)

    if new_error > current_error:
        self.restore_children(store_left, store_right, store_attr, store_split)


def predict_sample(node, sample):
    """
    Function Name: predict_sample
    Input: node, sample
    Output: int
    Purpose: Predict the class of the given sample based on the decision tree
    """
    if node.is_leaf:
        return node.split
    if node.left == None:
        return predict_sample(node.right, sample)
    if node.right == None:
        return predict_sample(node.left, sample)
    if sample[node.attr] <= node.split:
        return predict_sample(node.left, sample)
    else:
        return predict_sample(node.right, sample)


def predict_batch(node, samples):
    """
    Function Name: predict_batch
    Input: node, List of Dictionaries
    Output: List of Integers
    Purpose: Predict the classes of the given samples based on the decision tree
    """
    predictions = []
    for sample in samples:
        predictions.append(predict_sample(node, sample))
    return predictions


def calculate_accuracy(predictions, labels):
    """
    Function Name: calculate_accuracy
    Input: List of Integers, List of Integers
    Output: Float
    Purpose: Calculate the accuracy of the given predictions
    """
    if len(predictions) != len(labels):
        raise ValueError("Length of predictions and labels must be the same.")

    correct_predictions = sum(1 for pred, label in zip(
        predictions, labels) if pred == label)
    total_samples = len(predictions)

    accuracy = correct_predictions / total_samples
    return accuracy


def calculate_batch_error(node, samples):
    """
    Function Name: calculate_batch_error
    Input: node, List of Dictionaries
    Output: Float
    Purpose: Calculate the error of the given samples based on the decision tree
    """
    predictions = predict_batch(node, samples)
    labels = [row['Class'] for row in samples]
    return 1-calculate_accuracy(predictions, labels)


def calculate_accuracy_test(predictions, labels):
    """
    Function Name: calculate_accuracy_test
    Input: List of Integers, List of Integers
    Output: Float
    Purpose: Calculate the accuracy of the given predictions
    """
    correct = 0
    with open('predicted.txt', 'w') as file:
        for i in range(len(predictions)):
            prediction_label = convert_to_label(predictions[i])
            actual_label = convert_to_label(labels[i])
            is_correct = predictions[i] == labels[i]

            file.write(
                f"Prediction: {prediction_label}, Actual: {actual_label}, {'Correct' if is_correct else 'Wrong'}\n")

            if is_correct:
                correct += 1

    accuracy = correct / len(predictions)
    return accuracy


def calculate_metrics(predicted, actual, class_labels={0, 1, 2, 3}):
    """
    Function Name: calculate_metrics
    Input: List of Integers, List of Integers, Set of Integers
    Output: Dictionary
    Purpose: Calculate the precision, recall, f1_score, support and accuracy of the given predictions
    """
    metrics = {}

    for label in class_labels:
        true_positive = sum((a == label and p == label)
                            for a, p in zip(actual, predicted))
        false_positive = sum((a != label and p == label)
                             for a, p in zip(actual, predicted))
        false_negative = sum((a == label and p != label)
                             for a, p in zip(actual, predicted))

        precision = true_positive / max(1, (true_positive + false_positive))
        recall = true_positive / max(1, (true_positive + false_negative))
        f1_score = 2 * (precision * recall) / max(1, (precision + recall))
        support = sum(a == label for a in actual)
        accuracy= calculate_accuracy(predicted, actual)

        metrics[label] = {
            'precision': precision,
            'recall': recall,
            'f1_score': f1_score,
            'support': support,
            'accuracy': accuracy
        }

    return metrics

with open('reverse_label_mapping.json', 'r') as f:
    # Load the reverse mapping from the file
    reverse_label_mapping = json.load(f)

def get_label(attribute, value):
    """
    Function Name: get_label
    Input: String, Any
    Output: String
    Purpose: Get the label of the given value for the given attribute
    """
    return reverse_label_mapping[attribute][str(value)]

def visualize_tree(node, graph, parent_node_id='root'):
    """
    Function Name: visualize_tree
    Input: node, graph, parent_node_id
    Output: None
    Purpose: Visualize the decision tree
    """
    if node.is_leaf:

        class_label = get_label("Class", node.split)
        graph.node(parent_node_id, class_label, shape='box', fillcolor='lightgreen', style='filled')
    else:

        attr_label = f"{node.attr} <= {get_label(node.attr, node.split)}\nEntropy: {node.entropy}"
        graph.node(parent_node_id, attr_label, shape='ellipse', fillcolor='lightblue', style='filled')

        left_child_id = f"{parent_node_id}_L"
        right_child_id = f"{parent_node_id}_R"
        graph.edge(parent_node_id, left_child_id, 'True')
        graph.edge(parent_node_id, right_child_id, 'False')
        visualize_tree(node.left, graph, left_child_id)
        visualize_tree(node.right, graph, right_child_id)


