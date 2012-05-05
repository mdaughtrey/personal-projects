#!/bin/bash

date=$1
tempfile=/tmp/tempmon.txt

#2009/01/18,00:00:01,326,325,320,324,368,367,446,0,
cat /var/local/tempmon_${date}.log | sed -e 's/,/ /g;' | perl -ne '@arr=split;print "$arr[0] $arr[1] "; print join(" ", map {$_/16} @arr[2..$#arr]); print "\n";' > $tempfile

gnuplot - <<GPFILE
set xdata time
set timefmt "%H:%M:%S"
#set datafile separator ","
set title "Radiant Heat $date"
set ylabel "Degrees C"
set bmargin 4

plot '$tempfile' using 2:3 with lines title "Loop A Return"
replot '$tempfile' using 2:4 with lines title "Loop B Return"
replot '$tempfile' using 2:5 with lines title "Loop C Return"
replot '$tempfile' using 2:6 with lines title "Hot Manifold"
replot '$tempfile' using 2:7 with lines title "From Furnace"
replot '$tempfile' using 2:8 with lines title "To House"
replot '$tempfile' using 2:9 with lines title "From House"
replot
pause 90
GPFILE


