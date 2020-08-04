#!/bin/bash

<<<<<<< HEAD
PROJECT=fm131
=======
PROJECT=fm128
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06

#CONTROLLER=vproj
CONTROLLER=DESKTOP-TOJK787

#./fileuploader.py --ip $CONTROLLER --port 5000 --dir /home/mattd/capture --nodelete --frames=3
#./fileuploader.py --ip $CONTROLLER --port 5000 --dir /home/mattd/capture 
./fileuploader.py --ip $CONTROLLER --port 5000 --dir /mnt/exthd/${PROJECT}

