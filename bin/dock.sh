#!/bin/bash

images=($(ls -1q ~/personal-projects/docker/*/*.Dockerfile | egrep -o '[0-9a-zA-Z]*\.Dockerfile' | cut -d\. -f1))

if [[ "$1" == "list" ]]; then
    for ii in ${images[@]}; do
        echo $ii
    done
    exit 0
fi

image=${1:-unknown}

[[ " ${images[@]} " =~ " $1 " ]] && found=1 || (echo "Image not found" && exit 0)


run()
{
    docker run -dh ${1} --name ${1} -it  -v ~/personal-projects:/personal-projects ${1}:latest 
}


case "$2" in 
    run) run $1 ;;
    attach) docker attach $1 ;;
    stop) docker stop $1; docker rm $1 ;;
    *) echo What command? ;;
esac
    


