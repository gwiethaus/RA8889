
#include "stc8h.h"       //包含此头文件后，不需要再包含"reg51.h"头文件

/* P2.SPI-4(硬件) IO口配置*/
sbit RA8889_RST	= P2 ^ 1;		// Reset RA8889				（通过IO口接到RA8889/RA8876进行硬件复位）
sbit SPI_SS   	= P2 ^ 2;		// Slave Select				（RA8889片选）
sbit SPI_MISO		= P2 ^ 3;		// Master In Slave Out（接RA8889的SDO）
sbit SPI_MOSI		= P2 ^ 4;		// Master Out Slave In（接RA8889的SDI）
sbit SPI_SCK		= P2 ^ 5;		// Serial Clock

/* SPI-4读写函数 */
void SPI4_Init();
void SPI4_SendByte(unsigned char byte);
unsigned char SPI4_ReceiveByte();

/* LCD API接口 */
void LCD_CmdWrite(unsigned char cmd);
void LCD_DataWrite(unsigned char Data);
unsigned char LCD_DataRead(void);
unsigned char LCD_StatusRead(void);
