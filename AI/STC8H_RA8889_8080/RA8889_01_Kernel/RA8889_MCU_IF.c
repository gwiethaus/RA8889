#include "RA8889_MCU_IF.h"
#include "delay.h"
#include <intrins.h>


/*********************************************************/
/*  8080 通信协议                                         */
/*********************************************************/

void LCD_CmdWrite(unsigned char Cmd)
{	 
	Data_Bus = Cmd;		//Reg Addr
	RA8889_CS = 0;
  RA8889_RS = 0;
	RA8889_RD = 1;
	RA8889_WR = 0;
	RA8889_WR = 1;
  RA8889_CS=1;
	delay(10);
}


void LCD_DataWrite(unsigned char Data)
{
	Data_Bus = Data;		//Reg Addr
	RA8889_CS = 0;
  RA8889_RS = 1;
	RA8889_RD = 1;
	RA8889_WR = 0;
	RA8889_WR = 1;
  RA8889_CS=1;
	delay(10);
}


unsigned char LCD_DataRead(void)
{
	unsigned char Data;
	Data_Bus = 0xff;
	RA8889_CS = 0;
	RA8889_RS = 1;
	RA8889_WR = 1;
	RA8889_RD = 0;
	Data = Data_Bus;
	RA8889_RD = 1;
	RA8889_CS = 1;	
	delay(10);
	return Data;
}


unsigned char LCD_StatusRead(void)
{
	unsigned char Data;
	Data_Bus = 0xff;
	RA8889_CS = 0;
	RA8889_RS = 0;
	RA8889_WR = 1;
	RA8889_RD = 0;	
	Data = Data_Bus;
	RA8889_RD = 1;	
	RA8889_CS = 1;
	delay(10);
	return Data;
}
