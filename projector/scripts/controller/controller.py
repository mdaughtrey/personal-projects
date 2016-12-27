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
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman, args.workman, args.project)
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
        if False in [request.args[elem] for elem in ('container', 'file', 'project')]:
            return json.dumps(['BADCOMMAND'])
        return json.dumps(uploadImage(request))
    except:
        return json.dumps(['BADCOMMAND'])
    return json.dumps(['OK'])

# set up a triple for processing
#@app.route('/triple', methods = ['GET'])
#def triple():
#    if False in [request.args[elem] for elem in ('container', 'files', 'project')]:
#        return json.dumps(['BADCOMMAND'])
#    files = urllib.parse.unquote(request.args['files'])
#    return json.dumps(jobman.triple(project, container, files))
#
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
        #ff = request.args['file']
        arguments = []

        for elem in ['project','container','file']:
            arguments.append(request.args[elem].encode('ascii','ignore'))

        if pstore.rawFileExists(*arguments):
            return json.dumps(['EXISTS'])

        fileman.newFile(request.data, *arguments)
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

# Various commands
@app.route('/command', methods = ['GET'])
def command():
    try:
        if 'gentitle' in request.args:
            return json.dumps(jobman.generateTitle(request.args['project']))
        # Set or create project
#        if 'project' in request.args:
#            return json.dumps(cstore.setProject(request.args['project']))
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
    #return fileman.newTitleFile(ff, *arguments)


if __name__ == "__main__":
    app.run('0.0.0.0')
