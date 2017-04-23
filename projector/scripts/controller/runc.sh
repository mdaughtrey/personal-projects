#!/bin/bash

#PROJECT=test0001
PROJECT=test8mm000

#./controller.py --jobmode proc --project blister --film super8 #--worker pc --worker ac  # --worker tf --worker gc
./controller.py --jobmode proc --project $PROJECT --film 8mm
