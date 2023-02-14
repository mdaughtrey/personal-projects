#https://avisynthplus.readthedocs.io/en/latest/avisynthdoc/contributing/posix.html
#https://github.com/Dogway/Avisynth-Scripts
#http://avisynth.nl/index.php/AviSynth%2B_x64_plugins
FROM debian:buster
RUN apt-get update 
RUN apt install -y software-properties-common
RUN add-apt-repository -s 'deb http://deb.debian.org/debian buster main'
RUN add-apt-repository -s 'deb http://deb.debian.org/debian buster-backports main'
RUN add-apt-repository -s 'deb http://deb.debian.org/debian buster-updates main'
RUN apt-get update 
RUN apt-get -y build-dep ffmpeg
RUN apt-get -y install nasm libsdl2-dev gcc vim git gdb build-essential \
    cmake ninja-build checkinstall
RUN apt-get -y install libdevil-dev wget

WORKDIR /
RUN git clone https://github.com/AviSynth/AviSynthPlus
WORKDIR AviSynthPlus
RUN   mkdir avisynth-build && \
    cd avisynth-build && \
    cmake ../ -G Ninja && \
    ninja && \
        checkinstall --pkgname=avisynth --pkgversion="$(grep -r \
            Version avs_core/avisynth.pc | cut -f2 -d " ")-$(date --rfc-3339=date | \
                sed 's/-//g')-git" --backup=no --deldoc=yes --delspec=yes --deldesc=yes \
                    --strip=yes --stripso=yes --addso=yes --fstrans=no --default ninja install


#https://github.com/pinterf/RemoveDirt
WORKDIR /
RUN git clone https://github.com/pinterf/RemoveDirt
WORKDIR /RemoveDirt
RUN cmake -B build -S .
RUN cmake --build build


#https://github.com/pinterf/RgTools/releases
WORKDIR /
RUN git clone https://github.com/pinterf/RgTools.git
WORKDIR /RgTools
RUN cmake -B build -S .
RUN cmake --build build

#https://github.com/pinterf/mvtools
WORKDIR /
RUN git clone https://github.com/pinterf/mvtools
WORKDIR /mvtools
RUN cmake -B build -S .
RUN cmake --build build


WORKDIR /
RUN git clone https://git.videolan.org/git/ffmpeg.git
WORKDIR /ffmpeg
RUN ./configure --prefix=$HOME/ffmpeg_build --enable-gpl --enable-version3 \
    --disable-doc --disable-debug --enable-pic --enable-avisynth && \
    make -j$(nproc) && \
    make install

#ARG USER=mattd
#RUN groupadd -g 1000 ${USER}
#RUN useradd -r -u 1000 -g 1000 ${USER}
#RUN usermod -a -G dialout mattd
#USER ${USER}

#
#
##https://github.com/pinterf/AviSynthPlus
#WORKDIR /
#RUN wget https://github.com/AviSynth/AviSynthPlus/archive/refs/tags/v3.7.2.tar.gz
#RUN tar xzvf v3.7.2.tar.gz
#WORKDIR /AviSynthPlus-3.7.2/
#RUN cmake ./
#ENTRYPOINT ["/ffmpeg/ffmpeg"]
