#https://avisynthplus.readthedocs.io/en/latest/avisynthdoc/contributing/posix.html
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


WORKDIR /
RUN git clone https://git.videolan.org/git/ffmpeg.git
WORKDIR /ffmpeg
RUN ./configure --prefix=$HOME/ffmpeg_build --enable-gpl --enable-version3 \
    --disable-doc --disable-debug --enable-pic --enable-avisynth && \
    make -j$(nproc) && \
    make install

WORKDIR /
RUN git clone https://github.com/pinterf/RemoveDirt
WORKDIR /RemoveDirt
RUN cmake -B build -S .
RUN cmake --build build

