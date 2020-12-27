NAME=gcc

run:
	docker run -dh ${NAME} --name ${NAME} -ti ${NAME}:latest
	#docker run -dh ${TAG} -it --expose 22 -v /c/scratch:/scratch --cap-add=SYS_PTRACE --security-opt seccomp=unconfined gcc:latest 
	#docker run -dh vproj --expose 5000 -it  -v /p/vproj:/media/sf_vproj vproj:latest 

attach:
	docker attach ${NAME}

stop:
	docker stop ${NAME}
	docker rm ${NAME}

tag:
	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	docker push mdaughtrey/projects:${TAG}

build:
	docker build -f ${NAME}.Dockerfile . --tag=${NAME}
