硬件：STC8H8K64U + RA8889开发板（硬件跳线变更为并口8080模式，PS=00x，R143，R142不接，R141无关）

【v1.3 代码说明】

（1）该代码采用8080，通信接口定义：
	P2		Data_Bus     	8位通信数据线    DB0--DB7
	P0.0	RA8889_CS		RA8889片选		 XnSC
	P0.1	RA8889_RD		RA8889的读		 XnRD(XEN)
	P0.2	RA8889_RS		RA8889的地址	 XA0
	P0.3	RA8889_WR		RA8889的写		 XnWR(XRNW)
	P0.4	RA8889_RST		RA8889硬件复位	

（2）使用J-Link等工具将RA8889_Demo文件夹内的RA8889_Demo_F103_8080_SPI_floating_firmware.hex烧录到RA8889开发板上，将F103与RA8889的接线设定为高阻态。

（3）核心文件存放在RA8889_01_Kernel文件夹下：
	RA8889_51.c 		为底层接口文件
	RA8889_API_51.c 	为用户层接口文件
	RA8889_MCU_IF.c 	为单片机接口文件（通信接口定义及通信函数）
	UserDef.h 			为用户自定义参数文件
	delay.c				延迟函数，请根据单片机的主频修正
		
（4）演示以下常用功能：
	Introduction();				//文字介绍
	Geometry_Demo();			//几何绘图演示
	Show_JPG();					//调用FLASH的JPG图片显示
	Show_BMP();					//调用FLASH的BMP图片显示
	Fade_in_Fade_out();			//图片淡入淡出演示
	Show_AVI();					//调用FLASH的AVI影片播放
	Show_PNG();					//调用FLASH的PNG图片显示


***访问论坛获取更多信息：bbs.lcdvision.com.cn
