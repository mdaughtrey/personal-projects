#!/usr/bin/python

from collections import namedtuple
from operator import mul
import scipy
import pdb

Info= namedtuple('Info', 'start height')

lookin = [
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,1,1,1,1,1,0,0,0,0],
[0,0,1,1,1,1,0,1,0,0,0],
[0,0,0,1,1,1,0,0,0,0,0],
[0,0,0,1,1,1,0,1,0,0,0],
[0,0,0,0,0,0,0,0,0,1,1],
[0,0,0,0,0,0,0,0,0,1,1]]

def max_size(mat, value=0):
    """Find height, width of the largest rectangle containing all `value`'s."""
    it = iter(mat)
    hist = [(el==value) for el in next(it, [])]
    max_size = max_rectangle_size(hist)
    for row in it:
        print "this row %s" % str(row)
        hist = [(1+h) if el == value else 0 for h, el in zip(hist, row)]
        max_size = max(max_size, max_rectangle_size(hist), key=area)
    return max_size

def max_rectangle_size(histogram):
    """Find height, width of the largest rectangle that fits entirely under
    the histogram."""
    stack = []
    top = lambda: stack[-1]
    max_size = (0, 0) # height, width of the largest rectangle
    pos = 0 # current position in the histogram
    for pos, height in enumerate(histogram):
#        print "pos %u height %u" % (pos, height)
        start = pos # position where rectangle starts
        while True:
            if not stack or height > top().height:
                print "Start %u" % start
                stack.append(Info(start, height)) # push
            elif stack and height < top().height:
                max_size = max(max_size, (top().height, (pos - top().start)), key=area)
                start, _ = stack.pop()
                print "pos %u start %u max_size %s height %s" % (pos, start, str(max_size), str(top().height))
                continue
            break # height == top().height goes here

    pos += 1
    for start, height in stack:
        max_size = max(max_size, (height, (pos - start)), key=area)    
    return max_size

def area(size):
    return reduce(mul, size)

print max_size(lookin, 1)
