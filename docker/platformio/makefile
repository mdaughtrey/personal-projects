TAG=platformio
<<<<<<< HEAD
=======
PPDIR=/c/Users/matt/Documents/dev/personal-projects
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06

all:
	docker build -f test.Dockerfile --tag=${TAG} .

run:
<<<<<<< HEAD
	docker run -dh ${TAG} -it --expose 22 --device=/dev/ttyUSB0 ${TAG}:latest 
=======
	docker run -dh ${TAG} -it --expose 22 --device=/dev/ptmx -v ${PPDIR}:/personal-projects ${TAG}:latest 
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06

attach:
	docker exec -it `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

<<<<<<< HEAD
make:
	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG
=======
#make:
#	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG}
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06

push:	
	docker push mdaughtrey/projects:${TAG}
