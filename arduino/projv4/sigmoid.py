#!/bin/env python3

import math
import pdb
import numpy as np

sm=np.round(1/(1+np.exp(-np.linspace(-10, 10, 64))), 3)
[print(f'{a}{b}') for a,b in zip(sm, len(sm) * ',')]

#for ii in range(0,10):
#    print((64//10)*ii)

