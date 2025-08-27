/****************************************
  * @file		main.c
  * @author		Application Team
  * @version	V1.0
  * @date		2020/03/24
  * @brief  
	* RAIO copyright
****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsmc_sram.h"
#include "stm32f10x_it.h"
//#include "stm32f10x_exti.h"
//#include "misc.h"
#include "stm32f10x_conf.h"
#include "usart.h"
#include "delay.h"

#include "sdcard.h"
#include "integer.h"
#include "ff.h"
#include "diskio.h"
#include "RTC_Time.h"
#include "SPI_Flash.h"

//Include RAiO files
#include "UserDef.h"
#include "RA8889_MCU_IF.h"
#include "RA8889.h"

#include "RA8889_API.h"	
#include "API_Demo.h"
//#include "RA8889_Application_Demo.h"
//END of include RAiO



time_t current_time;
struct tm time_now;

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BlockSize            512 /* Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)
#define NumberOfBlocks       2  /* For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)

//#define Pic_800_480_start  948224

u8 B1_event_flag = 0;//B1 ����Ĳ�o�ƥ�
u8 B2_event_flag = 0;//B2 ����Ĳ�o�ƥ�

SD_CardInfo SDCardInfo;
uint32_t Buffer_Block_Tx[BufferWordsSize], Buffer_Block_Rx[BufferWordsSize];
uint32_t Buffer_MultiBlock_Tx[MultiBufferWordsSize], Buffer_MultiBlock_Rx[MultiBufferWordsSize];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;

//unsigned short buffer[1600];  //16bit for bin low byte first
unsigned char buffer[5760];  //1920x3byte=5760byte

unsigned char buffer_data[2]; 

u8 Brightness_level;
int temp;
//char tmp[20];
int b;
unsigned int i,j;
unsigned int cx,cy;
/* Private functions ---------------------------------------------------------*/
FATFS		fs;            	// Work area (file system object) for logical drive
FIL 		fsrc, fdst;     // file objects
BYTE 		bufferpic[500];
FRESULT 	res;         	// FatFs function common result code
UINT 		br, bw;         // File R/W count
/* Private function prototypes -----------------------------------------------*/


GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;



/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void EXTI_Configuration(void);

void sd_showpic_bin(unsigned short w,unsigned short h,char * fname);

void sd_showpic(unsigned short x,unsigned short y,char * fname);

void detect_button_B1(void);
void detect_button_B2(void);

void D3_timer3_ini_PWM_enable(void);
void D3_timer3_ini_PWM_disable(void);
void D4_timer3_ini_PWM_enable(void);
void D4_timer3_ini_PWM_disable(void);

u8 Program_bin2flash(char * fname);
u8 PageProgram_bin2flash(char * fname);
u8 verify_flash_data(char * fname);


GPIO_InitTypeDef GPIO_InitStructure;

// STM32 Read SDCard, then write to [RA8889+SDRAM]. 
void sd_showpic_bin_1bpp_16bit(unsigned short w,unsigned short h,char * fname);
void sd_showpic_256_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_65k_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_16M_bin(unsigned short w,unsigned short h,char * fname);


int main(void)
{

	/* System Clocks Configuration */
	RCC_Configuration(); 
	delay_init(72);  
	
	GPIO_Configuration();
	NVIC_Configuration();
	EXTI_Configuration();
	SPI_Peripheral_Init();
	USART_Configuration();//set BaudRate 19200


//	/*RTC��l��*///
//	RTC_Config();
//	time_now = Time_GetCalendarTime(); 
//	if (time_now.tm_year<2014)
//	{
//		time_now.tm_year = 2014;
//		time_now.tm_mon = 10;//�]�w�ȥ[1 �]�w12�|�ܦ�0 
//		time_now.tm_mday = 16;
//		time_now.tm_hour = 23;
//		time_now.tm_min = 59;
//		time_now.tm_sec = 55;
//		Time_SetCalendarTime(time_now);
//	} 


	/* Enable the FSMC Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	delay_ms(1);


	while(1)
	{
	
		
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0))
		{ 
			GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off ��O
			GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //LED D4 off ���O 
			printf("\r\n Demo mode\r\n");


			/* Enable the FSMC Clock */
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
				FSMC_LCD_Init_H();

			/* Configure FSMC Bank1 NOR/PSRAM */			
			while(1)	
			{
				RA8889_Initial();

			/*RA8889_02_Function_Demo*/													  			
				API_Demo();

				while(1);


			
			}
		}
	}
}


 /*
  *
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval : None
  */
