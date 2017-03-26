#!/usr/bin/python

from flask import Flask, request
import pdb
import json
from ProjectStore import ProjectStore
from FileManager import FileManager
from JobManager import JobManager
<<<<<<< HEAD
#from ControllerStore import ControllerStore
=======
from ControllerStore import ControllerStore
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
from nx300 import NX300
import logging
import signal
import sys
import argparse
<<<<<<< HEAD
import code

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
=======
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

parser = argparse.ArgumentParser()
parser.add_argument('--jobmode', dest='jobmode', default='proc', 
    choices = ['proc', 'inline', 'disable'], help='background job mode')
parser.add_argument('--project', required = True, dest='project', help='set jobman project name')
parser.add_argument('--film', required = True, dest='film', choices=['8mm','super8'], help="film mode")
<<<<<<< HEAD
config = parser.parse_args()

ROOTOFALL='/media/sf_vm_externalhd/scans/'
=======
parser.add_argument('--worker', dest='workers', action='append')
config = parser.parse_args()

ROOTOFALL='/media/sf_vmshared/scans/'
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

logFormat='%(asctime)s %(levelname)s %(name)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=logFormat)
logger = logging.getLogger('Controller')
fileHandler = logging.FileHandler('controller.log')
fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter(logFormat)
fileHandler.setFormatter(formatter)
logger.addHandler(fileHandler)
logging.getLogger('ProjectStore').addHandler(fileHandler)
logging.getLogger('FileManager').addHandler(fileHandler)
logging.getLogger('JobManager').addHandler(fileHandler)
<<<<<<< HEAD
#logging.getLogger('ControllerStore').addHandler(fileHandler)
=======
logging.getLogger('ControllerStore').addHandler(fileHandler)
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
logging.getLogger('RemoteDev').addHandler(fileHandler)

app = Flask(__name__)
pstore = ProjectStore(logging.getLogger('ProjectStore'), ROOTOFALL)
fileman = FileManager(logging.getLogger('FileManager'), ROOTOFALL)
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman, config, ROOTOFALL)
<<<<<<< HEAD
#cstore = ControllerStore(logging.getLogger('ControllerStore'), ROOTOFALL)
#remotedev = NX300(logging.getLogger('RemoteDev'), fileman)
=======
cstore = ControllerStore(logging.getLogger('ControllerStore'), ROOTOFALL)
remotedev = NX300(logging.getLogger('RemoteDev'), fileman)
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

def signal_handler(signal, frame):
    logger.debug("Control C")
    jobman.shutdown()
    logger.debug("Shutdown")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

#
# Upload a new raw file
#
<<<<<<< HEAD
@app.route('/upload', methods = ['PUT','GET'])
def upload():
    try:
        if 'GET' == request.method:
           jobman.uploadsDone(config.project) 
           return json.dumps(['OK'])
=======
@app.route('/upload', methods = ['PUT'])
def upload():
    try:
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        container, filename = pstore.getNextImageLocation(config.project)
        fileman.newFile(request.data, config.project, container, filename)
        pstore.newRawFile(config.project, container, filename)
        return json.dumps(['OK'])

    except KeyError as ee:
        return json.dumps(['ERROR', ee.message])

# Upload title file
@app.route('/titlefile', methods = ['PUT'])
def titlefile():
    try:
        return json.dumps(fileman.newTitleFile(request.data, config.project, request.config['page']))
    except:
        return json.dumps(['ERROR'])

#
# Start title generation
#
@app.route('/gentitle', methods = ['PUT'])
def gentitle():
    try:
        result = fileman.newTitleFile(request.data, config.project, request.config['page'])
    except:
        return json.dumps(['ERROR'])

    jobman.genTitle()
    return json.dumps(['OK'])

#
# Generate Movie
#
@app.route('/genmovie', methods = ['GET'])
def genmovie():
    #jobman.genMovie()
    return json.dumps(['OK'])

if __name__ == "__main__":
    app.run('0.0.0.0')
