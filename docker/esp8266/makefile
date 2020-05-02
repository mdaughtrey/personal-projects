TAG=lpodkalicki/esp8266-toolchain
TOOL=esp8266-toolchain

no:
	echo No.
init:
	curl https://raw.githubusercontent.com/lpodkalicki/esp8266-toolchain-docker/master/install.sh | bash -s --

run:
	docker run -dh esp8266 --device=/dev/ttyUSB0  -v ${HOME}/personal-projects/docker/esp8266/project/hello_world:/project -w /project -it ${TAG}:latest
#
attach:
	docker exec -it `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash
#
stop:
	docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`
#
#tag:
#	docker tag ${IMAGE} mdaughtrey/projects:${TAG}
#
make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG}
#
#push:	
#	docker push mdaughtrey/projects:${TAG}
#
#commit:
#	docker commit `docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
#

