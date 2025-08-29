
#include "stc8h.h"       //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�

/* P2.SPI-4(Ӳ��) IO������*/
sbit RA8889_RST	= P2 ^ 1;		// Reset RA8889				��ͨ��IO�ڽӵ�RA8889/RA8876����Ӳ����λ��
sbit SPI_SS   	= P2 ^ 2;		// Slave Select				��RA8889Ƭѡ��
sbit SPI_MISO		= P2 ^ 3;		// Master In Slave Out����RA8889��SDO��
sbit SPI_MOSI		= P2 ^ 4;		// Master Out Slave In����RA8889��SDI��
sbit SPI_SCK		= P2 ^ 5;		// Serial Clock

/* SPI-4��д���� */
void SPI4_Init();
void SPI4_SendByte(unsigned char byte);
unsigned char SPI4_ReceiveByte();

/* LCD API�ӿ� */
void LCD_CmdWrite(unsigned char cmd);
void LCD_DataWrite(unsigned char Data);
unsigned char LCD_DataRead(void);
unsigned char LCD_StatusRead(void);
