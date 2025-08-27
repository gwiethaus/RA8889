
  /*************** RAIO Technology Inc. ***************
  * file		: RA8889_MCU_IF.c
  * author		: RAIO Application Team 
  * version		: V1.0 
  * date		: 2020/03
  * brief		: 	
  ****************************************************/	

#include <stdlib.h>
#include "stm32f10x.h"

#include "UserDef.h"
#include "RA8889_MCU_IF.h"


unsigned int Serial_t = 1;

#ifdef Serial_I2C	 //set gpio open drain and pull up on SCL and SDA

///******************************************************************************
//I2C_Delay()
//******************************************************************************/
void I2C_Delay(void)
{
	unsigned char t;
	t = Serial_t;
	while (t != 0 )		//延時2*t個機器週期
	{
	 t--;
	}
}

void I2C_Initial(void)
{
  Reset_I2CA0;   //set i2c hardware address               
  Reset_I2CA1;                 
  Reset_I2CA2;       
	Reset_I2CA3;    
	Reset_I2CA4;		
	Reset_I2CA5;		

 	Set_I2CSDA;		//GPIO_SetBits(GPIOE, GPIO_Pin_10);//
	I2C_Delay();
	Set_I2CSCL;		//GPIO_SetBits(GPIOE, GPIO_Pin_9);//
	I2C_Delay();
}

///******************************************************************************
//        I2C_Start()
//        產生I2C總線的起始條件
//        SCL處於高電平期間，當SDA出現下降沿時啟動I2C總線
//	本函數也用來產生重複起始條件
//******************************************************************************/
void I2C_Start()
{
	Set_I2CSDA;			//	GPIO_SetBits(GPIOE, GPIO_Pin_9);//
	I2C_Delay();
	Set_I2CSCL;			// 	GPIO_SetBits(GPIOE, GPIO_Pin_10);//
	I2C_Delay();
	Reset_I2CSDA;		//	GPIO_ResetBits(GPIOE, GPIO_Pin_9);//
	I2C_Delay();
	Reset_I2CSCL;			// 	GPIO_ResetBits(GPIOE, GPIO_Pin_10);//
	I2C_Delay();
}

//
///******************************************************************************
//        I2C_Write()
//        向I2C總線寫1個字節的數據
//        dat是要寫到總線上的數據
//******************************************************************************/
void I2C_Write(unsigned char dat)
{
	unsigned char t = 8;

	do
	{
		if ((dat & 0x80)==0x80)
		{
		  Set_I2CSDA;			//
		}
		else
		{
		  Reset_I2CSDA;		//	
		}
		dat <<= 1;
		I2C_Delay();
		Set_I2CSCL;			//
		I2C_Delay();
		Reset_I2CSCL;		//
		I2C_Delay();
		
	} while ( --t != 0 );

 
}




//
///******************************************************************************
//       I2C_Read()
//       從從機讀取1個字節的數據
//       返回：讀取的1個字節數據
//******************************************************************************/
unsigned char I2C_Read()
{
	unsigned char dat=0,temp=0;
	unsigned char t = 8;
	  
	do
	{
		Set_I2CSDA;
		I2C_Delay();
		Set_I2CSCL;				//GPIO_SetBits(GPIOE, GPIO_Pin_10);//	
		I2C_Delay();
		dat <<= 1;
		temp=Read_I2CSDA;		//temp=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9);
		if ( temp==0x01 ) 
		{
		  dat++;
		}
		I2C_Delay();
		Reset_I2CSCL;			//GPIO_ResetBits(GPIOE, GPIO_Pin_10);
		I2C_Delay();
	} while ( --t != 0 );
		
	return dat;
 
}

//
///******************************************************************************
//    I2C_GetAck()
//    讀取從機應答位（應答或非應答），用於判斷：從機是否成功接收主機數據
//    返回：
//	  0：從機應答
//	  1：從機非應答
//   從機在收到每一個字節後都要產生應答位，主機如果收到非應答則應當終止傳輸
//******************************************************************************/
unsigned char I2C_GetAck()
{
	unsigned char ack;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	Set_I2CSDA;
	I2C_Delay();
	Set_I2CSCL;
	I2C_Delay();
	ack=Read_I2CSDA;			//ack = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9);  
	Reset_I2CSCL;             //GPIO_ResetBits(GPIOE, GPIO_Pin_10);//
	I2C_Delay();

	return ack;
}

