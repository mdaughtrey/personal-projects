TAG=gcc

all:
	docker build -f test.Dockerfile --tag=${TAG} .

run:
	docker run -h ${TAG} -v ${PWD}:/work -ti gcc:latest
	#docker run -dh ${TAG} -it --expose 22 -v /c/scratch:/scratch --cap-add=SYS_PTRACE --security-opt seccomp=unconfined gcc:latest 
	#docker run -dh vproj --expose 5000 -it  -v /p/vproj:/media/sf_vproj vproj:latest 

attach:
	docker exec -it `docker ps -a | grep gcc:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	docker stop `docker ps -a | grep gcc:latest | grep -v Exited | cut -d' ' -f1`

tag:
	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	docker push mdaughtrey/projects:${TAG}

make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG}
