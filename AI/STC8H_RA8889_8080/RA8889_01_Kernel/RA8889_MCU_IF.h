#include "stc8h.h"       //包含此头文件后，不需要再包含"reg51.h"头文件

/* 8080 IO口配置*/
sbit  RA8889_CS  = P0 ^ 0;
sbit	RA8889_RD  = P0 ^ 1;
sbit	RA8889_RS  = P0 ^ 2;
sbit	RA8889_WR	 = P0 ^ 3;
sbit  RA8889_RST = P0 ^ 4;		

#define Data_Bus  P2

 
/* 8080 读写函数 */
void LCD_CmdWrite(unsigned char Cmd);
void LCD_DataWrite(unsigned char Data);
unsigned char LCD_StatusRead(void);
unsigned char LCD_DataRead(void);