void RCC_Configuration(void)
{   
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();

  /* Enable Key Button GPIO Port, GPIO_LED and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);


  /* �ϯ�USART1&�ϯ�GPIO port clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
}											

//////////////////////////////////////
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
   #ifdef   VECT_TAB_RAM    //�p�GC/C++ Compiler\Preprocessor\Defined symbols�����w�q�FVECT_TAB_RAM�]���{�Ǯw��鷺�e�����^
		   /*Set the Vector Table base location at 0x20000000*/
           NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); //�bRAM�ո�

   #else                                              //�p�G�S���w�qVECT_TAB_RAM
		   /*Set the Vector Table base location at 0x08000000*/
           NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);//�bFlash�ո�

    #endif                                                                  

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn  ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  
}	


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	//LED3 PB0  //LED4	PB1	//LED5	PB10  //LED6	PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//������X
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11);	 //SET ALL LED ON

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //MCU_RST (TO RA8889)
  GPIO_Init(GPIOE, &GPIO_InitStructure);  	


 //CONFIGURATION SPI 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;    //SPII_CS3
  GPIO_Init(GPIOB, &GPIO_InitStructure);
								//SPII_NSS//SPII_SCK//SPII_MOSI
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7 ;   
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  //CONFIGURATION I2C1 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		 //I2C1Z-RESET
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //test pin
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ; 	 //test pin
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;    //I2C1_SCL,I2C1_SDA
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����`��
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	
  //PENIRQ, SO	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 //�U�Ԧ���J
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	 //SPII_MISO
  GPIO_Init(GPIOA, &GPIO_InitStructure);


							//PB5 MCU_INT//����2,����1 �ϥΥ~��PULL HIGH  //PB15 MCU_WAIT	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��J�a��
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PD6 MCU_WAIT
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��J�a��
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5; //WP(sd�g�O�@),CD(sd ����) 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //�W�Ԧ���J
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
/*-- GPIO Configuration ------------------------------------------------------*/  

    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����`��
	#ifdef Parallel_8080   //used stm32 fsmc
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����`��
	
	#endif
	
	#ifdef Parallel_6800
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	#endif
	
	#ifdef Serial_3
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	#endif
	
	#ifdef Serial_4
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	#endif
	
	#ifdef Serial_I2C
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	#endif

   //PD0(D2),PD1(D3),PD4(RD),PD5(WR),PD8(D13),PD9(D14),PD10(D15),PD14(D0),PD15(D1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

  
  #ifdef Serial_I2C	  //must pull up on SCL and SDA pin when  set GPIO_Mode_Out_OD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  #endif
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 ;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
   

   /* LCD Data(D0-D7) lines configuration */
  
  //GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1 ;
  //GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
  //GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* NOE and NWE configuration */  
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_4;
  //GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE1 configuration */	  //CS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* RS */					  //RS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_Init(GPIOD, &GPIO_InitStructure); 


  
  GPIO_SetBits(GPIOD, GPIO_Pin_7);			//CS=1 
  GPIO_SetBits(GPIOD, GPIO_Pin_14| GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1);  	 
  GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);   
  GPIO_ResetBits(GPIOE, GPIO_Pin_0);		//I2C1_RESET
  GPIO_ResetBits(GPIOE, GPIO_Pin_1);		//RESET=0
  GPIO_SetBits(GPIOD, GPIO_Pin_4);		    //RD=1
  GPIO_SetBits(GPIOD, GPIO_Pin_5);			//WR=1

  GPIO_SetBits(GPIOB, GPIO_Pin_6);			//PEn
			//
  GPIO_SetBits(GPIOB, GPIO_Pin_8);		//I2C1_SCL
  GPIO_SetBits(GPIOB, GPIO_Pin_9);		//I2C1_SDA

  //GPIO_SetBits(GPIOD, GPIO_Pin_13);			
  GPIO_SetBits(GPIOB, GPIO_Pin_7);			//SPII_CS3

  GPIO_SetBits(GPIOA, GPIO_Pin_4);			//SPI NSS

  GPIO_ResetBits(GPIOC, GPIO_Pin_1);		//test pin
 
  GPIO_SetBits(GPIOC, GPIO_Pin_3);		   //test pin


}


void EXTI_Configuration(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;


	    /*connect PB5 to EXTI*/
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
//    /*Config EXTI8*/
//    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);

    /* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

   

    //EXTI_GenerateSWInterrupt(EXTI_Line8);
} 


/*
void sd_showpic_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp;
	FRESULT res;
	
	f_mount(0, &fs);   //���U�@�Ӥu�@�� 

    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}��� 
	
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*2,&br);	//�C��Ū���@�� 
		 for(i=0;i<w;i++) 	//�C���g�@�� 
		 {
		  //data_temp = buffer[i*2];
		  //data_temp =((data_temp <<8) | buffer[i*2+1]);
		  data_temp =((buffer[i*2+1]<<8)|buffer[i*2]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd
		 // Check_RA8889_WAIT();
		 Check_Mem_WR_FIFO_not_Full();
		 }

		 if (res || br == 0) break;   // error or eof	   	  
      }

	    f_close(&fsrc);
		//f_close(&file);
}
*/
/*
void sd_showpic_bin_1bpp_16bit(unsigned short w,unsigned short h,char * fname)
{ 
  unsigned short i,j;
  FRESULT res;

  f_mount(0, &fs);   //���U�@�Ӥu�@�� 

  res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}��� 
	
  //Main_Image_Start_Address(0);				
  //Main_Image_Width(1024*2);							
  //Main_Window_Start_XY(0,0);
	
  //Canvas_Image_Start_address(0);
  //Canvas_image_width(1024*2);

  //Active_Window_XY(0,0);
  //Active_Window_WH(w,h);

  //RGB_16b_8bpp();
  //RGB_16b_16bpp();
  //RGB_16b_24bpp_mode1();
  //RGB_16b_24bpp_mode2();

  //Memory_8bpp_Mode();
  //Memory_16bpp_Mode();
  //Memory_24bpp_Mode();
	
  //Select_Main_Window_8bpp();
  //Select_Main_Window_16bpp();
  //Select_Main_Window_24bpp();
  
  //Graphic_Mode();
  //Goto_Pixel_XY(0,0);
  LCD_CmdWrite(0x04);
  	   
  for(j=0;j< h;j++)
  {
    res = f_read(&fsrc, buffer, w,&br);	//�C��Ū���@�� 
	for(i=0;i<w;i=i+2) 	//�C���g�@�� 
	{
	  LCD_DataWrite(buffer[i]<<8 | buffer[i+1]);	
//	  Check_RA8889_WAIT();
	  Check_Mem_WR_FIFO_not_Full();
	}

	if (res || br == 0) break;   // error or eof	   	  
  }
  Check_Mem_WR_FIFO_Empty();
  f_close(&fsrc);

}
*/
/*
void sd_showpic_256_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);   //���U�@�Ӥu�@��
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}���
	   
	for(j=0;j<h;j++)
	{
		res = f_read(&fsrc, buffer, w,&br);	//�C��Ū���@��
		
		for(i=0;i<w;i++) 	//�C���g�@��
		{
			LCD_DataWrite(buffer[i]);	//write 8 bits 1 pixel data to SDRAM  
			//delay_ms(1);
			//Check_RA8889_WAIT();
			Check_Mem_WR_FIFO_not_Full();
		}
		if (res || br == 0) break;   // error or eof	   	  
	}

	f_close(&fsrc);
	//f_close(&file);


Check_Mem_WR_FIFO_Empty();	
	
}
*/

/*
void sd_showpic_65k_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp;
	FRESULT res;
	
	f_mount(0, &fs);   //���U�@�Ӥu�@��
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}���
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*2,&br);	//�C��Ū���@��
		 
		 for(i=0;i<w;i++) 	//�C���g�@��
		 {
			Check_Mem_WR_FIFO_not_Full();
			data_temp =((buffer[i*2+1]<<8)|buffer[i*2]);
			LCD_DataWrite(data_temp);	//write 16 bits 1 pixel data to SDRAM 
			//Check_RA8889_WAIT();
		  	Check_Mem_WR_FIFO_not_Full();
		 }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
		

Check_Mem_WR_FIFO_Empty();


}

*/
/*
void sd_showpic_16M_bin(unsigned short w,unsigned short h,char * fname)
{ 
	unsigned short i,j,data_temp1,data_temp2,data_temp3;
	FRESULT res;
	
	f_mount(0, &fs);   //���U�@�Ӥu�@��
	
	res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}���
	
	for(j=0;j< h;j++)
	{
		res = f_read(&fsrc, buffer, w*3,&br);	//�C��Ū���@��
		
		for(i=0;i<(w/2);i++) 	//�C���g�@��
		{
			data_temp1 =((buffer[i*6+1]<<8)|buffer[i*6]);
			data_temp2 =((buffer[i*6+3]<<8)|buffer[i*6+2]);		  
			data_temp3 =((buffer[i*6+5]<<8)|buffer[i*6+4]);
			
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp1);	//write 24 bits 2 pixel data to SDRAM 
			//Check_RA8889_WAIT();
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp2);	//write 24 bits 2 pixel data to SDRAM 
			//Check_RA8889_WAIT();
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp3);	//write 24 bits 2 pixel data to SDRAM 
			//Check_RA8889_WAIT();
			//Check_Mem_WR_FIFO_not_Full();
		}
		
		if (res || br == 0) break;   // error or eof	   	  
	}

	f_close(&fsrc);
	//f_close(&file);
	

Check_2D_Busy();
}
*/

/*****************************************************
* ��ܤ��t���Y�ɪ�BIN��	 bin file low byte first
******************************************************/
/*
void sd_showpic_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);   //���U�@�Ӥu�@��
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//���}���
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*2,&br);	//�C��Ū���@��
		 
		 for(i=0;i< w;i++) 			//�C���g�@��
		 {LCD_DataWrite(buffer[i]);}

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
}
*/
/*****************************************************
* ��ܤ��t���Y�ɪ�BIN��	 //�Ĳv���C	bin file low byte first
******************************************************/
/*
void sd_showpic_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);

	   
	   while(1)
	   {
	     res = f_read(&fsrc, buffer, 2,&br);
		 if (res || br == 0) break;   // error or eof
		 
		 LCD_DataWrite(buffer[0]);
		} 
	   	  
        

	    f_close(&fsrc);
		//f_close(&file);
}
 */

/*****************************************************
* ��ܧt���Y�ɪ�BIN��
******************************************************/
/*
void sd_showpic(unsigned short x,unsigned short y,char * fname)
{ 
    unsigned short i,j,fwh[2];
	FRESULT res;
	
	f_mount(0, &fs);
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);

	res = f_read(&fsrc, fwh, sizeof(fwh), &br);

    br=1;

	XY_Coordinate(x,y);
	Active_Window(x,(x+fwh[0]-1),y,(y+fwh[1]-1)); //�]�w�ʧ@�����_�l�P����

	LCD_CmdWrite(0x02); //

	for (j=0;j<fwh[1];j++) {
		res = f_read(&fsrc, buffer, fwh[0]*2,&br);
		for(i=0;i<fwh[0];i++) 
		 {LCD_DataWrite(buffer[i]);}

	    if (res || br == 0) break;   // error or eof	  
         }

	    f_close(&fsrc);
		//f_close(&file);
}
*/



void detect_button_B1(void)
{
 
  if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))//B1 ����
   {
    delay_ms(1);
	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))//B1 ����
	 B1_event_flag = 1;
   }
 
}

void detect_button_B2(void)
{
 
  if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12))//B2 ����
   {
    delay_ms(1);
	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12))//B2 ����
	B2_event_flag = ~B2_event_flag;
	 //B2_event_flag = 1;
   }
 
}


  uint16_t CCR3_Val = 500;   

void D3_timer3_ini_PWM_enable(void)
{

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  

  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //�]�wTimer�����w��
  TIM_DeInit(TIM3);
  //�ĥΤ����ɶ����ѵ�TIM3
  TIM_InternalClockConfig(TIM3);

  TIM_TimeBaseStructure.TIM_Prescaler = 0x1c1f;        //100us �����w���W��  �����W�v = 72/(�����w���W+1) 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //�Ҧ�  �V�W�p��
  TIM_TimeBaseStructure.TIM_Period = 999;       //�۰ʭ��˸��H�s���g��	  PWM ��i�g��

  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //�ɶ����έ�
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  
  TIM_OCStructInit(&TIM_OCInitStructure);
 //PWM�t�m �t�m�e�Ť�M�q�D  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //PWM�Ҧ�
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��X�l��
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;  //�m�e�Ť� �e�Ť�=CCR3_Val/(TIM_Period +1)=50%

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�����X����
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //�t�m�q�D3

  TIM_Cmd(TIM3, ENABLE);     //���}�w����3
  TIM_CtrlPWMOutputs(TIM3,ENABLE);
}

void D3_timer3_ini_PWM_disable(void)
{
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,DISABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//������X
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  TIM_Cmd(TIM3, DISABLE); 
  TIM_CtrlPWMOutputs(TIM3,DISABLE);
}

void D4_timer3_ini_PWM_enable(void)
{

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  

  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //�]�wTimer�����w��
  TIM_DeInit(TIM3);
  //�ĥΤ����ɶ����ѵ�TIM3
  TIM_InternalClockConfig(TIM3);

  TIM_TimeBaseStructure.TIM_Prescaler = 0x1c1f;        //100us �����w���W��  �����W�v = 72/(�����w���W+1) 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //�Ҧ�  �V�W�p��
  TIM_TimeBaseStructure.TIM_Period = 999;       //�۰ʭ��˸��H�s���g��	  PWM ��i�g��

  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //�ɶ����έ�
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  
  TIM_OCStructInit(&TIM_OCInitStructure);
 //PWM�t�m �t�m�e�Ť�M�q�D  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //PWM�Ҧ�
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��X�l��
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;  //�m�e�Ť� �e�Ť�=CCR3_Val/(TIM_Period +1)=50%

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�����X����
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //�t�m�q�D4

  TIM_Cmd(TIM3, ENABLE);     //���}�w����3
  TIM_CtrlPWMOutputs(TIM3,ENABLE);
}

void D4_timer3_ini_PWM_disable(void)
{
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,DISABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//������X
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  TIM_Cmd(TIM3, DISABLE); 
  TIM_CtrlPWMOutputs(TIM3,DISABLE);
}


u8 Program_bin2flash(char * fname)
{
	u32 i;
    FIL file;
	FRESULT res;
	
	D3_timer3_ini_PWM_enable();	

	f_mount(0, &fs);

    res = f_open(&file, fname, FA_OPEN_EXISTING | FA_READ);
	   
 		i=0;

		while(1)
		 {
		  res = f_read(&file,buffer_data,1,&br);	//�C��Ū1 byte
		  if (res || br == 0) 
		  {	
		  break;   // error or eof
    	  }	 
		 WRITE_DATA_BYTE(i,buffer_data[0]);//write to serial flash
		 i++;  		   	  
    	 }

	 f_close(&file);

  return 0;
}

u8 PageProgram_bin2flash(char * fname)
{
	u32 i;
    FIL file;
	FRESULT res;
	
	D3_timer3_ini_PWM_enable();	

	f_mount(0, &fs);

    res = f_open(&file, fname, FA_OPEN_EXISTING | FA_READ);
	
  
 		i=0;

		while(1)
		 {
		  res = f_read(&file,buffer,256,&br);	//�C��Ū256 byte
		 
		  WRITE_DATA_PAGE(i,buffer);//write to serial flash

		  i+=256;  

		  if (res || br == 0) 
		  {	
		  break;   // error or eof
    	  }	
		   		   	  
    	 }

	  f_close(&file);

      return 0;
}


u8 verify_flash_data(char * fname)
{
 	u32 i;
	u8 temp;

	FIL file;
	FRESULT res;
	
	f_mount(0, &fs);

    res = f_open(&file, fname, FA_OPEN_EXISTING | FA_READ);
	   
 		i=0;
		
		while(1)
		 {
	      res = f_read(&file,buffer_data,1,&br);	//�C��Ū1 byte
		  if (res || br == 0) 
		   {
		   D3_timer3_ini_PWM_disable();
		   break;   // error or eof	
		   } 

		  temp = READ_DATA_BYTE(i); //read from serial flash
		  i++; 

		   //compare 
		   if(temp!=buffer_data[0]) 
		   {
		   	D3_timer3_ini_PWM_disable();
		   return 1;} 	   	  		    	  
    	 }
		
	 f_close(&file);

	 D3_timer3_ini_PWM_disable();

	return 0;
}



/**
   * @brief  Fill the global buffer
  * @param pBuffer: pointer on the Buffer to fill
  * @param BufferSize: size of the buffer to fill
  * @param Offset: first value to fill on the Buffer
  */

 

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


								

/******************* (C) COPYRIGHT 2009  *****END OF FILE****/
