#!/bin/bash

OUTFILE=$(echo "DJ_Pokey_$(date).mp3" | sed 's/ /_/g;s/:/_/g')
timeout 100m wget https://securestreams4.autopo.st:1794/hi -o /dev/null -O /home/mattd/Downloads/"${OUTFILE}" >> /var/tmp/whrw.txt
