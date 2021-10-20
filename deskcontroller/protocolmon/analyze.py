#!/usr/bin/python3

for line in open('serial.log', 'r').readlines():
    print(f'{len(line)} ', end = '')


