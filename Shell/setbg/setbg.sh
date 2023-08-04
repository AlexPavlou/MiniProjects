#!/bin/sh

# Depending on the day of the week, a different wallpaper will be used.

today=$(date +"%a")

case $today in
Mon)
xwallpaper --zoom "/home/alex/.config/bg/mon.jpg"
;;
Tue)
xwallpaper --zoom "/home/alex/.config/bg/tue.jpg"
;;
Wed)
xwallpaper --zoom "/home/alex/.config/bg/wed.jpg"
;;
Thu)
xwallpaper --zoom "/home/alex/.config/bg/thu.jpg"
;;
Fri)
xwallpaper --zoom "/home/alex/.config/bg/fri.jpg"
;;
Sat)
xwallpaper --zoom "/home/alex/.config/bg/sat.jpg"
;;
Sun)
xwallpaper --zoom "/home/alex/.config/bg/sun.jpg"
;;
*)
;;
esac
