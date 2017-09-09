#!/bin/sh

ifconfig eth0 down
ifconfig usb0 down
ifconfig ppp0 down
ifconfig wlan0 down
sleep 1 
pppd call wcdma-mf210 &


