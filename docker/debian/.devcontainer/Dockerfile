FROM debian:latest
USER root

RUN mkdir /app
COPY <<EOF /app/devcontainer.json
{
    "image":"myimage",
    "extensions":["ms-vscode.cpptools"],
    "forwardPorts": [2345]
}
EOF

#RUN apt-get update
#RUN apt-get -y install vim python3 python3-pip git
#RUN apt-get -y install gcc-12-base

#ARG USER=mattd
#RUN groupadd -g 1000 ${USER}
#RUN useradd -r -u 1000 -g 1000 ${USER}
#RUN usermod -a -G dialout mattd
#RUN mkdir -p /home/${USER}/
#RUN chown -R ${USER}:${USER} /home/${USER}
#USER ${USER}
#RUN ln -s /personal-projects/bin/vimrc ~/.vimrc

#RUN cd /home/${USER} && \
#    git clone https://github.com/schoolpost/PyDNG && \
#    cd PyDNG && \
#    python3 -m pip install ./src/.

