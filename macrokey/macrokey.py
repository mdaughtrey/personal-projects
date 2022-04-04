#!/usr/bin/env python3

import argparse
from flask import Flask
from hidmap import hidmap
import json
import logging
import pdb
import sys
import time

class Meta():
    def __init__(tag, name):
        self.tag = name

cmdline = None
logger = None
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
    soutHandler.setFormatter(logging.Formatter(fmt=FormatString))
    soutHandler.setLevel(logging.DEBUG)
    logger.addHandler(soutHandler)

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
#    try:
    while True:
        config = json.load(open('config.json', 'rb'))
        for ii in range(len(config['buttons'])):
            logger.debug(config['buttons'][ii]['label'])
            config['buttons'][ii]['id'] = ii
            report = []
            for chunk in config['buttons'][ii]['emit']:
                for ee in chunk.get('keys', []):
                    hid = hidmap[ee]
                    if isinstance(hid, int):
                        report.append([0, 0, hid, 0, 0, 0, 0, 0])
                    else:
                        report.append([hid[0], 0, hid[1], 0, 0, 0, 0, 0])
                if chunk.get('meta', None):
                    for mm in chunk['meta'].split(','):
                        hid = hidmap[mm]
                        report.append([0, 0, hid, 0, 0, 0, 0, 0])

            dups = [ii==jj for ii,jj in zip(report[:-1],report[1:])]
            r2 = []
            for kk,ll in enumerate(dups):
                r2.append(report[kk])
                if ll == True: r2.append([0] * 8)

            report = r2 + [report[-1]] + [[0] * 8]
            config['buttons'][ii]['report'] = report
        logger.debug('loadConfig exit')
        return config

#    except Exception as ee:
#        pdb.set_trace()
#        logger.error(str(ee))

            
def homepage():
    top = """<html><head><title>Macrokey</title>
    <script>
        buttonpress = function(id) {
            xhr = new XMLHttpRequest()
            xhr.open('GET', 'http://macrokey:8000/buttonpress/' + id)
            xhr.send()
        }
    </script><style>""" + open('style.css', 'rb').read().decode() + """</style>
    </head><body><div class="main">"""
    cols = 6
    index = 0
    rowHtml = '<div class="bouter"><button class="button" onclick="buttonpress({});" style="background-image: linear-gradient(to bottom,white,{} 10%);">{}</button></div>'
    buttons = [rowHtml.format(button['id'],button['color'],button['label']) for button in config['buttons']]
    row = []
    while index < len(buttons):
        row.append('<div class="row">{}</div><!-- row -->'.format(''.join(buttons[index:index + min(len(buttons)-index,cols)])))
        index += cols
    bottom = '</div><!-- main --></body></html>'
    return top + ''.join(row) + bottom

#def resolve(id):
#    pdb.set_trace()
#    keys = config['buttons'][int(id)]['keys']
#    tosend = []
#    for key in keys:
#        tosend.append([0, 0, hidmap[key], [0] * 5])
#    pass

def procButtonPress(id):
    logger.debug('procButtonPress {}'.format(id))
    writeHid(config['buttons'][id]['report'])
    return ""

def writeHid0(report):
    logger.debug("writeHid")
    return
    with open('/dev/hidg0', 'wb+') as hid:
        for buf in report:
            try:
                logger.debug('writing')
                n = hid.raw.write(bytearray(buf))
                logger.debug('written {}'.format(n))

            except BlockingIOError as ee:
                logger.debug('writeHid exception: ' + ee)
                time.sleep(0.01)
    logger.debug('exit writeHid')


def writeHid(report):
    with open('/dev/hidg0', 'wb+') as hid:
        for buf in report:
            written = 0
            logger.debug('Writing {}'.format(buf))
            while written < 8:
                try:
                    written += hid.write(bytearray(buf))
                    hid.flush()

                except BlockingIOError as ee:
#                    pdb.set_trace()
#                    logger.debug('writeHid exception: ' + ee)
#                    written += ee.characters_written
                    time.sleep(0.01)

@app.route('/')
def home():
    return homepage()

@app.route('/buttonpress/<id>')
def buttonPress(id):
    return procButtonPress(int(id) )

def main():
    global config
    setLogging()
    config = loadConfig()
    app.run(host='0.0.0.0', port=8000)

main()

