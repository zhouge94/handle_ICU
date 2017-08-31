实现功能：
/**************V1.0****************/
1、oled显示正常
2、心电数据接收正常，接收到的心电数据是250hz左右，频率降低一倍显示在oled上
3、心率数据接收正常

/**************V2.0****************/
1、加上ADC,两个通道，PA0.PA1
2、ADC采用两个通道连续循环采样
HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, 4);

函数里面二个参数为4的话
uhADCxConvertedValue[0],uhADCxConvertedValue[2]为通道1的采集结果；
uhADCxConvertedValue[1],uhADCxConvertedValue[3]为通道2的采集结果；

3、如果显示屏有错误的话，看看GPIO_Write();这个函数

/**************V3.0****************/
1、加上SD卡模块的程序
2、修改启动文件里的堆栈大小，即下面两个参数

Stack_Size		EQU     0x800//======================

Heap_Size      EQU     0x400//========================
3、我们想使用DMA模式读写SD CARD
在sd_diskio.c文件中我们发现了SD读写的函数如图所示
用到的是这两个函数，
BSP_SD_ReadBlocks();//==========115行
BSP_SD_WriteBlocks();//==========139行
修改为
BSP_SD_ReadBlocks_DMA();
BSP_SD_WriteBlocks_DMA();
4、死锁的bug：
SD_DMA_RxCplt()一直在死等 while(hsd->SdTransferCplt == 0)
死锁原因：DMA的完成中断里死等SD完成的信号。
此信号在SD中断里清除，而SD的中断跟DMA中断是同一优先级，无法抢占。
解决办法：
在stm32f4xx_hal_sd.c中将
SD_DMA_RxCplt()里去掉 while(hsd->SdTransferCplt == 0)//============2293行
SD_DMA_TxCplt()里去掉while(hsd->SdTransferCplt ==0)//============2333行

/**************V4.0****************/
1、加上震动马达和按键，按键通过查询的方式来获取按键的键值

/**************V7.0****************/
1、串口接收中断，每次接受一个数据，将接受的数据存到一个数组里，接收的数据数量够的话开始处理

