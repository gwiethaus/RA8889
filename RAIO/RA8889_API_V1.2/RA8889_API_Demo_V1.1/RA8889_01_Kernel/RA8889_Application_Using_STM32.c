
  /*************** RAIO Technology Inc. ***************
  * file		: RA8889_Application_Using_STM32.c 
  * author		: Raio Application Team
  * version		: V1.0
  * date		: 2014/06/24
  * brief		: 	
  ****************************************************/	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f10x.h"
#include "ff.h"//FatFs - FAT file system module include file
#include "delay.h"

#include "UserDef.h"
#include "RA8889_MCU_IF.h"
#include "RA8889.h"
#include "RA8889_Application_Using_STM32.h"

extern unsigned char buffer[5760];  //1920x3byte=5760byte
extern unsigned char buffer_data[2]; 

/* Private functions ---------------------------------------------------------*/
extern FATFS	fs;            // Work area (file system object) for logical drive
extern FIL fsrc, fdst;      // file objects
extern FRESULT res;         // FatFs function common result code
extern UINT br, bw;         // File R/W count
/* Private function prototypes -----------------------------------------------*/

//GPIO_SetBits(GPIOB, GPIO_Pin_0);	//LED D3 off 綠燈
//GPIO_SetBits(GPIOB, GPIO_Pin_1);  //LED D4 off 紅燈 
//GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off 綠燈
//GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //LED D4 off 紅燈 

/*******************************************************************************
* @brief Write WxH_picture to SDRAM from read SD_Card 
*******************************************************************************/
void sd_showpic_256_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	
//	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	for(j=0;j<h;j++)
	{
		res = f_read(&fsrc, buffer, w,&br);	//每次讀完一行
		
		for(i=0;i<w;i++) 	//每次寫一行
		{
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(buffer[i]);	//write 8 bits 1 pixel data to SDRAM  
			//delay_ms(1);
			
		}
	//	Check_2D_Busy();
		if (res || br == 0) break;   // error or eof	   	  
	}

	f_close(&fsrc);
	//f_close(&file);
	Check_Mem_WR_FIFO_Empty();
//****//
Check_2D_Busy();	
	
}

void sd_showpic_65k_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp;
//	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	{
		res = f_read(&fsrc, buffer, w*2,&br);	//每次讀完一行
		
		for(i=0;i<w;i++) 	//每次寫一行
		{
			Check_Mem_WR_FIFO_not_Full();
			data_temp =((buffer[i*2+1]<<8)|buffer[i*2]);
			LCD_DataWrite(data_temp);	//write 16 bits 1 pixel data to SDRAM 
		
		
		}
		
		if (res || br == 0) break;   // error or eof	   	  
	}
	
	f_close(&fsrc);
		//f_close(&file);
	
	Check_Mem_WR_FIFO_Empty();
//****//
Check_2D_Busy();
}

void sd_showpic_16M_bin(unsigned short w,unsigned short h,char * fname)
{ 
	unsigned short i,j,data_temp1,data_temp2,data_temp3;
//	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
	res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	
	for(j=0;j< h;j++)
	{
		res = f_read(&fsrc, buffer, w*3,&br);	//每次讀完一行
		
		for(i=0;i<(w/2);i++) 	//每次寫一行
		{
			data_temp1 =((buffer[i*6+1]<<8)|buffer[i*6]);
			data_temp2 =((buffer[i*6+3]<<8)|buffer[i*6+2]);		  
			data_temp3 =((buffer[i*6+5]<<8)|buffer[i*6+4]);
			
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp1);	//write 24 bits 2 pixel data to SDRAM 
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp2);	//write 24 bits 2 pixel data to SDRAM 
			Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(data_temp3);	//write 24 bits 2 pixel data to SDRAM 
		}
		
		if (res || br == 0) break;   // error or eof	   	  
	}

	f_close(&fsrc);
	//f_close(&file);

	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}


void sd_showpic_16M_mode1_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp1,data_temp2,data_temp3;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*3,&br);	//每次讀完一行
			
		   for(i=0;i<(w/2);i++)  //每次寫一行
          {
           data_temp1 =((buffer[i*6+1]<<8)|buffer[i*6]);
           data_temp2 =((buffer[i*6+3]<<8)|buffer[i*6+2]);    
           data_temp3 =((buffer[i*6+5]<<8)|buffer[i*6+4]);
   
            LCD_DataWrite(data_temp1); //write 24 bits 2 pixel data to SDRAM 
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(data_temp2); //write 24 bits 2 pixel data to SDRAM 
             Check_Mem_WR_FIFO_not_Full();
             LCD_DataWrite(data_temp3); //write 24 bits 2 pixel data to SDRAM 
            Check_Mem_WR_FIFO_not_Full();
           }
		   
		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);

	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}

