#! /bin/bash

NAME=$(whiptail --backtitle "Copyright Luigi 20-20"\
    --title "Dă-i.sh"\
    --menu "\n   Ce vrei de la viață?\n\n" 15 45 0 \
    "make build" ""\
    "make clean" ""\
    "make run_server" ""\
    "make run_subscriber_1" ""\
    "make run_subscriber_2" ""\
    3>&1 1>&2 2>&3)
$NAME

{
    for ((i = 0 ; i <= 100 ; i+=10));
    do
        sleep 0.1
        echo $i
    done
} | whiptail --gauge "Aveți răbdare..." 6 50 0

exit