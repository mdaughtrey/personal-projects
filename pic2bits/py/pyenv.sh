#!/bin/bash

case "$1" in 
init) python3 -m venv pic2bits ;;
activate) source pic2bits/bin/activate ;;
deactivate) deactivate ;;
*) echo What? ;;
esac
