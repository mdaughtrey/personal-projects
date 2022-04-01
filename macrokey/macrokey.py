#!/usr/bin/env python3

import argparse
from flask import Flask
import json
import logging
import pdb
import sys

cmdline = None
logger = None
config = {'buttons': []}
app = Flask(__name__)

def setLogging():
    global logger
    FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('macrokey')
    fileHandler = logging.FileHandler(filename = './macrokey.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)
    soutHandler = logging.StreamHandler(stream=sys.stdout)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

def parseCommandLine():
    parser = argparse.ArgumentParser()
    parser.add_argument('--frames', dest='frames', type=int, default=1e6, help='frames to capture')
    parser.add_argument('--dev', dest='dev', help='image device')
    parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
    parser.add_argument('--nofilm', dest='nofilm', action='store_true', default=False, help='run with no film loaded')
    parser.add_argument('--noled', dest='noled', action='store_true', default=False, help='run with no LED')
    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8')
    parser.add_argument('--dir', dest='dir', required=True, help='set project directory')
    parser.add_argument('--single', dest='singleframe', action='store_true', default=False, help='One image per frame')
    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
    parser.add_argument('--raspid', dest='raspid', type=int, default=0, help='raspistill PID')
    parser.add_argument('--picamera', dest='picamera', action='store_true', default=False, help='use picamera lib')
    parser.add_argument('--picameracont', dest='picameracont', action='store_true', default=False, help='use picamera lib')
    parser.add_argument('--frameinterval', dest='frameinterval', type=int, default=45, help='Frame Interval')
    parser.add_argument('--nocam', dest='nocam', action='store_true', default=False, help='no camera operations')
    parser.add_argument('--intervals', dest='intervals', help='n,n,n...', required=True)
    parser.add_argument('--res', dest='res', choices=['draft','1k', 'hd', 'full'], help="resolution")
    global cmdline
    cmdline = parser.parse_args()

def loadConfig():
    try:
        global config
        config = json.load(open('config.json', 'rb'))
        for ii in range(len(config['buttons'])):
            config['buttons'][ii]['id'] = ii

    except Exception as ee:
        print(ee)
            
def homepage():
    top = """<html><head><title>Macrokey</title>
    <script>
        buttonpress = function(id) {
            xhr = new XMLHttpRequest()
            xhr.open('GET', 'http://macrokey:8000/buttonpress/' + id)
            xhr.send()
        }
    </script>
    </head><body><table>"""
    cols = 6
    index = 0
    buttons = ['<td><button onclick="buttonpress({});">{}</button></td>'.format(button['id'],button['label']) for button in config['buttons']]
    row = []
    while index < len(buttons):
        row.append('<tr>{}</tr>'.format(''.join(buttons[index:index + min(len(buttons)-index,cols)])))
        index += cols
    bottom = '</table></body></html>'
    return top + ''.join(row) + bottom

def procButtonPress(id):
    logger.debug('procButtonPress {}'.format(id))
    writeHid()
    return ""

def writeHid():
    with open('/dev/hidg0', 'rb+') as hid:
        hid.write('\0\0\4\0\0\0\0\0'.encode())
        hid.flush()
    pass

@app.route('/')
def home():
    return homepage()

@app.route('/buttonpress/<id>')
def buttonPress(id):
    return procButtonPress(id) 

def main():
    setLogging()
    logging
    loadConfig()
    app.run(host='0.0.0.0', port=8000)

main()

