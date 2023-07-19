TAG=ngnk
DOCKER=docker.exe

ifeq ($(shell hostname),md)
PPHOME='\\wsl\$\Ubuntu\home\matt\personal-projects'
else
PPHOME='\\wsl$$\Ubuntu\home\mattd\personal-projects'
endif

all:
	echo PPHONE ${PPHOME}

run:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
    --volume ${PPHOME}:/personal-projects \
    -w /personal-projects/ml/nn/mnist

runi0:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
	--publish 4000:4000  \
	--publish 35729:35729  \
	-v '\\wsl\$\Ubuntu\home\mattd\personal-projects':/personal-projects \
    -v '\\wsl\$\Ubuntu\home\mattd\personal-projects\jekyll':/home/mattd/sites  \
    -w /home/mattd \
	${TAG}:latest /bin/bash 

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
    --volume ${PPHOME}:/personal-projects \
    -w /personal-projects/ml/nn/mnist \
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
	cp ~/.vimrc .
	cp ~/.tmux.conf .
	cp ~/.bashrc .
	${DOCKER} build --no-cache -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
