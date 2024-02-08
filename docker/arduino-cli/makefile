NAME=acli
DOCKER=docker

all:
	${DOCKER} build -f ${NAME}.Dockerfile --tag=${NAME} .

run:
	${DOCKER} run -dh ${NAME} -v ~/personal-projects:/personal-projects --name ${NAME} -it ${NAME}:latest 

runb:
	${DOCKER} run --rm -v ~/personal-projects:/personal-projects --name ${NAME} ${NAME}:latest 

attach:
	docker attach ${NAME}

stop:
	${DOCKER} stop ${NAME}
	${DOCKER} rm ${NAME}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

b:
	${DOCKER} build -f ${NAME}.Dockerfile . --tag=${NAME}

c:
	${DOCKER} build --no-cache -f ${NAME}.Dockerfile . --tag=${NAME}

tag:
	${DOCKER} tag ${IMAGE} mdaughtrey/projects:${NAME}

push:
	${DOCKER} push mdaughtrey/projects:${NAME}
