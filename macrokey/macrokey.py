#!/usr/bin/env python3

import argparse
import colorsys
from flask import Flask, redirect, url_for
from hidmap import hidmap
import json
import logging
import pdb
import platform
import sys
import time

#class Meta():
#    def __init__(tag, name):
#        self.tag = name

cmdline = None
logger = None
pageCache = {}
app = Flask(__name__)

def setLogging():
    global logger
    FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
    #logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logging.basicConfig(level = logging.ERROR, format=FormatString)
    logger = logging.getLogger('macrokey')
    fileHandler = logging.FileHandler(filename = './macrokey.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
#    logger.addHandler(fileHandler)
    soutHandler = logging.StreamHandler(stream=sys.stdout)
    soutHandler.setFormatter(logging.Formatter(fmt=FormatString))
    soutHandler.setLevel(logging.DEBUG)
#    logger.addHandler(soutHandler)

def parseCommandLine():
    parser = argparse.ArgumentParser()
    parser.add_argument('--new', dest='new', action='store_true', help='new config')
    parser.add_argument('--nosend', dest='nosend', action='store_true', help='no HID reports')
#    parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
#    parser.add_argument('--nofilm', dest='nofilm', action='store_true', default=False, help='run with no film loaded')
#    parser.add_argument('--noled', dest='noled', action='store_true', default=False, help='run with no LED')
#    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8')
#    parser.add_argument('--dir', dest='dir', required=True, help='set project directory')
#    parser.add_argument('--single', dest='singleframe', action='store_true', default=False, help='One image per frame')
#    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
#    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
#    parser.add_argument('--raspid', dest='raspid', type=int, default=0, help='raspistill PID')
#    parser.add_argument('--picamera', dest='picamera', action='store_true', default=False, help='use picamera lib')
#    parser.add_argument('--picameracont', dest='picameracont', action='store_true', default=False, help='use picamera lib')
#    parser.add_argument('--frameinterval', dest='frameinterval', type=int, default=45, help='Frame Interval')
#    parser.add_argument('--nocam', dest='nocam', action='store_true', default=False, help='no camera operations')
#    parser.add_argument('--intervals', dest='intervals', help='n,n,n...', required=True)
#    parser.add_argument('--res', dest='res', choices=['draft','1k', 'hd', 'full'], help="resolution")
#    global cmdline
#    cmdline = parser.parse_args()
    return parser.parse_args()

def procConfigChunk0(chunk):
    report = []
    for chunk in config['buttons'][ii]['emit']:
        report = [0] * 8
        for mm in chunk.get('meta',"").split(','):
            hid = hidmap[mm]
            report[0] |= hid

        for ee in chunk.get('keys', []):
            hid = hidmap[ee]
            if isinstance(hid, int):
                report[2] = hid
            else:
                report[0] |= hid[0]
                report[2] = hid[1]

        for aa in chunk.get('action', "").split(','):
            hid = hidmap[aa]
            if isinstance(hid, int):
                report[2] = hid
            else:
                report[0] |= hid[0]
                report[2] = hid[1]
                
    return report

def procConfigChunk(chunk):
    report = []
    if chunk.get('meta', None):
        for mm in chunk['meta'].split(','):
            m = mm.split('*')
            loop = 1
            if len(m) == 2: loop = int(m[1])
            for ll in range(loop):
                hid = hidmap[m[0]]
                if isinstance(hid, int):
                    report.append([0, 0, hid, 0, 0, 0, 0, 0])
                else:
                    report.append([hid[0], 0, hid[1], 0, 0, 0, 0, 0])

    for ee in chunk.get('keys', []):
        hid = hidmap[ee]
        if isinstance(hid, int):
            report.append([0, 0, hid, 0, 0, 0, 0, 0])
        else:
            report.append([hid[0], 0, hid[1], 0, 0, 0, 0, 0])

#        for aa in chunk.get('action', "").split(','):
#            hid = hidmap[aa]
#            if isinstance(hid, int):
#                report.append([0, 0, hid, 0, 0, 0, 0, 0])
#            else:
#                report.append([hid[0], 0, hid[1], 0, 0, 0, 0, 0])
    return report


def loadConfig():
#    try:
    while True:
        config = json.load(open('config.json', 'rb'))
        for ii in range(len(config['buttons'])):
            logger.debug(config['buttons'][ii]['label'])
            config['buttons'][ii]['id'] = ii
            # Build the HID report string
            report = []
            for chunk in config['buttons'][ii]['emit']:
                report.extend(procConfigChunk(chunk))

            dups = [ii==jj for ii,jj in zip(report[:-1],report[1:])]
            r2 = []
            for kk,ll in enumerate(dups):
                r2.append(report[kk])
                if ll == True: r2.append([0] * 8)

            report = r2 + [report[-1]] + [[0] * 8]
            config['buttons'][ii]['report'] = report

            # Build upper and lower colors
            color = int(config['buttons'][ii]['color'][1:], 16)
            rgb = [(color >> 16 & 0xff)/255, (color >> 8 & 0xff)/255, (color & 0xff)/255]
            upperColor = list([ee for ee in colorsys.rgb_to_hls(*rgb)])
            lowerColor = list([ee for ee in colorsys.rgb_to_hls(*rgb)])
            config['buttons'][ii]['uppercolor'] = 'hsl({},{}%,60%)'.format(360 * upperColor[0], 100 * upperColor[2])
            config['buttons'][ii]['lowercolor'] = 'hsl({},{}%,30%)'.format(360 * lowerColor[0], 100 * lowerColor[2])

        logger.debug('loadConfig exit')
        return config

def loadConfigNew():
#    try:
    while True:
        config = json.load(open('config.json.new', 'rb'))
        for page in config['pages']:
            for ii in range(len(page['buttons'])):
                logger.debug(page['buttons'][ii]['label'])
                page['buttons'][ii]['id'] = ii
                # Build the HID report string
                report = []
                if 'emit' in page['buttons'][ii].keys():
                    for chunk in page['buttons'][ii]['emit']:
                        report.extend(procConfigChunk(chunk))

                    dups = [ii==jj for ii,jj in zip(report[:-1],report[1:])]
                    r2 = []
                    for kk,ll in enumerate(dups):
                        r2.append(report[kk])
                        if ll == True: r2.append([0] * 8)

                    report = r2 + [report[-1]] + [[0] * 8]
                    page['buttons'][ii]['report'] = report

                # Build upper and lower colors
                color = int(page['buttons'][ii]['color'][1:], 16)
                rgb = [(color >> 16 & 0xff)/255, (color >> 8 & 0xff)/255, (color & 0xff)/255]
                upperColor = list([ee for ee in colorsys.rgb_to_hls(*rgb)])
                lowerColor = list([ee for ee in colorsys.rgb_to_hls(*rgb)])
                page['buttons'][ii]['uppercolor'] = 'hsl({},{}%,60%)'.format(360 * upperColor[0], 100 * upperColor[2])
                page['buttons'][ii]['lowercolor'] = 'hsl({},{}%,30%)'.format(360 * lowerColor[0], 100 * lowerColor[2])
            config[page['page']] = page

        logger.debug('loadConfig exit')
        return config

#    except Exception as ee:
#        logger.error(str(ee))

            
def genpage(pageid):
    global pageCache
    if pageid not in config.keys(): return ""
    if pageid in pageCache.keys(): return pageCache[pageid]
    top = """<html><head><title>Macrokey</title>
    <script>
        buttonpress = function(pageid, id) {{
            xhr = new XMLHttpRequest()
            xhr.open('GET', 'http://{}:8000/click/' + pageid + '/' + id)
            xhr.send()
        }}
    </script><style>""".format(platform.node()) + open('style.css', 'rb').read().decode() + """</style>
    </head><body><div class="main">"""
    buttonAction = '<div class="bouter"><button class="button" onclick="buttonpress(\'{}\',{});" style="background-image: linear-gradient(to bottom,{},{} 10%,{} 80%,{} 95%);">{}</button></div>'
    buttonPage = '<div class="bouter"><a class="button" href="http://{}:8000/{}" style="background-image: linear-gradient(to bottom,{},{} 10%,{} 80%,{} 95%);">{}</a></div>'

    rows = []
    cols = 6
    index = 0
    buttons = config[pageid]['buttons']
    for button in buttons:
        if 'page' in button.keys():
            rows.append(buttonPage.format(platform.node(), 
            *[button[x] for x in ('page', 'uppercolor', 'color', 'color', 'lowercolor', 'label')]))
        else:
            rows.append(buttonAction.format(pageid,
            *[button[x] for x in ('id','uppercolor', 'color', 'color', 'lowercolor', 'label')]))

    html = ''
    while index < len(rows):
        html += '<div class="row">{}</div><!-- row -->'.format(''.join(rows[index:index + min(len(rows)-index,cols)]))
        index += cols
    bottom = '</div><!-- main --></body></html>'
    pageCache[pageid] = top + ''.join(html) + bottom
    return pageCache[pageid]

def procButtonPress(pageid, id):
    if pageid not in config.keys():
        logger.error('Unknown page id {}'.format(pageid))
        return
    if id >= len(config[pageid]['buttons']):
        logger.error('Unknown button id {}/{}'.format(pageid, id))
        return ""
    
    logger.debug('procButtonPress {}/{}'.format(pageid, id))
    button = config[pageid]['buttons'][id]
    if 'page' in button.keys():
        return genpage(button['page'])
#        return redirect('http://{}:8000/{}'.format(platform.node(), button['page'], 302))
    writeHid(button['report'])
    return ""


def writeHid(report):
    logger.debug('writeHid {}'.format(report))
    if cmdLine.nosend: return
    with open('/dev/hidg0', 'wb+') as hid:
        for buf in report:
            written = 0
            logger.debug('Writing {}'.format(buf))
            while written < 8:
                try:
                    written += hid.write(bytearray(buf))
                    hid.flush()

                except BlockingIOError as ee:
#                    logger.debug('writeHid exception: ' + ee)
#                    written += ee.characters_written
                    time.sleep(0.01)

                except BrokenPipeError as ee:
                    logger.error(ee)
                    return

@app.route('/')
def home():
    return genpage('home')

@app.route('/<pageid>')
def page(pageid):
    return genpage(pageid)

@app.route('/click/<pageid>/<id>')
def buttonPress(pageid, id):
    return procButtonPress(pageid, int(id) )
    

def main():
    global cmdLine
    cmdLine = parseCommandLine()
    global config
    setLogging()
    if cmdLine.new:
        config = loadConfigNew()
    else:
        config = loadConfig()
    app.run(host='0.0.0.0', port=8000)

main()