///******************************************************************************
//       I2C_PutAck()
//      主機產生應答位（應答或非應答），用於通知從機：主機是否成功接收從機數據
//      
//	ack=0：主機應答
//	ack=1：主機非應答
//      主機在收到每一個字節後都要產生應答，在收到最後一個字節後，應當產生非應答
//******************************************************************************/
void I2C_PutAck(unsigned char ack)
{
	if(ack==0x01)
	{
		Set_I2CSDA;
	}
	else
	{
		Reset_I2CSDA;
	}
	I2C_Delay();
	Set_I2CSCL;
	I2C_Delay();

	Reset_I2CSCL;
	I2C_Delay();


}
//
//******************************************************************************
//       I2C_Stop()
//       產生I2C總線的停止條件
//       SCL處於高電平期間，當SDA出現上升沿時停止I2C總線
//*****************************************************************************
void I2C_Stop()
{
	unsigned int t;

	Reset_I2CSDA;//GPIO_ResetBits(GPIOE, GPIO_Pin_9);//	
	I2C_Delay();

	Set_I2CSCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);//
	I2C_Delay();

	Set_I2CSDA;//GPIO_SetBits(GPIOE, GPIO_Pin_9);//	
	I2C_Delay();
	t = 2;
	while ( --t != 0 );
}
 
 #endif



#ifdef Serial_I2C	 //set gpio open drain and pull up on SCL and SDA

//
///******************************************************************************
//I2C																									    
//******************************************************************************/

 void LCD_CmdWrite(unsigned char cmd)
{

	I2C_Start();  //啟動I2C總線
	I2C_Write(0x00); //發送從機地址	 CMDW 0000_0100
	if ( I2C_GetAck() )
	{
	  I2C_Stop();
	}
	                 	
	I2C_Write(cmd);	     //發送命令
	if ( I2C_GetAck() )
	{
	  I2C_Stop();
	}

 	I2C_Stop(); 
  	 					
}

//----------------------//

void LCD_DataWrite(unsigned char Data)
{
	I2C_Start();  //啟動I2C總線
	I2C_Write(0x02); //發送從機地址	 DATW 0000_0110
	I2C_GetAck();

	I2C_Write(Data);	     //發送命令
	if ( I2C_GetAck() )
	{
	  I2C_Stop();
	}
	I2C_Stop();      //發送完畢，停止I2C總線，返回
}

//----------------------//
unsigned char LCD_DataRead(void)
{
	unsigned char dat;

	I2C_Start();         //發送重複起始條件
	I2C_Write(0x03);     //發送從機地址//DATW 0000_0111
   

	I2C_GetAck();
	dat= I2C_Read();

	I2C_PutAck(1);
	I2C_Stop();          //接收完畢，停止I2C總線，返回

	return dat;
}  
//-----------------------//
unsigned char LCD_StatusRead(void)
{
   unsigned char dat_x;

	I2C_Start();       //發送重複起始條件
	I2C_Write(0x01);   //發送從機地址//DATW 0000_0101	 

	if ( I2C_GetAck() )
	{
		I2C_Stop();
	}
	dat_x = I2C_Read();

	I2C_PutAck(0);
	I2C_Stop();        //接收完畢，停止I2C總線，返回

	return dat_x;
	 
}

#endif


#ifdef Serial_4	

void SPI4_Delay(void)
{
  unsigned char t;
  t=Serial_t;
  while(t!=0)
  {
    t--;
  }
}

void SPI4_Init(void)
{
	Set_SPI4_ZCS;   // 	GPIO_SetBits(GPIOE, GPIO_Pin_7);			
	Set_SPI4_SDO;   // 	GPIO_SetBits(GPIOE, GPIO_Pin_8);			
	Set_SPI4_SDA;	// 	GPIO_SetBits(GPIOE, GPIO_Pin_9);		
	Set_SPI4_SCL;   // 	GPIO_SetBits(GPIOE, GPIO_Pin_10);			
}


