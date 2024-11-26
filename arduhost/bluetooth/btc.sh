#!/bin/bash

# https://raspberrypi.stackexchange.com/questions/50496/automatically-accept-bluetooth-pairings#55589

sudo hciconfig hci0 reset
sudo hciconfig hci0 up
sudo hciconfig hci0 piscan
sudo hciconfig hci0 sspmode 1

exit 0

cat << CMDS |
power on
discoverable on
pairable on
trust 24:29:34:93:39:AB
pair 24:29:34:93:39:AB
agent NoInputNoOutput
default-agent
quit
CMDS

while read line; do
    bluetoothctl ${line}
done
