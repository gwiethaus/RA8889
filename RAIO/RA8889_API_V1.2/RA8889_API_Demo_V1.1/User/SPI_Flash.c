#include "stm32f10x.h"
#include "SPI_Flash.h"
#include "stm32f10x_spi.h"
#include "delay.h"


extern void D3_timer3_ini_PWM_enable(void);
extern void D3_timer3_ini_PWM_disable(void);

extern void D4_timer3_ini_PWM_enable(void);
extern void D4_timer3_ini_PWM_disable(void);

/*******************************************************************************
* Function Name  : SPI_Peripheral_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Peripheral_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  /* Enable SPI1 GPIOB clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 ,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);
  /* Configure SPI1 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

    //触摸電路的SPI1 片選
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  //當另外使用SPI TP controller 如ADS7846 時使用
//  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz; //當另外使用SPI TP controller 如ADS7846 時使用
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //當另外使用SPI TP controller 如ADS7846 時使用
//  GPIO_Init(GPIOB, &GPIO_InitStructure);	 //當另外使用SPI TP controller 如ADS7846 時使用

  /* Configure PB.12 as Output push-pull, used as Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  Flash_cs_high(); //nss = 1;
  

  /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);   
  Flash_cs_high();   
//  GPIO_SetBits(GPIOB, GPIO_Pin_7); //禁止触摸電路的片選	//當另外使用SPI TP controller 如ADS7846 時使用

}

/*******************************************************************************
* Function Name  : SPI_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_Peripheral_ReadByte(void)
{
  return (SPI_Peripheral_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Peripheral_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  //NotSelect_Flash();  while(1);
  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}




//write enable
void WREN(void)
{
	Flash_cs_low();
	SPI_Peripheral_SendByte(0x06);
	Flash_cs_high();
}

//write disable
void WRDI(void)
{

	Flash_cs_low();
	SPI_Peripheral_SendByte(0x04); 
	Flash_cs_high();
	
}	

//write ststus register
void WRSR(u8 UpdataValue)
{	
    WREN();
	Flash_cs_low();
	SPI_Peripheral_SendByte(0x01);
	SPI_Peripheral_SendByte(UpdataValue);
	Flash_cs_high();
	
}


//check write in progress bit
//1:busy
//0:normal
u8 WIP(void)
{
u8 temp;
temp = RDSR();

if((temp&0x01)==0x01)
return 1;
else   
return 0;
}

//check write enable latch bit 
//1: write latch
//0: no latch
u8 WEL(void)
{
u8 temp;
temp = RDSR();

if((temp&0x02)==0x02)
return 1;
else 
return 0;	
}


//read status register
u8 RDSR(void)
{
	u8 temp;

	Flash_cs_low();
	SPI_Peripheral_SendByte(0x05);
	temp = SPI_Peripheral_ReadByte();
	Flash_cs_high();
	return(temp);	
}
  

//
 void WRITE_DATA_BYTE(u32 address,u8 UpdataValue)
{
    do{
    WREN(); 
    }
	while(WEL()==0);  	
	Flash_cs_low();
    SPI_Peripheral_SendByte(0x02);
	SPI_Peripheral_SendByte((address&0xffffff)>>16);
	SPI_Peripheral_SendByte((address&0xffff)>>8);
	SPI_Peripheral_SendByte(address&0xff);

	SPI_Peripheral_SendByte(UpdataValue);	
	Flash_cs_high(); 

	do{}
	while(WIP());
} 

 void WRITE_DATA_PAGE(u32 address,u8 *UpdataValue)
{	u16 i ;

    do{
    WREN(); 
    }
	while(WEL()==0); 
	Flash_cs_low();
    SPI_Peripheral_SendByte(0x02);
	SPI_Peripheral_SendByte((address&0xffffff)>>16);
	SPI_Peripheral_SendByte((address&0xffff)>>8);
	SPI_Peripheral_SendByte(address&0xff);
	for(i=0;i<256;i++)
	{
	SPI_Peripheral_SendByte(UpdataValue[i]);
	}	
	Flash_cs_high(); 

	do{}
	while(WIP());
}
     
   
//
u8 READ_DATA_BYTE(u32 address)
{
	u8 temp;

	
	 Flash_cs_low();
     SPI_Peripheral_SendByte(0x03);
	 SPI_Peripheral_SendByte((address&0xffffff)>>16);
	 SPI_Peripheral_SendByte((address&0xffff)>>8);
	 SPI_Peripheral_SendByte(address&0xff);
	 temp=SPI_Peripheral_ReadByte();	
	 Flash_cs_high(); 
	return(temp);
	
}

void CHIP_ERASE(void)
{
 Flash_cs_low();
 SPI_Peripheral_SendByte(0x60);
 Flash_cs_high();
}
//
/*
void RDID(void)
{
	Flash_cs_low();	
  	SPI_Peripheral_SendByte(0x9F);
  	MID= SPI_Peripheral_ReadByte();
	DID= SPI_Peripheral_ReadByte();
  	Flash_cs_high();	
}
*/

/****************************
*FLASH 整片擦除
*返回1 異常
*返回0 成功
*****************************/ 
u8 FLASH_ERASE(void)
{ 
     u8 h;
	 //u8 temp;
	// u32 i;
	 GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off 綠燈

     WREN();

	 if(WEL()==0) //如果WEL=1,write enable失敗
	 {return 1 ;}

	 CHIP_ERASE();//

	 h=0;
	 do{
	   GPIO_SetBits(GPIOB, GPIO_Pin_1);    //LED D4 on 	紅燈
	   delay_ms(500);
	   GPIO_ResetBits(GPIOB, GPIO_Pin_1);    //LED D4 off 紅燈
	   delay_ms(500);
	   h++;
	  }
	 while(WIP()&&(h<100));	 //等待100秒   64Mbit 50秒完成,	128Mbit 80秒完成

	 if(h==100)		 //如100秒還未完成返回1 異常
	 {return 1;}
	 
	 //if needs verify 
	 /* 
	//verify erase 	check all value = 0xff

	  D4_timer3_ini_PWM_enable();//

	  for(i=0;i<FLASH_CHIP;i++)
	  {			 
	    temp=READ_DATA_BYTE(i);	 
      
		if(temp != 0xff)
		{
		 D4_timer3_ini_PWM_disable();
		return 1;}	
		
		//GPIO_SetBits(GPIOB, GPIO_Pin_0);    //LED D3 on 	 
		//GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off	
	  }

    D4_timer3_ini_PWM_disable();
	*/	  	    
   return 0;  //片擦除完成

} 


/****************************
*FLASH 	燒錄
*返回1 異常
*返回0 成功
*****************************/ 
 u8 FLASH_PROGRAM(void)
{

   if(FLASH_ERASE())
    {
	 return 1;
    }
	else
	{
	 //add your code
	}

 	return 0;
 }

