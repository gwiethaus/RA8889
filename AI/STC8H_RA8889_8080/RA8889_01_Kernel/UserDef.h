
#ifndef __USERDEF_H
#define __USERDEF_H

#include "stc8h.h"       //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�


/* MCU����ָʾ������ */
sbit LED1 = P1 ^ 7;
sbit LED2 = P1 ^ 6;

#define LED1_ON  	LED1 = 0;
#define LED1_OFF 	LED1 = 1;
#define LED2_ON  	LED2 = 0;
#define LED2_OFF 	LED2 = 1;


/* FLASHƬѡ���� */
#define BUS0  0
#define BUS1  1
#define CS0  0
#define CS1  1
#define CS2  2
#define CS3  3


/* RA8889�Դ����ʾ���� */
#define canvas_image_width  1600		//������ȣ��趨Ϊ�����n��
#define main_image_width  1600			//ͼ���ȣ��趨Ϊ�����n��
#define LCD_Width  800 
#define LCD_Height  480


/* ͼ�㶨�� */
#define Layer0  768000*3*0		//ͼ��0��ַ(��24λɫ����:canvas_image_width*LCD_Height*24/8)
#define Layer1  768000*3*1		//ͼ��1��ַ
#define Layer2  768000*3*2		//ͼ��2��ַ
#define Layer3  768000*3*3		//ͼ��3��ַ
#define Layer4  768000*3*4		//ͼ��4��ַ


/* RA8889Ƶ������ */
#define OSC_FREQ	10			//����Ƶ�ʣ���Ӿ�����ʹ��10MHz
#define DRAM_FREQ	133			//SDRAMˢ��Ƶ�ʣ����166MHz
#define CORE_FREQ	120			//RA8889����Ƶ�ʣ����133MHz 
#define SCAN_FREQ	30			//PCLKҺ��ɨ��Ƶ�ʣ����100MHz


/* Һ�����ͺ� */
#define L80480R70		//800x480


/*==== MCU�ӿ�ͨ��λ������ʾɫ��� ====*/
//#define MCU_8bit_ColorDepth_8bpp  
//#define MCU_8bit_ColorDepth_16bpp
#define MCU_8bit_ColorDepth_24bpp
//#define MCU_16bit_ColorDepth_16bpp
//#define MCU_16bit_ColorDepth_24bpp_Mode_1
//#define MCU_16bit_ColorDepth_24bpp_Mode_2


/* ��æ��ʽѡ������æѭ��while�Ῠס�򽫸������� */
#define Check_While_Busy


/* 8bpp��ɫ���� */
#define color256_black   0x00
#define color256_white   0xff
#define color256_red     0xe0
#define color256_green   0x1c
#define color256_blue    0x03
#define color256_yellow  color256_red|color256_green
#define color256_cyan    color256_green|color256_blue
#define color256_purple  color256_red|color256_blue


/* 16bpp��ɫ���� */
#define color65k_black   0x0000
#define color65k_white   0xffff
#define color65k_red     0xf800
#define color65k_green   0x07e0
#define color65k_blue    0x001f
#define color65k_blue2   0x051F
#define color65k_gray    0xF7DE
#define color65k_magenta 0xF81F
#define color65k_yellow  color65k_red|color65k_green		//0xFFE0
#define color65k_cyan    color65k_green|color65k_blue		//0x7FFF
#define color65k_purple  color65k_red|color65k_blue


/* 24bpp��ɫ���� */
#define color16M_black   0x00000000
#define color16M_white   0x00ffffff
#define color16M_red     0x00ff0000
#define color16M_green   0x0000ff00
#define color16M_blue    0x000000ff
#define color16M_yellow  color16M_red|color16M_green
#define color16M_cyan    color16M_green|color16M_blue
#define color16M_purple  color16M_red|color16M_blue


/* 16bpp�ҽ�ɫ���� */
#define color65k_grayscale1    2113
#define color65k_grayscale2    2113*2
#define color65k_grayscale3    2113*3
#define color65k_grayscale4    2113*4
#define color65k_grayscale5    2113*5
#define color65k_grayscale6    2113*6
#define color65k_grayscale7    2113*7
#define color65k_grayscale8    2113*8
#define color65k_grayscale9    2113*9
#define color65k_grayscale10   2113*10
#define color65k_grayscale11   2113*11
#define color65k_grayscale12   2113*12
#define color65k_grayscale13   2113*13
#define color65k_grayscale14   2113*14
#define color65k_grayscale15   2113*15
#define color65k_grayscale16   2113*16
#define color65k_grayscale17   2113*17
#define color65k_grayscale18   2113*18
#define color65k_grayscale19   2113*19
#define color65k_grayscale20   2113*20
#define color65k_grayscale21   2113*21
#define color65k_grayscale22   2113*22
#define color65k_grayscale23   2113*23
#define color65k_grayscale24   2113*24
#define color65k_grayscale25   2113*25
#define color65k_grayscale26   2113*26
#define color65k_grayscale27   2113*27
#define color65k_grayscale28   2113*28
#define color65k_grayscale29   2113*29
#define color65k_grayscale30   2113*30


/* 24�� ��Ԥ���� */
#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216
#define Line10         240
#define Line11         264
#define Line12         288
#define Line13         312
#define Line14         336
#define Line15         360
#define Line16         384
#define Line17         408
#define Line18         432
#define Line19         456
#define Line20         480
#define Line21         504
#define Line22         528
#define Line23         552
#define Line24         576


/* λ����Ԥ���� */
#define	cSetb0		0x01
#define	cSetb1		0x02
#define	cSetb2		0x04
#define	cSetb3		0x08
#define	cSetb4		0x10
#define	cSetb5		0x20
#define	cSetb6		0x40
#define	cSetb7		0x80

#define	cClrb0		0xfe
#define	cClrb1		0xfd
#define	cClrb2		0xfb
#define	cClrb3		0xf7
#define	cClrb4		0xef
#define	cClrb5		0xdf
#define	cClrb6		0xbf
#define	cClrb7		0x7f


#endif /*__UserDef_H*/
