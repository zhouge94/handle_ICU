#!/bin/sh

#!/bin/sh

gateway=192.168.1.20
addr=192.168.1.200
name=ciscosb
pw=1234567890

function usage()
{
    echo "Usage: -g <gateway-addr> -i <wifi-ip> -e <wifi name> -k <wifi password>"
    echo "eg: ./wifi.sh -g 192.168.1.20 -i 192.168.1.200 -e ciscosb -k 1234567890"
    echo " -g : your gateway address"
    echo " -i : set a default wlan address"
    echo " -e : wifi name" 
    echo " -k : wifi password"
}

function parse_args()
{
#    TEMP=`getopt -o "b:r:t:i:d:a:k:chvs" -- "$@"`
#    eval set -- "$TEMP"

    while true; do
        case "$1" in
            -g ) gateway=$2;echo $gateway;shift 2 ;;
            -i ) addr=$2;echo $addr; shift 2 ;;
            -e ) name=$2;echo $name;shift 2 ;;
            -k ) pw=$2;echo $pw; break;;
            -h ) usage; exit 1 ;;
            -- ) break ;;
            *  ) echo "invalid option $1"; usage; exit 1 ;;
        esac
    done
}

parse_args $@
echo after parse_args
echo gateway $gateway
echo addr $addr
echo name $name
echo pw $pw

insmod /lib/modules/wlan.ko
sleep 3 
ifconfig wlan0 down 
ifconfig eth0 down  
ifconfig usb0 down  
ifconfig ppp0 down  
sleep 3                                                                                                                                                                           
ifconfig wlan0 up
sleep 3                                                                                                                                                                                  
ifconfig wlan0
iwlist wlan0 scan
sleep 3                                                                                                                                                                                                                                                                                                                                                 

ifconfig wlan0 $addr                                                                                                                                                                    
sleep 2
iwconfig wlan0 essid $name                                                                                                                                                                    
sleep 2 
iwconfig wlan0 key $pw                                                                                                                                                                    
sleep 2 
route add default gw $gateway                                                                                                                                                                  
sleep 3 
udhcpc -iwlan0  
sleep 3                                                                                                                                                                                   
ping $gateway