///******************************************************************************
//SPI_Write()
//******************************************************************************/
void SPI4_Write(unsigned char dat)
{
  unsigned char t = 8;

	Set_SPI4_SDO;		//GPIO_SetBits(GPIOE, GPIO_Pin_8);			
	do
	{
	  if  ((dat & 0x80)==0x80) 
		{
	    Set_SPI4_SDA;//  GPIO_SetBits(GPIOE, GPIO_Pin_9);				 
		}
		else
	  {
		  Reset_SPI4_SDA;//GPIO_ResetBits(GPIOE, GPIO_Pin_9);				
		}
		dat <<= 1;
		Reset_SPI4_SCL;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);			
		SPI4_Delay();
		Set_SPI4_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				
		SPI4_Delay();

  } while ( --t != 0 );
	
	Set_SPI4_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);			
	Set_SPI4_SDA;//GPIO_SetBits(GPIOE, GPIO_Pin_9);		
}

///******************************************************************************
//SPI_Read()
//******************************************************************************/
unsigned char SPI4_Read(void)
{
  unsigned char dat=0;
  unsigned char temp=0;
  unsigned char t = 8;

	do
	{	   
		Reset_SPI4_SCL;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);			
    SPI4_Delay();
		dat <<= 1;
		temp=Read_SPI4_SDO;//temp=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8);
		if ( temp==0x01 ) 
		{
		  dat++;
		}
	  Set_SPI4_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);			
    SPI4_Delay();
	} while ( --t != 0 );
	   
	return dat;
}


void LCD_DataWrite(unsigned char Data)
{
	Reset_SPI4_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);			
	Set_SPI4_SDO;//GPIO_SetBits(GPIOE, GPIO_Pin_8);				 
	Set_SPI4_SDA;//GPIO_SetBits(GPIOE, GPIO_Pin_9);				
	Set_SPI4_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);			
	SPI4_Write(0x80); 
	SPI4_Write(Data);
	SPI4_Delay();
	Set_SPI4_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);			
  SPI4_Delay();
}


unsigned char LCD_DataRead(void)
{
  unsigned char Data;
	GPIO_InitTypeDef GPIO_InitStructure;

 	Set_SPI4_SDO;// 	GPIO_SetBits(GPIOE, GPIO_Pin_8);				
  Set_SPI4_SDA;//	GPIO_SetBits(GPIOE, GPIO_Pin_9);				
 	Set_SPI4_SCL;//  GPIO_SetBits(GPIOE, GPIO_Pin_10);				
	Reset_SPI4_ZCS;//	GPIO_ResetBits(GPIOE, GPIO_Pin_7);				

	SPI4_Write(0xc0);  

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Data = SPI4_Read();

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	SPI4_Delay();
	Set_SPI4_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);		
	return Data;
} 


unsigned char LCD_StatusRead(void)
{
	unsigned char Data;

	GPIO_InitTypeDef GPIO_InitStructure;

	Set_SPI4_SDO;// 	GPIO_SetBits(GPIOE, GPIO_Pin_8);			
 	Set_SPI4_SDA;//	GPIO_SetBits(GPIOE, GPIO_Pin_9);			
	Set_SPI4_SCL;//    GPIO_SetBits(GPIOE, GPIO_Pin_10);			
	Reset_SPI4_ZCS;//	GPIO_ResetBits(GPIOE, GPIO_Pin_7);				
	SPI4_Delay();
	SPI4_Write(0x40); 
		
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Data = SPI4_Read();

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	SPI4_Delay();
	Set_SPI4_ZCS;//	GPIO_ResetBits(GPIOE, GPIO_Pin_7);				
	return Data;
}

void LCD_CmdWrite(unsigned char cmd)
{	
 	Set_SPI4_SDO;// GPIO_SetBits(GPIOE, GPIO_Pin_8);				 
 	Set_SPI4_SDA;//	GPIO_SetBits(GPIOE, GPIO_Pin_9);				
 	Set_SPI4_SCL;// GPIO_SetBits(GPIOE, GPIO_Pin_10);				 
	Reset_SPI4_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);				
	SPI4_Delay();
	SPI4_Write(0x00); 
	SPI4_Write(cmd);
	SPI4_Delay();
	Set_SPI4_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);				 
	SPI4_Delay();
}	
  
#endif


