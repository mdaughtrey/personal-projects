FROM debian:buster
RUN apt-get update -y
RUN apt-get install -y vim tmux curl
RUN export PATH=${PATH}:/usr/local/arduino-cli
RUN cd /usr/local/ && curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
RUN apt-get install -y git make wget
RUN arduino-cli core update-index
#COPY ${HOME}/personal-projects/bin/vimrc ~/.vimrc
#COPY ${HOME}/personal-projects/bin/tmux.conf ~/.tmux.conf
# https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
# https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/linux-setup.html

