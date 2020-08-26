IMAGE=b163ddd353c9
TAG=tutorial

all:
	docker build -f test.Dockerfile --tag=${TAG} .

run:
<<<<<<< HEAD
	docker run -dh vproj --expose 22 -p5000:5000 -it -v /p/vproj:/media/sf_vproj -v /c/import:/import vproj:latest 
=======
	docker run -dh vproj --expose 22 -p5000:5000 -it -v /c/Users/matt/Documents/dev/personal-projects:/personal-projects -v /p/vproj:/media/sf_vproj -v /c/import:/import vproj:latest 
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
	#docker run -dh vproj --expose 5000 -it  -v /p/vproj:/media/sf_vproj vproj:latest 

attach:
	docker exec -it `docker ps -a | grep vproj:latest | grep -v Exited | cut -d' ' -f1 ` /bin/bash

stop:
	docker stop `docker ps -q`

tag:
	docker tag ${IMAGE} mdaughtrey/projects:${TAG}

push:
	docker push mdaughtrey/projects:${TAG}

vproj:
	docker build -f debian.Dockerfile . --tag=vproj 
