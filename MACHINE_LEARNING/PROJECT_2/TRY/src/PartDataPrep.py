#!/usr/bin/env python
# coding: utf-8
# @author: Shivam Kumar Jha
# @rollno: 17CS30033

import pandas as pd

with open('../data/seeds_dataset.txt', 'r') as data:
    with open('../data/seeds_dataset.csv', 'w+') as wData:
        for line in data.readlines():
            wData.write(','.join(line.split()))
            wData.write('\n')

df = pd.read_csv('../data/song.csv', header=None)
df.drop(df.index[0], inplace=True)
df.drop(df.columns[0], axis=1, inplace=True)
df = df.astype(float)
first_column = df.pop(1)
df[len(df.columns)+2] = first_column
df.columns = range(len(df.columns))
print(df.head())

for i in range(13):
    df[i] = (df[i] - df[i].mean())/df[i].std(ddof=0)
# df.head()

one_hot = pd.get_dummies(df[13], prefix='_')
df = df.drop(13, axis=1)
df = df.join(one_hot)
# df.head()


train = df.sample(frac=0.8, random_state=200)
test = df.drop(train.index)

train.to_csv('../data/training_data.csv', header=None, index=False)
test.to_csv('../data/testing_data.csv', header=None, index=False)

print(f"length of training data: {len(train)}")
print(f"shape of training data: {train.shape}")
