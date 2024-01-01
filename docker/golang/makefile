TAG=golang
ifeq ($(shell hostname),md)
DOCKER=docker
else
DOCKER=docker.exe
endif
HOMEDIR=/home/matt/personal-projects/

run:
	${DOCKER} run -dh ${TAG} --name ${TAG} --publish 8000:8000 -v /c/Users/matt/Documents/dev/personal-projects:/personal-projects ${TAG}:latest /home/mattd/demo.sh

runi:
	${DOCKER} run -h ${TAG} --name ${TAG} -it  -v ${HOMEDIR}:/personal-projects ${TAG}:latest  /bin/bash


attach:
	${DOCKER} attach ${TAG}

#	${DOCKER} exec -it `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop ${TAG}
	${DOCKER} rm ${TAG}
#	${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

tag:
	${DOCKER} tag ${TAG} mdaughtrey/projects:${TAG}

make:
	${DOCKER} build -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
