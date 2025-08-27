
  /*************** RAIO Technology Inc. ***************
  * file		: RA8889_MCU_IF.h
  * author		: RAIO Application Team 
  * version		: V1.0 
  * date		: 2014/03/17 
  * brief		: 	
  ****************************************************/	


#ifndef __RA8889_MCU_IF_H
#define __RA8889_MCU_IF_H



#define Set_D0    GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define Set_D1    GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define Set_D2    GPIO_SetBits(GPIOD, GPIO_Pin_0)
#define Set_D3    GPIO_SetBits(GPIOD, GPIO_Pin_1)
#define Set_D4    GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define Set_D5    GPIO_SetBits(GPIOE, GPIO_Pin_8)
#define Set_D6    GPIO_SetBits(GPIOE, GPIO_Pin_9)
#define Set_D7    GPIO_SetBits(GPIOE, GPIO_Pin_10)
#define Set_D8    GPIO_SetBits(GPIOE, GPIO_Pin_11)
#define Set_D9    GPIO_SetBits(GPIOE, GPIO_Pin_12)
#define Set_D10   GPIO_SetBits(GPIOE, GPIO_Pin_13)
#define Set_D11   GPIO_SetBits(GPIOE, GPIO_Pin_14)
#define Set_D12   GPIO_SetBits(GPIOE, GPIO_Pin_15)
#define Set_D13   GPIO_SetBits(GPIOD, GPIO_Pin_8)
#define Set_D14   GPIO_SetBits(GPIOD, GPIO_Pin_9)
#define Set_D15   GPIO_SetBits(GPIOD, GPIO_Pin_10)



#define Reset_D0    GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define Reset_D1    GPIO_ResetBits(GPIOD, GPIO_Pin_15)
#define Reset_D2    GPIO_ResetBits(GPIOD, GPIO_Pin_0)
#define Reset_D3    GPIO_ResetBits(GPIOD, GPIO_Pin_1)
#define Reset_D4    GPIO_ResetBits(GPIOE, GPIO_Pin_7)
#define Reset_D5    GPIO_ResetBits(GPIOE, GPIO_Pin_8)
#define Reset_D6    GPIO_ResetBits(GPIOE, GPIO_Pin_9)
#define Reset_D7    GPIO_ResetBits(GPIOE, GPIO_Pin_10)
#define Reset_D8    GPIO_ResetBits(GPIOE, GPIO_Pin_11)
#define Reset_D9    GPIO_ResetBits(GPIOE, GPIO_Pin_12)
#define Reset_D10   GPIO_ResetBits(GPIOE, GPIO_Pin_13)
#define Reset_D11   GPIO_ResetBits(GPIOE, GPIO_Pin_14)
#define Reset_D12   GPIO_ResetBits(GPIOE, GPIO_Pin_15)
#define Reset_D13   GPIO_ResetBits(GPIOD, GPIO_Pin_8)
#define Reset_D14   GPIO_ResetBits(GPIOD, GPIO_Pin_9)
#define Reset_D15   GPIO_ResetBits(GPIOD, GPIO_Pin_10)



#define Read_D0		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)
#define Read_D1		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)
#define Read_D2		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)
#define Read_D3		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)
#define Read_D4		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)
#define Read_D5		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
#define Read_D6		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)
#define Read_D7		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)
#define Read_D8		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define Read_D9		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)
#define Read_D10	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)
#define Read_D11	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)
#define Read_D12	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)
#define Read_D13	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)
#define Read_D14	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9)
#define Read_D15	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)


//8080
#ifdef Parallel_8080 
	#define LCD_BASE0        ((u32)0x60000000)//RA8889
	#define LCD_BASE1        ((u32)0x60020000)//RA8889
	
	#define LCD_CmdWrite(cmd)	  *(vu16*) (LCD_BASE0)= (cmd);
	#define LCD_DataWrite(data)   *(vu16*) (LCD_BASE1)= (data);
	#define	LCD_StatusRead()	 *(vu16*) (LCD_BASE0) //if use read  Mcu interface DB0~DB15 needs increase pull high 
	#define	LCD_DataRead()   	 *(vu16*) (LCD_BASE1) //if use read  Mcu interface DB0~DB15 needs increase pull high 
#endif

//6800
#ifdef Parallel_6800
	#define Set_68RS  GPIO_SetBits(GPIOD,GPIO_Pin_11)
	#define	Set_68RW  GPIO_SetBits(GPIOD,GPIO_Pin_5)
	#define	Set_68EN  GPIO_SetBits(GPIOD,GPIO_Pin_4)
	#define Set_68CS  GPIO_SetBits(GPIOD,GPIO_Pin_7)
	#define Reset_68RS	GPIO_ResetBits(GPIOD,GPIO_Pin_11)
	#define	Reset_68RW	GPIO_ResetBits(GPIOD,GPIO_Pin_5)
	#define	Reset_68EN	GPIO_ResetBits(GPIOD,GPIO_Pin_4)
	#define Reset_68CS	GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#endif

//SPI3
#ifdef Serial_3
	#define Set_SPI3_ZCS      GPIO_SetBits(GPIOE, GPIO_Pin_7)
	#define Set_SPI3_SDO   	  GPIO_SetBits(GPIOE, GPIO_Pin_8)
	#define Set_SPI3_SCL      GPIO_SetBits(GPIOE, GPIO_Pin_10)
	#define Reset_SPI3_ZCS    GPIO_ResetBits(GPIOE, GPIO_Pin_7)
	#define Reset_SPI3_SDO    GPIO_ResetBits(GPIOE, GPIO_Pin_8)
	#define Reset_SPI3_SCL    GPIO_ResetBits(GPIOE, GPIO_Pin_10)
	#define Read_SPI3_SDO	  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
