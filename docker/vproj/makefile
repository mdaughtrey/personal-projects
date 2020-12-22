IMAGE=b163ddd353c9
NAME=vproj
DOCKER=docker
LOCALMOUNT=/mnt/wsl/docker-desktop-bind-mounts/Ubuntu-20.04/049b79db909a79235e055e4cf7075df95da01076802d73f73ca0755f35f1872a/personal-projects/

all:
	${DOCKER} build -f i${NAME}.Dockerfile --tag=${NAME} .

run:
	${DOCKER} run -dh ${NAME} --name ${NAME} --expose 22 -p5000:5000 -it -v ~/dev/personal-projects/:/personal-projects -v /mnt/p/vproj:/media/sf_vproj -v /mnt/c/:/c ${NAME}:latest 

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
