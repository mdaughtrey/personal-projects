FROM debian:buster
ARG USER=builder
RUN dpkg --add-architecture i386 
RUN apt-get update -y
RUN apt-get install  apt-utils -y
RUN apt-get -y dist-upgrade -o APT::Immediate-Configure=0
RUN apt-get -y install libc6 tmux python3 python3-pip vim git less rsync openssh-client
RUN apt-get -y install gcc-xtensa-lx106 libc6-dev-i386
RUN apt-get -y install make unrar-free autoconf automake libtool-bin gcc g++ gperf 
RUN apt-get -y install flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial 
RUN apt-get -y install sed unzip bash help2man wget bzip2
RUN useradd -ms /bin/bash builder
USER builder
RUN cd ~ && git clone https://github.com/micropython/micropython.git
RUN cd ~/micropython/mpy-cross && make
RUN cd ~/micropython && git clone https://github.com/pfalcon/esp-open-sdk
RUN cd ~/micropython/esp-open-sdk/crosstool-NG/.build/tarballs && wget https://sourceforge.net/projects/expat/files/expat/2.4.1/expat-2.4.1.tar.gz
RUN cd ~/micropython/esp-open-sdk && make
#        Change line 193 at: esp-open-sdk/crosstool-NG/configure.ac #         |$EGREP '^GNU bash, version ([0-9\.]+)')
RUN cd ~/micropython/esp-open-sdk && make
# edit ~/micropython/esp-open-sdk/crosstool-NG/config/companion_libs/expat.in
# replace 2_4_1 with 2_410
#DELMEgit submodule update --init



#RUN cd /micropython/ports/esp8266 && make submodules &&  make V=1 BOARD=GENERIC_512K CFLAGS_EXTRA=-I/home/builder/micropython/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr/include
# cd /micropython/ports/esp8266 && git submodule update --init


