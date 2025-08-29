#include "stc8h.h"       //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�

/* 8080 IO������*/
sbit  RA8889_CS  = P0 ^ 0;
sbit	RA8889_RD  = P0 ^ 1;
sbit	RA8889_RS  = P0 ^ 2;
sbit	RA8889_WR	 = P0 ^ 3;
sbit  RA8889_RST = P0 ^ 4;		

#define Data_Bus  P2

 
/* 8080 ��д���� */
void LCD_CmdWrite(unsigned char Cmd);
void LCD_DataWrite(unsigned char Data);
unsigned char LCD_StatusRead(void);
unsigned char LCD_DataRead(void);
