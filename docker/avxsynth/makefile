NAME=avxsynth

run:
	docker run --rm -h ${NAME} --name ${NAME} -ti -v ${PWD}:/frames ${NAME}:latest 

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
