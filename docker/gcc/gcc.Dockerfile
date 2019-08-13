FROM debian:stretch
RUN apt-get update -y
RUN apt-get install g++ vim git python3-pip git python3-numpy python3-pil python3-scipy gdb -y
RUN git config --global user.email "matt@daughtrey.com"
RUN git config --global user.name "mdaughtrey"
RUN git clone https://github.com/mdaughtrey/personal-projects
RUN cp /personal-projects/bin/vimrc ~/.vimrc
RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf

