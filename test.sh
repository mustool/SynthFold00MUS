#!/bin/bash

jalv.gtk http://musens.pluggar/lv2/lv2pftci/synthFold00MUS &

sleep 2

jack_connect "synthFold00MUS:out1" "system:playback_1" &

jack_connect "synthFold00MUS:out2" "system:playback_2" &

#aconnect $1:0 $2:0
