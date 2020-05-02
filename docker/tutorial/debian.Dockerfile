FROM debian:stretch
EXPOSE 5000
RUN dpkg --add-architecture i386
RUN apt-get update -y
RUN apt-get install parallel tmux python3 python3-pip python3-flask vim python3-numpy python3-pil sqlite3 -y
RUN apt-get install python3-scipy enfuse bc vim mplayer mjpegtools libav-tools wine32 -y
#RUN apt-get install  apt-utils -y
#RUN apt-get install  git -y
#RUN apt-get install  tmux -y
#RUN apt-get install  python3 -y
#RUN apt-get install  git -y
#RUN apt-get install  python3-flask -y
#RUN apt-get install  vim -y
#RUN apt-get install  python3-numpy -y
#RUN apt-get install  python3-pil -y
#RUN apt-get install  sqlite3 -y
RUN apt-get install  imagemagick -y
#RUN apt-get install  python3-scipy -y
#RUN apt-get install  enfuse -y
#RUN apt-get install bc -y
#RUN apt-get install vim -y
#RUN apt-get install python3-pip -y
#RUN git config --global user.email "matt@daughtrey.com"
#RUN git config --global user.name "mdaughtrey"
#RUN git clone https://github.com/mdaughtrey/personal-projects
#RUN git pull
RUN pip3 install opencv-python-headless imageio
RUN cp /personal-projects/bin/vimrc ~/.vimrc
RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
RUN echo mkdir -p ~/.wine/drive_c/windows/system > /init.sh 
RUN echo cp -R /media/sf_vproj/scans/software/avx/scripts/plugins/ ~/.wine/drive_c/windows/system >> /init.sh

