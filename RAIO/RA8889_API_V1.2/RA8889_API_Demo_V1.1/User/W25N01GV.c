
//**************************************************************//
/*
File Name : W25N01GV.c                                   
Author    : RAiO Application Team                             
Edit Date : 02/05/2016
Version   : v1.0
Note: 

*/
//**************************************************************//
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "UserDef.h"

#include "RA8889.h"
#include "RA8889_MCU_IF.h"

#include "W25N01GV.h"
#include "delay.h"


unsigned char LUT_list[100];
unsigned char Buffer_Page[2048];

//**************************************************************//
//write enable
//**************************************************************//
void W25N01_Write_Enable(void)
{
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x06);
  nSS_Inactive();
}
//**************************************************************//
//write disable
//**************************************************************//
void W25N01_Write_Disable(void)
{
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x04);
  nSS_Inactive();	
}

//**************************************************************//
//check write enable latch bit   //1: write latch  //0: no latch
//**************************************************************//
bool W25N01_Check_WEL(void)
{
  unsigned char temp;
 
  temp = W25N01_Read_Status_Register3();
 
  if((temp&0x02)==0x02)
  return TRUE;
  else 
  return FALSE;	
}
//**************************************************************//
//**************************************************************//
void W25N01_Device_Reset(void)
{
  //delay for power on
  delay_us(500);
  
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xFF); 
  nSS_Inactive();
  
  //delay for reset
  delay_us(500);
}
//**************************************************************//
//**************************************************************//
void W25N01_Read_JEDEC_ID(unsigned char *manufacturer_id,unsigned short *device_id)
{
  unsigned short temp;

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x9F); 
  SPI_Master_FIFO_Data_Put(0xFF);

  *manufacturer_id = SPI_Master_FIFO_Data_Put(0xff);
  temp=SPI_Master_FIFO_Data_Put(0xff);
  temp<<=8;
  temp&=0xff00;
  temp|=SPI_Master_FIFO_Data_Put(0xff);
  nSS_Inactive();

  *device_id = temp;

}
//**************************************************************//
//**************************************************************//
unsigned char W25N01_Read_Status_Register1(void)
{
  unsigned char temp;
  
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);
  SPI_Master_FIFO_Data_Put(0xA0);
  delay_us(1);
  temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value 
  nSS_Inactive();
	
  return(temp);
}

//**************************************************************//
//**************************************************************//
unsigned char W25N01_Read_Status_Register2(void)
{
  unsigned char temp;

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);
  SPI_Master_FIFO_Data_Put(0xB0);
  delay_us(1);
  temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value 
  nSS_Inactive();
	
  return(temp);
}
//**************************************************************//
//**************************************************************//
unsigned char W25N01_Read_Status_Register3(void)
{
  unsigned char temp;

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x0F);
  SPI_Master_FIFO_Data_Put(0xC0);
  delay_us(1);
  temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value 
  nSS_Inactive();
	
  return(temp);
}

//**************************************************************//
//**************************************************************//
void W25N01_Write_Status_Register1(unsigned char data)
{
  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x1F);
  SPI_Master_FIFO_Data_Put(0xA0);
  SPI_Master_FIFO_Data_Put(data); 
  nSS_Inactive();
	
}
//**************************************************************//
//**************************************************************//
void W25N01_Write_Status_Register2(unsigned char data)
{

  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x1F);
  SPI_Master_FIFO_Data_Put(0xB0);
  SPI_Master_FIFO_Data_Put(data); 
  nSS_Inactive();
	
}
//**************************************************************//
//**************************************************************//
void W25N01_Write_Status_Register3(unsigned char data)
{

  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x1F);
  SPI_Master_FIFO_Data_Put(0xC0);
  SPI_Master_FIFO_Data_Put(data); 
  nSS_Inactive();
	
}

