#!/bin/bash

IP=192.168.107.1

init()
{
    wget -q http://$IP/api/v1/input/inject?keydown=Super_L 2>&1 > /dev/null
    wget -q http://$IP/api/v1/input/inject?keydown=Super_R 2>&1 > /dev/null
    wget -q http://$IP/api/v1/input/inject?keyup=Super_L 2>&1 > /dev/null
    wget -q http://$IP/api/v1/input/inject?keyup=Super_R 2>&1 > /dev/null
}

init
