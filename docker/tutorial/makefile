IMAGE=b163ddd353c9
TAG=tutorial
DOCKER=docker.exe

all:
	${DOCKER} build -f test.Dockerfile --tag=${TAG} .

run:
	${DOCKER} run -dh vproj --expose 22 -p5000:5000 -it -v /c/Users/matt/Documents/dev/personal-projects:/personal-projects -v /p/vproj:/media/sf_vproj -v /c/import:/import vproj:latest 
	#docker run -dh vproj --expose 5000 -it  -v /p/vproj:/media/sf_vproj vproj:latest 

attach:
	${DOCKER} exec -it `docker ps -a | grep vproj:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop `docker ps -q`

tag:
	${DOCKER} tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

vproj:
	${DOCKER} build -f debian.Dockerfile . --tag=vproj 
