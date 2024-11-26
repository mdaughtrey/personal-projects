#!/usr/bin/env python3

import numpy as np
from sklearn import datasets
import pdb

np.random.seed(0)
features,labels = datasets.make_moons(100,noise=0.1)

with open('features.csv','w') as file:
    for f in features:
        file.write('{},{},\n'.format(f[0], f[1]))

with open('labels.csv','w') as file:
    for l in labels:
        file.write('{},\n'.format(l))

