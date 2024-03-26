FROM debian:bullseye
ARG USER=mattd
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim python3 python3-pip git

RUN mkdir -p /home/${USER}/
RUN chown -R ${USER}:${USER} /home/${USER}
USER ${USER}
RUN ln -s /personal-projects/bin/vimrc ~/.vimrc
RUN pip install nikola
ENV PATH="${PATH}:/home/mattd/.local/bin"
WORKDIR /personal-projects/docker/nikola/daughtrey.com
#COPY demo.sh /home/mattd
#RUN cd /home/mattd && nikola init -q --demo daughtrey.com && cd daughtrey.com && nikola build && nikola serve
# RUN cd daughtrey.com && nikola build && nikola serve
