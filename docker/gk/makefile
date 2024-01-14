TAG=gk
ifeq ($(shell hostname),md)
DOCKER=docker
HOMEDIR=/home/matt/personal-projects/
else
DOCKER=docker
HOMEDIR=/home/mattd/personal-projects/
endif

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it  -v ${HOMEDIR}:/personal-projects \
		-p 5000:5000 ${TAG}:latest /bin/bash

run:
	${DOCKER} run -h ${TAG} --name ${TAG} -p5000:5000 -v ${HOMEDIR}:/personal-projects ${TAG}:latest /bin/bash -c gdbgui --args gk -c test/test069


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
