#!/bin/bash
timeout 100m wget https://securestreams4.autopo.st:1794/hi \
    -o /dev/null \
    -O /home/mattd/Downloads/"DJ Pokey $(date).mp3"

