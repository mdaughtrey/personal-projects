#!/usr/bin/python

from flask import Flask, request
import pdb
import json
from PersistentStore import PersistentStore
from FileManager import FileManager
from JobManager import JobManager
import logging

logFormat='%(asctime)s %(levelname)s %(name)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=logFormat)
logger = logging.getLogger('autocrop')
fileHandler = logging.FileHandler('controller.log')
fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter(logFormat)
fileHandler.setFormatter(formatter)
logger.addHandler(fileHandler)
logging.getLogger('PersistentStore').addHandler(fileHandler)
logging.getLogger('FileManager').addHandler(fileHandler)
logging.getLogger('JobManager').addHandler(fileHandler)

app = Flask(__name__)
pstore = PersistentStore(logging.getLogger('PersistentStore'))
fileman = FileManager(logging.getLogger('FileManager'))
jobman = JobManager(logging.getLogger('JobManager'), pstore, fileman)


#
# Upload a new raw file
#
@app.route('/upload', methods = ['GET', 'POST'])
def uploadFile():
    if request.method == 'POST':
        project = request.args['project']
        ff = request.files['filename']
        fileman.newFileStream(request.args['project'], request.args['container'], ff)
        pstore.newRawFile(request.args['project'], request.args['container'], ff.filename)
        jobman.newRawFile(request.args['project'])
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
