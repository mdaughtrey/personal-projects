#!/usr/bin/python3

from flask import Flask, request
import pdb
import json
from ProjectStore import ProjectStore
from FileManager import FileManager
from JobManager import JobManager
from JobManagerRaw import JobManagerRaw
#from nx300 import NX300
import logging
import signal
import sys
import argparse
import code

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())

parser = argparse.ArgumentParser()
parser.add_argument('--jobmode', dest='jobmode', default='proc', 
    choices = ['proc', 'inline', 'disable', 'uploadonly'], help='background job mode')
parser.add_argument('--project', required = True, dest='project', help='set jobman project name')
parser.add_argument('--film', required = True, dest='film', choices=['8mm','super8'], help="film mode")
parser.add_argument('--saveroot', required = True, dest='saveroot', help="root dir to save to")
parser.add_argument('--raw', required = True, dest='raw', default='False', action='store_true', help="process RAW files")
parser.add_argument('--iimport', dest='iimport', help="set import directory")
parser.add_argument('--single', dest='single', help="one image per frame", action='store_true', default='False')
config = parser.parse_args()

ROOTOFALL=config.saveroot 

logFormat='%(asctime)s %(process)d %(levelname)s %(name)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=logFormat)
logger = logging.getLogger('Controller')
fileHandler = logging.FileHandler('controller.log')
fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter(logFormat)
fileHandler.setFormatter(formatter)
logger.addHandler(fileHandler)
logging.getLogger('ProjectStore').addHandler(fileHandler)
logging.getLogger('FileManager').addHandler(fileHandler)
if config.raw:
    logging.getLogger('JobManagerRaw').addHandler(fileHandler)
else:
    logging.getLogger('JobManager').addHandler(fileHandler)
#logging.getLogger('ControllerStore').addHandler(fileHandler)
logging.getLogger('RemoteDev').addHandler(fileHandler)


app = Flask(__name__)
pstore = ProjectStore(logging.getLogger('ProjectStore'), ROOTOFALL + config.project, config)
fileman = FileManager(logging.getLogger('FileManager'), ROOTOFALL, config)

if 'uploadonly' != config.jobmode:
    if config.raw:
        jobman = JobManagerRaw(logging.getLogger('JobManagerRaw'), pstore, fileman, config, ROOTOFALL)
    else:
        jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman, config, ROOTOFALL)
#cstore = ControllerStore(logging.getLogger('ControllerStore'), ROOTOFALL)
#remotedev = NX300(logging.getLogger('RemoteDev'), fileman)

def signal_handler(signal, frame):
    logger.debug("Control C")
    jobman.shutdown()
    logger.debug("Shutdown")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
# DEBUG
#jobman.uploadsDone(config.project) 

#
# Upload a new raw file
#
@app.route('/upload/<tag>', methods = ['PUT','GET'])
def upload(tag):
    try:
        if 'GET' == request.method:
           jobman.uploadsDone(config.project) 
           return json.dumps(['OK'])
#        pdb.set_trace()
        container, filename = pstore.getNextImageLocation(config.project, tag)
        logger.debug("Container %s Filename %s" % (container, filename))
        fileman.newFile(request.data, config.project, container, filename, tag)
        pstore.newRawFile(config.project, container, filename, tag)
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
# Upload reference files
#
@app.route("/reference", methods=['PUT'])
def referenceFile():
    try:
        return json.dumps(fileman.newReferenceFile(request.data, config.project, request.args))
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
