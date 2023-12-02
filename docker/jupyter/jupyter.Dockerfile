FROM debian:bullseye
RUN apt-get update
RUN apt-get -y install python3-pip
RUN pip install jupyterlab notebook opencv-python numpy 
#CMD jupyter notebook --allow-root
