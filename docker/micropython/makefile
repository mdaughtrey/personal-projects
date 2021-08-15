NAME=micropython-esp8266
DOCKER=docker

all:
	${DOCKER} build -f ${NAME}.Dockerfile --tag=${NAME} .

run:
	${DOCKER} run -dh ${NAME} --name ${NAME} --expose 22 -it ${NAME}:latest 

hqtest:
	${DOCKER} run -dh ${NAME} --name ${NAME} -it -v ~/personal-projects/:/personal-projects -v ~/Documents/hqtest:/hqtest ${NAME}:latest 

attach:
	docker attach ${NAME}
#	${DOCKER} exec -it `docker ps -a | grep vproj:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop ${NAME}
	${DOCKER} rm ${NAME}

#tag:
#	${DOCKER} tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

build:
	${DOCKER} build -f ${NAME}.Dockerfile . --tag=${NAME}
