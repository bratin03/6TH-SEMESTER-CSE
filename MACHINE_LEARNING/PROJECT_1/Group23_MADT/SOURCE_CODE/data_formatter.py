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
import json

def label_encode_columns(df, label_mappings):
    """
    Function Name: label_encode_columns
    Input: DataFrame, Label Mappings
    Output: DataFrame
    Purpose: Label encode the columns in the DataFrame based on the label mappings
    """
    for col, mapping in label_mappings.items():
        if col in df.columns:
            df[col] = df[col].map(mapping)

    return df

# Read the CSV file
input_file_path = "../DATASET/mc_test.csv"
df = pd.read_csv(input_file_path)

# Extract the file name without extension
file_name_without_extension = input_file_path.split('/')[-1].split('.')[0]

# Read label mappings from JSON file
json_file_path = "label_mappings.json"
with open(json_file_path, 'r') as json_file:
    label_mappings = json.load(json_file)

# Apply label encoding using the function
df_encoded = label_encode_columns(df, label_mappings)

# Generate the output file name based on the input file name
output_file_path = f"{file_name_without_extension}_formatted.csv"

# Save the cleaned DataFrame to the new CSV file
df_encoded.to_csv(output_file_path, index=False)
