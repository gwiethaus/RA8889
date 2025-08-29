#include <intrins.h>
#include "delay.h"

//****************************************************
// ms延时函数(根据自己的系统频率修正)
//****************************************************
void delay_ms(unsigned int n)
{
    unsigned short i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 1500; j++);
}

//通信延时函数
void delay(unsigned short n)
{
	while(n--);
}
