FROM debian:latest

USER root
RUN apt-get update
RUN apt-get -y install python3 jupyter-notebook python3-matplotlib python3-numpy python3-opencv python3-pandas
#RUN mkdir -p /root/.jupyter

COPY jupyter_notebook_config.json  /root/.jupyter/jupyter_notebook_config.json
