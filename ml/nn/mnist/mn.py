#!/usr/bin/env python3
#https://python-course.eu/neural_network_mnist.php

import numpy as np
import matplotlib.pyplot as plt 
import pdb
import pickle

def sigmoid(x):
    return 1 / (1 + np.exp(-x))

def sigmoid_derivative(x):
    return np.exp(-x) / ((1 + np.exp(-x)) ** 2)

def forward_propagation(W1, W2):
    a1 = np.dot(X, W1) + b1
    z1 = sigmoid(a1)
    a2 = np.dot(z1, W2) + b2
    y_pred = sigmoid(a1)
    return a1, z1, a2, y_pred

def compute_cost(y_pred):
    J = 0.5 * sum((y - y_pred) ** 2)
    return J

def backward_propagation(y_pred, z2, a1, z1):
    delta2 = np.multiply(-(y - y_pred), sigmoid_derivative(z2))
    dJ_dW2 = np.dot(a1.T, delta2)
    delta1 = np.dot(delta2, W2.T) * sigmoid_derivative(z1)
    dJ_dW1 = np.dot(X.T, delta1)
    return dJ_dW1, dJ_dW2


def load():
    global train_images
    global train_labels
    global train_labels_1hot
    global test_images
    global test_labels
    global test_labels_1hot
    num_labels = 10
    fac = 0.99 / 255

    train_images = np.fromfile(open('train-images-idx3-ubyte', 'r'), dtype=np.uint8)[16:]
    test_images = np.fromfile(open('t10k-images-idx3-ubyte', 'r'), dtype=np.uint8)[16:]
    train_labels = np.fromfile(open('train-labels-idx1-ubyte', 'r'), dtype=np.uint8)[8:]
    test_labels = np.fromfile(open('t10k-labels-idx1-ubyte', 'r'), dtype=np.uint8)[8:]
    train_images = train_images.reshape((-1, 784))
    test_images = test_images.reshape((-1, 784))

    train_images = np.asfarray(train_images) * fac + 0.01
    lr = np.arange(num_labels)

    train_labels_1hot = [] # np.empty((len(train_labels), len(lr)))
    for tr in train_labels:
        train_labels_1hot.append(lr == tr)

    test_labels_1hot = [] # np.empty((len(train_labels), len(lr)))
    for tr in test_labels:
        test_labels_1hot.append(lr == tr)

    train_labels_1hot = np.asfarray(train_labels_1hot).astype(np.float)
    train_labels_1hot[train_labels_1hot == 0] = 0.01
    train_labels_1hot[train_labels_1hot == 1] = 0.99

    test_labels_1hot = np.asfarray(test_labels_1hot).astype(np.float)
    test_labels_1hot[test_labels_1hot == 0] = 0.01
    test_labels_1hot[test_labels_1hot == 1] = 0.99



def show():
    pdb.set_trace()
    for ii in range(10):
        img = train_images[ii].reshape((28, 28))
        plt.imshow(img, cmap="Greys")
        plt.savefig(f'fig{ii}.png')
#        plt.show()

np.vectorize
def sigmoid(x):
    return 1 / (1 + np.e ** -x)
activation_function = sigmoid

from scipy.stats import truncnorm

def truncated_normal(mean=0, sd=1, low=0, upp=10):
    return truncnorm((low - mean) / sd, 
                     (upp - mean) / sd, 
                     loc=mean, 
                     scale=sd)

