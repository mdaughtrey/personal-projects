#!/bin/env ipython3
# https://thecleverprogrammer.com/2020/09/07/neural-network-with-python-code/

import matplotlib.pyplot as plt
import numpy as np
import pdb
import random
from sklearn.preprocessing import StandardScaler

traincount = 1000
#numInput = 1
#numHidden = 4
#numOutput = 3  # red, green, blue

def showtrainingdata(train):
    fig, ax = plt.subplots()
    ax.hist(train, bins=100)
    plt.savefig('training.png')

#pdb.set_trace()
X = np.fromfile(open('../mnist/train-images-idx3-ubyte', 'r'), dtype=np.uint8)[16:]
labels = np.fromfile(open('../mnist/train-labels-idx1-ubyte', 'r'), dtype=np.uint8)[8:]
X = X.reshape([len(X)//(28*28), 28*28])
#X = X.astype('float32')/(255/2)-1
X = X.astype('float32')/X.max()
#X = StandardScaler().fit_transform(X)

y = np.zeros([labels.shape[0], 10])
for ii, jj in enumerate(labels):
    y[ii][jj] = 1.0

pdb.set_trace()

#numInput = 28*28
#numHidden = 100
#numOutput = 10
#train = np.array([random.gauss(1000, 500) for x in range(traincount)])
#train = np.append(train, [random.gauss(2000, 500) for x in range(traincount)])
#train = np.append(train, [random.gauss(3000, 500) for x in range(traincount)])

#train = train.reshape([len(labels), numInput])
#showtrainingdata(train)
#labelnums = np.arange(len(labels))
#labelnums = labelnums.reshape([len(labelnums), 1])


#rgbwant = np.full([traincount, 3], [1.0, 0.0, 0.0])
#rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 1.0, 0.0]), axis=0)
#rgbwant = np.append(rgbwant, np.full([traincount, 3], [0.0, 0.0, 1.0]), axis=0)

#W1 = np.random.randn(numInput, numHidden)
#b1 = np.ones([1, numHidden])
#W2 = np.random.randn(numHidden, numOutput)
#b2 = np.ones([1, numOutput])

# X = input of our 3 input XOR gate
# set up the inputs of the neural network (right from the table)
#X = np.array(([0,0,0],[0,0,1],[0,1,0], \
#              [0,1,1],[1,0,0],[1,0,1],[1,1,0],[1,1,1]), dtype=float)
# y = our output of our neural network
#y = np.array(([1], [0], [0], [0], [0], \
#              [0], [0], [1]), dtype=float)

# what value we want to predict
#xPredicted = np.array(([0,0,1]), dtype=float)
#X = X/np.amax(X, axis=0) # maximum of X input array
## maximum of xPredicted (our input data for the prediction)
#xPredicted = xPredicted/np.amax(xPredicted, axis=0)

class Neural_Network (object):
    def __init__(self):
        #parameters
        self.inputLayerSize = 28*28 # X1,X2,X3
        self.outputLayerSize = 10 # Y1
        self.hiddenLayerSize = 100 # Size of the hidden layer

        # build weights of each layer
        # set to random values
        # look at the interconnection diagram to make sense of this
        # 3x4 matrix for input to hidden
        self.W1 = np.random.randn(self.inputLayerSize, self.hiddenLayerSize)
        # 4x1 matrix for hidden layer to output
        self.W2 = np.random.randn(self.hiddenLayerSize, self.outputLayerSize)

    def feedForward(self, X):
        # feedForward propagation through our network
        # dot product of X (input) and first set of 3x4 weights
        self.z = np.dot(X, self.W1)
        # the activationSigmoid activation function - neural magic
        self.z2 = self.activationSigmoid(self.z)
        # dot product of hidden layer (z2) and second set of 4x1 weights
        self.z3 = np.dot(self.z2, self.W2)
        # final activation function - more neural magic
        o = self.activationSigmoid(self.z3)
        return o

    def backwardPropagate(self, X, y, o):
        pdb.set_trace()
        # backward propagate through the network
        # calculate the error in output
        self.o_error = y - o
        # apply derivative of activationSigmoid to error
        self.o_delta = self.o_error*self.activationSigmoidPrime(o)
        # z2 error: how much our hidden layer weights contributed to output
        # error
        self.z2_error = self.o_delta.dot(self.W2.T)
        # applying derivative of activationSigmoid to z2 error
        self.z2_delta = self.z2_error*self.activationSigmoidPrime(self.z2)
        # adjusting first set (inputLayer --&gt; hiddenLayer) weights
        self.W1 += X.T.dot(self.z2_delta)
        # adjusting second set (hiddenLayer --&gt; outputLayer) weights
        self.W2 += self.z2.T.dot(self.o_delta)


    def trainNetwork(self, X, y):
        # feed forward the loop
        o = self.feedForward(X)
        # and then back propagate the values (feedback)
        self.backwardPropagate(X, y, o)

    def activationSigmoid(self, s):
        # activation function
        # simple activationSigmoid curve as in the book
        return 1/(1+np.exp(-s))
    def activationSigmoidPrime(self, s):
        # First derivative of activationSigmoid
        # calculus time!
        return s * (1 - s)

    def saveSumSquaredLossList(self,i,error):
        pass
    def saveWeights(self):
        # save this in order to reproduce our cool network
        np.savetxt("weightsLayer1.txt", self.W1, fmt="%s")
        np.savetxt("weightsLayer2.txt", self.W2, fmt="%s")

#    def predictOutput(self):
#        print ("Predicted XOR output data based on trained weights: ")
#        print ("Expected (X1-X3): \n" + str(xPredicted))
#        print ("Output (Y1): \n" + str(self.feedForward(xPredicted)))


myNeuralNetwork = Neural_Network()
trainingEpochs = 1000


for i in range(trainingEpochs):
#    for x0,y0 in zip(X,y):
        x0 = X
        y0 = y
        print ("Epoch # " + str(i) + "\n")
#        print ("Network Input : \n" + str(x0))
#        print ("Expected Output of XOR Gate Neural Network: \n" + str(y0))
#        print ("Actual Output from XOR Gate Neural Network: \n" + str(myNeuralNetwork.feedForward(x0)))
# mean sum squared loss
        Loss = np.mean(np.square(y0 - myNeuralNetwork.feedForward(x0)))
        myNeuralNetwork.saveSumSquaredLossList(i,Loss)
        print ("Sum Squared Loss: \n" + str(Loss))
        print ("\n")
        myNeuralNetwork.trainNetwork(x0, y0)



myNeuralNetwork.saveWeights()
#myNeuralNetwork.predictOutput()


