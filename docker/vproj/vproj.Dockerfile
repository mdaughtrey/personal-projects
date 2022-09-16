FROM debian:bullseye
#ARG USER=mattd
EXPOSE 5000
RUN dpkg --add-architecture i386 
RUN apt-get update -y
RUN apt-get install  apt-utils -y
RUN apt-get -y dist-upgrade -o APT::Immediate-Configure=0
RUN apt-get install libc6
RUN apt-get install parallel tmux python3.9 python3-pip python3-flask vim python3-numpy python3-pil sqlite3 -y
RUN apt-get install python3-scipy enfuse bc vim mplayer mjpegtools ffmpeg wine32 -y
RUN apt-get install  imagemagick git -y
RUN python3 -m pip install --upgrade pip
RUN python3 -m pip install scikit-build imageio
RUN apt-get -y install python3-opencv
RUN apt-get -y install less rsync
RUN apt-get -y install openssh-client
RUN git clone https://github.com/Breakthrough/PySceneDetect.git
RUN cd /PySceneDetect && python3 setup.py install
#RUN groupadd -g 1000 ${USER}
#RUN useradd -r -u 1000 -g 1000 ${USER}
#RUN usermod -a -G dialout ${USER}
#RUN mkdir /home/mattd
#RUN chown -R mattd:mattd /home/mattd
#USER ${USER}
RUN echo mkdir -p /root/.wine/drive_c/windows/system  > /init.sh
RUN echo cp -R /software/avx/scripts/plugins/ /root/.wine/drive_c/windows/system >> /init.sh
#RUN ssh-keygen -t rsa  -f /root/.ssh/id_rsa -N ''
RUN mkdir /root/.ssh
COPY id_rsa.pub /root/.ssh
COPY .vimrc /root/.vimrc
COPY .tmux.conf /root/.tmux.conf
WORKDIR /personal-projects/projector/scripts/controller
