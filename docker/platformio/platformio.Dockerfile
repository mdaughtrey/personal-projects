<<<<<<< HEAD
FROM debian:stretch
RUN apt-get update -y
RUN apt-get install vim git python3 -y 
RUN git config --global user.email "matt@daughtrey.com"
RUN git config --global user.name "mdaughtrey"
RUN git clone https://github.com/mdaughtrey/personal-projects
RUN cp /personal-projects/bin/vimrc ~/.vimrc
RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
RUN apt-get update -y
RUN apt-get install python3-pip -y 
RUN pip3 install -U platformio
RUN export LC_ALL=C.UTF-8
RUN export LANG=C.UTF-8

=======
FROM debian:buster
RUN apt-get update -y
RUN apt-get install -y python3 python3-pip vim less
RUN pip3 install -U platformio
#RUN git config --global user.email "matt@daughtrey.com"
#RUN git config --global user.name "mdaughtrey"
#RUN git clone https://github.com/mdaughtrey/personal-projects
#RUN cp /personal-projects/bin/vimrc ~/.vimrc
#RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
#RUN apt-get update -y
#RUN apt-get install python3-pip -y 
#RUN export LC_ALL=C.UTF-8
#RUN export LANG=C.UTF-8
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
