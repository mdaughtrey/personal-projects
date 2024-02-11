FROM debian:latest

USER root
RUN apt-get update
RUN apt-get -y install python3 jupyter-notebook python3-matplotlib python3-numpy python3-opencv python3-pandas
#RUN mkdir -p /root/.jupyter

COPY <<EOF /root/.jupyter/jupyter_notebook_config.json
{
  "NotebookApp": {
    "password": "argon2:$argon2id$v=19$m=10240,t=10,p=8$xjDgfWd4BDJbWL06LhyLaQ$cPkeMPLo9oCJCjD9eEh7/w"
}
EOF

