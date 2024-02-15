#!/bin/bash
timeout 95m wget https://securestreams4.autopo.st:1794/hi \
    -o /dev/null \
    -O "DJ Pokey $(date).mp3"

