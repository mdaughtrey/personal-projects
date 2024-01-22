TAG=nikola
DOCKER=docker.exe
PUB=--publish 8000:8000
VOL=-v .:/home/mattd/

run:
	${DOCKER} run -dh ${TAG} --name ${TAG} ${PUB} ${VOL} /home/mattd/demo.sh

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
