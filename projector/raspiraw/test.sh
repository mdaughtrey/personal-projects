#/bin/bash
#./raspiraw -md 7 -t 1000 -ts tstamps.csv -hd0 hd0.32k -h 64 --vinc 1F --fps 660 -r "380A,0040;3802,78;3806,0603" -sr 1 -o /dev/shm/out.%04d.raw 

rm /dev/shm/*.raw /dev/shm/*.ppm
./camera_i2c
#time raspistill --raw -o /tmp/test.jpg
mode=0
i2c=0
FILENAME=out.raw
FPS=1
EXP=2000

./raspiraw --mode ${mode} --header --i2c ${i2c} --expus 40000 --fps $FPS -t 250 -sr 1 -o /dev/shm/out.40k.%04d.raw 
./raspiraw --mode ${mode} --header --i2c ${i2c} --expus 80000 --fps $FPS -t 500 -sr 1 -o /dev/shm/out.80k.%04d.raw 
./raspiraw --mode ${mode} --header --i2c ${i2c} --expus 1200000 --fps $FPS -t 2000 -sr 1 -o /dev/shm/out.1m2.%04d.raw 
ls /dev/shm

for ff in /dev/shm/*.raw; do
    dcraw/dcraw $ff
done
