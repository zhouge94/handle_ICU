#!/bin/sh
echo "ATE0" > /dev/ttyUSB2  
echo "AT^NDISDUP=1,1,\"cmnet\""> /dev/ttyUSB2

ifconfig eth0 down 
ifconfig wlan0 down 
ifconfig ppp0 down 

ifconfig usb0 up 
udhcpc -iusb0 
ifconfig eth0 up