#ifdef Serial_3	
void SPI3_Delay(void)
{
  unsigned int t;
  t=Serial_t;
  while(t!=0)
  {t--;}
}


///******************************************************************************
//SPI_Init()
//******************************************************************************/
void SPI3_Init(void)
{
	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				   
	Set_SPI3_SDO;// GPIO_SetBits(GPIOE, GPIO_Pin_8);				  
	Set_SPI3_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);			    
}
///******************************************************************************
//SPI_Write()
//******************************************************************************/
void SPI3_Write(unsigned char dat)
{
	unsigned char t = 8;

	do
	{
	  if  ((dat & 0x80)==0x80) 
		{
	    Set_SPI3_SDO;// GPIO_SetBits(GPIOE, GPIO_Pin_8);				   
		}
		else
	  {
		  Reset_SPI3_SDO;//GPIO_ResetBits(GPIOE, GPIO_Pin_8);				
		}

		dat <<= 1;
		Reset_SPI3_SCL;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);			  
		Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				   
	}while(--t !=0);
	
	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				   
	Set_SPI3_SDO;// GPIO_SetBits(GPIOE, GPIO_Pin_8);				   
}


///******************************************************************************
//SPI_Read()
//******************************************************************************/
unsigned char SPI3_Read()
{
	unsigned char temp =0;
	unsigned char dat=0;
	unsigned char t = 8;

	do
	{
	  
		Reset_SPI3_SCL;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);	//SCL
		//SPI3_Delay();

		temp=Read_SPI3_SDO;//temp=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8);
		dat <<= 1;
		SPI3_Delay();
		if(temp==0x01)
		{
	    dat++;
	  }
	  Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);	//SCL
		  
	}while(--t !=0 );

	return dat;
}



void LCD_CmdWrite(unsigned char Command)
{	
	unsigned char cmd=Command;
	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				   //SCL
	Set_SPI3_SDO;//GPIO_SetBits(GPIOE, GPIO_Pin_8);				   //SDO
	Reset_SPI3_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);			       //ZCS
	SPI3_Write(0x00); 
	SPI3_Write(cmd);
	Set_SPI3_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);			       //ZCS	.


}


unsigned char LCD_DataRead(void)
{	     
  unsigned int Data;

	GPIO_InitTypeDef GPIO_InitStructure;
	  
	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				  
	Set_SPI3_SDO;//GPIO_SetBits(GPIOE, GPIO_Pin_8);				       
	Reset_SPI3_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);			      
	SPI3_Write(0xc0);

	GPIO_SetBits(GPIOE, GPIO_Pin_8);		
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Data = SPI3_Read();

 	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	Set_SPI3_ZCS;//GPIO_SetBits(GPIOE, GPIO_Pin_7);			    

	return Data;
}



void LCD_DataWrite(unsigned char Data)
{   
  unsigned int writedata;

	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				   
	Set_SPI3_SDO;//GPIO_SetBits(GPIOE, GPIO_Pin_8);				  
	Reset_SPI3_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);			    

	SPI3_Write(0x80); 
	SPI3_Write(Data);
	Set_SPI3_ZCS;//	GPIO_SetBits(GPIOE, GPIO_Pin_7);			 	.


}


unsigned char LCD_StatusRead(void)						
{
	unsigned char Data;
	GPIO_InitTypeDef GPIO_InitStructure;

	Set_SPI3_SCL;//GPIO_SetBits(GPIOE, GPIO_Pin_10);				
	Set_SPI3_SDO;//GPIO_SetBits(GPIOE, GPIO_Pin_8);				 
	Reset_SPI3_ZCS;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);			      
	SPI3_Write(0x40);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  ;
															 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	  
	Data = SPI3_Read();

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Set_SPI3_ZCS;	//GPIO_SetBits(GPIOE, GPIO_Pin_7);			    	.

	return Data;
}


#endif


#ifdef Parallel_6800


void Parallel_6800_ini(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
															GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
															GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 	

	Set_68CS;//GPIO_ResetBits(GPIOD,GPIO_Pin_7);						
	Set_68RS;//GPIO_SetBits(GPIOD,GPIO_Pin_11);							 
	Reset_68RW;//GPIO_ResetBits(GPIOD,GPIO_Pin_5);						
	Reset_68EN;//GPIO_ResetBits(GPIOD,GPIO_Pin_4);	
}


