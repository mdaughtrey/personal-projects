#!/bin/bash

sendwait()
{
    waitfor=$1
    shift
    send=${@// / Space }
    tmux send-keys $send 'C-m';
    capture=''
    while [[ ! $capture =~ $waitfor ]]; do
        tmux capture-pane
        readarray -t screenbuf <<< $(tmux show-buffer | sed '/^$/d' )
        capture=${screenbuf[((${#screenbuf[@]}-1))]}
    done
}

lastbuf()
{
    echo ${screenbuf[((${#screenbuf[@]}-1))]}
}

sbfind()
{
    look=$1
    for ii in `seq 0 ${#screenbuf[@]}` ; do
       if [[ ${screenbuf[$ii]} =~ $look ]]; then echo ${screenbuf[$ii]}; fi
    done
}

sblock()
{
    start=$1
    end=$2
    let write=0
    for ii in `seq 0 ${#screenbuf[@]}` ; do
#        line=${screenbuf[$ii]}
#        if [[ $line =~ $start ]] && [[ $line =~ $end ]]; then continue; fi
        if ((!$write)) && [[ ${screenbuf[$ii]} =~ $start ]]; then let write=1; continue; fi
        if (($write)) && [[ ${screenbuf[$ii]} =~ $end ]]; then let write=0; continue; fi
        if (($write)); then echo ${screenbuf[$ii]}; fi
    done
}

msgbox()
{
    title=$1
    let maxlen=0
    text=''
    for ii in `seq 0 ${#display[@]}`; do
        thisline=${display[$ii]}
        #echo thisline $thisline
        if ((${#thisline} > $maxlen)); then
            maxlen=${#thisline}
        fi
        read -r -d '' text <<ABC
$text
$thisline
ABC
 
    done
    whiptail --title "$title" --msgbox "$text" ${#display[@]} $((maxlen * 2))
}

listdirs() 
{
    set +o xtrace
    sendwait '(password:|\$)$' 'ssh mattd@converter'
    xx=$(lastbuf)
    if [[ $xx =~ password: ]]; then sendwait '\$$' tension5; fi
    sendwait '\$$' 'clear && echo xyzstart && ls -1 /mnt/exthd/ && echo xyzend && exit'
    readarray -t display <<< $(sblock xyzstart xyzend)
    msgbox Directories
}

showmenu()
{
    result=$(whiptail --title 'Main Menu' --menu 'Choose an option' 25 78 16\
    'quit' 'Exit'\
    'listdir' 'List Directories' 2>&1 >/dev/tty)
    rc=$?
    set -o xtrace
    if [[ $rc ]] && [[ "$result" == "listdir" ]]; then listdirs; fi
}
showmenu
