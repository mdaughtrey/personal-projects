#!/bin/env python3
# https://stackoverflow.com/questions/28632349/process-to-test-and-add-new-wireless-network-configuration-through-wpa-cli

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306
import pdb
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
import re
import subprocess

from optparse import OptionParser
import sys
import dbus
import dbus.service
import dbus.mainloop.glib
try:
  from gi.repository import GObject, GLib
except ImportError:
  import gobject as GObject
  import glib as GLib
import bluetooth
#import subprocess
#import pdb
#import re

BUS_NAME = 'org.bluez'
AGENT_INTERFACE = 'org.bluez.Agent1'
AGENT_PATH = '/test/agent'

bus = None
device_obj = None
dev_path = None
mainloop = GLib.MainLoop()
disp = Adafruit_SSD1306.SSD1306_128_64(rst=0)


def getssid():
    r = subprocess.run(['iwconfig'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    line = [re.search(b'ESSID:"(\w+)"', ll) for ll in r.stdout.split() if re.search(b'ESSID:"(\w+)"', ll)]
    return line[0].groups()[0]
    
def ask(prompt):
    try:
        return raw_input(prompt)
    except:
        return input(prompt)

def set_trusted(path):
    props = dbus.Interface(bus.get_object('org.bluez', path),
                    'org.freedesktop.DBus.Properties')
    props.Set('org.bluez.Device1', 'Trusted', True)

def dev_connect(path):
    dev = dbus.Interface(bus.get_object('org.bluez', path),
                            'org.bluez.Device1')
    dev.Connect()

class Rejected(dbus.DBusException):
    _dbus_error_name = 'org.bluez.Error.Rejected'

class Agent(dbus.service.Object):
    exit_on_release = True

    def set_exit_on_release(self, exit_on_release):
        self.exit_on_release = exit_on_release

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='', out_signature='')
    def Release(self):
        print('Release')
        if self.exit_on_release:
            mainloop.quit()

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='os', out_signature='')
    def AuthorizeService(self, device, uuid):
        print(f'AuthorizeService {device},{uuid}')
        authorize = ask('Authorize connection (yes/no): ')
        if (authorize == 'yes'):
            return
        raise Rejected('Connection rejected by user')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='o', out_signature='s')
    def RequestPinCode(self, device):
        print(f'RequestPinCode {device}')
        set_trusted(device)
        return ask('Enter PIN Code: ')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='o', out_signature='u')
    def RequestPasskey(self, device):
        print(f'RequestPasskey {device}')
        set_trusted(device)
        passkey = ask('Enter passkey: ')
        return dbus.UInt32(passkey)

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='ouq', out_signature='')
    def DisplayPasskey(self, device, passkey, entered):
        print(f'DisplayPasskey ({device}, {passkey:03} entered {entered})')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='os', out_signature='')
    def DisplayPinCode(self, device, pincode):
        print(f'DisplayPinCode {device}, {pincode}')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='ou', out_signature='')
    def RequestConfirmation(self, device, passkey):
        print(f'Autopair {device}')
#        print('RequestConfirmation (%s, %06d)' % (device, passkey))
#        confirm = ask('Confirm passkey (yes/no): ')
#        if (confirm == 'yes'):
        set_trusted(device)
        mainloop.quit()
#        raise Rejected('Passkey doesn't match')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='o', out_signature='')
    def RequestAuthorization(self, device):
        print(f'RequestAuthorization {device}')
        auth = ask('Authorize? (yes/no): ')
        if (auth == 'yes'):
            return
        raise Rejected('Pairing rejected')

    @dbus.service.method(AGENT_INTERFACE,
                    in_signature='', out_signature='')
    def Cancel(self):
        print('Cancel')

def pair_reply():
    print('Device paired')
    set_trusted(dev_path)
    dev_connect(dev_path)
    mainloop.quit()

def pair_error(error):
    err_name = error.get_dbus_name()
    if err_name == 'org.freedesktop.DBus.Error.NoReply' and device_obj:
        print('Timed out. Cancelling pairing')
        device_obj.CancelPairing()
    else:
        print(f'Creating device failed: {error}')


    mainloop.quit()


def set_network(ssid,password):
    def tryrun(cmd, exmsg):
        proc = subprocess.run(f'wpa_cli {cmd}'.split(), stdout = subprocess.PIPE) 
        if proc.returncode: raise RuntimeError(exmsg)
        ok = [m for m in proc.stdout.split(b'\n') if m == b'OK']
        if 0 == len(ok): raiseRuntimeError(exmsg)

    proc = subprocess.run(f'wpa_cli add_network'.split(), stdout = subprocess.PIPE) 
    id = [re.match(b'\d+',m) for m in proc.stdout.split() if re.match(b'\d+',m)]
    if id[0]:
        id = id[0].group(0).decode()
    else:
        raise RuntimeError('id')

    tryrun(f'set_network {id} ssid "{ssid}"', 'ssid')
    tryrun(f'set_network {id} psk "{password}"','psk')
    tryrun(f'enable {id}', 'enable')
    tryrun(f'save_config', 'config')
    tryrun(f'reconfigure', 'reconfigure')


