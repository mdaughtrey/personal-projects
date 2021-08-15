NAME=acli-bluemouth
DOCKER=docker

all:
	${DOCKER} build -f ${NAME}.Dockerfile --tag=${NAME} .

run:
	${DOCKER} run -dh ${NAME} --name ${NAME} --expose 22 -it ${NAME}:latest 
#		-v ~/personal-projects:/personal-projects

attach:
	docker attach ${NAME}

stop:
	${DOCKER} stop ${NAME}
	${DOCKER} rm ${NAME}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

build:
	${DOCKER} build -f ${NAME}.Dockerfile . --tag=${NAME}
