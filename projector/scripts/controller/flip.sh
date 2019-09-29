#/usr/bin/env bash
for ff in ???/converted/*.jpg; do
    echo $ff 
    convert $ff -flip $ff
done
