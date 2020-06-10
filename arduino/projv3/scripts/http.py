#!/usr/bin/python

import httplib
import pdb
import re

conn = httplib.HTTPConnection('192.168.0.222')
print("Requesting /command.cgi?op=100&DIR=/DCIM")
conn.request("GET", "/command.cgi?op=100&DIR=/DCIM")
print("Requested /command.cgi?op=100&DIR=/DCIM")
response = conn.getresponse()

lines = response.read().split('\r\n')[1:-1]
print lines
dirNums = []
for line in lines:
    elems = line.split(',')
    matched = re.search('(\d\d\d)PHOTO', elems[1])
    if matched is None:
        continue
    dirNums.append(int(matched.groups()[0]))

print "dirNums %s" % dirNums
photoFiles = []
for dirNum in dirNums[:1]:
    print("Requesting /command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)
    conn.request("GET", "/command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)
    print("Requested /command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)
    response = conn.getresponse()
    lines = response.read().split('\r\n')[1:-1]
    for line in lines:
        elems = line.split(',')
        matched = re.search('SAM_\d+.JPG', elems[1])
        if matched is None:
            continue
        url = "/DCIM/%sPHOTO/%s" % (dirNum, elems[1])
        print url
        conn.request("GET", url)
        jpg = conn.getresponse()
        try:
            open(elems[1], 'w').write(jpg.read())
        except:
            print "Error reading %s" % url

        print "Deleting %s" % url
        conn.request("GET", "/upload.cgi?DEL=%s" % url)
        delresponse = conn.getresponse()
        print delresponse.read()
#        photoFiles.append(elems[1])
    # delete the folder
    print "Deleting %03uPHOTO" % dirNum
    conn.request("GET", "/upload.cgi?DEL=/DCIM/%03uPHOTO" % dirNum)
    delresponse = conn.getresponse()
    print delresponse.read()


