#!/usr/bin/env python3

import cv2
import pdb
import numpy as np

face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

cap = cv2.VideoCapture(0)

centers = np.zeros((100,2), np.int32)

def avg_center(center):
    global centers
    if center is not None:
        centers = np.append(centers[1:], [center], axis=0)
    weights = list(range(1, 1+len(centers)))
    totalweight = sum(weights)
    xa = sum([ii*jj for ii,jj in zip(centers[:,0],weights)]) / totalweight
    ya = sum([ii*jj for ii,jj in zip(centers[:,1],weights)]) / totalweight
    return (int(xa),int(ya))

while True:
    _, img = cap.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 4, 0, (200, 200))
    if len(faces):
        for (x,y,w,h) in faces:
            center = np.asarray([int(x+(w/2)), int(y+(h/2))], np.int32)
            cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 255), 2)
            cv2.circle(img, tuple(center), 5, (0, 0, 255), 2)
            cv2.circle(img, avg_center(center), 5, (0, 255, 0), 2)
    else:
        cv2.circle(img, avg_center(None), 5, (0, 255, 0), 2)
    cv2.imshow('img', img)
    k=cv2.waitKey(30) & 0xff
    if k == 27:
        break;

cap.release()

