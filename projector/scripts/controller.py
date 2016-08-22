#!/usr/bin/python

from flask import Flask, request
import pdb
import json

app = Flask(__name__)

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
        pdb.set_trace()
        ff = request.files['filename']
        ff.save('somefile')
        return ''

#
# Set project root
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
