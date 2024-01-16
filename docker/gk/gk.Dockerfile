FROM debian:latest
RUN apt-get update 
RUN apt-get -y install gcc vim git gdb build-essential tmux screen curl wget python3-pygments \
exuberant-ctags python3-venv

RUN mkdir -p /root/.vim/colors
#COPY vimrc /root/.vimrc
#COPY bashrc /root/.bashrc
#COPY molokai.vim /root/.vim/colors
WORKDIR /root
RUN git clone https://github.com/cmh25/gk.git
RUN git clone https://github.com/cyrus-and/gdb-dashboard.git
RUN cp gdb-dashboard/.gdbinit .
RUN mkdir -p /root/.config/gdb/
RUN echo "set auto-load safe-path /" /root/.config/gdb/gdbinit
RUN curl -fLo /root/.vim/autoload/plug.vim --create-dirs https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
RUN vim +PlugInstall +qall
WORKDIR /root/gk
COPY bashrc /root/.bashrc
RUN make gkd
RUN ctags -R
RUN python3 -m venv venv
RUN . venv/bin/activate && python3 -m pip install pipx && \
pipx install gdbgui && python3 -m userpath append /root/.local/bin

