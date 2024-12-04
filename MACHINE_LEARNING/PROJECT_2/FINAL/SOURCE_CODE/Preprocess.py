"""
Group Number: 29
Student Name: Bratin Mondal
Roll Number: 21CS10016
Project Title: Song Popularity using Neural Networks
"""

import pandas as pd
import time
def Preprocess(input_file="../DATASET/song.csv", randSeed=int(time.time())):
    """
    Module Name: Preprocess
    Purpose: To preprocess the input dataset and split it into training and testing datasets
    Input: input_file (str) -> Path to the input dataset
           randSeed (int) -> Random seed for reproducibility
    Output: None
    Functionality:
        1. Read the input dataset
        2. Drop the first column and remove duplicates
        3. Normalize the dataset
        4. Split the dataset into training and testing datasets
        5. Save the training and testing datasets
    """
    df = pd.read_csv(input_file, skiprows=1, header=None)
    df.drop(df.columns[0], axis=1, inplace=True)
    df.drop_duplicates(inplace=True)

    df = df.astype(float)
    first_column = df.pop(1)
    df[len(df.columns)+2] = first_column
    df.columns = range(len(df.columns))

    for i in range(0, len(df.columns)-1):
        df[i] = (df[i] - df[i].mean())/df[i].std()

    train = df.sample(frac=0.8, random_state=randSeed)
    test = df.drop(train.index)

    train.to_csv("../DATASET/train.csv", index=False, header=False)
    test.to_csv("../DATASET/test.csv", index=False, header=False)


if __name__ == "__main__":
    Preprocess()
