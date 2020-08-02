TAG=esp32
TOOL=esp32-toolchain

#init:
#	curl https://raw.githubusercontent.com/lpodkalicki/esp32-toolchain-docker/master/install.sh | bash -s --

no:
	echo No.
run:
	docker run -dh ${TAG} --device=/dev/ttyUSB0  -v ${HOME}/personal-projects/docker/esp32:/project -w /project -it ${TAG}:latest
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

