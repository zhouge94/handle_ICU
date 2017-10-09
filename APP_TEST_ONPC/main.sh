killall Handle_Startup_GUI
killall uart_server
./Handle_Startup_GUI -stylesheet style.qss &
#./uart_server -d /dev/ttyAMA0 &
./uart_server -d /dev/ttyUSB0 &
