#!/bin/bash

PROJECT=fm131

#CONTROLLER=vproj
CONTROLLER=DESKTOP-TOJK787

#./fileuploader.py --ip $CONTROLLER --port 5000 --dir /home/mattd/capture --nodelete --frames=3
#./fileuploader.py --ip $CONTROLLER --port 5000 --dir /home/mattd/capture 
./fileuploader.py --ip $CONTROLLER --port 5000 --dir /mnt/exthd/${PROJECT}

