#!/usr/bin/env python3

import argparse
import colorsys
from flask import Flask, redirect, url_for
import getpass
from hidmap import hidmap
import json
import logging
import pdb
import platform
import signal
import sys
import threading as th
import time

import base64
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.fernet import Fernet

cmdline = None
logger = None
pageCache = {}
app = Flask(__name__)
lock = th.Lock()

def handler(signum, frame):
    print("Ctrl-C: Acquiring Lock")
    lock.acquire()
    print("Ctrl-C: Releasing Lock")
    lock.release()
    print("Ctrl-C: Exit")
    sys.exit(1)


def setLogging():
    global logger
    FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
    #logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logging.basicConfig(level = logging.ERROR, format=FormatString)
    logger = logging.getLogger('macrokey')
    fileHandler = logging.FileHandler(filename = './macrokey.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)
    soutHandler = logging.StreamHandler(stream=sys.stdout)
    soutHandler.setFormatter(logging.Formatter(fmt=FormatString))
    soutHandler.setLevel(logging.DEBUG)
#    logger.addHandler(soutHandler)

def parseCommandLine():
    parser = argparse.ArgumentParser()
    parser.add_argument('--nosend', dest='nosend', action='store_true', help='no HID reports')
#    parser.add_argument('--jigglemin', dest='jigglemin', type=int, help='set jiggle period (mins)')
#    parser.add_argument('--jigglekey', dest='jigglekey', help='set jiggle key (from hidmap.py)')
    parser.add_argument('--noenc', dest='noenc', action='store_true', help='config file is not encrypted')
    parser.add_argument('--configfile', dest='configfile', help='config file location', default='config.json')
    parser.add_argument('--enc', dest='enc', action='store_true', help='encrypt')
    parser.add_argument('--dec', dest='dec', action='store_true', help='decrypt')
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
    return report


def loadConfig(file, password):
    while True:
        if '' == password:
            config = json.loads(open(file, 'rb').read())
        else:
            config = json.loads(dec(file, password))

        if 'jiggle' in config:
            report = []
            report.extend(procConfigChunk(config['jiggle']['emit']))
            report.append([0] * 8)
            config['jiggle']['report'] = report

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
    try:
        writeHid(button['report'])
    except:
        pass
    return ""


def writeHid(report):
    logger.debug('writeHid {}'.format(report))
    lock.acquire()
#    if cmdLine.nosend: return
    with open('/dev/hidg0', 'wb+') as hid:
        for buf in report:
            written = 0
            logger.debug('Writing {}'.format(buf))
            while written < 8:
                try:
                    written += hid.write(bytearray(buf))
                    hid.flush()

                except BlockingIOError as ee:
                    time.sleep(0.01)

                except BrokenPipeError as ee:
                    logger.error(ee)
                    lock.release()
                    return
    lock.release()

def genkey(provided):
    password = provided.encode()
    salt = b'adeff'
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend())

    key = base64.urlsafe_b64encode(kdf.derive(password))
    return key

def enc(file, password):
    key = genkey(password)
    f = Fernet(key)
    data = f.encrypt(open(file, 'rb').read())
    open(file, 'wb').write(data)

def dec(file, password):
    key = genkey(password)
    f = Fernet(key)
    data = f.decrypt(open(file, 'rb').read())
    return data

@app.route('/')
def home():
    return genpage('home')

@app.route('/<pageid>')
def page(pageid):
    return genpage(pageid)

@app.route('/click/<pageid>/<id>')
def buttonPress(pageid, id):
    return procButtonPress(pageid, int(id) )
    
def jiggle():
    print('Jiggle!\n')
    try:
        writeHid(config['jiggle']['report'])
    except:
        pass

    t = th.Timer(config['jiggle']['emit']['mins'] * 60, jiggle)
    t.start()

def main():
    global cmdLine
    signal.signal(signal.SIGINT, handler)
    cmdline = parseCommandLine()
#    if (None, None) != (cmdline.jigglekey, cmdline.jigglemin):
#        if (cmdline.jigglemin != cmdline.jigglekey) and None in (cmdline.jigglemin, cmdline.jigglekey):
#            print('jigglekey and jigglemin must both be specified')
#            sys.exit(1)
#
#        if cmdline.jigglekey not in hidmap.keys():
#            print('jigglekey {} is unknown'.format(cmdline.jigglekey))
#            sys.exit(1)

    if cmdline.noenc:
        password = ''
    else:
        password = getpass.getpass()

    if cmdline.dec:
        data = dec(cmdline.configfile, password)
        open(cmdline.configfile, 'wb').write(data)
        sys.exit(0)
    elif cmdline.enc:
        enc(cmdline.configfile, password)
        sys.exit(0)

    global config
    setLogging()
    config = loadConfig(cmdline.configfile, password)
    if 'jiggle' in config:
        t = th.Timer(config['jiggle']['emit']['mins'], jiggle)
        t.start()
    app.run(host='0.0.0.0', port=8000)

main()

