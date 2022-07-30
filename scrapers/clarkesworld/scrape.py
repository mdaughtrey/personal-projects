#!/usr/bin/env python3

import argparse
from bs4 import BeautifulSoup
import logging
import os
import pdb
import requests
import subprocess

def initlog():
    FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('scraper')
    fileHandler = logging.FileHandler(filename = './scraper.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)
    return logger

def commandline():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dryrun', dest='dryrun', default=False, action='store_true')
    parser.add_argument( dest='src', help='source (URL)')
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
    return parser.parse_args()

def getstory(log, title, url):
    if os.path.exists(f'{title}.html'):
        return
    log.info(title)
    doc = requests.get(url)
    if 200 != doc.status_code:
        log.error(f'getstory returns {r.status_code}')
        return
    soup = BeautifulSoup(doc.text, 'html.parser')
    text = soup.find(class_='story-text')
    open(f'{title}.html', 'wb').write(f'{soup.html.title}<body>{text.prettify()}</body>'.encode())
    cmd = ['pandoc', '--to', 'epub', '--metadata', f'title:{title}', f'{title}.html', '-o', f'{title}.epub']
    log.info(cmd)
    subprocess.run(cmd)

def backissue(log, cmd):
#    if not os.path.exists('backissue.html'):
    doc = requests.get(cmd.src)
    if 200 != doc.status_code:
        log.error('http returns {}'.r.status_code)
        return
    html = doc.text
#    open('backissue.html', 'wb').write(html.encode())
#    html = open('backissue.html', 'rb').read().decode()
    soup = BeautifulSoup(html, 'html.parser')
    #stories = [story.a for story in soup.find_all(class_='story')]
    for index in soup(class_='index-table'):
        for section in index(class_='section'):
            if not 'FICTION' == section.text:
                continue
            stories = [story.a for story in section.parent(class_='story')]
            for title,url in [(s.text, s['href']) for s in stories]:
                getstory(log, title,url)
        
#    sections = [x.text for y in indices for x in y[class_='section']]

def main():
    log = initlog()
    cmdline = commandline()
    backissue(log, cmdline)

main()

