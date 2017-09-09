echo "Welcome to USE"
cd /zgene/
insmod g_file_storage.ko file=/dev/mmcblk0p3 removable=1 stall=0 &
mount -o sync /dev/mmcblk0p3 disk
