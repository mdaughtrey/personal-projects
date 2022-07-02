#!/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import pdb

traincount = 10
numInput = 3
numHidden = 4
numOutput = 3  # red, green, blue

#redtrain = [float(x) for x in open('redtrain.txt', 'rb').read().split()][:10]
#greentrain = [float(x) for x in open('greentrain.txt', 'rb').read().split()][:10]
#bluetrain = [float(x) for x in open('bluetrain.txt', 'rb').read().split()][:10]

pdb.set_trace() 
train = np.fromfile(open('redtrain.txt', 'rb'), sep="\n", count=traincount)
train = np.append(train, np.fromfile(open('greentrain.txt', 'rb'),
	 sep="\n", count=traincount), axis = 0)
train = np.append(train, np.fromfile(open('bluetrain.txt', 'rb'), 
	 sep="\n",count=traincount), axis = 0)

train = train.reshape([len(train), 1])

rgbwant = np.full([traincount, 3], [1.0, 0.0, 0.0])
rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 1.0, 0.0]), axis=0)
rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 0.0, 1.0]), axis=0)

W1 = np.random.randn(1, numHidden)
#b1 = np.ones([1, numHidden])
W2 = np.random.randn(numHidden, numOutput)
#b2 = np.ones([1, numOutput])

def showtrainingData(redtrain, greentrain, bluetrain):
#    plt.style.use('_mpl-gallery')
    fig, ax = plt.subplots()
    ax.hist(redtrain, bins=100, color='red')
    ax.hist(greentrain, bins=100, color='green')
    ax.hist(bluetrain, bins=100, color='blue')

    plt.show()
    

def sigmoid(x): return 1 /1 +(np.exp(-x))
#
#def sigderivative(x):
#	return np.exp(-x)/ ((1 + np.exp(x)) ** 2)

def forward(W1, W2):
	pdb.set_trace()
	layer =  1 / (1 + np.exp(-np.dot(train, W1)))
	output = sigmoid(np.dot(layer, W2))
	return layer, output

def backward(layer, output):
	learnrate = 0.05
	dW1 = np.dot(layer.T,  (2*(rgbwant - output) * sigderivative(output)))
	dW2 = np.dot(train.T,
		(np.dot(2*(rgbwant - output) * sigderivative(output), W2.T) * 
		sigderivative(layer)))

	return W1 + dW1 * learnrate, W2 + dW2 * learnrate

#	delta2 = np.multiply(-(rgbwant - z2), sigderivative(z2))
##	dW2 = np.dot(a1.T, delta2)
#	delta1 = np.dot(delta2, W2.T) * sigderivative(z1)
#	dW1 = np.dot(rgbtrain.T, delta1)
#	return dW1, dW2

#def compute_cost_red(pred):
#    return 0.5 * sum((1.0 - pred) ** 2)

for ii in range(10):
	layer, output = forward(W1, W2)
	W1, W2 = backward(layer, output)


#showtrainingData(redtrain, greentrain, bluetrain)


 


