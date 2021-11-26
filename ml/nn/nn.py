#!/usr/bin/env python3

import numpy as np
import pdb
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

pdb.set_trace()
W1 = np.random.randn(input_no, hidden_no)
b1 = np.ones([1, hidden_no])

W2 = np.random.randn(hidden_no, output_no)
b2 = np.ones([1, output_no])

a1 = np.dot(X,W1) + b1
pass
