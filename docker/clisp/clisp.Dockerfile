FROM debian:bullseye
ARG USER=mattd
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim vim-common python3 python3-pip git clisp tmux

RUN mkdir -p /home/${USER}/
RUN chown -R ${USER}:${USER} /home/${USER}
USER ${USER}
COPY .vimrc /home/mattd/.vimrc
COPY .tmux.conf /home/mattd/.tmux.conf
COPY .bashrc /home/mattd/.bashrc
ENV SHELL=/bin/bash
ENV EDITOR=vim
