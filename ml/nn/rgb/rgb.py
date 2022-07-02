#!/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import pdb
import random
from sklearn.preprocessing import StandardScaler

traincount = 1000
numInput = 1
numHidden = 4
numOutput = 3  # red, green, blue

def showtrainingdata(train):
    fig, ax = plt.subplots()
    ax.hist(train, bins=100)
    plt.savefig('training.png')

train = np.array([random.gauss(1000, 500) for x in range(traincount)])
train = np.append(train, [random.gauss(2000, 500) for x in range(traincount)])
train = np.append(train, [random.gauss(3000, 500) for x in range(traincount)])

train = train.reshape([len(train), 1])
showtrainingdata(train)
train = StandardScaler().fit_transform(train)

rgbwant = np.full([traincount, 3], [1.0, 0.0, 0.0])
rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 1.0, 0.0]), axis=0)
rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 0.0, 1.0]), axis=0)

W1 = np.random.randn(numInput, numHidden)
#b1 = np.ones([1, numHidden])
W2 = np.random.randn(numHidden, numOutput)
#b2 = np.ones([1, numOutput])


#def sigmoid(x): return 1 /1 +(np.exp(-x))

def sigmoidd(x):
    f = 1/(1+np.exp(-x))
    df = f * (1-f)
    return df

def forward(W1, W2):
    layer =  1 / (1 + np.exp(-np.dot(train, W1)))
    output = 1 / (1 + np.exp(-np.dot(layer, W2)))
    return layer, output

def backward(layer, output):
    learnrate = 0.001
    dW2 = np.dot(layer.T,  (2*(rgbwant - output) * sigmoidd(output)))
    dW1 = np.dot(train.T,
        (np.dot(2*(rgbwant - output) * sigmoidd(output), W2.T) * 
        sigmoidd(layer)))

    return W1 + dW1 * learnrate, W2 + dW2 * learnrate

#    delta2 = np.multiply(-(rgbwant - z2), sigderivative(z2))
##    dW2 = np.dot(a1.T, delta2)
#    delta1 = np.dot(delta2, W2.T) * sigderivative(z1)
#    dW1 = np.dot(rgbtrain.T, delta1)
#    return dW1, dW2

#def compute_cost_red(pred):
#    return 0.5 * sum((1.0 - pred) ** 2)

def plotErrors(errors):
    fig, ax = plt.subplots()
    ax.plot(errors)
    plt.savefig('errors.png')


def mae(rgbwant, pred):
    return np.sum(abs(rgbwant - pred))/len(rgbwant)

errors = []
for ii in range(100):
    layer, output = forward(W1, W2)
    W1, W2 = backward(layer, output)
    error = mae(rgbwant, output)
    print('Error {}\n'.format(error))
    errors.append(error)

plotErrors(errors)

#showtrainingData(redtrain, greentrain, bluetrain)


 