unsigned int LCD_DataRead(void)
{	     
	unsigned int Data=0;
	GPIO_InitTypeDef GPIO_InitStructure;


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |  GPIO_Pin_10 | 
															GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
															GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	Reset_68CS;//GPIO_ResetBits(GPIOD,GPIO_Pin_7);						
	Set_68RS;//GPIO_SetBits(GPIOD,GPIO_Pin_11);								 								   				 
	Set_68RW;//GPIO_SetBits(GPIOD,GPIO_Pin_5);								  							
	Set_68EN;//GPIO_SetBits(GPIOD,GPIO_Pin_4);									
					 
	Data |= Read_D15; Data<<=1;
	Data |=	Read_D14; Data<<=1;
	Data |=	Read_D13; Data<<=1;
	Data |=	Read_D12; Data<<=1;
	Data |=	Read_D11; Data<<=1;
	Data |=	Read_D10; Data<<=1;
	Data |=	Read_D9;  Data<<=1;
	Data |=	Read_D8;  Data<<=1;
	Data |=	Read_D7;  Data<<=1;
	Data |=	Read_D6;  Data<<=1;
	Data |=	Read_D5;  Data<<=1;
	Data |=	Read_D4;  Data<<=1;
	Data |=	Read_D3;  Data<<=1;
	Data |=	Read_D2;  Data<<=1;
	Data |=	Read_D1;  Data<<=1;
	Data |=	Read_D0;  

	Reset_68EN;//GPIO_ResetBits(GPIOD,GPIO_Pin_4);							
	Set_68CS;//GPIO_SetBits(GPIOD,GPIO_Pin_7);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
															GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
															GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
															GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
															GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
							
	return Data;
}
	 
 void LCD_DataWrite(unsigned int Data)
{   	 
	Reset_68CS;//GPIO_ResetBits(GPIOD,GPIO_Pin_7);						
	Set_68RS;//GPIO_SetBits(GPIOD,GPIO_Pin_11);							 
	Reset_68RW;//GPIO_ResetBits(GPIOD,GPIO_Pin_5);											
  	  
	if (Data&0x0001)
	  Set_D0;//GPIO_SetBits(GPIOD, GPIO_Pin_14);     
	else   
	  Reset_D0;//GPIO_ResetBits(GPIOD, GPIO_Pin_14); 
	  	 
  if (Data&0x0002)
    Set_D1;//GPIO_SetBits(GPIOD, GPIO_Pin_15);
  else
    Reset_D1;//GPIO_ResetBits(GPIOD, GPIO_Pin_15);
 
  if (Data&0x0004)
    Set_D2;//GPIO_SetBits(GPIOD, GPIO_Pin_0);
  else
    Reset_D2;//GPIO_ResetBits(GPIOD, GPIO_Pin_0);
 
  if (Data&0x0008)
    Set_D3;//GPIO_SetBits(GPIOD, GPIO_Pin_1);
  else
    Reset_D3;//GPIO_ResetBits(GPIOD, GPIO_Pin_1);
  
  if (Data&0x0010)
    Set_D4;// GPIO_SetBits(GPIOE, GPIO_Pin_7);
  else
    Reset_D4;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);
  
  if (Data&0x0020)
	  Set_D5;//GPIO_SetBits(GPIOE, GPIO_Pin_8);
  else
    Reset_D5;//GPIO_ResetBits(GPIOE, GPIO_Pin_8);
  
  if (Data&0x0040)
    Set_D6;//GPIO_SetBits(GPIOE, GPIO_Pin_9);
  else
    Reset_D6;//GPIO_ResetBits(GPIOE, GPIO_Pin_9);
  
  if (Data&0x0080)
    Set_D7;//GPIO_SetBits(GPIOE, GPIO_Pin_10);
  else
    Reset_D7;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);
  
  if (Data&0x0100)
    Set_D8;//GPIO_SetBits(GPIOE, GPIO_Pin_11);
  else
    Reset_D8;//GPIO_ResetBits(GPIOE, GPIO_Pin_11);
  
  if (Data&0x0200)
    Set_D9;//GPIO_SetBits(GPIOE, GPIO_Pin_12);
  else
    Reset_D9;//GPIO_ResetBits(GPIOE, GPIO_Pin_12);
  
  if (Data&0x0400)
    Set_D10;//GPIO_SetBits(GPIOE, GPIO_Pin_13);
  else
    Reset_D10;//GPIO_ResetBits(GPIOE, GPIO_Pin_13);
  
  if (Data&0x0800)
    Set_D11;//GPIO_SetBits(GPIOE, GPIO_Pin_14);
  else
    Reset_D11;//GPIO_ResetBits(GPIOE, GPIO_Pin_14);
  
  if (Data&0x1000)
    Set_D12;//GPIO_SetBits(GPIOE, GPIO_Pin_15);
  else
    Reset_D12;//GPIO_ResetBits(GPIOE, GPIO_Pin_15);
  
  if (Data&0x2000)
    Set_D13;//GPIO_SetBits(GPIOD, GPIO_Pin_8);
  else
    Reset_D13;//GPIO_ResetBits(GPIOD, GPIO_Pin_8);
  
  if (Data&0x4000)
    Set_D14;//GPIO_SetBits(GPIOD, GPIO_Pin_9);	
  else
    Reset_D14;//GPIO_ResetBits(GPIOD, GPIO_Pin_9);
  
  if (Data&0x8000)
    Set_D15;//GPIO_SetBits(GPIOD, GPIO_Pin_10);
  else
    Reset_D15;//GPIO_ResetBits(GPIOD, GPIO_Pin_10);

  Set_68EN;//GPIO_SetBits(GPIOD,GPIO_Pin_4);						
  Reset_68EN;//GPIO_ResetBits(GPIOD,GPIO_Pin_4);					
  Set_68CS;//GPIO_SetBits(GPIOD,GPIO_Pin_7);					
}



