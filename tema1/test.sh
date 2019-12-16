#!/bin/bash

SPEED=20
DELAY=100
LOSS=0
CORRUPT=0
REORDER=0

killall link 2> /dev/null
killall recv 2> /dev/null
killall send 2> /dev/null

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT reorder=$REORDER &> /dev/null &
sleep 1
./recv &
sleep 1

./send fileX $SPEED $DELAY
