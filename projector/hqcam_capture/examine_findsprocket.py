#!/usr/bin/env python3
import cv2
from matplotlib import pyplot as plt
import numpy as np
from picam_utils import *
import pdb


def examine(filename):
    pdb.set_trace()
    image = cv2.imread(filename,cv2.IMREAD_GRAYSCALE)
    low,high = (100,170)
    image[image<low] = 255 
    image[image>high] = 255
    image[image != 255] = 0
    plt.imshow(image,cmap='gray')
    plt.show()
    pass


def find_zero_rectangle(filename):
    pdb.set_trace()
    image = cv2.imread(filename,cv2.IMREAD_GRAYSCALE)
    low,high = (144,170)
    image[image<low] = 255 
    image[image>high] = 255
    image[image != 255] = 0
    image[image == 255] = 1
    # Find the connected components in the image
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(image, connectivity=8)

    # Initialize variables to track the largest rectangle of zeros
    max_area = 0
    max_rect = (0, 0, 0, 0)

    # Iterate through the connected components
    for i in range(1, num_labels):
        # Get the bounding box of the current component
        x, y, w, h, area = stats[i]

        # Check if the component is a rectangle of zeros
        if area == w * h and image[y:y+h, x:x+w].sum() == 0:
            # Update the largest rectangle if necessary
            if area > max_area:
                max_area = area
                max_rect = (x, y, w, h)

    return max_rect

filename='frames/20240402_1/findsprocket/clearfilm/198_eroded.png'
#examine(filename)
logger = FakeLogger()
#print(find_zero_rectangle(filename))
image = cv2.imread(filename,cv2.IMREAD_GRAYSCALE)
pdb.set_trace()
findSprocket(logger, image, show=False,savework=True)
