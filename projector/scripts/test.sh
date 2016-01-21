#!/bin/bash

case "$1" in
    one) echo one ;;
    two|three) echo something else;;
esac
