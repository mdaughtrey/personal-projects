TAG=clisp
DOCKER=docker.exe

run:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} \
    -v '\\wsl\$\Ubuntu\home\matt\personal-projects':/personal-projects \
	-w /personal-projects/docker/clisp ${TAG}:latest clisp db.clisp

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
    -v '\\wsl\$\Ubuntu\home\matt\personal-projects':/personal-projects \
	-w /personal-projects/docker/clisp ${TAG}:latest clisp db.clisp


attach:
	${DOCKER} attach ${TAG}

#	${DOCKER} exec -it `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop ${TAG}
	${DOCKER} rm ${TAG}
#	${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

tag:
	${DOCKER} tag ${TAG} mdaughtrey/projects:${TAG}

make:
	cp ~/.vimrc .
	cp ~/.tmux.conf .
	cp ~/.bashrc .
	${DOCKER} build -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	${DOCKER} push mdaughtrey/projects:${TAG}

commit:
	${DOCKER} commit `${DOCKER} stop `${DOCKER} ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:${TAG}
