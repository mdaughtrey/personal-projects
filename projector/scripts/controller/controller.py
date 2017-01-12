#!/usr/bin/python

from flask import Flask, request
import pdb
import json
from ProjectStore import ProjectStore
from FileManager import FileManager
from JobManager import JobManager
from ControllerStore import ControllerStore
from nx300 import NX300
import logging
import signal
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--workman', dest='workman', default='proc', 
    choices = ['proc', 'inline', 'disable'], help='worker manager mode')
parser.add_argument('--project', required = True, dest='project', help='set jobman project name')
parser.add_argument('--film', required = True, dest='film', choices=['8mm','super8'], help="film mode")
args = parser.parse_args()

ROOTOFALL='/media/sf_vmshared/scans/'

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
logging.getLogger('ControllerStore').addHandler(fileHandler)
logging.getLogger('RemoteDev').addHandler(fileHandler)

app = Flask(__name__)
pstore = ProjectStore(logging.getLogger('ProjectStore'), ROOTOFALL)
fileman = FileManager(logging.getLogger('FileManager'), ROOTOFALL)
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman,
    args.workman, args.project, args.film, ROOTOFALL)
cstore = ControllerStore(logging.getLogger('ControllerStore'), ROOTOFALL)
remotedev = NX300(logging.getLogger('RemoteDev'), fileman)

def signal_handler(signal, frame):
    logger.debug("Control C")
    jobman.shutdown()
    logger.debug("Shutdown")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

#
# Upload a new raw file
#
@app.route('/upload', methods = ['PUT'])
def upload():
    try:
        container, filename = pstore.getNextImageLocation(args.project)
        fileman.newFile(request.data, args.project, container, filename)
        pstore.newRawFile(args.project, container, filename)
        return json.dumps(['OK'])

    except KeyError as ee:
        return json.dumps(['ERROR', ee.message])

# Upload title file
@app.route('/titlefile', methods = ['PUT'])
def titlefile():
    try:
        return json.dumps(fileman.newTitleFile(request.data, args.project, request.args['page']))
    except:
        return json.dumps(['ERROR'])

#
# Start title generation
#
@app.route('/gentitle', methods = ['PUT'])
def gentitle():
    try:
        result = fileman.newTitleFile(request.data, args.project, request.args['page'])
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
