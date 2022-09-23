TAG=jekyll
DOCKER=docker.exe

run:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} \
	--publish 4000:4000  \
	--publish 35729:35729  \
    --volume \\\\wsl\$\\Ubuntu\\home\\mattd\\personal-projects:/personal-projects \
    --volume \\\\wsl\$\\Ubuntu\\home\\mattd\\personal-projects\\jekyll:/home/mattd/sites \
    -w /home/mattd ${TAG}:latest /home/mattd/runserver.sh

runi0:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
	--publish 4000:4000  \
	--publish 35729:35729  \
	-v '\\wsl\$\Ubuntu\home\mattd\personal-projects':/personal-projects \
    -v '\\wsl\$\Ubuntu\home\mattd\personal-projects\jekyll':/home/mattd/sites  \
    -w /home/mattd \
	${TAG}:latest /bin/bash 

runi:
	${DOCKER} run --rm -h ${TAG} --name ${TAG} -it \
	--publish 4000:4000  \
    --volume \\\\wsl\$\\Ubuntu\\home\\mattd\\personal-projects:/personal-projects \
    --volume \\\\wsl\$\\Ubuntu\\home\\mattd\\personal-projects\\jekyll:/home/mattd/sites \
    -w /home/mattd \
	${TAG}:latest /bin/bash 

network:
	${DOCKER} inspect --format '{{ .NetworkSettings.IPAddress }}' $(shell ${DOCKER} ps -a | grep jekyll | awk '{print $$1}')

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
