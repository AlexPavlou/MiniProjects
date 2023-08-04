#!/bin/bash

# either scans the user's ipv4 address or receives a custom ip address and returns info about it, by curl-ing ipinfo.io

while getopts "h?si:" opt; do
    case "$opt" in
        h)
            echo "-s (sh ip.sh -s (scan)): scans ip address automatically and returns information"
            echo "-i (sh ip.sh -i 1.1.1.1 (input)): receives an ip address as input from the user and returns all info on it."
            echo "-h (help): shows this help page"
            ;;
        i)
            ip=$OPTARG
            echo $(curl ipinfo.io/$ip)
            ;;
        s)
            ip=$(curl -4 icanhazip.com)
            echo $(curl ipinfo.io/$ip)
            ;;
    esac
done
