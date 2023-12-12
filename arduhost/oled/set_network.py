#!/bin/env python3

import pdb
import re
import subprocess

def set_network(ssid, password):
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

set_network('testssid','testpass')


