FROM debian:stretch
RUN apt-get update -y
RUN apt-get install vim git python3 nodejs -y 
RUN git config --global user.email "matt@daughtrey.com"
RUN git config --global user.name "mdaughtrey"
RUN git clone https://github.com/mdaughtrey/personal-projects
RUN cp /personal-projects/bin/vimrc ~/.vimrc
RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
RUN apt-get update -y
RUN apt-get install python3-pip -y 
RUN export LC_ALL=C.UTF-8
RUN export LANG=C.UTF-8

