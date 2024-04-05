import cv2
from libcamera import Transform
import logging
from logging import FileHandler, StreamHandler
from matplotlib import pyplot as plt
import numpy as np
from picamera2 import Picamera2, Preview
from scipy import ndimage
import sys

count = 0

def init_picam(exposure:int) -> dict:
    camera = {'picam': None}
    lores={"size":(640,480),"format":"RGB888"}
    main={"size":(2304,1296),"format":"RGB888"}

#    global picam
    controls={'FrameDurationLimits':(50000,50000),'ExposureTime': exposure,
              'AeEnable': False, 'AwbEnable': False}
    transform = Transform(hflip=True)
    #Picamera2.set_logging(Picamera2.ERROR)
    Picamera2.set_logging(Picamera2.ERROR)
    camera['picam']= Picamera2()
    cam_config = camera['picam'].create_video_configuration(queue=False,main=main,lores=lores,transform=transform,controls=controls)
    #cam_config = picam.create_video_configuration()

    camera['picam'].configure(cam_config)
    camera['picam'].align_configuration(cam_config)
    camera['picam'].start()
    return camera

def waitSprocket(logger, picam, desired):
#    global picam
    global count
    start = time.time()
    while (time.time() - start) < 5.0:
        buffer = picam.capture_array("lores")
        count += 1
        logger.debug(str(picam.capture_metadata()))
        inSprocket = findSprocket(logger, buffer, show = False)
        logger.debug(f'inSprocket {inSprocket}, need {str(desired)}')
        if desired == inSprocket:
            return
    raise RuntimeError('timeout')

def inccount():
    global count
    count += 1

def findSprocket(logger, image, show=False,savework=False):
    logger.debug(count)
    y,x = image.shape[:2]
    if show:
        plt.imshow(image)
        plt.title('Input Image')
        plt.show()
    if savework:
        cv2.imwrite(f'/tmp/{count}_input.png', image)
    image = image[int(y/3):y-int(y/3),0:100]
    if show:
        plt.imshow(image)
        plt.title('Sliced')
        plt.show()
    if savework:
        cv2.imwrite(f'/tmp/{count}_sliced.png', image)
    image2 = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    image3 = np.asarray(image2, dtype=np.uint8)
    image3 = ndimage.grey_erosion(image3, size=(5,5))
    if savework:
        cv2.imwrite(f'/tmp/{count}_eroded.png', image3)

#    _, image3 = cv2.threshold(image3, 80, 255, cv2.THRESH_BINARY)
    logger.debug(str(image3[80]))
    low,high = (100,170)
    image[image<low] = 255 
    image[image>high] = 255
    image[image != 255] = 0
    image[image == 255] = 1
    if savework:
        cv2.imwrite(f'/tmp/{count}_threshold.png', image3)
#    image3 = np.where(image3 < 40, 0, np.where(image >= 40, np.where(image3 <= 60, 1, 0), image3))
#    _, image3 = cv2.t#hreshold(image3, 80, 255, cv2.THRESH_BINARY)
    if show:
        plt.imshow(image3,cmap='gray')
        plt.title('Eroded')
        plt.show()

    def whtest(contour):
        (x,y,w,h) = cv2.boundingRect(contour)
        return (40 < w < 60) & (60 < h < 90)

    # Find the contours in the thresholded image
    contours, _ = cv2.findContours(image3, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for c in contours:
        logger.debug('Pre filter Area: ' + str(cv2.contourArea(c)))
        x, y, width, height = cv2.boundingRect(c)
        logger.debug(f'x {x} y {y} width {width} height {height}')
    contours = list(filter(whtest, contours))
    logger.debug(f'Found {len(contours)} contours')
    if 1 != len(contours):
        return False
    if show == False:
        return True

    for c in contours:
        logger.debug('Post filter Area: ' + str(cv2.contourArea(c)))
        x, y, width, height = cv2.boundingRect(c)
        logger.debug(f'x {x} y {y} width {width} height {height}')
    contour = contours[0]

    # Get the bounding box of the largest contour
    x, y, width, height = cv2.boundingRect(contour)

    # Print the size and location of the white square
    logger.debug(f'White square size: {width}x{height} pixels')
    logger.debug(f'White square location: ({x}, {y})')
    cv2.drawContours(image3, [contour], -1, (100,100,100), thickness=cv2.FILLED)
    plt.imshow(image3,cmap='gray')
    if save:
        cv2.imwrite(f'/tmp/{count}_identified.png', image3)
    plt.title('Identified')
    plt.show()

    return True

def setLogging(name:str,logfilename:str) -> dict:
    logger = {'logger': None}
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
#    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    
    logger['logger'] = logging.getLogger(name)
    logger['logger'].setLevel(logging.DEBUG)
    fileHandler = FileHandler(filename = logfilename)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger['logger'].addHandler(fileHandler)

    stdioHandler = StreamHandler(sys.stdout)
    stdioHandler.setFormatter(logging.Formatter(fmt=FormatString))
    stdioHandler.setLevel(logging.INFO)
    logger['logger'].addHandler(stdioHandler)
    return logger
