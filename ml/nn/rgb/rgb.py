#!/bin/env python3
#https://victorzhou.com/blog/intro-to-neural-networks/

import argparse
import matplotlib.pyplot as plt
import numpy as np
import pdb
import random
from sklearn.preprocessing import StandardScaler

#parser = argparse.ArgumentParser()
#parser.add_argument('do', required=True, choices=['train','validate','test'])
#args = parser.parse_args()

traincount = 1000
numInput = 1
numHidden = 4
numOutput = 3  # red, green, blue

def showtrainingdata(train):
    fig, ax = plt.subplots()
    ax.hist(train, bins=100)
    plt.show()
#    plt.savefig('training.png')

train = np.array([random.gauss(3000, 500) for x in range(traincount)])
train = np.append(train, [random.gauss(6000, 500) for x in range(traincount)])
train = np.append(train, [random.gauss(9000, 500) for x in range(traincount)])

train = train.reshape([len(train), 1])
#showtrainingdata(train)
#t2 =  train.astype('float32') / max(train)
#sc = StandardScaler()
#t3 = sc.fit_transform(t2)
train = train.astype('float') / max(train) #StandardScaler().fit_transform(train)

truth = np.full([traincount, 3], [1.0, 0.0, 0.0])
truth = np.append(truth, np.full([traincount, 3], [0.0, 1.0, 0.0]), axis=0)
truth = np.append(truth, np.full([traincount, 3], [0.0, 0.0, 1.0]), axis=0)

bias = 1
W1 = np.random.randn(numInput, numHidden)
#b1 = np.ones([1, numHidden])
W2 = np.random.randn(numHidden, numOutput)
#b2 = np.ones([1, numOutput])

#def sigmoid(x): return 1 /1 +(np.exp(-x))

def sigmoid(x):
    return 1 / ( 1 + np.exp(-x))

def sigmoidd(x):
    f = 1/(1+np.exp(-x))
    df = f * (1-f)
    return df

#def forward0(input, weights):
#    return sigmoid(np.dot(inputs, weights))

#def forward(train, W1, W2):
#    layer =  1 / (1 + np.exp(-np.dot(train, W1)))
#    output = 1 / (1 + np.exp(-np.dot(layer, W2)))
#    return layer, output

#def backward0(input, error, weights):
#    return np.dot(input.T, error * sigmoidd(input)) 
#def backward(layer, output, truth):
#    learnrate = 0.015
#    dW2 = np.dot(layer.T,  (truth - output) * sigmoidd(output)))
##    dW1 = np.dot(train.T,
#        (np.dot(2*(truth - output) * sigmoidd(output), W2.T) * 
#        sigmoidd(layer)))
#
#    return W1 + dW1 * learnrate, W2 + dW2 * learnrate

def plotErrors(errors):
    fig, ax = plt.subplots()
    ax.plot(errors)
#    plt.savefig('errors.png')
    plt.show()


def mae(truth, pred):  # mean absolute error?
    return np.sum(abs(truth - pred))/len(truth)

def mse(truth, pred):  # mean absolute error?
    return ((truth - pred) ** 2).mean()

pdb.set_trace()
errors = []
for ii in range(100):
    # Forward
    layer = sigmoid(np.dot(train, W1))
    output = sigmoid(np.dot(layer, W2))

    # Cost 
    error = truth - output
    print('Error {}\n'.format(mm:=mse(truth, output)))
    errors.append(mm)

    # Backward
    so = 2 * error * sigmoidd(output)  # sigmoid output
    dW2 = np.dot(layer.T, so)
    dW1 = np.dot(train.T, np.dot(so, W2.T) * sigmoidd(layer))
    W1 += dW1
    W2 += dW2

#    layer, output = forward(train, W1, W2)
#    W1, W2 = backward(layer, output, truth)

plotErrors(errors)

#showtrainingData(redtrain, greentrain, bluetrain)


 


