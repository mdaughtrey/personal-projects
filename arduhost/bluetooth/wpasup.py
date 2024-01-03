#!/bin/env python3

import pdb

def set_network(ssid,password):
    wps_file='/etc/wpa_supplicant/wpa_supplicant.conf'
    #wps_file='wpa_supplicant.conf'
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

set_network('yfinity2','newpass2')
