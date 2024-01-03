FROM debian:bullseye

RUN apt update
RUN apt -y install vim-nox python3 python3-pip git wget
RUN apt -y install curl
RUN apt -y install vim-airline vim-airline-themes

#RUN mkdir -p /home/${USER}
RUN mkdir -p /root/.vim/colors
COPY vimrc /root/.vimrc
COPY bashrc /root/.bashrc
COPY molokai.vim /root/.vim/colors
WORKDIR /root
RUN wget https://go.dev/dl/go1.21.5.linux-amd64.tar.gz && tar -C /root -xzvf go1*.gz && rm /root/go1*.gz
#RUN vim -c 'q!'
RUN curl -fLo /root/.vim/autoload/plug.vim --create-dirs https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
RUN vim +PlugInstall +GoInstallBinaries +qall
RUN apt -y install tmux screen
#RUN vim touch.go +qall && rm touch.go

