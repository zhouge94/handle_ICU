#!/bin/bash
outinfo()
{
echo "选项"
echo "  [1]fastboot download root.img"
echo "  [2]fastboot download boot.img"
echo "  [3]make root.img and boot img"
echo "  [r]reboot the target system"
echo "  [q]quit"

}

while true
do
	outinfo
	echo -n "Enter your opition:"   #参数-n的作用是不换行，echo默认是换行
	read  option                   #从键盘输入
	echo "your option is $option"    #显示信息

	if [ "$option"x = ""x ] ;
	then
		echo "please enter"
	elif [ "$option" = "q" ] ;
	then 
		exit 0 
	elif [ "$option" = "1" ] ;  #[1]fastboot download root.img
	then 
		sudo fastboot flash system ./tmp/root.img
	elif [ "$option" = "2" ] ;  #[2]fastboot download boot.img
	then 
		sudo fastboot flash boot ./tmp/boot.img
	elif [ "$option" = "3" ] ;  #[3]make root.img and boot img
	then 
		sudo ./build.sh
	elif [ "$option" = "r" ] ;  #[r]reboot the target system"
	then 
		sudo fastboot reboot
	else
		echo "You are born beauty,but I have err!"
	fi
	sleep 1s
done
