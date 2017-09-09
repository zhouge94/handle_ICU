#!/bin/sh
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
ACTTION=$1

CMD=eval$( cat /proc/cmdline )

CMDA=$( awk -F "tp=" '{print $2}' << EOF
$CMD
EOF
)

TP=$(awk '{print $1}' << EOF
$CMDA
EOF
)

CMDA=$( awk -F "lcd=" '{print $2}' << EOF
$CMD
EOF
)

LCD=$(awk '{print $1}' << EOF
$CMDA
EOF
)

if [ "$TP" == "cap" ];then
echo "cap touch"    
else
echo "resistor touch"
fi

if [ "$LCD" == "mipi7" ];then
QWS_SIZE="1024x600"
elif [ "$LCD" == "lvds10" ];then
echo "lvds10"
QWS_SIZE="1280x800"
else
QWS_SIZE="800x480"
fi


export T_ROOT=/usr/local/tslib
export LD_LIBRARY_PATH=/usr/local/tslib/lib:$LD_LIBRARY_PATH
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/event1
export TSLIB_PLUGINDIR=$T_ROOT/lib/ts
export TSLIB_CONFFILE=$T_ROOT/etc/ts.conf
export POINTERCAL_FILE=/etc/pointercal
export TSLIB_CALIBFILE=/etc/pointercal


export QTDIR=/forlinx/qt
export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QT_QPA_GENERIC_PLUGINS=tslib
export QT_QWS_FONTDIR=$QTDIR/lib/fonts
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTDIR/plugins

export QWS_DISPLAY="LinuxFB:mmWidth212:0"
export QT_QPA_FB_TSLIB=1
export QWS_SIZE
export QTS_DISPLAY=LinuxFb:/dev/fb0
export KEYPAD_DEV=/dev/input/event0
export QWS_KEYBOARD="/dev/input/event0"
export LD_LIBRARY_PATH=/forlinx/qt/lib/plugins/imageformats:$LD_LIBRARY_PATH
export QT_PLUGIN_PATH=/forlinx/qt/lib/plugins
TOUCH=`cat /etc/t2m`  > /dev/null 2>&1
if [ "$TOUCH" == "M" ];then
export QWS_MOUSE_PROTO="Tslib:/dev/input/event1 mouseman:/dev/input/mice"
else
export QWS_MOUSE_PROTO="Tslib:/dev/input/event1"
fi


#if [ "$ACTTION" == "start" ];then

#if [ ! -e /etc/pointercal ];then
#cd /usr/local/tslib/bin
#./ts_calibrate 
#sync
#fi

#pidof lighttpd  > /dev/null 2>&1
#if [ "$?" != "0" ];then
#cd /forlinx/web/lighttpd/sbin
#./lighttpd -f ../config/lighttpd.conf
#fi

#pidof matrix_browser > /dev/null 2>&1 
#if [ "$?" != "0" ];then
#cd /forlinx/qt/bin
#./matrix_browser -qws http://127.0.0.1 &
#fi

#else
#	killall matrix_browser
#	killall lighttpd
#fi
