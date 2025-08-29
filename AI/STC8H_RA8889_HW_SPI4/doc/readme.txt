
硬件：STC8H8K64U + RA8889开发板（硬件跳线变更为SPI-4模式，PS=101，R143，R141短接，R142不接）


【v1.3 代码说明】

（1）该代码采用硬件SPI-4，通信接口定义：

	STC8H寄存器P_SW1之[B3:B2]=SPI_S[1:0]
		SS		MOSI	MISO	SCLK
	00: P1.2	P1.3	P1.4	P1.5
	01:	P2.2	P2.3	P2.4	P2.5（本例选择P2.SPI）
	10:	P5.4	P4.0	P4.1	P4.3
	11:	P3.5	P3.4	P3.3	P3.2
	
	≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡STC8H8K64U硬件接线≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡
	[P2.1] GPIO		----> RA8889的复位	RA8889_RST
	[P2.2] SPI_SS	----> RA8889的片选	DB[4] (XnSCS)
	[P2.3] SPI_MOSI	----> RA8889的SDI 	DB[6] (XSSDI)
	[P2.4] SPI_MISO	<---- RA8889的SDO 	DB[5] (XSSDO)
	[P2.5] SPI_SCK	----> RA8889的SCK 	DB[7] (XSSCL)
	≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡
		
（2）使用J-Link等工具将RA8889_Demo文件夹内的RA8889_Demo_F103_SPI_firmware.hex烧录到RA8889开发板上，将F103与RA8889的接线设定为高阻态。

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
