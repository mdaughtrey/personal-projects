TAG=platformio

all:
	docker build -f ${TAG}.Dockerfile --tag=${TAG} .

run:
	docker run -dh ${TAG} --device=/dev/ttyUSB0  -v ${HOME}/personal-projects:/personal-projects -w /personal-projects -it ${TAG}:latest

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
