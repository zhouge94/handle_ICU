echo $1
if [ $1 = "33" ];then
	echo back_home
	./reset.sh >/dev/null 
elif [ $1 == "17" ];then
	echo "poweroff now"
	poweroff
fi
