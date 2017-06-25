#!/bin/bash

CONTROLLER=192.168.0.42

#./fileuploader.py --ip 192.168.0.41 --port 5000 --dir /home/mattd/cam/images
./fileuploader.py --ip $CONTROLLER --port 5000 --dir /tmp
