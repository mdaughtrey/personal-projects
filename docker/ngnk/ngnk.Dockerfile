FROM debian:bullseye
ARG USER=mattd
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim vim-common python3 python3-pip git tmux 
RUN apt -y install rlwrap 
USER ${USER}
COPY --chown=mattd .vimrc /home/mattd/.vimrc
COPY --chown=mattd .tmux.conf /home/mattd/.tmux.conf
COPY --chown=mattd .bashrc /home/mattd/.bashrc
RUN cd /home/mattd 
ENV SHELL=/bin/bash
ENV K=/home/mattd/k/k
ENV KDIR=/home/mattd/k
ENV EDITOR=vim
RUN cd /home/mattd && git clone https://codeberg.org/ngn/k.git && cd k && make CC=gcc-10
