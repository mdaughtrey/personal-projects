FROM espressif/idf
RUN apt-get update -y
RUN apt-get install -y vim tmux
#COPY ${HOME}/personal-projects/bin/vimrc ~/.vimrc
#COPY ${HOME}/personal-projects/bin/tmux.conf ~/.tmux.conf

