FROM debian:buster
ARG USER=mattd
RUN apt-get  update -y
RUN apt-get -y install vim tmux curl git make wget python3 python3-pip 
RUN apt-get -y install unzip screen exuberant-ctags avrdude
RUN pip3 install pyserial
#RUN arduino-cli core update-index
#RUN mkdir -p  /root/Arduino/libraries
RUN ln -s /usr/bin/python3 /usr/bin/python
RUN groupadd -g 1000 ${USER}
RUN useradd -r -u 1000 -g 1000 ${USER}
RUN usermod -a -G dialout mattd
RUN mkdir -p /home/${USER}/Arduino
RUN mkdir -p /home/${USER}/.arduino15
RUN chown -R ${USER}:${USER} /home/${USER}
USER ${USER}
RUN mkdir ~/acli
RUN mkdir ~/tmp
WORKDIR ~
RUN cd ~/acli && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
ENV PATH=~/acli/bin:${PATH}
RUN export PATH=~/acli/bin:${PATH}
RUN mkdir -p ~/acli/Arduino/libraries
RUN cd ~/acli && wget https://github.com/ZinggJM/GxEPD/archive/master.zip && unzip master.zip -d ~/acli/Arduino/libraries && rm master.zip
##COPY ${PWD}/arduino-cli.yaml /root/.arduino15
#COPY ${PWD}/arduino-cli.yaml /home/${USER}/.arduino15
#RUN export PATH=${PATH}:/usr/local/arduino-cli
RUN ~/acli/bin/arduino-cli config init
RUN ~/acli/bin/arduino-cli lib install 'Adafruit GFX Library'
RUN ~/acli/bin/arduino-cli lib install 'Adafruit APDS9960 Library'
RUN ~/acli/bin/arduino-cli lib install gxepd2
COPY arduino-cli.patch0 /tmp/arduino-cli.patch0
RUN cd ~/.arduino15 && patch -p0 arduino-cli.yaml /tmp/arduino-cli.patch0
RUN ~/acli/bin/arduino-cli core update-index
RUN ~/acli/bin/arduino-cli core install esp8266:esp8266
RUN ~/acli/bin/arduino-cli core install esp32:esp32
RUN git config --global user.email "mdaughtrey@gmail.com"
RUN git config --global user.name mdaughtrey
ENTRYPOINT ["/home/mattd/acli/bin/arduino-cli"]
#COPY ~/personal-projects/bin/vimrc ~/.vimrc
#COPY ~/personal-projects/bin/tmux.conf ~/.tmux.conf
## https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
## https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/linux-setup.html
##[https://arduino.esp8266.com/stable/package_esp8266com_index.json]
