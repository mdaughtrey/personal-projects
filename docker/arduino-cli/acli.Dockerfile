FROM debian:buster
RUN apt-get update -y
RUN apt-get install -y vim tmux curl python python-serial make screen vim-ctrlp exuberant-ctags ips git
RUN export PATH=${PATH}:/usr/local/arduino-cli
RUN cd /usr/local/ && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
RUN arduino-cli core update-index --additional-urls https://dl.espressif.com/dl/package_esp32_index.json,http://arduino.esp8266.com/stable/package_esp8266com_index.json
RUN arduino-cli core install esp32:esp32 --additional-urls https://dl.espressif.com/dl/package_esp32_index.json,http://arduino.esp8266.com/stable/package_esp8266com_index.json
RUN cd /usr/local/bin && git clone https://github.com/rupa/z.git
RUN export PATH=${PATH}:/usr/local/bin/z
COPY bashrc /root/.bashrc
WORKDIR /personal-projects/arduino/arduino-cli/LilyGo-T-Wristband/libdeps/SparkFun_MPU-9250-DMP_Arduino_Library/examples/LilyGoWristband
