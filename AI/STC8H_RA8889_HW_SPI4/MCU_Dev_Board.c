#include "MCU_Dev_Board.h"
#include "UserDef.h"
#include "RA8889_MCU_IF.h"
#include "delay.h"


/*****************************************************************
  * @brief  闪烁LED1
  * @param  无
  * @retval 无
  * @note   
  ****************************************************************/
void LED1_FLASH(void)
{
	LED1_ON;
	delay_ms(50);
	LED1_OFF;
	delay_ms(50);
	LED1_ON;
	delay_ms(50);
	LED1_OFF;
	delay_ms(1000);
}


/*****************************************************************
  * @brief  闪烁LED2
  * @param  无
  * @retval 无
  * @note   
  ****************************************************************/
void LED2_FLASH(void)
{
	LED2_ON;
	delay_ms(50);
	LED2_OFF;
	delay_ms(50);
	LED2_ON;
	delay_ms(50);
	LED2_OFF;
	delay_ms(1000);
}


/*****************************************************************
  * @brief  读写RA8889寄存器测试函数
  * @param  无
  * @retval 无
  * @note   用于检测通信接口是否正常，正常时闪烁LED灯指示
  ****************************************************************/
void Comm_Test(void)
{
	unsigned char temp;

	while (1) 
	{
		// 寄存器写入
		LCD_CmdWrite(0xAD);		//指定寄存器0xAD
		LCD_DataWrite(0xA5);	//写入数据0xA5

		// 寄存器读回
		LCD_CmdWrite(0xAD);
		temp = LCD_DataRead();

		if (temp == 0xA5) 
		{
			LED2_ON;
			delay_ms(50);
			LED2_OFF;
			delay_ms(50);
			LED2_ON;
			delay_ms(50);
			LED2_OFF;
			delay_ms(500);
		}
	}
}
