#!/usr/bin/python

from collections import namedtuple
from operator import mul
import scipy
import pdb

Info= namedtuple('Info', 'start height')

lookin = [
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,0,0,0,1,1,1,1,1,1],
[0,0,0,0,0,0,0,0,0,0,1],
[0,0,0,0,0,0,0,0,0,1,1],
[0,0,0,1,1,0,0,1,1,1,1],
[0,1,1,1,1,1,1,1,1,1,1],
[0,1,1,1,1,1,1,1,1,1,1],
[0,1,1,1,1,1,1,1,1,1,1],
[0,0,0,0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,0,0,0,0]]

def max_size(mat, value=0):
    """Find height, width of the largest rectangle containing all `value`'s."""
    it = iter(mat)
    rowIdx = 1
    endRow = 0
    endCol = 0
    hist = [(el==value) for el in next(it, [])]
    max_size, endCol = max_rectangle_size(hist)
    for row in it:
        hist = [(1+h) if el == value else 0 for h, el in zip(hist, row)]
        print "row %s hist %s" % (row, hist)
#        pdb.set_trace()
        this_size, thisEnd = max_rectangle_size(hist)
        print "this_size %s thisEnd %u" % (this_size, thisEnd)
        if area(this_size) > area(max_size):
            max_size = this_size
            endRow = rowIdx
            endCol = thisEnd
        rowIdx += 1
    return max_size, (endRow, endCol)

def max_rectangle_size(histogram):
    """Find height, width of the largest rectangle that fits entirely under
    the histogram."""
    stack = []
    top = lambda: stack[-1]
    max_size = (0, 0) # height, width of the largest rectangle
    pos = 0 # current position in the histogram
    endCol = 0
    for pos, height in enumerate(histogram):
        start = pos # position where rectangle starts
        while True:
            if not stack or height > top().height:
                stack.append(Info(start, height)) # push
            elif stack and height < top().height:
                thisArea = (top().height, pos - top().start)
                if area(thisArea) > area(max_size):
                    endCol = pos
                    max_size = thisArea
                start, _ = stack.pop()
                continue
            break # height == top().height goes here

    pos += 1
    for start, height in stack:
        max_size = max(max_size, (height, (pos - start)), key=area)    
    print "endCol %u" % endCol
    return max_size, endCol

def area(size):
    return reduce(mul, size)

print max_size(lookin, 1)
