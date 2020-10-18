FROM debian:buster
ARG USER=mattd
RUN apt-get update -y
RUN apt-get install -y vim tmux curl
RUN cd /usr/local/ && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
RUN export PATH=${PATH}:/usr/local/arduino-cli
RUN apt-get install -y git make wget python3 python3-pip 
RUN pip3 install pyserial
#RUN arduino-cli core update-index
#RUN mkdir -p  /root/Arduino/libraries
RUN ln -s /usr/bin/python3 /usr/bin/python
RUN apt-get -y install unzip screen exuberant-ctags avrdude
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd
RUN mkdir -p /home/${USER}/Arduino
RUN mkdir -p /home/${USER}/.arduino15
RUN chown -R ${USER}:${USER} /home/${USER}
USER ${USER}
#RUN cd /tmp && wget https://github.com/ZinggJM/GxEPD/archive/master.zip && unzip master.zip -d /root/Arduino/libraries && rm master.zip
RUN cd /tmp && wget https://github.com/ZinggJM/GxEPD/archive/master.zip && unzip master.zip -d ~/Arduino/libraries && rm master.zip
#COPY ${PWD}/arduino-cli.yaml /root/.arduino15
COPY ${PWD}/arduino-cli.yaml /home/${USER}/.arduino15
RUN arduino-cli lib install 'Adafruit GFX Library'
RUN arduino-cli lib install gxepd2
RUN arduino-cli core update-index
RUN arduino-cli core install esp32:esp32
RUN git config --global user.email "mdaughtrey@gmail.com"
RUN git config --global user.name mdaughtrey

#COPY ${HOME}/personal-projects/bin/vimrc ~/.vimrc
#COPY ${HOME}/personal-projects/bin/tmux.conf ~/.tmux.conf
# https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
# https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/linux-setup.html
