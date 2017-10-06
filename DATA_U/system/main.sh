cd /zgene/disk/system/ 
killall Handle_Startup_GUI
killall uart_server
./Handle_Startup_GUI -stylesheet style.qss -qws &
./uart_server -d /dev/ttyAMA0 &
