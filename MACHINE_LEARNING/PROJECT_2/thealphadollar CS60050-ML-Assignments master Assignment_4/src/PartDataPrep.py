import pandas as pd
import math

# Read the CSV file
df = pd.read_csv('../data/song.csv', skiprows=1, header=None)

# Drop the first column
df.drop(df.columns[0], axis=1, inplace=True)
df.drop_duplicates(inplace=True)

# Convert the data type of remaining columns to float
df = df.astype(float)
first_column = df.pop(1)
df[len(df.columns)+2] = first_column
df.columns = range(len(df.columns))

# print(df.head())
for i in range(0, len(df.columns)-1):
    df[i] = (df[i]-df[i].min())/(df[i].max()-df[i].min())

test_data = df.sample(frac=0.2, random_state=200)
train_data = df.drop(test_data.index)

train_data.to_csv('../data/training_data_prep.csv', index=False, header=False)
test_data.to_csv('../data/testing_data_prep.csv', index=False, header=False)