//**************************************************************//
//return 1 = BUSY
//**************************************************************//
bool W25N01_Check_Flash_Busy(void)
{ 
  unsigned char temp;
  temp=W25N01_Read_Status_Register3();
  temp&=0x01;

  if(temp==0x01)
  return TRUE;
  else
  return FALSE; 
}
//**************************************************************//
//**************************************************************//
void W25N01_Clear_BUF(void)
{ 
  unsigned char temp;

  temp=W25N01_Read_Status_Register2();
  temp&=0xf7;
  W25N01_Write_Status_Register2(temp);
  delay_us(500);
}
//**************************************************************//
//**************************************************************//
void W25N01_Set_ECC(void)
{ 
  unsigned char temp;

  temp=W25N01_Read_Status_Register2();
  temp|=0x10;
  W25N01_Write_Status_Register2(temp);
  delay_us(500);
}
//**************************************************************//
//**************************************************************//
unsigned short W25N01_Factory_Check_Bad_Blocks(unsigned char Buffer_Page[] )
{
  unsigned int i,j,pageAddr;
  unsigned char first_byte_data;
 
  j=0;
  for(i=0;i<1024;i++) //reserved last 24 block for BBM LUT
  {

	//delay_us(5); //delay
	while(W25N01_Check_Flash_Busy());

    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x13);	 //page data read(13h)
    SPI_Master_FIFO_Data_Put(0x00);	 //8Dummy clock
 
	pageAddr = i*64;

	SPI_Master_FIFO_Data_Put(pageAddr>>8);   //page address MSB
	SPI_Master_FIFO_Data_Put(pageAddr);   //page address LSB
    nSS_Inactive();

	delay_us(500); //delay for buffer load

	//read data (03h)
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x03);
	SPI_Master_FIFO_Data_Put(0xff);
	SPI_Master_FIFO_Data_Put(0xff);
	SPI_Master_FIFO_Data_Put(0xff);
	
	first_byte_data=SPI_Master_FIFO_Data_Put(0xff);
	
    nSS_Inactive();

    
	      
	//Check bad block
	if(first_byte_data!=0xff)	
    {
	  Buffer_Page[j]=i>>8;
	  Buffer_Page[j+1]=i;

//	  Buffer_Page[j]=0x00;
//	  Buffer_Page[j+1]=first_byte_data;

	  j=j+2;
	}	   

  }
  if(j==0)
    return 0;
  else
  	return j;
} 

//**************************************************************//
//**************************************************************//
//this function only use for first time check bad block in factory 
void W25N01_Find_Bad_Block(void)
{
  unsigned short i,j;
  unsigned long pageAddr;
  unsigned char first_byte_data;
  j=0;
   for(i=0;i<1000;i++) //reserved last 24 block for BBM LUT
   {
     //page data read(13h)
     nSS_Active();
     SPI_Master_FIFO_Data_Put(0x13);
	 SPI_Master_FIFO_Data_Put(0xff);   //8Dummy clock

     pageAddr = i*64;
     SPI_Master_FIFO_Data_Put(pageAddr>>8);   //page address MSB  
	 SPI_Master_FIFO_Data_Put(pageAddr);   //page address LSB
     nSS_Inactive();  
       
	 delay_us(500); //delay for buffer load

     //read data (03h)
     nSS_Active();
     SPI_Master_FIFO_Data_Put(0x03);
     SPI_Master_FIFO_Data_Put(0xff);   //8Dummy clock
     SPI_Master_FIFO_Data_Put(0xff);   //8Dummy clock
     SPI_Master_FIFO_Data_Put(0xff);   //8Dummy clock

     first_byte_data=SPI_Master_FIFO_Data_Put(0xff);
       
     //Check bad block
     if(first_byte_data==0xff)
     {
	   printf("%u",i); 
	   printf(" good block"); 
	   printf("%x",first_byte_data);
     }		
	 else
	 {
       printf("%u",i); 
	   printf(" bad block"); 
	   printf("%x",first_byte_data);
	   j++;
	 }

  }
  printf("%u",j); 
  printf(" bad block!");
}

//**************************************************************//
//**************************************************************//
void W25N01_Read_BBM_LUT(unsigned char *LUT_list)
{

  unsigned char i;


  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xA5);
  SPI_Master_FIFO_Data_Put(0xFF);  //8Dummy clock
   
  for(i=0; i<20;i++)
  {
	*LUT_list = i;
	LUT_list++;
	*LUT_list = SPI_Master_FIFO_Data_Put(0xFF);//
	LUT_list++;	
	*LUT_list = SPI_Master_FIFO_Data_Put(0xFF);//
	LUT_list++;
	

	*LUT_list = SPI_Master_FIFO_Data_Put(0xFF);//	
	LUT_list++;

    if(i==19) //for finish the cycle and set scs to high
	{
      *LUT_list = SPI_Master_FIFO_Data_Put(0xFF);//
      LUT_list++;
	  nSS_Inactive();
	}
	else
	{
	  *LUT_list = SPI_Master_FIFO_Data_Put(0xFF);//
	  LUT_list++;
	}
 
  }
}




