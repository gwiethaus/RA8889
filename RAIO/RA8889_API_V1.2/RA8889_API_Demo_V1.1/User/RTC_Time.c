//#include "stm32f10x_lib.h"
#include "stm32f10x_conf.h"

#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"


#include "RTC_Time.h"


/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : 轉換UNIX時間戳為日歷時間
* Input 		 : u32 t  當前時間的UNIX時間戳
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtime轉換結果的tm_year是相對值，需要轉成絕對值
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : 寫入RTC時鐘當前時間
* Input 		 : struct tm t
* Output		 : None
* Return		 : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  //外部tm結构体存儲的年份為2008格式
						//而time.h中定義的年份格式為1900年開始的年份
						//所以，在日期轉換時要考慮到這個因素。

	return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : 從RTC取當前時間的Unix時間戳值
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    : 從RTC取當前時間的日歷時間﹛struct tm﹛
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    : 將給定的Unix時間戳寫入RTC
* Input 		 : time_t t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    : 將給定的Calendar格式時間轉換成UNIX時間戳寫入RTC
* Input 		 : struct tm t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}



////////////////////////////////////////////////////////////////////////////////
// RTC時鐘初始化﹛
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : 來重新配置RTC和BKP，僅在檢測到后備寄存器數据丟失時使用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
	unsigned int i=0;
	//啟用PWR和BKP的時鐘﹛from APB1﹛

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	//后稱辦稈瑪
	PWR_BackupAccessCmd(ENABLE);

	//備份寄存器模塊复位
	BKP_DeInit();

	//外部32.768K
	RCC_LSEConfig(RCC_LSE_ON);
	//等待穩定
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	
	{
	 i++;
	 if (i>0x1fffff); break;
	}

	//RTC時鐘源配置成LSE﹛外部32.768K﹛
	//RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	//RTC開啟
	RCC_RTCCLKCmd(ENABLE);

	//開啟后需要等待APB1時鐘与RTC時鐘同步，才能讀寫寄存器

	RTC_WaitForSynchro();

	//讀寫寄存器前，上一個RTC寫操作操作完成
	RTC_WaitForLastTask();

	//設置RTC分頻器，使RTC時鐘為1Hz
	//RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)

	RTC_SetPrescaler(32767);


	//等待寄存器寫入完成

	RTC_WaitForLastTask();

	//使能秒中斷

	RTC_ITConfig(RTC_IT_SEC, ENABLE);    

	//等待寫入完成

	RTC_WaitForLastTask();

	return;
} 

/*******************************************************************************
* Function Name  : RTC_Config
* Description    : 上電時調用本函數，自動檢查是否需要RTC初始化， 
* 				  若需要重新初始化RTC，則調用RTC_Configuration()完成相應操作
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Config(void)
{
	//我們在BKP的后備寄存器1中，存了一個特殊字符0xA5A5
	//第一次上電或后備電源掉電后，該寄存器數据丟失，
	//表明RTC數据丟失，需要重新配置

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//重新配置RTC
		RTC_Configuration();
		//配置完成后，向后備寄存器中寫特殊字符0xA5A5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		//若后備寄存器沒有掉電，則無需重新配置RTC
		//這里我們可以利用RCC_GetFlagStatus()函數查看本次复位類型
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		//這是上電复位
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
		//這是外部RST管腳复位
		}
		//清除RCC中复位標志
		RCC_ClearFlag();

		//雖然RTC模塊不需要重新配置，且掉電后依靠后備電池依然運行

		RCC_RTCCLKCmd(ENABLE);
		//等待RTC時鐘与APB1時鐘同步
		RTC_WaitForSynchro();

		//使能秒中斷
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//等待操作完成

		RTC_WaitForLastTask();
	}
    RCC_ClearFlag();

    /* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	return;
}
