#ifndef __DELAY_H
#define __DELAY_H 			   
//使用SysTick的普通計數模式對延遲進行管理
//包括delay_us,delay_ms  

//2008/12/14
//V1.2
//修正了中斷中調用出現死循環的錯誤
//防止延時不准确,采用do while結构!
	 
static unsigned char  fac_us=0;//us延時倍乘數
static unsigned short fac_ms=0;//ms延時倍乘數
//初始化延遲函數

void delay_init(unsigned char SYSCLK);
void delay_ms(unsigned short nms);
void delay_us(unsigned long Nus);

void delay_seconds(unsigned long ns);
#endif

//
//__asm void delay_T(void);
