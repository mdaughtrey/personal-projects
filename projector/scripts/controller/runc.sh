#!/bin/bash

<<<<<<< HEAD
#PROJECT=test0001
PROJECT=test8mm000

#./controller.py --jobmode proc --project blister --film super8 #--worker pc --worker ac  # --worker tf --worker gc
./controller.py --jobmode proc --project $PROJECT --film 8mm
=======
#./controller.py --workman proc --project blister --film super8 --worker pc --worker ac
./controller.py --jobmode disable --project blister --film super8 
#./controller.py --workman inline --project blister --film super8
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
