FROM debian:bullseye
RUN apt-get update
RUN apt-get -y install python3-pip
RUN pip install jupyterlab notebook matplotlib numpy opencv-python anaconda pandas
RUN mkdir /data
