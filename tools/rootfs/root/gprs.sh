#!/bin/sh
ifconfig eth0 down
ifconfig wlan0 down
ifconfig usb0 down
sleep 1
pppd call gprs /dev/ttyAMA0 &
sleep 6 
ifconfig