void sd_showpic_16M_mode2_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*3,&br);	//每次讀完一行
		 
		 for(i=0;i<w/2;i++) 	//每次寫一行
		 {
		  //data_temp = buffer[i*2];
		  //data_temp =((data_temp <<8) | buffer[i*2+1]);
		  //data_temp =((buffer[i*2+1]<<8)|buffer[i*2]);
		  data_temp =((buffer[i*6+1]<<8)|buffer[i*6]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd 
		   Check_Mem_WR_FIFO_not_Full();
		  data_temp =(buffer[i*6+2]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd 
		    Check_Mem_WR_FIFO_not_Full();
		  data_temp =((buffer[i*6+4]<<8)|buffer[i*6+3]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd 
		   Check_Mem_WR_FIFO_not_Full();
		  data_temp =(buffer[i*6+5]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd 
		   Check_Mem_WR_FIFO_not_Full();
		 }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);

	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}

void sd_showpic_MPU8_to_65k_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*2,&br);	//每次讀完一行
		 
		 for(i=0;i<w;i++) 	//每次寫一行
		 {
		  LCD_DataWrite(buffer[i*2+1]);
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(buffer[i*2]);	//write 16 bits data to lcd 
		  Check_Mem_WR_FIFO_not_Full();

		 }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}
void sd_showpic_MPU8_to_16M_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*3,&br);	//每次讀完一行
		 
		 for(i=0;i<w;i++) 	//每次寫一行
		 {
		  LCD_DataWrite(buffer[i*3]);	
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(buffer[i*3+1]);
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(buffer[i*3+2]);	  
		  Check_Mem_WR_FIFO_not_Full();
		  //delay_us(1);
		 }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}


void sd_BTE_showpic_256_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w,&br);	//每次讀完一行
		 
		 for(i=0;i<w;i++) 	//每次寫一行
		 {
		  LCD_DataWrite(buffer[i]);	//write 16 bits data to lcd 

		  Check_Mem_WR_FIFO_not_Full();//BTE 寫入時用

		 }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}

void sd_BTE_showpic_65k_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*2,&br);	//每次讀完一行
		 
		 for(i=0;i<w;i++) 	//每次寫一行
		 {
		  //data_temp = buffer[i*2];
		  //data_temp =((data_temp <<8) | buffer[i*2+1]);
		    //data_temp =((buffer[i*2+1]<<8)|buffer[i*2]);
		   data_temp =((buffer[i*2]<<8)|buffer[i*2+1]);
		  LCD_DataWrite(data_temp);	//write 16 bits data to lcd 

		  Check_Mem_WR_FIFO_not_Full();//BTE 寫入時用
		  }

		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}

void sd_BTE_showpic_16M_mode1_bin(unsigned short w,unsigned short h,char * fname)
{ 
    unsigned short i,j,data_temp1,data_temp2,data_temp3;
	FRESULT res;
	
	f_mount(0, &fs);   //註冊一個工作區
	
    res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	//打開文件
	   
	 for(j=0;j< h;j++)
	  {
	     res = f_read(&fsrc, buffer, w*3,&br);	//每次讀完一行
			
		   for(i=0;i<(w/2);i++)  //每次寫一行
          {
           data_temp1 =((buffer[i*6+1]<<8)|buffer[i*6]);
           data_temp2 =((buffer[i*6+3]<<8)|buffer[i*6+2]);    
           data_temp3 =((buffer[i*6+5]<<8)|buffer[i*6+4]);
   
            LCD_DataWrite(data_temp1); //write 24 bits 2 pixel data to SDRAM 

            Check_Mem_WR_FIFO_not_Full();
			//delay_us(5);
            LCD_DataWrite(data_temp2); //write 24 bits 2 pixel data to SDRAM 
    
             Check_Mem_WR_FIFO_not_Full();
              //delay_us(5);
             LCD_DataWrite(data_temp3); //write 24 bits 2 pixel data to SDRAM 

            Check_Mem_WR_FIFO_not_Full();
			 //delay_us(5);
           }
		   
		 if (res || br == 0) break;   // error or eof	   	  
         }

	    f_close(&fsrc);
		//f_close(&file);
	Check_Mem_WR_FIFO_Empty();

Check_2D_Busy();
}





/*******************************************************************************
* @brief RA8889_SPI command to Serial_Flash 
*******************************************************************************/
//Write Enable (WREN) Sequence (Command 06)
void RA8889_SPI_Flash_WREN(void)
{
	do
	{
	  nSS_Active();
	  SPI_Master_FIFO_Data_Put(0x06);  //Serial Flash WREN
	  //while( EMTI_Flag()!=1);
	  // Clear_EMTI_Flag();
	  delay_us(1); 
	  nSS_Inactive();		

	}while(!(RA8889_SPI_Flash_RDSR()& 0x02));
}

// Write Disable (WRDI) Sequence (Command 04)
void RA8889_SPI_Flash_WRDI(void)
{
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x04);  //Serial Flash WREN
	//while( EMTI_Flag()!=1);
	// Clear_EMTI_Flag();
	delay_us(1); 
	nSS_Inactive();
	while(RA8889_SPI_Flash_RDSR()& 0x01);
}


