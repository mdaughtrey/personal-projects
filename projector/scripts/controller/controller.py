#!/usr/bin/python

from flask import Flask, request
import pdb
import json
from PersistentStore import PersistentStore
from FileManager import FileManager
from JobManager import JobManager
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
logging.getLogger('PersistentStore').addHandler(fileHandler)
logging.getLogger('FileManager').addHandler(fileHandler)
logging.getLogger('JobManager').addHandler(fileHandler)

app = Flask(__name__)
pstore = PersistentStore(logging.getLogger('PersistentStore'), ROOTOFALL)
fileman = FileManager(logging.getLogger('FileManager'), ROOTOFALL)
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman)

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
def uploadFile():
    if request.method == 'POST':
        project = request.args['project']
        ff = request.files['filename']
        arguments = []
        for elem in ['project','container','filename']:
            arguments.append(request.args[elem])
        fileman.newFileStream(ff, *arguments)
        pstore.newRawFile(*arguments)
        return json.dumps(['OK'])

#
# Start precrop (with parameters)
# Start autocrop
# Start tonefuse
# Set film mode
# Set title
# Get state and progress
# Cancel running operation
# Reset to state


if __name__ == "__main__":
    app.run('0.0.0.0')
