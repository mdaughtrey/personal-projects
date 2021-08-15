FROM larsks/esp-open-sdk:latest
#RUN apt-get -y install bash
RUN cd ~/ && git clone https://github.com/micropython/micropython.git
RUN cd ~/micropython  && git submodule update --init && make -C mpy-cross
RUN cd ~/micropython/ports/esp8266 && make V=1 BOARD=GENERIC_512K
ENTRYPOINT /bin/bash
#
#
#RUN apt-get update -y
#RUN apt-get install  apt-utils -y
#
#RUN apt-get -y install make unrar-free autoconf automake libtool gcc g++ gperf 
#RUN apt-get -y install flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial 
#RUN apt-get -y install sed git unzip bash help2man wget bzip2 vim
#RUN apt-get -y install python3-dev python3-pip libtool-bin
#RUN useradd -ms /bin/bash builder
#
#RUN cd /home/builder && wget https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
#RUN cd /home/builder && tar -xzf xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
#
##USER builder
##WORKDIR /home/builder
#
#WORKDIR /root
#
#RUN pip3 install rshell esptool
#
#USER builder
#WORKDIR /home/builder
#RUN git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
#RUN rm -rf ~/esp-open-sdk/crosstool-NG
#RUN cd ./esp-open-sdk && git clone https://github.com/jcmvbkbc/crosstool-NG
#RUN cd ./esp-open-sdk/crosstool-NG && git checkout xtensa-1.22.x
#RUN cd ./esp-open-sdk/crosstool-NG && autoconf && ./configure && make && CT_EXPAT_VERSION="2.4.1"
#ENV PATH=/home/builder/xtensa-lx106-elf/bin/:$PATH
#RUN git clone https://github.com/micropython/micropython.git
#RUN cd ~/micropython && git submodule update --init && make -C mpy-cross


#RUN cd ~/ && git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
#WORKDIR ~/esp-open-sdk
#RUN rm -rf ./crosstool-NG && git clone https://github.com/crosstool-ng/crosstool-ng
##RUN mv ./crosstool-ng ./crosstool-NG
#RUN make STANDALONE=y
# Add Makefile 130         mkdir -p crosstool-NG/local-patches/gcc/4.8.5
#path the file
#RUN make STANDALONE=y 

#        Change line 193 at: esp-open-sdk/crosstool-NG/configure.ac #         |$EGREP '^GNU bash, version ([0-9\.]+)')


#RUN cd ~/esp-open-sdk && make STANDALONE=y
#RUN export PATH=/home/builder/esp-open-sdk/xtensa-lx106-elf/bin:$PATH && cd ~/ && git clone https://github.com/micropython/micropython.git
#RUN cd ~/micropython && git submodule update --init && make -C mpy-cross
#RUN cd ~/micropython/ports/esp8266 && make axtls && make 


