/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

//#include <stm32f10x_lib.h>              /* STM32F10x Library Definitions      */
#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_conf.h"
#include <stdio.h>
#include "usart.h"

void USART_Configuration(void)  {

  USART_InitTypeDef USART_InitStructure;
  
  GPIO_InitTypeDef  GPIO_InitStructure;

   /* Enable USART1 and GPIOA clock                                            */
  /*RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);*/
 

  /* Configure USART1 Rx (PA10) as input floating                             */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART1 Tx (PA9) as alternate function push-pull                */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
/* USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  USART_Init(USART1, &USART_InitStructure);
    

  USART_Cmd(USART1, ENABLE);  */

  /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - Two Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
   USART_InitStructure.USART_BaudRate            = 19200;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No ;//USART_Parity_Odd
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
 

 
  USART_Init(USART1, &USART_InitStructure);
 //	if you want to set the parity bit ,you must select 9bit data
 
  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);

/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
   USART1 transmit data register is empty */  
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

/* Enable the USART Receive interrupt: this interrupt is generated when the 
   USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);

}




/* Implementation of putchar (also used by printf function to output data)    */
int SendChar (int ch)  {                /* Write character to Serial Port     */

  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
  return (ch);
}


int GetKey (void)  {                    /* Read character from Serial Port    */

  while (!(USART1->SR & USART_FLAG_RXNE));
  return (USART_ReceiveData(USART1));
}



/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }

  return ch;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
