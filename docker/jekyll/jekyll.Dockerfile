FROM debian:bullseye
#ARG USER=mattd
#RUN groupadd -g 1000 ${USER}
#RUN useradd -r -u 1000 -g 1000 ${USER}
#RUN usermod -a -G dialout mattd

RUN apt update
RUN apt -y install vim vim-common python3 python3-pip git tmux 
RUN apt -y install ruby ruby-dev 
RUN gem install bundler jekyll
RUN cd /tmp && jekyll new mysite
COPY runserver.sh /root/runserver.sh
#RUN cd /tmp/mysite && bundle exec jekyll serve

#RUN mkdir -p /home/${USER}/
#RUN chown -R ${USER}:${USER} /home/${USER}
#USER ${USER}
COPY .vimrc /root/.vimrc
COPY .tmux.conf /root/.tmux.conf
COPY .bashrc /root/.bashrc
ENV SHELL=/bin/bash
ENV EDITOR=vim
