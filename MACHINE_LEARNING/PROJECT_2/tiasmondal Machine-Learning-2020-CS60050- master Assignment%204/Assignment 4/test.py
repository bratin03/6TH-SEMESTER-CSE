import numpy as np
import sys
import csv
import pandas as pd
import random

mean=np.zeros(14,)
squared=np.zeros(14,)
sd=np.zeros(14,)


def preprocess(data,test_train_split=0.8):
    # shuffle
    np.random.shuffle(data)
    dataset_size = data.shape[0]
    for i in range(0,14):
        mean[i]=np.sum(data[:,i])/dataset_size
        sd[i]=(np.sum((data[:,i]-mean[i])**2)/dataset_size)**0.5
        data[:,i]=(data[:,i]-mean[i])/sd[i]
    train_data=data[0:int(dataset_size*test_train_split),:14]
    test_data=data[int(dataset_size*test_train_split):,:14]
    train_label=data[0:int(dataset_size*test_train_split),14:15]
    test_label=data[int(dataset_size*test_train_split):,14:15]
    return train_data,train_label,test_data,test_label

mini_batch=[]
mini_batch_labels=[]

def data_loader(data,label):
    count=0
    k=0
    batch_size = 32
    dataset_size = data.shape[0]
    num_batches = int(dataset_size/batch_size)
    print(f"num_batches: {num_batches}")
    for i in range(0,num_batches+1):
        mini_batch.append(data[k:k+batch_size,:])
        mini_batch_labels.append(label[k:k+batch_size,:])
        # print(mini_batch[i].shape)
        k=k+batch_size
        count=count+1

def weight_initializer():
    w1=2*np.random.rand(32,14)-1
    w2=2*np.random.rand(1,33)-1
    return w1,w2

def softmax(arr):
    tot=np.sum(np.exp(arr))
    return(np.exp(arr)/tot)

def forward_pass(weight,x,status):
    x=np.insert(x,0,1,axis=0)
    if(status=='hidden'):
        return(1/(1+np.exp(-weight.dot(x))))
    else:
        return(weight.dot(x))
    
def backprop(weight1,weight2,input1,out1,out2,label,alpha):
    delta2=-1*(label-out2)
    delta1=(out1*(1-out1))*np.transpose(weight2).dot(delta2)
    return delta1,delta2

def oneinsert(array):
    array=np.insert(array,1,0,axis=0)
    return array

def one_hot(n):
    x=np.zeros((1,1))
    x[n]=1
    return x

def train_batch(minibatch,minibatch_labels,w1,w2,alpha,test_data,test_label,train_data,train_label):
    tr=[]
    te=[]
    x=[]
    prevw1=w1
    prevw2=w2
    total_batch_size = len(minibatch)
    for epochs in range(0,200):
        error=0
        cap_delta0=0
        cap_delta1=0
        cap_delta2=0
        for i in range(0,total_batch_size):
            if(i==total_batch_size-1):
                batch_size = len(minibatch[i])
            else:
                batch_size = 32
            for j in range(0,batch_size):
                wl=forward_pass(w1,oneinsert(minibatch[i][j]),'hidden')
                wl=forward_pass(w2,oneinsert(wl),'output')
                error=error+np.sum((wl-minibatch_labels[i][j])**2)

                wl=np.insert(wl,0,1,axis=0)

def read_dataset(file_path):
    data = []
    with open(file_path, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip header
        for row in reader:
            # Convert all attributes except the first to float
            attributes = [float(value) for value in row[1:]]
            # Move popularity to the last position
            popularity = attributes.pop(0)
            attributes.append(popularity)
            data.append(attributes)
    return np.array(data)



if __name__ == "__main__":
    file_path = "song.csv"
    data = read_dataset(file_path)
    # print(data)
    train_data, train_label, test_data, test_label = preprocess(data)
    # print(train_data)
    # print(train_label)
    # print(test_data)
    # print(test_label)
    data_loader(train_data, train_label)

    
