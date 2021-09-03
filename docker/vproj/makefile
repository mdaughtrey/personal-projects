IMAGE=b163ddd353c9
NAME=vproj
DOCKER=docker

all:
	${DOCKER} build -f i${NAME}.Dockerfile --tag=${NAME} .

run:
	${DOCKER} run -dh ${NAME} --name ${NAME} --expose 22 -p5000:5000 -it -v ~/dev/personal-projects/:/personal-projects -v /mnt/p/vproj:/media/sf_vproj -v /mnt/c/:/c ${NAME}:latest 

hqtest:
	${DOCKER} run -dh ${NAME} --name ${NAME} -it -v ~/personal-projects/:/personal-projects -v ~/Documents/hqtest:/hqtest ${NAME}:latest 

attach:
	docker attach ${NAME}
#	${DOCKER} exec -it `docker ps -a | grep vproj:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop ${NAME}
	${DOCKER} rm ${NAME}

tag:
	${DOCKER} tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

build:
	${DOCKER} build -f ${NAME}.Dockerfile . --tag=vproj 
