TAG=nikola
DOCKER=docker
PUB=--publish 8000:8000
VOL=-v ${HOME}/personal-projects:/personal-projects

run:
	${DOCKER} run -dh ${TAG} --name ${TAG} ${PUB} ${VOL} ${TAG} /personal-projects/docker/nikola/demo.sh

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} ${PUB} -it ${VOL} ${TAG}:latest  /bin/bash

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
	${DOCKER} build --no-cache -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
