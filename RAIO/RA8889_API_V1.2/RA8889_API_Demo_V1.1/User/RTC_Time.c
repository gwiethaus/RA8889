//#include "stm32f10x_lib.h"
#include "stm32f10x_conf.h"

#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"


#include "RTC_Time.h"


/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : �ഫUNIX�ɶ��W������ɶ�
* Input 		 : u32 t  ��e�ɶ���UNIX�ɶ��W
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtime�ഫ���G��tm_year�O�۹�ȡA�ݭn�ন�����
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : �g�JRTC������e�ɶ�
* Input 		 : struct tm t
* Output		 : None
* Return		 : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  //�~��tm�����^�s�x���~����2008�榡
						//��time.h���w�q���~���榡��1900�~�}�l���~��
						//�ҥH�A�b����ഫ�ɭn�Ҽ{��o�Ӧ]���C

	return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : �qRTC����e�ɶ���Unix�ɶ��W��
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
* Description    : �qRTC����e�ɶ�������ɶ���struct tm��
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
* Description    : �N���w��Unix�ɶ��W�g�JRTC
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
* Description    : �N���w��Calendar�榡�ɶ��ഫ��UNIX�ɶ��W�g�JRTC
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
// RTC������l�ơ�
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : �ӭ��s�t�mRTC�MBKP�A�Ȧb�˴���Z�ƱH�s�����u�ᥢ�ɨϥ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
	unsigned int i=0;
	//�ҥ�PWR�MBKP��������from APB1��

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	//�Z�ٿ�z��
	PWR_BackupAccessCmd(ENABLE);

	//�ƥ��H�s���Ҷ��`��
	BKP_DeInit();

	//�~��32.768K
	RCC_LSEConfig(RCC_LSE_ON);
	//����í�w
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	
	{
	 i++;
	 if (i>0x1fffff); break;
	}

	//RTC�������t�m��LSE���~��32.768K��
	//RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	//RTC�}��
	RCC_RTCCLKCmd(ENABLE);

	//�}�ҦZ�ݭn����APB1�����ORTC�����P�B�A�~��Ū�g�H�s��

	RTC_WaitForSynchro();

	//Ū�g�H�s���e�A�W�@��RTC�g�ާ@�ާ@����
	RTC_WaitForLastTask();

	//�]�mRTC���W���A��RTC������1Hz
	//RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)

	RTC_SetPrescaler(32767);


	//���ݱH�s���g�J����

	RTC_WaitForLastTask();

	//�ϯ���_

	RTC_ITConfig(RTC_IT_SEC, ENABLE);    

	//���ݼg�J����

	RTC_WaitForLastTask();

	return;
} 

/*******************************************************************************
* Function Name  : RTC_Config
* Description    : �W�q�ɽեΥ���ơA�۰��ˬd�O�_�ݭnRTC��l�ơA 
* 				  �Y�ݭn���s��l��RTC�A�h�ե�RTC_Configuration()���������ާ@
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Config(void)
{
	//�ڭ̦bBKP���Z�ƱH�s��1���A�s�F�@�ӯS��r��0xA5A5
	//�Ĥ@���W�q�ΦZ�ƹq�����q�Z�A�ӱH�s�����u�ᥢ�A
	//���RTC���u�ᥢ�A�ݭn���s�t�m

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//���s�t�mRTC
		RTC_Configuration();
		//�t�m�����Z�A�V�Z�ƱH�s�����g�S��r��0xA5A5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		//�Y�Z�ƱH�s���S�����q�A�h�L�ݭ��s�t�mRTC
		//�o���ڭ̥i�H�Q��RCC_GetFlagStatus()��Ƭd�ݥ����`������
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		//�o�O�W�q�`��
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
		//�o�O�~��RST�޸}�`��
		}
		//�M��RCC���`��Ч�
		RCC_ClearFlag();

		//���MRTC�Ҷ����ݭn���s�t�m�A�B���q�Z�̾a�Z�ƹq���̵M�B��

		RCC_RTCCLKCmd(ENABLE);
		//����RTC�����OAPB1�����P�B
		RTC_WaitForSynchro();

		//�ϯ���_
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//���ݾާ@����

		RTC_WaitForLastTask();
	}
    RCC_ClearFlag();

    /* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	return;
}
