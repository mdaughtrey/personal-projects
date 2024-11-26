#!/bin/env python3

import pdb
import pexpect
import subprocess
import sys


def main():
    pdb.set_trace()

    shcommands = ['power on','agent off','default-agent','pairable on','discoverable on']
    [ subprocess.run(f'bluetoothctl {x}'.split()) for x in shcommands ]
    bc = pexpect.spawn('/usr/bin/bluetoothctl')
#    bc.logfile = sys.stdout.buffer
#    bc.sendline('power on')
#    bc.expect('succeeded')
#    bc.sendline('agent off')
#    bc.expect('unregistered')
#    bc.sendline('default-agent')
#    bc.expect('registered')
#    bc.sendline('pairable on')
#    bc.expect('succeeded')
#    bc.sendline('discoverable on')
#    bc.expect('succeeded')
    bc.expect_exact('[New]')
    bc.expect_exact('Confirm passkey')
    bc.sendline('yes')
    bc.expect_exact('yes/no')
    bc.sendline('yes')

main()
