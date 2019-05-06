FROM debian:stretch
EXPOSE 5000
RUN apt-get update
RUN apt-get install git tmux python3 python3-flask vim python3-numpy python3-pil python3-pip sqlite3 imagemagick python3-scipy -y
RUN git clone https://github.com/mdaughtrey/personal-projects
RUN pip3 install opencv-python
RUN cp /personal-projects/bin/vimrc ~/.vimrc
RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
RUN git config --global user.email "matt@daughtrey.com"
RUN git config --global user.name "mdaughtrey"

