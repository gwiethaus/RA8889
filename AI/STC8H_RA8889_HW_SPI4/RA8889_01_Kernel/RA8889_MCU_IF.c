#include "RA8889_MCU_IF.h"
#include "stc8h.h"       //包含此头文件后，不需要再包含"reg51.h"头文件


/*********************************************************/
/*  SPI-4 通信协议：硬件方式                             */
/*********************************************************/

/* SPI-4 初始化 */
void SPI4_Init()
{
	SPCTL |=  (1 << 7); //忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
	SPCTL |=  (1 << 6); //使能 SPI 功能
	SPCTL &= ~(1 << 5); //先发送/接收数据的高位（ MSB）
	SPCTL |=  (1 << 4); //设置主机模式
	SPCTL |=  (1 << 3); //SCLK 空闲时为高电平，SCLK 的前时钟沿为下降沿，后时钟沿为上升沿
	SPCTL |=  (1 << 2); //数据在 SCLK 的前时钟沿驱动，后时钟沿采样
	SPCTL = (SPCTL & ~3) | 2;   //SPI 时钟频率选择, 0: 4T, 1: 8T,  2: 16T,  3: 2T  （时钟频率先调整到最慢，再依照自己的系统调整）
	
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO口选择P2. 0：P1.2/P5.4 P1.3 P1.4 P1.5  ||  1：P2.2 P2.3 P2.4 P2.5  ||  2：P5.4 P4.0 P4.1 P4.3  ||  3：P3.5 P3.4 P3.3 P3.2

	SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志	
}

/* SPI-4 发送数据 */
void SPI4_SendByte(unsigned char dat)
{
	SPDAT = dat;
	while((SPSTAT & SPIF) == 0) ;
	SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志
}

/* SPI-4 接收数据 */
unsigned char SPI4_ReceiveByte()
{
	SPDAT = 0xff;
	while((SPSTAT & SPIF) == 0) ;
	SPSTAT = SPIF + WCOL;   //清0 SPIF和WCOL标志
	return (SPDAT);
}



/* LCD API: 写入命令 */
void LCD_CmdWrite(unsigned char cmd)
{
	SPI_SS=0;
	SPI4_SendByte(0x00);
	SPI4_SendByte(cmd);
	SPI_SS=1;
}

/* LCD API: 写入数据 */
void LCD_DataWrite(unsigned char Data)
{
	SPI_SS=0;
	SPI4_SendByte(0x80);
	SPI4_SendByte(Data);
	SPI_SS=1;
}

/* LCD API: 读取数据 */
unsigned char LCD_DataRead(void)
{
	unsigned char Data;
	
	SPI_SS=0;
	SPI4_SendByte(0xc0);
	Data = SPI4_ReceiveByte();
	SPI_SS=1;
	return Data;
}

/* LCD API: 读取芯片状态 */
unsigned char LCD_StatusRead(void)
{
	unsigned char Data;
	
	SPI_SS=0;
	SPI4_SendByte(0x40);
	Data = SPI4_ReceiveByte();
	SPI_SS=1;
	return Data;
}
