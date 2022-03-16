#!/bin/bash

# half turn lift
PROJECT=mechtest6
sudo ./hqcap1.py \
    --film=8mm \
    --dev=/mnt/exthd \
    --dir /mnt/exthd/${PROJECT} \
    --single --picameracont \
    --intervals=43,43,43,42,43,43,43,42,43,43,43,42,43,43,43,42,43,43,42
