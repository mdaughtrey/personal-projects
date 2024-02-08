TAG=debian
DOCKER=docker.exe

#run:
#	${DOCKER} run -dh ${TAG} --name ${TAG} -it  -v /c/Users/matt/Documents/dev/personal-projects:/personal-projects ${TAG}:latest 

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it ${TAG}:latest  /bin/bash

#run:
#	${DOCKER} run -dh ${TAG} --name ${TAG} --publish 8000:8000 -v ${HOMEDIR}:/personal-projects ${TAG}:latest /home/mattd/demo.sh

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
	${DOCKER} build  -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