#endif


//SPI4
#ifdef Serial_4
	#define Set_SPI4_ZCS 	    GPIO_SetBits(GPIOE, GPIO_Pin_7)
	#define Set_SPI4_SDO   	    GPIO_SetBits(GPIOE, GPIO_Pin_8)
	#define Set_SPI4_SDA	    GPIO_SetBits(GPIOE, GPIO_Pin_9)
	#define Set_SPI4_SCL   	    GPIO_SetBits(GPIOE, GPIO_Pin_10)
	#define Reset_SPI4_ZCS      GPIO_ResetBits(GPIOE, GPIO_Pin_7)
	#define Reset_SPI4_SDO      GPIO_ResetBits(GPIOE, GPIO_Pin_8)
	#define Reset_SPI4_SDA	    GPIO_ResetBits(GPIOE, GPIO_Pin_9)
	#define Reset_SPI4_SCL      GPIO_ResetBits(GPIOE, GPIO_Pin_10)
	#define Read_SPI4_SDO	    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)
	#define Read_SPI4_SDA	    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)
#endif

//I2C
#ifdef Serial_I2C
	#define Set_I2CA0	    GPIO_SetBits(GPIOD, GPIO_Pin_14)	//I2C I/F: IIC Slave Address A0.
	#define Set_I2CA1	    GPIO_SetBits(GPIOD, GPIO_Pin_15)	//I2C I/F: IIC Slave Address A1.
	#define Set_I2CA2	    GPIO_SetBits(GPIOD, GPIO_Pin_0)		//I2C I/F: IIC Slave Address A2.
	#define Set_I2CA3	    GPIO_SetBits(GPIOD, GPIO_Pin_1)		//I2C I/F: IIC Slave Address A3.
	#define Set_I2CA4	    GPIO_SetBits(GPIOE, GPIO_Pin_7)		//I2C I/F: IIC Slave Address A4.
	#define Set_I2CA5	    GPIO_SetBits(GPIOE, GPIO_Pin_8)		//I2C I/F: IIC Slave Address A5.
	#define Set_I2CSDA 	 	   GPIO_SetBits(GPIOE, GPIO_Pin_9)
	#define Set_I2CSCL	 	   GPIO_SetBits(GPIOE, GPIO_Pin_10)
	#define Reset_I2CA0	    GPIO_ResetBits(GPIOD, GPIO_Pin_14)	//I2C I/F: IIC Slave Address A0.
	#define Reset_I2CA1	    GPIO_ResetBits(GPIOD, GPIO_Pin_15)	//I2C I/F: IIC Slave Address A1.
	#define Reset_I2CA2	    GPIO_ResetBits(GPIOD, GPIO_Pin_0)	//I2C I/F: IIC Slave Address A2.
	#define Reset_I2CA3	    GPIO_ResetBits(GPIOD, GPIO_Pin_1)	//I2C I/F: IIC Slave Address A3.
	#define Reset_I2CA4	    GPIO_ResetBits(GPIOE, GPIO_Pin_7)	//I2C I/F: IIC Slave Address A4.
	#define Reset_I2CA5	    GPIO_ResetBits(GPIOE, GPIO_Pin_8)	//I2C I/F: IIC Slave Address A5.
	#define Reset_I2CSDA 	    GPIO_ResetBits(GPIOE, GPIO_Pin_9)
	#define Reset_I2CSCL	    GPIO_ResetBits(GPIOE, GPIO_Pin_10)
	#define Read_I2CSDA  	    GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)

#endif  
 
 



//6800  
#ifdef Parallel_6800
	void Parallel_6800_ini(void);
	void LCD_CmdWrite(unsigned int Command);
	unsigned int LCD_StatusRead(void);
	void LCD_DataWrite(unsigned int Data);
	unsigned int LCD_DataRead(void);
#endif

//Serial_3
#ifdef Serial_3
	void LCD_CmdWrite(unsigned char Command);
	unsigned char LCD_StatusRead(void);
	void LCD_DataWrite(unsigned char Data);
	unsigned char LCD_DataRead(void);
	void SPI3_Delay(void);
	void SPI3_Init(void);
	void SPI3_Write(unsigned char dat);
	unsigned char SPI3_Read();
#endif

//Serial_4   
#ifdef Serial_4
	void SPI4_Delay(void);
	void SPI4_Init(void);
	void SPI4_Write(unsigned char dat);
	unsigned char SPI4_Read(void);
	void LCD_CmdWrite(unsigned char Command);
	unsigned char LCD_StatusRead(void);
	void LCD_DataWrite(unsigned char Data);
	unsigned char LCD_DataRead(void);
#endif


// I2C  
#ifdef Serial_I2C
	void I2C_Stop(void);
	void I2C_PutAck(unsigned char ack);
	unsigned char I2C_GetAck(void);
	unsigned char I2C_Read(void);
	void I2C_Write(unsigned char dat);
	void I2C_Start(void);
	void I2C_Initial(void);
	void I2C_Delay(void);
	void LCD_CmdWrite(unsigned char Command);
	unsigned char LCD_StatusRead(void);
	void LCD_DataWrite(unsigned char Data);
	unsigned char LCD_DataRead(void);
#endif
   
void GPIO_Configuration_8080(void);
void GPIO_Configuration_6800_SP3_SP4_I2C(void);




#endif /*__RA8889_MCU_IF_H*/