//Chip Erase (CE) Sequence (Command 60 or C7)
void RA8889_SPI_Flash_CE(void)
{
	Enable_SFlash_SPI();

	SPI_Clock_Period(1);
	Select_nSS_drive_on_xnsfcs1();
	Reset_CPOL();
//	Set_CPOL();
	Reset_CPHA();
//	Set_CPHA();	
  
	RA8889_SPI_Flash_WREN();
  
	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x60);  //Serial Flash Chip Erase
	//while( EMTI_Flag()!=1);
	// Clear_EMTI_Flag();
	delay_us(1); 
	nSS_Inactive();
	do
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off 綠燈
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //LED D4 off 紅燈 
		delay_ms(100);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);	//LED D3 on 綠燈
		GPIO_SetBits(GPIOB, GPIO_Pin_1);  //LED D4 on 紅燈 
		delay_ms(100);
	}
	while(RA8889_SPI_Flash_RDSR()& 0x01);
	RA8889_SPI_Flash_WRDI();

	delay_ms(10);

//	Disable_SFlash_SPI();
}

//Read Status Register (RDSR) Sequence (Command 05)
unsigned char RA8889_SPI_Flash_RDSR(void)
{
 unsigned char temp;

	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x05);
	delay_us(1);
	temp=SPI_Master_FIFO_Data_Put(0xff);//dummy cycle and read back value
	nSS_Inactive();
	return(temp);	
}



/************************************************************************************************************************************************/	
void RA8889_Write_SD_to_MX25L25635E_PP(unsigned long flash_addr, unsigned short w, unsigned short h, unsigned char bpp, char * fname )//W=256xN
{
	unsigned short i,j;
	unsigned short k;

//**************************RA8889->MX25L25635E	
	Enable_SFlash_SPI();
	

	SPI_Clock_Period(1);

	//Select_nSS_drive_on_xnsfcs0();
	  Select_nSS_drive_on_xnsfcs1();

	Reset_CPOL();						   
	//Set_CPOL();
	Reset_CPHA();
	//Set_CPHA();

	nSS_Active();					   //nSS port will goes low
	SPI_Master_FIFO_Data_Put(0xB7);  //to enter 4-byte mode: address bit is changed from 24-bit to 32-bit.
	delay_ms(1); 
	nSS_Inactive();				   //nSS port will goes high	
		
	//**************************SD->STM32->RA8889->MX25L25635E	
	f_mount(0, &fs);   //registers a file system object
	res = f_open(&fsrc, fname, FA_OPEN_EXISTING | FA_READ);	// creates a file object to be used to access the file

	if( w%256 == 0 ) 
	{
		do
		{			
			res = f_read( &fsrc, buffer, w*(bpp/8), &br );	//每次讀完一行存在buffer[]	//1024*3=3072, 3072/256=12, 
			k = ( w*(bpp/8) ) / (256) ;
			i=0;	
			
			do
			{
				RA8889_SPI_Flash_WREN();

				do{}
				while(RA8889_SPI_Flash_RDSR()& 0xFD); //b11 1111 01				
				
				nSS_Active();//nSS port will goes low
				delay_us(1); 

				SPI_Master_FIFO_Data_Put(0x02);// set MX25L25635E = PP mode

				SPI_Master_FIFO_Data_Put(flash_addr>>24);
				SPI_Master_FIFO_Data_Put(flash_addr>>16);
				SPI_Master_FIFO_Data_Put(flash_addr>>8);
				SPI_Master_FIFO_Data_Put(flash_addr);	
					
			
				for(j=0;j<256;j++) 	//每次寫一行
				{	
					delay_us(1); 
					SPI_Master_FIFO_Data_Put( buffer[i] );
					//SPI_Master_FIFO_Data_Put( 0x00 );
					i++;
					
				}	

				nSS_Inactive();				   //nSS port will goes high	
				
				while(RA8889_SPI_Flash_RDSR()& 0x01);				
				
				flash_addr=flash_addr+256;
				delay_us(1);
		
		
			}while( --k );


			if( h%2 )  
					{
						GPIO_SetBits(GPIOB, GPIO_Pin_0);	//LED D3 on 綠燈
						GPIO_SetBits(GPIOB, GPIO_Pin_1);  //LED D4 on 紅燈 
					}	
			else 
					{
						GPIO_ResetBits(GPIOB, GPIO_Pin_0);	//LED D3 off 綠燈
						GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //LED D4 off 紅燈 
					}			
		
		//	Check_2D_Busy();
		if (res || br == 0) break;   // error or eof	   	  
			
		}while( --h );


	}
		
	RA8889_SPI_Flash_WRDI();
	RA8889_SPI_Flash_RDSR();
	

	if( w%2 == 1 )
	{

	}	

	f_close(&fsrc);

	Check_2D_Busy();	

	nSS_Active();					//nSS port will goes low
	SPI_Master_FIFO_Data_Put(0xE9);	//設定Flash換回24位址模式
	delay_ms(1); 
	nSS_Inactive();					//nSS port will goes high	
	
	delay_ms(10);
//	Disable_SFlash_SPI();	
}