class NeuralNetwork:
    
    def __init__(self, 
                 no_of_in_nodes, 
                 no_of_out_nodes, 
                 no_of_hidden_nodes,
                 learning_rate):
        self.no_of_in_nodes = no_of_in_nodes
        self.no_of_out_nodes = no_of_out_nodes
        self.no_of_hidden_nodes = no_of_hidden_nodes
        self.learning_rate = learning_rate 
        self.create_weight_matrices()
        
    def create_weight_matrices(self):
        """ A method to initialize the weight matrices of the neural network"""
        rad = 1 / np.sqrt(self.no_of_in_nodes)
        X = truncated_normal(mean=0, 
                             sd=1, 
                             low=-rad, 
                             upp=rad)
        self.wih = X.rvs((self.no_of_hidden_nodes, 
                                       self.no_of_in_nodes))
        rad = 1 / np.sqrt(self.no_of_hidden_nodes)
        X = truncated_normal(mean=0, 
                             sd=1, 
                             low=-rad, 
                             upp=rad)
        self.who = X.rvs((self.no_of_out_nodes, 
                                        self.no_of_hidden_nodes))
        
    
    def train_single(self, input_vector, target_vector):
        """
        input_vector and target_vector can be tuple, 
        list or ndarray
        """
        
        output_vectors = []
        input_vector = np.array(input_vector, ndmin=2).T
        target_vector = np.array(target_vector, ndmin=2).T

        
        output_vector1 = np.dot(self.wih, 
                                input_vector)
        output_hidden = activation_function(output_vector1)
        
        output_vector2 = np.dot(self.who, 
                                output_hidden)
        output_network = activation_function(output_vector2)
        
        output_errors = target_vector - output_network
        # update the weights:
        tmp = output_errors * output_network * \
              (1.0 - output_network)     
        tmp = self.learning_rate  * np.dot(tmp, 
                                           output_hidden.T)
        self.who += tmp


        # calculate hidden errors:
        hidden_errors = np.dot(self.who.T, 
                               output_errors)
        # update the weights:
        tmp = hidden_errors * output_hidden * (1.0 - output_hidden)
        self.wih += self.learning_rate * np.dot(tmp, input_vector.T)
        

    def train(self, data_array, 
              labels_one_hot_array,
              epochs=1,
              intermediate_results=False):
        intermediate_weights = []
        for epoch in range(epochs):  
            print("*", end="")
            for i in range(len(data_array)):
                self.train_single(data_array[i], 
                                  labels_one_hot_array[i])
            if intermediate_results:
                intermediate_weights.append((self.wih.copy(), 
                                             self.who.copy()))
        return intermediate_weights        
            
    def confusion_matrix(self, data_array, labels):
        cm = {}
        for i in range(len(data_array)):
            res = self.run(data_array[i])
            res_max = res.argmax()
            target = labels[i] # [-1]
            if (target, res_max) in cm:
                cm[(target, res_max)] += 1
            else:
                cm[(target, res_max)] = 1
        return cm
        
    
    def run(self, input_vector):
        """ input_vector can be tuple, list or ndarray """
        
        input_vector = np.array(input_vector, ndmin=2).T

        output_vector = np.dot(self.wih, 
                               input_vector)
        output_vector = activation_function(output_vector)
        
        output_vector = np.dot(self.who, 
                               output_vector)
        output_vector = activation_function(output_vector)
    
        return output_vector
    
    def evaluate(self, data, labels):
        corrects, wrongs = 0, 0
        for i in range(len(data)):
            res = self.run(data[i])
            res_max = res.argmax()
            if res_max == labels[i]:
                corrects += 1
            else:
                wrongs += 1
        return corrects, wrongs
            
load()
epochs = 10
ANN = NeuralNetwork(no_of_in_nodes = 784,
	no_of_out_nodes = 10,
	no_of_hidden_nodes = 100,
	learning_rate = 0.15)

try:
	weights = pickle.load(open('weights', 'rb'))
except:
	weights = ANN.train(train_images, 
                    train_labels_1hot, 
                    epochs=epochs, 
                    intermediate_results=True)
	pickle.dump(weights, open('weights', 'wb'))

try:
	cm = pickle.load(open('cm', 'rb'))
except:
	cm = ANN.confusion_matrix(train_images, train_labels)
	pickle.dump(cm, open('cm', 'wb'))

        
print(ANN.run(train_images[0]))

cm = list(cm.items())
print(sorted(cm))

for i in range(epochs):  
    print("epoch: ", i)
    ANN.wih = weights[i][0]
    ANN.who = weights[i][1]
   
    corrects, wrongs = ANN.evaluate(train_images, train_labels)
    print("accuracy train: ", corrects / ( corrects + wrongs))
    corrects, wrongs = ANN.evaluate(test_images, test_labels)
    print("accuracy: test", corrects / ( corrects + wrongs))

