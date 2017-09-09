#!/bin/sh
if [ "$ACTION" == "add" ];then
	if [ -d /sys/block/*/$MDEV ] ; then
			mkdir -p /media/$MDEV
			mount /dev/$MDEV /media/$MDEV
	fi
elif [ "$ACTION" == "remove" ];then
	umount -l /media/$MDEV*
	rm -rf /media/$MDEV*
fi
