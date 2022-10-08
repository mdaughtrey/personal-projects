FROM debian:bullseye
ARG USER=mattd
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim vim-common python3 python3-pip git tmux 
RUN apt -y install ruby ruby-dev 
USER ${USER}
COPY --chown=mattd runserver.sh /home/mattd/runserver.sh
COPY --chown=mattd .vimrc /home/mattd/.vimrc
COPY --chown=mattd .tmux.conf /home/mattd/.tmux.conf
COPY --chown=mattd .bashrc /home/mattd/.bashrc
ENV GEM_HOME=/home/mattd/gems
ENV PATH=/home/mattd/gems/bin:${PATH}
RUN cd /home/mattd && gem install bundler jekyll minima
ENV SHELL=/bin/bash
ENV EDITOR=vim
