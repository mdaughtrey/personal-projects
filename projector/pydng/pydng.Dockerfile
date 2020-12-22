FROM debian:bullseye
ARG USER=mattd
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim python3 python3-pip git
RUn apt-get update
RUN apt -y install imagemagick 
RUn apt -y install ufraw-batch

RUN mkdir -p /home/${USER}/
RUN chown -R ${USER}:${USER} /home/${USER}
USER ${USER}
RUN ln -s /personal-projects/bin/vimrc ~/.vimrc

RUN cd /home/${USER} && \
    git clone https://github.com/schoolpost/PyDNG && \
    cd PyDNG && \
    python3 -m pip install ./src/.


