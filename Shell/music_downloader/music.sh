#!/bin/sh

# receives the commandline argument and in return downloads the audio of a video in the specified audio format

while getopts "h?f:m:" opt; do
        case "$opt" in
                h)
                        echo "-f (sh music.sh -f (flac))"
                        echo "-m (sh music.sh -m (mp3))"
                        echo "-h (help): shows this help page"
                        ;;
                f)
                        yt-dlp -x --audio-format flac ytsearch:$OPTARG
                        ;;
                m)
                        yt-dlp -x --audio-format mp3 ytsearch:$OPTARG
                        ;;
        esac
done
