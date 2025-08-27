 
#include "stm32f10x.h"
#include "delay.h" 
 void delay_init(unsigned char SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//��ܤ������� HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								
    
//����Nms
//�`�NNms���S��
//Nms<=0xffffff*8/SYSCLK
//��72M����U,Nms<=1864
void delay_ms(unsigned short nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//�ɶ��[��
	SysTick->VAL =0x00;           //�M�ŭp�ƾ�
	SysTick->CTRL=0x01 ;          //�}�l�˼�  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//���ݮɶ���F   
	SysTick->CTRL=0x00;       //�����p�ƾ�
	SysTick->VAL =0X00;       //�M�ŭp�ƾ�	  	    
}   
//����us								
   
void delay_us(unsigned long Nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //�ɶ��[��	  		 
	SysTick->VAL=0x00;        //�M�ŭp�ƾ�
	SysTick->CTRL=0x01 ;      //�}�l�˼� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//���ݮɶ���F   
	SysTick->CTRL=0x00;       //�����p�ƾ�
	SysTick->VAL =0X00;       //�M�ŭp�ƾ�	    
}

//����N��
void delay_seconds(unsigned long ns)
{
  do{
     delay_ms(999);
    }while(ns--);
   
}

//__asm void delay_T(void)
//{
//NOP
//}


