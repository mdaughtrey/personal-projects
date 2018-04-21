#!/bin/bash

#convert -page +0+154 /home/mattd/scans/nk//005/title/titleline_0.png -page +0+308 /home/mattd/scans/nk//005/title/titleline_1.png -page +0+462 /home/mattd/scans/nk//005/title/titleline_2.png -background transparent -layers merge /mnt/imageinput/titletext0.png
convert -page +154 /home/mattd/scans/nk//005/title/titleline_0.png -page +308 /home/mattd/scans/nk//005/title/titleline_1.png -page +462 /home/mattd/scans/nk//005/title/titleline_2.png -background transparent -layers merge /mnt/imageinput/titletext0.png
