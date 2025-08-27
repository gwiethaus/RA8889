#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_  1
//#include "stm32f10x_lib.h"
#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"

#define Dummy_Byte 0xA5

/* Select SPI FLASH: ChipSelect pin low  */
#define Flash_cs_low()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect SPI FLASH: ChipSelect pin high */
#define Flash_cs_high()    GPIO_SetBits(GPIOA, GPIO_Pin_4)

//#define FLASH_CHIP MX25L8
//#define FLASH_CHIP MX25L16
//#define FLASH_CHIP MX25L32
//#define FLASH_CHIP MX25L64
//#define FLASH_CHIP MX25L128

#define MX25L8		1048576 //memory size
#define MX25L16		2097152 //memory size
#define MX25L32		4194304 //memory size
#define MX25L64 	8388608 //memory size
#define MX25L128   16777216 //memory size



void SPI_Peripheral_Init(void);	        //SPI初始化
u8 SPI_Peripheral_ReadByte(void);	    //FLASH操作基本函數，讀一個字節
u8 SPI_Peripheral_SendByte(u8 byte);	//FLASH操作基本函數，發送一個字節


void WREN(void);
void WRDI(void);
void WRSR(unsigned char UpdataValue);

u8 WIP(void);
u8 WEL(void);
u8 RDSR(void);

void WRITE_DATA_BYTE(u32 address,u8 UpdataValue);
void WRITE_DATA_PAGE(u32 address,u8 *UpdataValue);

u8 READ_DATA_BYTE(u32 address);
void CHIP_ERASE(void);
void RDID(void);
u8 FLASH_ERASE(void);





#endif


