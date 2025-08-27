
  /*************** RAIO Technology Inc. ***************
  * file		: RA8889_Application_Using_STM32.h 
  * author		: Raio Application Team
  * version		: V1.0
  * date		: 2020/04/01
  * brief		: 	
  ****************************************************/	

#ifndef __RA8889_APPLICATION_USING_STM32_H
#define __RA8889_APPLICATION_USING_STM32_H



void sd_showpic_256_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_65k_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_16M_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_16M_mode1_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_16M_mode2_bin(unsigned short w,unsigned short h,char * fname);
void sd_BTE_showpic_256_bin(unsigned short w,unsigned short h,char * fname);
void sd_BTE_showpic_65k_bin(unsigned short w,unsigned short h,char * fname);
void sd_BTE_showpic_16M_mode1_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_MPU8_to_65k_bin(unsigned short w,unsigned short h,char * fname);
void sd_showpic_MPU8_to_16M_bin(unsigned short w,unsigned short h,char * fname);




/**for MX Serial_Flash**/
void RA8889_SPI_Flash_WREN(void);	// MX Serial_Flash Write Enable
void RA8889_SPI_Flash_WRDI(void);	// MX Serial_Flash Write Disable
void RA8889_SPI_Flash_CE(void);	// MX Serial_Flash Chip Erase
unsigned char RA8889_SPI_Flash_RDSR(void);	// MX Serial_Flash Read Status

void RA8889_Write_SD_to_MX25L25635E_CP(unsigned long flash_addr, unsigned short w, unsigned short h, unsigned char bpp, char * fname );
void RA8889_Write_SD_to_MX25L25635E_PP(unsigned long flash_addr, unsigned short w, unsigned short h, unsigned char bpp, char * fname );//»Ý¬°256­¿¼Æ






#endif /*__RA8889_APPLICATION_USING_STM32_H*/