//**************************************************************//
//**************************************************************//
void W25N01_Disable_Block_Protect(void)
{
  W25N01_Write_Status_Register1(0x00);
}

//**************************************************************//
//flash block erase 
//pageAddress:  input (0~1023)*64 page address
//return: 1  fail
//return: 0  pass
//(64-Pages,128K-Bytes)
//**************************************************************//
unsigned char W25N01_Block_Erase_128KB(unsigned short pageAddress)
{   
  unsigned short h;
    
  W25N01_Write_Enable();
  if(W25N01_Check_WEL()==0) //if WEL=1,write enable fail
  {

    return 1 ;
  }
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xD8);
  SPI_Master_FIFO_Data_Put(0xFF);  //8Dummy clock     
  SPI_Master_FIFO_Data_Put(pageAddress>>8);
  SPI_Master_FIFO_Data_Put(pageAddress);
  nSS_Inactive();

  delay_ms(1);

  h=0;
  while(W25N01_Check_Flash_Busy()&&(h<10))
  {
    //Block erase time max	10ms
	delay_ms(1);
	h++;
  }
  if(h>10)
  {
	return 1;
  }
	return 0;
}

//**************************************************************//
//whole flash erase 
//return: 1  error
//return: 0  done
//**************************************************************//
unsigned char W25N01_Whole_Flash_Erase(void)
{
  unsigned short i;
  
  
  for(i=0;i<1000;i++) //use 1024 of the block to program
  {
    if(W25N01_Block_Erase_128KB(i*64))
	{

	  return 1;
	}
  }

  return 0;
}

//**************************************************************//
//Load data in 2k data buffer
//lenght: page lenght
//*UpdataValue: data pointer
//**************************************************************//
void W25N01_Load_Program_Data(unsigned short lenght,unsigned char *UpdataValue)
{ 
  unsigned short i ;

  W25N01_Write_Enable();
  
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x02);
  SPI_Master_FIFO_Data_Put(0x00);     
  SPI_Master_FIFO_Data_Put(0x00);
  
  for(i=0;i<lenght;i++)
  {
    SPI_Master_FIFO_Data_Put(UpdataValue[i]);
	
  }	
  
  nSS_Inactive();

  while(W25N01_Check_Flash_Busy());
}

//**************************************************************//
//program data buffer content into the physical memory page 
//**************************************************************//
void W25N01_Program_Execute(unsigned short pageAddress)
{
  W25N01_Write_Enable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x10);
  SPI_Master_FIFO_Data_Put(0xff); //8 Dummy clocks
  SPI_Master_FIFO_Data_Put(pageAddress>>8);
  SPI_Master_FIFO_Data_Put(pageAddress);
  nSS_Inactive();
    
  while(W25N01_Check_Flash_Busy());
}

//**************************************************************//
//**************************************************************//
void W25N01_Read_PageSize(unsigned short pageAddr)
{
	unsigned short i=0;

    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x13);	 //page data read(13h)
    SPI_Master_FIFO_Data_Put(0x00);	 //8Dummy clock
	SPI_Master_FIFO_Data_Put(pageAddr>>8);   //page address MSB
	SPI_Master_FIFO_Data_Put(pageAddr);   //page address LSB
    nSS_Inactive();

	//delay_us(500); //delay for buffer load
	while(W25N01_Check_Flash_Busy());

	//read data (03h)
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x03);
	SPI_Master_FIFO_Data_Put(0xff);
	SPI_Master_FIFO_Data_Put(0xff);
	SPI_Master_FIFO_Data_Put(0xff);
	for(i=0; i<2048; i++)
	  Buffer_Page[i]=SPI_Master_FIFO_Data_Put(0xff);
    nSS_Inactive();

	    
}
//**************************************************************//
//**************************************************************//
void W25N01_Swap_Block(unsigned short BadBlockAddr, unsigned short GoodBlockAddr)
{
  unsigned short temp;

  //Write LBA-->PBA

  W25N01_Write_Enable();

  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xA1);
  temp = BadBlockAddr | 0x8000;

  SPI_Master_FIFO_Data_Put(temp>>8);
  SPI_Master_FIFO_Data_Put(temp);
                 
  temp = GoodBlockAddr;

  SPI_Master_FIFO_Data_Put(temp>>8);
  SPI_Master_FIFO_Data_Put(temp);
  nSS_Inactive();
	             
  while(W25N01_Check_Flash_Busy());

}