def socket_loop():
    uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

    server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    server_sock.bind(("",bluetooth.PORT_ANY))
    server_sock.listen(1)

    port = server_sock.getsockname()[1]

    bluetooth.advertise_service( server_sock, "SampleServer", service_id = uuid, 
                                 service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                                 profiles=[bluetooth.SERIAL_PORT_PROFILE], 
                                 )

    print("Waiting for connection on RFCOMM channel %d" % port)

    client_sock, client_info = server_sock.accept()
    disp_status('Connected')
    print("Accepted connection from ", client_info)

    # Automatically accept pairing 
    #client_sock.set_security(bluetooth.BT_SECURITY_LOW)

    ssid=None
    psk=None
    enc=None

    while True:
        try:
            data = client_sock.recv(1024)
            if not data: break
            print(f'received [{data}]')
            if data == b'y\r\n' and not None in [ssid, psk]:
                print('Writing config\r\n')
                try:
                    set_network(ssid.decode(),psk.decode())
                except RuntimeError as rt:
                    client_sock.send(f'set_network error {str(rt)}')
                    disp_status(str(rt))
                    return

                client_sock.send('Restarting dhcpcd...')
                disp_status('Restarting dhcpcd')
                proc = subprocess.run('systemctl restart dhcpcd'.split())
                client_sock.send(f'{proc.returncode}\r\n')

            params = re.match(b'WIFI:S:(\w+);T:(\w+);P:(\w+);', data)
            if params is not None:
                (ssid,enc,psk) = params.groups()
                client_sock.send(f'SSID {ssid.decode()} enc {enc.decode()} psk {psk.decode()} (y/n)?\r\n')
#            print(f'Sending {data.upper()}')
#            client_sock.send(data.upper())
        except:
            break
    

#if __name__ == '__main__':
def simple_agent():
    subprocess.run('hciconfig hci0 piscan'.split())
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    global bus
    bus = dbus.SystemBus()

    # NoInputNoOutput is broken in the distribution's latest version of bluez
    capability = 'KeyboardDisplay'

    parser = OptionParser()
    parser.add_option('-i', '--adapter', action='store',
                    type='string',
                    dest='adapter_pattern',
                    default=None)
    parser.add_option('-c', '--capability', action='store',
                    type='string', dest='capability')
    parser.add_option('-t', '--timeout', action='store',
                    type='int', dest='timeout',
                    default=60000)
    (options, args) = parser.parse_args()
    if options.capability:
        capability  = options.capability

    path = '/test/agent'
    agent = Agent(bus, path)

#    mainloop = GLib.MainLoop()

    obj = bus.get_object(BUS_NAME, '/org/bluez');
    manager = dbus.Interface(obj, 'org.bluez.AgentManager1')
    manager.RegisterAgent(path, capability)

    print('Agent registered')

    # Fix-up old style invocation (BlueZ 4)
#    if len(args) > 0 and args[0].startswith('hci'):
#        options.adapter_pattern = args[0]
#        del args[:1]

    if len(args) > 0:
        device = bluezutils.find_device(args[0],
                        options.adapter_pattern)
        dev_path = device.object_path
        agent.set_exit_on_release(False)
        device.Pair(reply_handler=pair_reply, error_handler=pair_error,
                                timeout=60000)
        global device_obj
        device_obj = device
    else:
        manager.RequestDefaultAgent(path)

    mainloop.run()

def disp_status(text):
    disp.clear()
    image = Image.new('1', (disp.width, disp.height))
    draw = ImageDraw.Draw(image)
    font = ImageFont.load_default()
    draw.multiline_text((0,-2), text, font=font, fill=255)
    disp.image(image)
    disp.display()


def main():
    disp.begin()
    disp_status('init')
    ssid = ''
    ipa = ''
    try:
        ssid = getssid().decode()
        ipa = subprocess.run('hostname -I'.split(), stdout=subprocess.PIPE).stdout.split()[0].strip().decode()
        disp_status(f'SSID: {ssid}\n{ipa}\n')
    except:
        disp_status(f'Wifi fail\nSSID: {ssid}\n{ipa}\n')

    if '' in [ssid,ipa]:
        disp_status(f'SSID: {ssid}\n{ipa}\nStarting Bluetooth\nWaiting to pair\n')
        simple_agent()
        disp_status('Paired\nWaiting for connection')
        socket_loop()


#    draw.multiline_text((0,-2), f'SSID: {ssid}\n{ipa}', font=font, fill=255)
#    disp.image(image)
#    disp.display()

main()
