#!/bin/bash

# emulates basic clock functions like:
#c) simply showing a clock with the current time
#s) counting time passed since ran (stopwatch)
# a) alarming the user with a desktop notification after x seconds

while getopts "h?csa:" opt; do
    case "$opt" in
        h)
                echo "-c (sh clock.sh -c (clock)): functions like a clock."
                echo "-s (sh clock.sh -s (x seconds) (stopwatch)): counts the time that has passed since the user started the program."
                echo "-a (sh clock.sh -a (x seconds) (alarm-countdown)): Alerts the user via notification after the time period he has set."
                echo "-h (help): shows this help page"
                ;;
        c)
                while true; do
                echo $(date +"%r %D")
                sleep 1
                clear; done
                ;;
        s)
                s=0
                m=0
                h=0
                while true; do
                        echo -ne "$h:$m:$s\033[0K\r"
                        sleep 1
                        s=$((s+1))
                        if [ "$s" -eq 60 ]
                        then
                                s=0
                                m=$((m+1))
                        fi
                        if [ "$m" -eq 60 ]
                        then
                                s=0
                                m=0
                                h=$((h+1))
                        fi;done
                ;;
        a)
                sec=$OPTARG
                min=$((sec/60))
                hour=$((min/60))
                sec=$((sec%60))
                min=$((min%60))
                while [ $hour -ge 0 ]; do
                        while [ $min -ge 0 ]; do
                                 while [ $sec -ge 0 ]; do
                                        echo -ne "$hour:$min:$sec\033[0K\r"
                                        let "sec=sec-1"
                                        sleep 1
                                done
                                sec=59
                                let "min=min-1"
                        done
                        min=59
                        let "hour=hour-1"
                done
                notify-send "Alarm is over!"
                # play abc.mp3
                ;;
    esac
done
