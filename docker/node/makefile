TAG=nodejs

run:
	docker run -dh ${TAG} -it --expose 22 -p41000:41000 ${TAG}:latest 

attach:
	docker exec -it `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1`

tag:
	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

make:
	docker build -f ${TAG}.Dockerfile . --tag=${TAG}

push:	
	docker push mdaughtrey/projects:${TAG}

commit:
	docker commit `docker stop `docker ps -a | grep ${TAG}:latest | grep -v Exited | cut -d' ' -f1` mdaughtrey/projects:nodejs
