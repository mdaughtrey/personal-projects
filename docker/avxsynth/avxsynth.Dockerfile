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
RUN git clone git://git.ffmpeg.org/ffmpeg.git
RUN git clone git://github.com/AviSynth/AviSynthPlus.git && \
    cd AviSynthPlus && \
    mkdir avisynth-build && \
    cd avisynth-build && \
    cmake ../ -G Ninja &&  ninja && \
    checkinstall --pkgname=avisynth --pkgversion="$(grep -r \
    Version avs_core/avisynth.pc | cut -f2 -d " ")-$(date --rfc-3339=date | \
    sed 's/-//g')-git" --backup=no --deldoc=yes --delspec=yes --deldesc=yes \
    --strip=yes --stripso=yes --addso=yes --fstrans=no --default ninja install
RUN cd ffmpeg && \
    ./configure --enable-gpl --enable-version3 \
    --disable-doc --disable-debug --enable-pic --enable-avisynth && \
    make -j$(nproc) && \
    make install && \
    checkinstall --pkgname=ffmpeg --pkgversion="7:$(git rev-list \
    --count HEAD)-g$(git rev-parse --short HEAD)" --backup=no --deldoc=yes \
    --delspec=yes --deldesc=yes --strip=yes --stripso=yes --addso=yes \
    --fstrans=no --default
RUN git clone git://github.com/ffms/ffms2.git && cd ffms2 && \
    PKG_CONFIG_PATH=$HOME/ffavx_build/lib/pkgconfig \
    CPPFLAGS="-I/usr/local/include/avisynth" \
    ./autogen.sh --enable-shared --enable-avisynth && \
    make -j$(nproc) && \
    checkinstall --pkgname=ffms2 --pkgversion="1:$(./version.sh)-git" \
    --backup=no --deldoc=yes --delspec=yes --deldesc=yes --strip=yes --stripso=yes \
    --addso=yes --fstrans=no --default
#RUN git config --global user.email "matt@daughtrey.com"
#RUN git config --global user.name "mdaughtrey"
#RUN git clone https://github.com/mdaughtrey/personal-projects
#RUN cp /personal-projects/bin/vimrc ~/.vimrc
#RUN cp /personal-projects/bin/tmux.conf ~/.tmux.conf

