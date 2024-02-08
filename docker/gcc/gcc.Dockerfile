FROM debian:latest
RUN apt-get update 
RUN apt-get -y install gcc vim git gdb build-essential python3 python3-pip
RUN apt-get -y install python3-venv
#RUN python3 -m pip install gdbgui
WORKDIR /root
RUN python3 -m venv venv
RUN . venv/bin/activate && python3 -m pip install pipx && pipx install gdbgui
#RUN git config --global user.email "matt@daughtrey.com"
#RUN git config --global user.name "mdaughtrey"
#RUN git clone https://github.com/mdaughtrey/personal-projects
#RUN cp /personal-projects/bin/vimrc ~/.vimrc
#RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf

