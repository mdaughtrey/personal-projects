#FROM debian:latest
FROM bitnami/minideb:latest
ARG USER=mattd
RUN apt-get  update -y
RUN apt-get -y install vim tmux curl git make wget python3 python3-pip 
RUN apt-get -y install unzip screen exuberant-ctags avrdude openocd python3-serial yq
RUN apt-get -y install gdb
#RUN pip3 install pyserial
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
RUN ~/acli/bin/arduino-cli config init
RUN yq -yi '.board_manager.additional_urls=["https://arduino.esp8266.com/stable/package_esp8266com_index.json","https://dl.espressif.com/dl/package_esp32_index.json","https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json"]' ~/.arduino15/arduino-cli.yaml
#COPY arduino-cli.patch0 /tmp/arduino-cli.patch0
#RUN cd ~/.arduino15 && patch -p0 arduino-cli.yaml /tmp/arduino-cli.patch0
RUN ~/acli/bin/arduino-cli core update-index
RUN ~/acli/bin/arduino-cli core install esp8266:esp8266
RUN ~/acli/bin/arduino-cli core install esp32:esp32
RUN ~/acli/bin/arduino-cli core install rp2040:rp2040
RUN git config --global user.email "mdaughtrey@gmail.com"
RUN git config --global user.name mdaughtrey
RUN ~/acli/bin/arduino-cli core install arduino:avr
RUN ~/acli/bin/arduino-cli lib install 'Adafruit GFX Library'
RUN cd ~/Arduino/libraries && git clone https://github.com/maniacbug/StandardCplusplus.git
RUN cd /var/tmp && wget https://github.com/T-vK/ESP32-BLE-Keyboard/archive/refs/tags/0.3.0.tar.gz && \
tar xzvf 0.3.0.tar.gz && mv ESP32-BLE-Keyboard-0.3.0/ /home/mattd/Arduino/libraries/
RUN ~/acli/bin/arduino-cli lib install 'Adafruit APDS9960 Library'
RUN ~/acli/bin/arduino-cli lib install gxepd2 Adafruit_VL53L0X AsyncStepperLib 'Adafruit MQTT Library' WS2812FX WiFiManager Effortless-SPIFFS
RUN ~/acli/bin/arduino-cli lib install HID-Project printex asynctimer TM1638plus
RUN ~/acli/bin/arduino-cli lib install 'SparkFun APDS9960 RGB and Gesture Sensor'
RUN ~/acli/bin/arduino-cli lib install 'RevEng PAJ7620'
RUN ~/acli/bin/arduino-cli lib install 'AccelStepper'
RUN cd /tmp && git clone https://github.com/oznotes/INA219.git && cp -R /tmp/INA219/INA219_driver ~/Arduino/libraries
USER root
RUN apt clean
USER ${USER}
