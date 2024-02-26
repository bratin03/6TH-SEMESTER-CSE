#!/usr/bin/env python
# coding: utf-8

# In[1]:


import os
import numpy as np
import pandas as pd
from scipy import stats
from sklearn.model_selection import train_test_split


# In[2]:


data = np.loadtxt('./data/data.txt')


# In[3]:


X = data[:,:7]


# In[4]:


y = np.array(data[:,7],dtype = np.int64)


# In[5]:


# one hot vector from y
b = np.zeros((y.size, y.max()))
b[np.arange(y.size),y-1] = 1


# In[6]:


y = b


# In[7]:


# z-score normalization
X = stats.zscore(X,axis = 1)


# In[8]:


X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.20, random_state=42)


# In[9]:


np.save('./data/X_train',X_train)
np.save('./data/y_train',y_train)
np.save('./data/X_test',X_test)
np.save('./data/y_test',y_test)
np.save('./data/X',X)
np.save('./data/y',y)

