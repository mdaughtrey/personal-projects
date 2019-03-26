#!/usr/bin/python
# 57 -W no white balance

import imageio
import numpy as np
import os
import pdb
import serial
import subprocess

DCRAW='/home/mattd/personal-projects/projector/dcraw/dcraw'
SerialPort="/dev/ttyUSB0"

def portWaitFor(port, text):
    accum = b''
    print("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
    print("Received %s" % text)
    return accum

def init():
    serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
    print("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    serPort.write(b' ')
    portWaitFor(serPort, b'{State:Ready}')
    serPort.write(b'l')
#    serPort.write("vc{0}t".format({'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]).encode('utf-8')) 
#    portWaitFor(serPort, b'{pt:')
    return serPort

def takeapic(exposure):
    runargs = ("/usr/local/bin/raspiraw --mode 0 --header --i2c 0 --expus %u " % exposure,
                "--fps 1 -t 1000 -sr 1 -o /mnt/extfd/exp_nw%u.raw" % exposure)
    retcode = subprocess.call(''.join(runargs), shell=True, stderr=None)
    if retcode:
        print("raspiraw retcode %u" % retcode)
        sys.exit(1)

#    pdb.set_trace()
    retcode = subprocess.call((DCRAW + ' -W  /mnt/extfd/exp_nw%u.raw' % exposure), shell=True, stderr=None)
#    retcode = subprocess.call((DCRAW + ' -W -T /mnt/extfd/exp%u.raw' % exposure), shell=True, stderr=None)
    if retcode:
        print("dcraw retcode %u" % retcode)
        sys.exit(1)

    thefile = np.array(imageio.imread('/mnt/extfd/exp_nw%u.ppm' % exposure), np.uint8)
    max = np.max(thefile)
    print("exposure %u max %u" % (exposure, max))
    return max


def main():
    exposure = 200;
    delta = exposure/2
    cwd = os.getcwd()
    os.chdir('/home/mattd/personal-projects/projector/raspiraw')
    retcode = subprocess.call('./camera_i2c', shell = True, stderr = None)
    print("camera_i2c %u" % retcode)
    if retcode:
        sys.exit(1)
    os.chdir(cwd)
    up = True
#    pdb.set_trace()

    while True:
        picmax = takeapic(exposure)
        print("picmax %u exposure %u delta %u" % (picmax, exposure, delta))
        if picmax < 254:
            exposure += delta
            if False == up:
                delta = int(delta/2)
                up = True
        elif picmax == 255:
            exposure -= delta
            if True == up:
                delta = int(delta/2)
                up = False
        else:
            break;

    print("Final exposure %u" % exposure)


init()
main()
