IMAGE=b163ddd353c9
NAME=vproj
ifeq ($(shell uname),Linux)
DOCKER=docker
else
DOCKER=docker.exe
endif
HOST=$(shell hostname)

h:
	echo $(HOST)

all:
	${DOCKER} build -f i${NAME}.Dockerfile --tag=${NAME} .

run:
ifeq ($(HOST),md)
	${DOCKER} run --rm -dh ${NAME} --name ${NAME} --expose 22 -p5000:5000 -it \
    -v '\\wsl\$\Ubuntu\home\matt\personal-projects':/personal-projects ${NAME}:latest 
else
	${DOCKER} run --rm -dh ${NAME} --name ${NAME} --expose 22 -p5000:5000 -it \
    --volume /c/vproj:/working \
    --volume /p/vproj/scans/software:/software \
    --volume /p/vproj/scans:/allscans \
    --volume \\\\wsl\$\\Ubuntu\\home\\mattd\\personal-projects:/personal-projects \
    ${NAME}:latest 
endif

hqtest:
	${DOCKER} run -dh ${NAME} --name ${NAME} -it -v ~/personal-projects/:/personal-projects -v ~/Documents/hqtest:/hqtest ${NAME}:latest 

attach:
	${DOCKER} attach ${NAME}
#	${DOCKER} exec -it `docker ps -a | grep vproj:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	${DOCKER} stop ${NAME}
	${DOCKER} rm ${NAME}

tag:
	${DOCKER} tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	${DOCKER} push mdaughtrey/projects:${TAG}

build:
	cp ~/.vimrc .
	cp ~/.tmux.conf .
	cp ~/.ssh/id_rsa.pub .
	${DOCKER} build -f ${NAME}.Dockerfile . --tag=vproj 
