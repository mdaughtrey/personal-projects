FROM debian:buster
RUN apt-get update -y
RUN apt-get install -y vim tmux curl
RUN export PATH=${PATH}:/usr/local/arduino-cli
RUN cd /usr/local/ && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
#COPY ${HOME}/personal-projects/bin/vimrc ~/.vimrc
#COPY ${HOME}/personal-projects/bin/tmux.conf ~/.tmux.conf