unsigned int LCD_StatusRead(void)
{
  unsigned int Data=0;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
	GPIO_Init(GPIOE, &GPIO_InitStructure);

 	Reset_68CS;	//GPIO_ResetBits(GPIOD,GPIO_Pin_7);						  
 	Reset_68RS;	//GPIO_ResetBits(GPIOD,GPIO_Pin_11);						  
 	Set_68RW;		//GPIO_SetBits(GPIOD,GPIO_Pin_5);						 
 	Reset_68EN;	//GPIO_ResetBits(GPIOD,GPIO_Pin_4);							 
	Set_68EN;	//GPIO_SetBits(GPIOD,GPIO_Pin_4);						 

	Data |=	Read_D7;  Data<<=1;
	Data |=	Read_D6;  Data<<=1;
	Data |=	Read_D5;  Data<<=1;
	Data |=	Read_D4;  Data<<=1;
	Data |=	Read_D3;  Data<<=1;
	Data |=	Read_D2;  Data<<=1;
	Data |=	Read_D1;  Data<<=1;
	Data |=	Read_D0;

 	Reset_68EN;	//GPIO_ResetBits(GPIOD,GPIO_Pin_4);							  
 	Set_68CS;		//GPIO_SetBits(GPIOD,GPIO_Pin_7);							 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
															GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
															GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	return Data;
 } 


