TAG=tensorflow
ifeq ($(shell uname),Linux)
DOCKER=docker
else
DOCKER=docker.exe
endif

ifeq ($(shell hostname),md)
PPHOME=/home/mattd/personal-projects/
else
#PPHOME='\\wsl$$\Ubuntu\home\mattd\personal-projects'
PPHOME=/home/mattd/personal-projects
endif

all:
	echo PPHONE ${PPHOME}

run:
	${DOCKER} run --rm -th ${TAG} --name ${TAG} -p 8888:8888 \
		--volume /media/frames:/frames --volume ${PPHOME}:/personal-projects ${TAG} 
#		jupyter notebook --ip 0.0.0.0 --allow-root 

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
    --volume ${PPHOME}:/personal-projects \
    -p 8888:8888 \
	${TAG}:latest /bin/bash 

#network:
#	${DOCKER} inspect --format '{{ .NetworkSettings.IPAddress }}' $(shell ${DOCKER} ps -a | grep jekyll | awk '{print $$1}')

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
	#${DOCKER} build --no-cache -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
