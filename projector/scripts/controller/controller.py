#!/usr/bin/python

from flask import Flask, request
import pdb
import json
import PersistentStore
import FileManager

import logging

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(message)s')
logger = logging.getLogger('autocrop')
fileHandler = RotatingFileHandler(filename='controller.log', maxBytes=10e6, backupCount=2)
fileHandler.setLevel(logging.DEBUG)

logger.addHandler(fileHandler)
app = Flask(__name__)
pstore = PersistentStore(logging.getLogger('PersistentStore'))
fileman = FileManager(logging.getLogger('FileManager'))

#@app.route('/')
#def hello():
#    return "root"
#
#@app.route('/newfile')
#def newfile():
#    filename = request.args['filename']
#    app.logger.debug('filename is %s' % filename)
#    return json.dumps(['one',{'two': (3, 'four')}])

#
# Upload a new raw file
#
@app.route('/upload', methods = ['GET', 'POST'])
def uploadFile():
    if request.method == 'POST':
        project = request.args['project']
        ff = request.files['filename']
        fileman.newFileStream(request.args['project'], request.args['container'], ff)
        pstore.newRawFile(request.args['project'], request.args['container', ff.name())
        return json.dumps(['OK'])
        #ff.save('somefile')
        #return ''

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
