TAG=platformio
PPDIR=/c/Users/matt/Documents/dev/personal-projects

all:
	docker build -f test.Dockerfile --tag=${TAG} .

run:
	docker run -dh ${TAG} -it --expose 22 --device=/dev/ptmx -v ${PPDIR}:/personal-projects ${TAG}:latest 

attach:
	docker exec -it `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

#make:
#	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	docker push mdaughtrey/projects:${TAG}
