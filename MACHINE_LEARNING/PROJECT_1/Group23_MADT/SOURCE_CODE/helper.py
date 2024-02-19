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
import pandas as pd
import random
import time
import math
import json
import os


def read_csv(fp):
    """
    Function Name: read_csv
    Input: File Path
    Output: DataFrame
    Purpose: Read the CSV file and return the DataFrame
    """
    data = pd.read_csv(fp)
    return data


def process_data(data):
    """
    Function Name: process_data
    Input: DataFrame
    Output: List of Dictionaries
    Purpose: Process the DataFrame and return a list of dictionaries
    """
    processed_data = []
    for i in range(0, len(data)):
        acc_h_value = data['ACC-H'][i] if 'ACC-H' in data else data['ACC_D'][i]

        processed_data.append(
            {
                'Age':     data['Age'][i],
                'Smoke':   data['Smoke'][i],
                'BMI':     data['BMI'][i],
                'ACC-H':   acc_h_value,
                'Income':  data['Income'][i],
                'Med_H':   data['Med_H'][i],
                'Class':   data['Class'][i]
            }
        )
    return processed_data


def train_valid_split(data, split_ratio=0.8):
    """
    Function Name: train_valid_split
    Input: List of Dictionaries, Split Ratio
    Output: Tuple of Lists
    Purpose: Split the data into training and validation sets
    """
    seed = int(time.time())
    random.seed(seed)
    if not 0 <= split_ratio <= 1:
        raise ValueError("split_ratio must be between 0 and 1.")
    random.shuffle(data)
    split_index = int(split_ratio * len(data))
    if split_index == 0 or split_index == len(data):
        raise ValueError("Invalid split_ratio. Either too small or too large.")
    X_train, X_valid = data[:split_index], data[split_index:]
    return X_train, X_valid


def calculate_entropy(data):
    """
    Function Name: calculate_entropy
    Input: List
    Output: Float
    Purpose: Calculate the entropy of the given list
    """
    class_counts = {}
    for label in data:
        if label in class_counts:
            class_counts[label] += 1
        else:
            class_counts[label] = 1

    total_samples = len(data)
    probabilities = [count / total_samples for count in class_counts.values()]
    ent = -sum(p * math.log2(p) for p in probabilities if p > 0)
    return ent


def calculate_gini(data):
    """
    Function Name: calculate_gini
    Input: List
    Output: Float
    Purpose: Calculate the gini index of the given list
    """
    class_counts = {}
    for label in data:
        if label in class_counts:
            class_counts[label] += 1
        else:
            class_counts[label] = 1

    total_samples = len(data)
    probabilities = [count / total_samples for count in class_counts.values()]
    gini = 1 - sum(p ** 2 for p in probabilities)
    return gini


def calculate_information_gain(data, attribute, split):
    """
    Function Name: calculate_information_gain
    Input: List of Dictionaries, Attribute, Split Value
    Output: Float
    Purpose: Calculate the information gain of the given attribute and split value
    """
    subset1 = [row['Class'] for row in data if row[attribute] <= split]
    subset2 = [row['Class'] for row in data if row[attribute] > split]
    entropy1 = calculate_entropy(subset1)
    entropy2 = calculate_entropy(subset2)
    entropy = calculate_entropy([row['Class'] for row in data])
    ig = entropy - (len(subset1) / len(data) * entropy1 +
                    len(subset2) / len(data) * entropy2)
    return ig


def calculate_gini_index(data, attribute, split):
    """
    Function Name: calculate_gini_index
    Input: List of Dictionaries, Attribute, Split Value
    Output: Float
    Purpose: Calculate the gini index of the given attribute and split value
    """
    subset1 = [row['Class'] for row in data if row[attribute] <= split]
    subset2 = [row['Class'] for row in data if row[attribute] > split]
    gini1 = calculate_gini(subset1)
    gini2 = calculate_gini(subset2)
    gini = calculate_gini([row['Class'] for row in data])
    ig = gini - (len(subset1) / len(data) * gini1 +
                 len(subset2) / len(data) * gini2)
    return ig


def select_attribute(data, attribute_list, criterion='entropy'):
    """
    Function Name: select_attribute
    Input: List of Dictionaries, List of Attributes, Criterion
    Output: Tuple
    Purpose: Select the best attribute and split value based on the criterion
    """
    max_gain = 0
    max_attr = None
    max_split = None
    random.shuffle(attribute_list)
    for attr in attribute_list:
        unique_values = set(entry[attr] for entry in data)

        for split in unique_values:
            if criterion == 'entropy':
                ig = calculate_information_gain(data, attr, split)
            elif criterion == 'gini':
                ig = calculate_gini_index(data, attr, split)

            if ig >= max_gain:
                max_gain = ig
                max_attr = attr
                max_split = split

    return max_attr, max_split, calculate_entropy([row['Class'] for row in data])


def convert_to_label(prediction):
    """
    Function Name: convert_to_label
    Input: Integer
    Output: String
    Purpose: Convert the integer prediction to a string label
    """
    if prediction == 0:
        return 'bad'
    elif prediction == 1:
        return 'ok'
    elif prediction == 2:
        return 'good'
    elif prediction == 3:
        return 'vgood'
    else:
        return 'unknown'


with open('reverse_label_mapping.json', 'r') as file:
    # Load the reverse mapping from the file
    reverse_mapping = json.load(file)


def write_metrics_to_file(metrics_dict, tree_max_height, tree_actual_height, output_file_path="./Output/metrics.txt"):
    """
    Function Name: write_metrics_to_file
    Input: Dictionary, Integer, Integer, String
    Output: None
    Purpose: Write the metrics to the output file
    """
    output_directory = os.path.dirname(output_file_path)
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    with open(output_file_path, 'a+') as file:
        file.write(
            f"Tree Height: {tree_max_height}\nTree Actual Height: {tree_actual_height}\n")
        for label, metrics in metrics_dict.items():
            file.write(f"Class {label} Metrics:\n")
            file.write(f"  Precision: {metrics['precision']:.4f}\n")
            file.write(f"  Recall: {metrics['recall']:.4f}\n")
            file.write(f"  F1-Score: {metrics['f1_score']:.4f}\n")
            file.write(f"  Support: {metrics['support']}\n")
            file.write(f"  Accuracy: {metrics['accuracy']:.4f}\n\n")
            if (label != 3):
                file.write(
                    f"Tree Height: {tree_max_height}\nTree Actual Height: {tree_actual_height}\n")
