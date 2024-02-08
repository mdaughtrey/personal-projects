FROM debian:bullseye
RUN apt-get update -y
RUN apt-get install vim git python3 nodejs udev -y 
RUN git config --global user.email "matt@daughtrey.com"
RUN git config --global user.name "mdaughtrey"
#RUN git clone --depth 1 https://github.com/mdaughtrey/personal-projects --branch master --single-branch icestorm
#RUN cp /personal-projects/bin/vimrc ~/.vimrc
#RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf
RUN apt-get update -y
RUN apt-get install python3-pip yosys arachne-pnr -y  \
	build-essential clang bison flex libreadline-dev \
        gawk tcl-dev libffi-dev git mercurial graphviz   \
        xdot pkg-config python python3 libftdi-dev \
        qt5-default python3-dev libboost-all-dev cmake \
	npm
RUN export LC_ALL=C.UTF-8
RUN export LANG=C.UTF-8
RUN pip3 install -U platformio
RUN platformio platform install lattice_ice40 --without-package toolchain-iverilog
RUN echo "ACTION==\"add\", ATTR{idVendor}==\"0403\", ATTR{idProduct}==\"6010\", MODE:=\"666\"" \
> /etc/udev/rules.d/53.lattice-ftdi.rules
RUN cd /icestorm && git clone https://github.com/cliffordwolf/icestorm.git icestorm && \
	cd icestorm && make -j$(nproc) && make install
RUN apt-get -y install iverilog 
#RUN cd / && git clone https://github.com/cocotb/cocotb.git && pip3 install ./cocotb && cd /

