#!/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
# Adapted and ported to python3 from 
# https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/test
# and
# https://github.com/pybluez/pybluez/blob/master/examples/simple/rfcomm-server.py

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
import subprocess
import pdb
import re

BUS_NAME = 'org.bluez'
AGENT_INTERFACE = 'org.bluez.Agent1'
AGENT_PATH = '/test/agent'

bus = None
device_obj = None
dev_path = None
mainloop = GLib.MainLoop()

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
    #wps_file='/etc/wpa_supplicant/wpa_supplicant.conf'
    wps_file='/home/mattd/personal-projects/arduhost/oled/wpa_supplicant.conf'
    wps =  open(wps_file).read().split()
    starts = [ii for (ii,vv) in enumerate(wps) if vv == 'network={']
    ends = [ii+1 for (ii,vv) in enumerate(wps) if vv == '}']
    header = wps[0:starts[0]]
    networks = []
    for (ii,jj) in zip(starts,ends):
        networks.append(wps[ii:jj])
    footer = wps[ends[-1]:]

    found = [ii for (ii,jj) in enumerate(networks) if jj[1] == f'ssid="{ssid}"']
    if found == []:
        networks.append(['network={',f'ssid="{ssid}"',f'psk={password}','}'])
    else:
        networks[found[0]] = ['network={',f'ssid="{ssid}"',f'psk={password}','}']

    with open(wps_file,'wb') as wps:
        wps.write('\n'.join(header).encode())
        wps.write(b'\n')
        for n in networks:
            wps.write('\n'.join(n).encode())
            wps.write(b'\n')
        wps.write('\n'.join(footer).encode())
        wps.write(b'\n')


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
                pdb.set_trace()
                set_network(ssid.decode(),psk.decode())
                client_sock.send('Restarting dhcpd...')
# proc = subprocess.run('systemctl restart dhcpd'.split()]
#                client_sock.send(f'rc {proc.returncode}\r\n')

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


def main():
    subprocess.run('hciconfig hci0 piscan'.split())
    simple_agent()
    print('mainloop done, start socketloop')
    socket_loop()

main()
