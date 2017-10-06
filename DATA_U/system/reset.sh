cd /zgene/disk/system/ 
killall Handle_Startup_GUI
killall uart_server
killall ECG_HX_GUI
killall ECG_GUI
killall MPU_GUI
./Handle_Startup_GUI -stylesheet style.qss -qws &
./uart_server -d /dev/ttyAMA0 &
