#!/usr/bin/python

from flask import Flask, request
import pdb
import json
from ProjectStore import ProjectStore
from FileManager import FileManager
from JobManager import JobManager
from ControllerStore import ControllerStore
import logging
import signal
import sys

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

app = Flask(__name__)
pstore = ProjectStore(logging.getLogger('ProjectStore'), ROOTOFALL)
fileman = FileManager(logging.getLogger('FileManager'), ROOTOFALL)
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman)
cstore = ControllerStore(logging.getLogger('ControllerStore'), ROOTOFALL)

def signal_handler(signal, frame):
    logger.debug("Control C")
    jobman.shutdown()
    logger.debug("Shutdown")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

#
# Upload a new raw file
#
@app.route('/upload', methods = ['GET', 'POST'])
def upload():
    if request.method == 'POST':
        project = request.args['project']
        if 'container' in request.args:
            return json.dumps(uploadImage(request))
        if 'titlepage' in request.args:
            return json.dumps(uploadTitleFile(request))
        return json.dumps(['BADCOMMAND'])

#
# Start title generation
#
@app.route('/gentitle', methods = ['POST', 'GET'])
def gentitle():
    return json.dumps(['TODO'])

#    if !jobman.quiescent() or !pstore.quiescent():
#        return json.dumps(['NOTREADY'])
#
#    jobman.gentitle()
#    return json.dumps(['DONE'])

#
# Start precrop (with parameters)
# Start autocrop
# Start tonefuse
# Set film mode
# Set title
# Get state and progress
# Cancel running operation
# Reset to state

def uploadImage(request):
    try:
        ff = request.files['filename']
        arguments = []

        for elem in ['project','container','filename']:
            arguments.append(request.args[elem])

        if pstore.rawFileExists(*arguments):
            return json.dumps(['EXISTS'])

        fileman.newFileStream(ff, *arguments)
        pstore.newRawFile(*arguments)
        return ['OK']

    except KeyError as ee:
        return json.dumps(['ERROR', ee.message])

def uploadTitleFile(request):
    try:
        ff = request.files['filename']
        arguments = []
        
        for elem in ['project','titlepage']:
            arguments.append(request.args[elem])

        return fileman.newTitleFile(ff, *arguments)

    except KeyError as ee:
        return json.dumps(['ERROR', ee.message])

def command(request):
    try:
        if 'gentitle' in request.args:
            return json.dumps(jobman.generateTitle(request.args['project']))
        if 'newproject' in request.args:
            return json.dumps(cstore.addProject(request.args['projectname']))
        if 'delproject' in request.args:
            response = []
            response.append(['database', cstore.deleteProject(request.args['projectname'])])
            if 1 == request.args['deletefiles']:
                response.append(['filemanager', fileman.deleteProject(request.args['projectname'])])
            return json.dumps(response)

    except KeyError as ee:
        return json.dumps(['ERROR', ee.message])

def getStatus(request):
    return json.dumps(['STATUS', 'RESTing ha ha'])

if __name__ == "__main__":
    app.run('0.0.0.0')
