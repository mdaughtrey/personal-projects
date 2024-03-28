#!/bin/env python3

import cv2
from  glob import glob, iglob
import numpy as np
from PIL import Image, ImageOps
from matplotlib import pyplot as plt
from scipy import ndimage
import pdb
#import cProfile
from picamera2 import Picamera2, Preview
from libcamera import Transform

#image = cv2.imread('/frames/20240326_640_480/capture/00000000_22000.png')

def findSprocket(image, show=False):
    y,x = image.shape[:2]
    if show:
        plt.imshow(image)
        plt.show()
    image = image[int(y/3):y-int(y/3),0:int(x/4)]
    if show:
        plt.imshow(image)
        plt.show()
    image2 = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    image3 = np.asarray(image2, dtype=np.uint8)
    image3 = ndimage.grey_erosion(image3, size=(5,5))
    _, image3 = cv2.threshold(image3, 100, 255, cv2.THRESH_BINARY)
    if show:
        plt.imshow(image3,cmap='gray')
        plt.show()

    def whtest(contour):
        (x,y,w,h) = cv2.boundingRect(contour)
        return (50 < w < 60) & (60 < h < 70)

    # Find the contours in the thresholded image
    contours, _ = cv2.findContours(image3, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours = list(filter(whtest, contours))
    if 1 != len(contours):
        return False
    if show == False:
        return True

    for c in contours:
        print('Contour Area: ' + str(cv2.contourArea(c)))
        x, y, width, height = cv2.boundingRect(c)
        print(f'x {x} y {y} width {width} height {height}')
    contour = contours[0]

    # Get the bounding box of the largest contour
    x, y, width, height = cv2.boundingRect(contour)

    # Print the size and location of the white square
    print(f'White square size: {width}x{height} pixels')
    print(f'White square location: ({x}, {y})')
    cv2.drawContours(image3, [contour], -1, (100,100,100), thickness=cv2.FILLED)
    plt.imshow(image3,cmap='gray')
    plt.show()

    return True

w,h = (640,480)
#pdb.set_trace()
#image = cv2.imread('/media/frames/frames_for_cam_sprocket/0877.png')
#findSprocket(image)
picam = Picamera2()
#config = picam.create_video_preview_configuration(main={'size':(640,480)})
config = picam.create_video_configuration(main={'size': (640,480), "format":"RGB888"},
                                          transform = Transform(hflip=True),
                                          lores={"size":(640,480),"format":"RGB888"},
                                          controls={'FrameDurationLimits':(1000000,1000000),
                                                    'ExposureTime':22000})
picam.configure(config)
picam.align_configuration(config)
picam.start()

while True:
#    pdb.set_trace()
    buffer = picam.capture_array("lores")
#    cv2.imwrite('test.png', buffer)
    #buffer = buffer[:w * h].reshape(h,w)
#    buffer = buffer.reshape(h,w,3)
    print(str(findSprocket(buffer,show = False)))
#    picam.stop()


#pdb.set_trace()
#for image in sorted(glob('/media/frames/frames_for_cam_sprocket/*.png')):
#    print(image + ' ' + str(findSprocket(cv2.imread(image), show = False)))
#image = cv2.imread('/media/frames/frames_for_cam_sprocket/0877.png')
#cProfile.run("findSprocket(image)")

