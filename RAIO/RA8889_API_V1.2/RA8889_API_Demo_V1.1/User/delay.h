#ifndef __DELAY_H
#define __DELAY_H 			   
//�ϥ�SysTick�����q�p�ƼҦ��冀��i��޲z
//�]�Adelay_us,delay_ms  

//2008/12/14
//V1.2
//�ץ��F���_���եΥX�{���`�������~
//����ɤ�����,����do while����!
	 
static unsigned char  fac_us=0;//us���ɭ�����
static unsigned short fac_ms=0;//ms���ɭ�����
//��l�Ʃ�����

void delay_init(unsigned char SYSCLK);
void delay_ms(unsigned short nms);
void delay_us(unsigned long Nus);

void delay_seconds(unsigned long ns);
#endif

//
//__asm void delay_T(void);
