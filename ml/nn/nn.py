#!/usr/bin/env python3

import numpy as np
import pdb

def sigmoid(x):
    return 1/(1+np.exp(-x))

def sigmoid_derivative(x):
    return np.exp(-x) / ((1 + np.exp(-x)) ** 2)

X = np.array([[0, 0, 0],
              [0, 0, 1],
              [0, 1, 0],
              [0, 1, 1],
              [1, 0, 0],
              [1, 0, 1],
              [1, 1, 0],
              [1, 1, 1]])

y = np.array([[0], [1], [0], [1], [0], [1], [1], [1]])

input_no = 3
output_no = 1
hidden_no = 4

W1 = np.random.randn(input_no, hidden_no)
b1 = np.ones([1, hidden_no])

W2 = np.random.randn(hidden_no, output_no)
b2 = np.ones([1, output_no])

def fp(W1, W2):
    a1 = np.dot(X,W1) + b1
    z1 = sigmoid(a1)
    a2 = np.dot(z1, W2) + b2
    y_pred = sigmoid(a2)
    return a1, z1, a2, y_pred

def bp(y_pred, z2, a1, z1):
    pdb.set_trace()
    delta2 = np.multiply(-(y - y_pred), sigmoid_derivative(z2))
    dJ_dW2 = np.dot(a1.T, delta2)
    delta1 = np.dot(delta2, W2.T) * sigmoid_derivative(z1)
    dJ_dW1 = np.dot(X.T, delta1)
    return dJ_dW1, dJ_dW2

def compute_cost(y_pred):
    return 0.5 * sum((y-y_pred) ** 2)

z1, a1, z2, y_pred = fp(W1, W2)
d1, d2 = bp(y_pred, z2, a1, z1)


