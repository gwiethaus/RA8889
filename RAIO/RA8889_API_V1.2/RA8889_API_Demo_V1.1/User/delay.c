 
#include "stm32f10x.h"
#include "delay.h" 
 void delay_init(unsigned char SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//選擇內部時鐘 HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								
    
//延時Nms
//注意Nms的范圍
//Nms<=0xffffff*8/SYSCLK
//對72M條件下,Nms<=1864
void delay_ms(unsigned short nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//時間加載
	SysTick->VAL =0x00;           //清空計數器
	SysTick->CTRL=0x01 ;          //開始倒數  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待時間到達   
	SysTick->CTRL=0x00;       //關閉計數器
	SysTick->VAL =0X00;       //清空計數器	  	    
}   
//延時us								
   
void delay_us(unsigned long Nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //時間加載	  		 
	SysTick->VAL=0x00;        //清空計數器
	SysTick->CTRL=0x01 ;      //開始倒數 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待時間到達   
	SysTick->CTRL=0x00;       //關閉計數器
	SysTick->VAL =0X00;       //清空計數器	    
}

//延遲N秒
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