void LCD_CmdWrite(unsigned int Conmand)
{			 	  
	Reset_68CS;//GPIO_ResetBits(GPIOD,GPIO_Pin_7);					  
	Reset_68RS;//GPIO_ResetBits(GPIOD,GPIO_Pin_11);				   
 	Reset_68RW;//GPIO_ResetBits(GPIOD,GPIO_Pin_5);					   
						
	if (Conmand&0x0001)
	  Set_D0;//GPIO_SetBits(GPIOD, GPIO_Pin_14);     
	else   
	  Reset_D0;//GPIO_ResetBits(GPIOD, GPIO_Pin_14); 
	 
	if (Conmand&0x0002)
	  Set_D1;//GPIO_SetBits(GPIOD, GPIO_Pin_15);
	else
	  Reset_D1;//GPIO_ResetBits(GPIOD, GPIO_Pin_15);

	if (Conmand&0x0004)
	  Set_D2;//GPIO_SetBits(GPIOD, GPIO_Pin_0);
	else
	  Reset_D2;//GPIO_ResetBits(GPIOD, GPIO_Pin_0);

	if (Conmand&0x0008)
	  Set_D3;//GPIO_SetBits(GPIOD, GPIO_Pin_1);
	else
	  Reset_D3;//GPIO_ResetBits(GPIOD, GPIO_Pin_1);

	if (Conmand&0x0010)
	  Set_D4;// GPIO_SetBits(GPIOE, GPIO_Pin_7);
	else
	  Reset_D4;//GPIO_ResetBits(GPIOE, GPIO_Pin_7);

	if (Conmand&0x0020)
	  Set_D5;//GPIO_SetBits(GPIOE, GPIO_Pin_8);
  else
    Reset_D5;//GPIO_ResetBits(GPIOE, GPIO_Pin_8);
  
  if (Conmand&0x0040)
    Set_D6;//GPIO_SetBits(GPIOE, GPIO_Pin_9);
  else
    Reset_D6;//GPIO_ResetBits(GPIOE, GPIO_Pin_9);
  
  if (Conmand&0x0080)
    Set_D7;//GPIO_SetBits(GPIOE, GPIO_Pin_10);
  else
    Reset_D7;//GPIO_ResetBits(GPIOE, GPIO_Pin_10);
  

	Set_68EN;//GPIO_SetBits(GPIOD,GPIO_Pin_4);	 				
	Reset_68EN;//GPIO_ResetBits(GPIOD,GPIO_Pin_4);				
	Set_68CS;//GPIO_SetBits(GPIOD,GPIO_Pin_7);						
 }
  
#endif  





void GPIO_Configuration_8080(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	//LED3 PB0  //LED4	PB1	//LED5	PB10  //LED6	PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽輸出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11);	 //SET ALL LED ON

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //MCU_RST (TO ra8875)
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

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;    //I2C1_SCL,I2C1_SDA
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽复用
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //PENIRQ, SO	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 //下拉式輸入
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	 //SPII_MISO
  GPIO_Init(GPIOA, &GPIO_InitStructure);

							//PB5 MCU_INT//按鍵2,按鍵1 使用外部PULL HIGH  //PB15 MCU_WAIT	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5; //WP(sd寫保護),CD(sd 偵測) 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //上拉式輸入
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
/*-- GPIO Configuration ------------------------------------------------------*/  

  //PD0(D2),PD1(D3),PD4(RD),PD5(WR),PD8(D13),PD9(D14),PD10(D15),PD14(D0),PD15(D1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽复用

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽复用

  GPIO_Init(GPIOE, &GPIO_InitStructure);
//  GPIO_Init(GPIOE, &GPIO_InitStructure); 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

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
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* RS */					  //RS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
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

  GPIO_ResetBits(GPIOA, GPIO_Pin_8);

  GPIO_ResetBits(GPIOC, GPIO_Pin_7);			
  		
}






void GPIO_Configuration_6800_SP3_SP4_I2C(void)
{


  GPIO_InitTypeDef GPIO_InitStructure;
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	//LED3 PB0  //LED4	PB1	//LED5	PB10  //LED6	PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽輸出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11);	 //SET ALL LED ON


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 //MCU_RST (TO ra8875)
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



  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;    //I2C1_SCL,I2C1_SDA
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽复用
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	
  //PENIRQ, SO	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 //下拉式輸入
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	 //SPII_MISO
  GPIO_Init(GPIOA, &GPIO_InitStructure);



							//PB5 MCU_INT//按鍵2,按鍵1 使用外部PULL HIGH  //PB15 MCU_WAIT	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//輸入懸空
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5; //WP(sd寫保護),CD(sd 偵測) 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //上拉式輸入
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
/*-- GPIO Configuration ------------------------------------------------------*/  

   //PD0(D2),PD1(D3),PD4(RD),PD5(WR),PD8(D13),PD9(D14),PD10(D15),PD14(D0),PD15(D1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(GPIOE, &GPIO_InitStructure);
//  GPIO_Init(GPIOE, &GPIO_InitStructure); 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

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
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* RS */					  //RS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
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

  GPIO_ResetBits(GPIOA, GPIO_Pin_8);

  GPIO_ResetBits(GPIOC, GPIO_Pin_7);			
 
  		
}
