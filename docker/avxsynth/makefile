NAME=avxsynth

runi:
	docker run --rm -h ${NAME} --name ${NAME} -ti -v ${HOME}/share:/frames -v ${PWD}:/local ${NAME}:latest

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
