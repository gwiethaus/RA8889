
/************** RAIO Technology Inc. / Lcdvision Co., Ltd.***************
 * file			: RA8889_51.c
 * author		: BW.SU
 * version	: V1.2
 * date			: 2024/06/07
 * brief		:
 *					（1）延时函数，原代码所有us级别全部改为ms级别
 *					（2）修改部份Check_While_Busy判断逻辑
************************* bbs.lcdvision.com.cn **************************/

#include "RA8889_51.h"
#include "RA8889_MCU_IF.h"
#include "UserDef.h"
#include "delay.h"


/*****************************************************************
  * @brief  RA8889初始化
  * @param  无
  * @retval 无
  * @note
  ****************************************************************/
void RA8889_Initial(void)
{
	unsigned char temp;

	Check_IC_ready();
	RA8889_SW_Reset();		//软件复位
	Check_IC_ready();

	RA8889_PLL(DRAM_FREQ, CORE_FREQ, SCAN_FREQ);
	RA8889_SDRAM_initial();
	

	/* LCD接口设定 */
	//**[01h]**//
    TFT_24bit();
//	TFT_18bit();
//	TFT_16bit();
//	Without_TFT();


	/* MCU communication bit setting */
//	0: 8-bit Parallel Host Data Bus.
//	1: 16-bit Parallel Host Data Bus.
	#if defined(MCU_8bit_ColorDepth_8bpp) || defined(MCU_8bit_ColorDepth_16bpp) || defined(MCU_8bit_ColorDepth_24bpp)
		Host_Bus_8bit();
	#endif
	#if defined(MCU_16bit_ColorDepth_16bpp) || defined(MCU_16bit_ColorDepth_24bpp_Mode_1) || defined(MCU_16bit_ColorDepth_24bpp_Mode_2)  
		Host_Bus_16bit();
	#endif



	/*=====[02h]register=====*/

	/* Data format setting: MCU interface, color depth */
//	0xb: Direct write for all 8 bits MPU I/F, 16 bits MPU I/F with 16bpp, 16 bits MPU I/F with 24bpp data mode 1 and serial host interface.
//	11b: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
	LCD_CmdWrite(0x02);	   	  //02h.bit7~6 register
	temp = LCD_DataRead();
	#if defined(MCU_8bit_ColorDepth_8bpp) || defined(MCU_8bit_ColorDepth_16bpp) || defined(MCU_8bit_ColorDepth_24bpp) || defined(MCU_16bit_ColorDepth_16bpp) || defined(MCU_16bit_ColorDepth_24bpp_Mode_1)  
	    temp &= cClrb7;  
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2
		temp |= cSetb7;
		temp |= cSetb6;
	#endif
	LCD_DataWrite(temp);


	/* Video memory write direction setting */
	MemWrite_Left_Right_Top_Down();
//	MemWrite_Right_Left_Top_Down();
//	MemRead_Down_Top_Left_Right();
//	MemRead_Top_Down_Left_Right();

	/* Display scan direction settings */
//	HSCAN_L_to_R();
//	VSCAN_T_to_B();
//	HSCAN_R_to_L();
//	VSCAN_B_to_T();


	/*=====[03h]寄存器=====*/ 
	Graphic_Mode();
//	Text_Mode();

	/* Memory_Select_SDRAM */
	Memory_Select_SDRAM();


//------------------------------------
//** Set color depth, define in [UserDef.h] **//

	#ifdef MCU_8bit_ColorDepth_8bpp	
		Select_Main_Window_8bpp();		//[10h] Set main window color depth
		Memory_8bpp_Mode();				//[5Eh] Set active memory color depth

		Select_PIP1_Window_8bpp();		//[11h] PIP 1 Window Color Depth
		Select_PIP2_Window_8bpp();		//[11h] PIP 2 Window Color Depth

		BTE_S0_Color_8bpp();			//[92h] Source_0 Color Depth
		BTE_S1_Color_8bpp();			//[92h] Source_1 Color Depth
		BTE_Destination_Color_8bpp();	//[92h] Destination Color Depth	
		
		IDEC_Destination_Color_8bpp();//
	#endif

	#if defined (MCU_8bit_ColorDepth_16bpp) || defined (MCU_16bit_ColorDepth_16bpp)
		Select_Main_Window_16bpp();		//[10h]Set main window color depth
		Memory_16bpp_Mode();			//[5Eh]Set active memory color depth

		Select_PIP1_Window_16bpp();		//[11h] PIP 1 Window Color Depth
		Select_PIP2_Window_16bpp();		//[11h] PIP 2 Window Color Depth

		BTE_S0_Color_16bpp();			//[92h] Source_0 Color Depth
		BTE_S1_Color_16bpp();			//[92h] Source_1 Color Depth
		BTE_Destination_Color_16bpp();	//[92h] Destination Color Depth	 
		
		IDEC_Destination_Color_16bpp();//
	#endif

	#if defined (MCU_8bit_ColorDepth_24bpp) || defined (MCU_16bit_ColorDepth_24bpp_Mode_1) || defined (MCU_16bit_ColorDepth_24bpp_Mode_2)	
		Select_Main_Window_24bpp();		//[10h]Set main window color depth
		Memory_24bpp_Mode();			//[5Eh]Set active memory color depth

		Select_PIP1_Window_24bpp();		//[11h] PIP 1 Window Color Depth
		Select_PIP2_Window_24bpp();		//[11h] PIP 2 Window Color Depth

		BTE_S0_Color_24bpp();			//[92h] Source_0 Color Depth
		BTE_S1_Color_24bpp();			//[92h] Source_1 Color Depth
		BTE_Destination_Color_24bpp();	//[92h] Destination Color Depth	   
		
		IDEC_Destination_Color_24bpp();//
	#endif

	Select_Main_Window_24bpp();
	Memory_24bpp_Mode();

//------------------------------------
//**[12h]~[1Fh]]**//
	Set_LCD_Panel();

//------------------------------------
//**[20h][21h][22h][23h]**//
	Main_Image_Start_Address(0);
//**[24h][25h]**//					
	Main_Image_Width(main_image_width);							
//**[26h][27h][28h][29h]**//
	Main_Window_Start_XY(0,0);	
//**[50h][51h][52h][53h]**//
	Canvas_Image_Start_address(0);
//**[54h][55h]**//
	Canvas_image_width(canvas_image_width);
//**[56h][57h][58h][59h]**//
	Active_Window_XY(0,0);
//**[5Ah][5Bh][5Ch][5Dh]**//
	Active_Window_WH(LCD_Width,LCD_Height);

//------------------------------------
//**[5E]**//
	Memory_XY_Mode();
//	Memory_Linear_Mode();

//------------------------------------
//**[5Fh][60h][61h][62h]**//
	Goto_Pixel_XY(0,0);	   

	Display_ON();

	Color_Bar_ON();
	delay_ms(500);
	Color_Bar_OFF();
}







/*================================================================*/
//									BOTTOM LEVEL API FUNCTIONS	
/*================================================================*/

void LCD_RegisterWrite(unsigned char Cmd,unsigned char Data)
{
//	Check_2D_Busy();
	LCD_CmdWrite(Cmd);
	LCD_DataWrite(Data);
}  


unsigned char LCD_RegisterRead(unsigned char Cmd)
{
	unsigned char temp;
//	Check_2D_Busy();
	LCD_CmdWrite(Cmd);
	temp=LCD_DataRead();

	return temp;
}


void RA8889_SW_Reset(void)
{/*
Software Reset
0: Normal operation.
1: Software Reset.
Software Reset only reset internal state machine. Configuration
Registers value wonˇt be reset. So all read-only flag in the
register will return to its initial value. User should have proper
action to make sure flag is in desired state.
Note: The bit will auto clear after reset.
*/	
	unsigned short i;
	unsigned char temp;
	
	LCD_CmdWrite(0x00);
	temp = LCD_DataRead();
	temp |= 0x01;
	LCD_DataWrite(temp);
	delay_ms(1);			// it must wait 100us after Software_Reset.
		
#ifdef Check_While_Busy
	while( LCD_RegisterRead(0x00)&0x01 );
#else
	// Please according to your usage to modify delay(check) time.
	for(i=0;i<100;i++)
	{
		if( (LCD_RegisterRead(0x00)&0x01)==0x00 )	{break;}
	}
#endif
}


void Check_IC_ready(void)
{
/*	[Status Register] bit1
Operation mode status
0: Normal operation state
1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.		
*/	
	unsigned short i;
		
#ifdef Check_While_Busy
	while(LCD_StatusRead() & 0x02);
#else
	// Please according to your usage to modify delay(check) time.
	for(i=0;i<1000;i++)
	{
		if( (LCD_StatusRead()&0x02)==0x00 ){break;}
	}
#endif	
}


void Check_SDRAM_Ready(void)
{
/*	[Status Register] bit2
SDRAM ready for access
0: SDRAM is not ready for access
1: SDRAM is ready for access		
*/	
	unsigned short i;
	
#ifdef Check_While_Busy
	while( (LCD_StatusRead()&0x04) == 0x00 );
#else
	// Please according to your usage to modify delay(check) time.
	for(i=0;i<1000;i++)
	{
		if( (LCD_StatusRead()&0x04)==0x04 ){break;}
	}
#endif	
}


void Check_Mem_WR_FIFO_not_Full(void)
{
    /*[Status Register] bit7
    Host Memory Write FIFO full
    0: Memory Write FIFO is not full.
    1: Memory Write FIFO is full.
    Only when Memory Write FIFO is not full, MPU may write another
    one pixel.
    */
	unsigned short i;
	
#ifdef Check_While_Busy
	while (LCD_StatusRead() & 0x80);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for(i=0;i<1000;i++)
	{
		if( (LCD_StatusRead()&0x80)==0 ){break;}
	}
#endif	
}


void Check_Mem_WR_FIFO_Empty(void)
{
    /*[Status Register] bit6
    Host Memory Write FIFO empty
    0: Memory Write FIFO is not empty.
    1: Memory Write FIFO is empty.
    When Memory Write FIFO is empty, MPU may write 8bpp data 64
    pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.
    */
	unsigned short i;

#ifdef Check_While_Busy
	while ((LCD_StatusRead() & 0x40) == 0x00);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for(i=0;i<1000;i++)
	{
		if ((LCD_StatusRead() & 0x40) == 0x40) { break; }
	}
#endif	
}


void Check_Mem_RD_FIFO_not_Full(void)
{
    /*	[Status Register] bit5
    Host Memory Read FIFO full
    0: Memory Read FIFO is not full.
    1: Memory Read FIFO is full.
    When Memory Read FIFO is full, MPU may read 8bpp data 32
    pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
    */
	unsigned short i;

#ifdef Check_While_Busy
	while (LCD_StatusRead() & 0x20);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for(i=0;i<1000;i++)
	{
		if ((LCD_StatusRead() & 0x20) == 0x00) { break; }
	}
#endif	
}



void Check_Mem_RD_FIFO_Full(void)
{
    /*	[Status Register] bit5
    Host Memory Read FIFO full
    0: Memory Read FIFO is not full.
    1: Memory Read FIFO is full.
    When Memory Read FIFO is full, MPU may read 8bpp data 32
    pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
    */
	unsigned short i;
	
#ifdef Check_While_Busy
	while ((LCD_StatusRead() & 0x20) == 0x00);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for(i=0;i<1000;i++)
	{
		if ((LCD_StatusRead() & 0x20) == 0x20) { break; }
	}
#endif	
}


void Check_Mem_RD_FIFO_not_Empty(void)
{
    /*	[Status Register] bit4
    Host Memory Read FIFO empty
    0: Memory Read FIFO is not empty.
    1: Memory Read FIFO is empty.
    */
	unsigned short i;
	
#ifdef Check_While_Busy
    while (LCD_StatusRead() & 0x10);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for(i=0;i<1000;i++)
	{
		if ((LCD_StatusRead() & 0x10) == 0x00) { break; }
	}
#endif	
}


void Check_2D_Busy(void)
{
    /*	[Status Register] bit3
    Core task is busy
    Following task is running:
    BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
    0: task is done or idle.
    1: task is busy.
    */
    unsigned short i, temp;
    // Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間

#ifdef Check_While_Busy
    // case1: using status register
    do{
        temp = LCD_StatusRead();
    } while (temp & 0x08);
#else
    for (i = 0; i < 1000; i++)
		{
        if ((LCD_StatusRead() & 0x08) == 0x00) { break; }
    }
#endif
}


unsigned char Power_Saving_Status(void)
{
    /*	[Status Register] bit1
    Operation mode status
    0: Normal operation state
    1: Inhibit operation state
    Inhibit operation state means internal reset event keep running or
    initial display still running or chip enter power saving state.
    */
    unsigned char temp;

    if ((LCD_StatusRead() & 0x02) == 0x02)
        temp = 1;
    else
        temp = 0;

    return temp;
}


void Check_Power_is_Saving(void)
{
    /*	[Status Register] bit1
    Operation mode status
    0: Normal operation state
    1: Inhibit operation state
    Inhibit operation state means internal reset event keep running or
    initial display still running or chip enter power saving state.
    */
	unsigned short i;
	
#ifdef Check_While_Busy
	while ((LCD_StatusRead() & 0x02) == 0x00);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for (i = 0; i < 1000; i++)
	{
		if ((LCD_StatusRead() & 0x02) == 0x02) { break; }
	}
#endif		
}


void Check_NO_Interrupt(void)
{
    /*	[Status Register] bit0
    Interrupt pin state
    0: without interrupt active
    1: interrupt active
    */
	unsigned short i;
	
#ifdef Check_While_Busy
	while (LCD_StatusRead() & 0x01);
#else
	// Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for (i = 0; i < 1000; i++)
	{
		if ((LCD_StatusRead() & 0x01) == 0x00) { break; }
	}
#endif	
}


void Check_Interrupt_Occur(void)
{
    /*	[Status Register] bit0
    Interrupt pin state
    0: without interrupt active
    1: interrupt active
    */
	unsigned short i;
	
#ifdef Check_While_Busy
	while ((LCD_StatusRead() & 0x01) == 0x00);
#else
    // Please according to your usage to modify delay(check) time. // 請依據您的使用情況修改執行時間
	for (i = 0; i < 1000; i++)
	{
		if ((LCD_StatusRead() & 0x01) == 0x01) { break; }
	}
#endif	
}



//void Set_LCD_Panel(void)
//{
//	//**[10h]**//
//	Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
////	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.
//	
//	PCLK_Falling();
////	PCLK_Rising();

//	VSCAN_T_to_B();
//	PDATA_Set_RGB();

//	HSYNC_Low_Active();
//	VSYNC_Low_Active();
//	DE_High_Active();
////	DE_Low_Active();

//	LCD_HorizontalWidth_VerticalHeight(800,480);
//	LCD_Horizontal_Non_Display(64);
//	LCD_HSYNC_Start_Position(208);
//	LCD_HSYNC_Pulse_Width(24);
//	LCD_Vertical_Non_Display(16);
//	LCD_VSYNC_Start_Position(22);
//	LCD_VSYNC_Pulse_Width(16);
//}






//[01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h][01h]
void Enable_PLL(void)
{
    /* 1: PLL enable; cannot change PLL parameter.*/
	unsigned char temp;
	unsigned short i;

	LCD_CmdWrite(0x01);
	temp = LCD_DataRead();
	temp |= cSetb7;
	LCD_DataWrite(temp);

	delay_ms(1); // PLL lock time = 1024 T OSC clocks, if OSC=10MHz, PLL lock time = 100 us.

#ifdef Check_While_Busy
	while ((LCD_RegisterRead(0x01) & 0x80) != 0x80); // check PLL was ready.
#else
	/*check PLL was ready ( Please according to your usage to modify. // 請依據您的使用情況修改)	 */
	for (i = 0; i < 1000; i++)
	{
		LCD_CmdWrite(0x01);
		temp = LCD_DataRead();
		if ((temp & 0x80) == 0x80) { break; }
	}
#endif
}





void Sent_XnWAIT_Status_When_CS_Low(void)
{ /*
     Mask XnWAIT on XnCS deassert
     0 : No mask
         XnWAIT keep assert if internal state keep busy and cannot
         accept next R/W cycle, no matter XnCS assert/deassert. If
         MCU cycle cannot be extended while XnWAIT keep low, user
         should poll XnWAIT and wait it goes high then start next
         access.
     1 : Mask
         XnWAIT deassert when XnCS deassert. Use in MCU cycle can
         be extended by XnWAIT automatically.
 */
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Sent_XnWAIT_Status_At_Any_Time(void)
{ /*
     Mask XnWAIT on XnCS deassert
     0 : No mask
         XnWAIT keep assert if internal state keep busy and cannot
         accept next R/W cycle, no matter XnCS assert/deassert. If
         MCU cycle cannot be extended while XnWAIT keep low, user
         should poll XnWAIT and wait it goes high then start next
         access.
     1 : Mask
         XnWAIT deassert when XnCS deassert. Use in MCU cycle can
         be extended by XnWAIT automatically.
 */
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Key_Scan_Enable(void)
{
    /*  0: Disable.
        1: Enable.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Key_Scan_Disable(void)
{
    /*  0: Disable.
        1: Enable.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void TFT_24bit(void) //
{
    /*  TFT Panel I/F Output pin Setting
    00b: 24-bits TFT output.
    01b: 18-bits TFT output.
    10b: 16-bits TFT output.
    11b: w/o TFT output.
    Other unused TFT output pins are set as GPIO or Key function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void TFT_18bit(void)
{
    /*  TFT Panel I/F Output pin Setting
    00b: 24-bits TFT output.
    01b: 18-bits TFT output.
    10b: 16-bits TFT output.
    11b: w/o TFT output.
    Other unused TFT output pins are set as GPIO or Key function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void TFT_16bit(void)
{
    /*  TFT Panel I/F Output pin Setting
    00b: 24-bits TFT output.
    01b: 18-bits TFT output.
    10b: 16-bits TFT output.
    11b: w/o TFT output.
    Other unused TFT output pins are set as GPIO or Key function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb4;
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Without_TFT(void)
{
    /*  TFT Panel I/F Output pin Setting
    00b: 24-bits TFT output.
    01b: 18-bits TFT output.
    10b: 16-bits TFT output.
    11b: w/o TFT output.
    Other unused TFT output pins are set as GPIO or Key function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb4;
    temp |= cSetb3;
    LCD_DataWrite(temp);
}

void RA8889_I2CM_Enable(void)
{
    /*  I2C master Interface Enable/Disable
            0: Disable (GPIO function)
            1: Enable (I2C master function)*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}

void RA8889_I2CM_Disable(void)
{
    /*  I2C master Interface Enable/Disable
            0: Disable (GPIO function)
            1: Enable (I2C master function)*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}

void Enable_SFlash_SPI(void)
{
    /*	Serial Flash or SPI Interface Enable/Disable
            0: Disable (GPIO function)
            1: Enable (SPI master function)
            When SDR SDRAM 32bits bus function enable, this bit is ignored
            & Serial flash pins become SDR SDRAM bus function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}

void Disable_SFlash_SPI(void)
{
    /*	Serial Flash or SPI Interface Enable/Disable
            0: Disable (GPIO function)
            1: Enable (SPI master function)
            When SDR SDRAM 32bits bus function enable, this bit is ignored
            & Serial flash pins become SDR SDRAM bus function.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Host_Bus_8bit(void)
{
    /*  Parallel Host Data Bus Width Selection
        0: 8-bit Parallel Host Data Bus.
        1: 16-bit Parallel Host Data Bus.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Host_Bus_16bit(void)
{
    /*  Parallel Host Data Bus Width Selection
        0: 8-bit Parallel Host Data Bus.
        1: 16-bit Parallel Host Data Bus.*/
    unsigned char temp;
    LCD_CmdWrite(0x01);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h][02h]

void Data_Format_8b_8bpp(void)
{
    /* MPU read/write data format when access memory data port.
    0xb: Direct write for
        all 8 bits MPU I/F,
        16 bits MPU I/F with 8bpp data mode 2,
        16 bits MPU I/F with 16bpp,
        16 bits MPU I/F with 24bpp data mode 1,
        and serial host interface.				 				*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Data_Format_8b_16bpp(void)
{
    /* MPU read/write data format when access memory data port.
    0xb: Direct write for
        all 8 bits MPU I/F,
        16 bits MPU I/F with 8bpp data mode 2,
        16 bits MPU I/F with 16bpp,
        16 bits MPU I/F with 24bpp data mode 1,
        and serial host interface.				 				*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Data_Format_8b_24bpp(void)
{
    /* MPU read/write data format when access memory data port.
    0xb: Direct write for
        all 8 bits MPU I/F,
        16 bits MPU I/F with 8bpp data mode 2,
        16 bits MPU I/F with 16bpp,
        16 bits MPU I/F with 24bpp data mode 1,
        and serial host interface.				 				*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}

void Data_Format_16b_8bpp(void)
{
    /* MPU read/write data format when access memory data port.
    10b: Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)	*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Data_Format_16b_16bpp(void)
{
    /* MPU read/write data format when access memory data port.
    0xb: Direct write for
        all 8 bits MPU I/F,
        16 bits MPU I/F with 8bpp data mode 2,
        16 bits MPU I/F with 16bpp,
        16 bits MPU I/F with 24bpp data mode 1,
        and serial host interface.				 				*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb7;
    //	temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Data_Format_16b_24bpp_mode1(void)
{
    /* MPU read/write data format when access memory data port.
    0xb: Direct write for
        all 8 bits MPU I/F,
        16 bits MPU I/F with 8bpp data mode 2,
        16 bits MPU I/F with 16bpp,
        16 bits MPU I/F with 24bpp data mode 1,
        and serial host interface.				 				*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb7;
    //	temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Data_Format_16b_24bpp_mode2(void)
{
    /* MPU read/write data format when access memory data port.
    11b: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)	*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb6;
    LCD_DataWrite(temp);
}

void MemRead_Left_Right_Top_Down(void)
{
    /* Host Read Memory Direction (Only for Graphic Mode)
    00b: Left .. Right then Top .. Bottom.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void MemRead_Right_Left_Top_Down(void)
{
    /* Host Read Memory Direction (Only for Graphic Mode)
    01b: Right .. Left then Top ..Bottom.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void MemRead_Top_Down_Left_Right(void)
{
    /* Host Read Memory Direction (Only for Graphic Mode)
    10b: Top .. Bottom then Left .. Right.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void MemRead_Down_Top_Left_Right(void)
{
    /* Host Read Memory Direction (Only for Graphic Mode)
    11b: Bottom .. Top then Left .. Right.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void MemWrite_Left_Right_Top_Down(void)
{
    /* Host Write Memory Direction (Only for Graphic Mode)
    00b: Left .. Right then Top ..Bottom.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb2;
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void MemWrite_Right_Left_Top_Down(void)
{
    /* Host Write Memory Direction (Only for Graphic Mode)
    01b: Right .. Left then Top .. Bottom.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp &= cClrb2;
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void MemWrite_Top_Down_Left_Right(void)
{
    /* Host Write Memory Direction (Only for Graphic Mode)
    10b: Top .. Bottom then Left .. Right.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb2;
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void MemWrite_Down_Top_Left_Right(void)
{
    /* Host Write Memory Direction (Only for Graphic Mode)
    11b: Bottom .. Top then Left .. Right.
    Ignored if canvas in linear addressing mode.		*/
    unsigned char temp;
    LCD_CmdWrite(0x02);
    temp = LCD_DataRead();
    temp |= cSetb2;
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
//[03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h][03h]
void Interrupt_Active_Low(void)
{
    /*  Output to MPU Interrupt active level
        0 : active low.
        1 : active high.						*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Interrupt_Active_High(void)
{
    /*  Output to MPU Interrupt active level
        0 : active low.
        1 : active high.						*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Debounce(void)
{
    /*  External interrupt input (XPS[0] pin) de-bounce
        0 : without de-bounce
        1 : enable de-bounce (1024 OSC clock)			*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Nodebounce(void)
{
    /*  External interrupt input (XPS[0] pin) de-bounce
        0 : without de-bounce
        1 : enable de-bounce (1024 OSC clock)			*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Low_Level_Trigger(void)
{
    /* External interrupt input (XPS[0] pin) trigger type
    00 : low level trigger
    */
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Input_High_Level_Trigger(void)
{
    /* External interrupt input (XPS[0] pin) trigger type
    01 : falling edge trigger
    */
    unsigned char temp;
    LCD_CmdWrite(0x03);

    temp = LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Falling_Edge_Trigger(void)
{
    /* External interrupt input (XPS[0] pin) trigger type
    10 : high level trigger
    */
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void ExtInterrupt_Input_Rising_Edge_Trigger(void)
{
    /* External interrupt input (XPS[0] pin) trigger type
    11 : rising edge trigger
    */
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void LVDS_Format1(void) // RA8877 only
{
    /* FPD-Link Data Format / LVDS Data Format
    0 : Format 1 (VESA format)
    1 : Format 2 (JEIDA format) 				*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void LVDS_Format2(void) // RA8877 only
{
    /* FPD-Link Data Format / LVDS Data Format
    0 : Format 1 (VESA format)
    1 : Format 2 (JEIDA format) 				*/
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Graphic_Mode(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Text_Mode(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Memory_Select_SDRAM(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Memory_Select_Gamma_Table(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Memory_Select_Graphic_Cursor_RAM(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Memory_Select_Color_Palette_RAM(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x03);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[05h]=========================================================================
//[06h]=========================================================================
//[07h]=========================================================================
//[08h]=========================================================================
//[09h]=========================================================================
//[0Ah]=========================================================================
//[0Bh]=========================================================================

void Enable_Resume_Interrupt(void)
{
    /*
    Wakeup/resume Interrupt Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Disable_Resume_Interrupt(void)
{
    /*
    Wakeup/resume Interrupt Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Enable_ExtInterrupt_Input(void)
{
    /*
    External Interrupt (PS[0] pin) Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Disable_ExtInterrupt_Input(void)
{
    /*
    External Interrupt (PS[0] pin) Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Enable_I2CM_Interrupt(void)
{
    /*
    I2C Master Interrupt Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Disable_I2CM_Interrupt(void)
{
    /*
    I2C Master Interrupt Enable
    0: Disable.
    1: Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Enable_Vsync_Interrupt(void)
{
    /*
    Vsync time base interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Disable_Vsync_Interrupt(void)
{
    /*
    Vsync time base interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Enable_KeyScan_Interrupt(void)
{
    /*
    Key Scan Interrupt Enable Bit
    0: Disable Key scan interrupt.
    1: Enable Key scan interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Disable_KeyScan_Interrupt(void)
{
    /*
    Key Scan Interrupt Enable Bit
    0: Disable Key scan interrupt.
    1: Enable Key scan interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Enable_DMA_Draw_BTE_Interrupt(void)
{
    /*
    Serial flash DMA Complete | Draw task finished | BTE Process
    Complete etc. Interrupt Enable
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Disable_DMA_Draw_BTE_Interrupt(void)
{
    /*
    Serial flash DMA Complete | Draw task finished | BTE Process
    Complete etc. Interrupt Enable
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Enable_PWM1_Interrupt(void)
{
    /*
    PWM timer 1 Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void Disable_PWM1_Interrupt(void)
{
    /*
    PWM timer 1 Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Enable_PWM0_Interrupt(void)
{
    /*
    PWM timer 0 Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Disable_PWM0_Interrupt(void)
{
    /*
    PWM timer 0 Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}

//[0Ch]=========================================================================
unsigned char Read_Interrupt_status(void)
{
    /*
    [Bit7]Read Function ..Resume Interrupt Status
    0: No Resume interrupt happens.
    1: Resume interrupt happens.
    [Bit6]Read Function .. PS[0] pin Interrupt Status
    0: No PS[0] pin interrupt happens.
    1: PS[0] pin interrupt happens.
    [Bit5]Read Function .. I2C master Interrupt Status
    0: No I2C master interrupt happens.
    1: I2C master interrupt happens.
    [Bit4]Read Function .. Vsync Interrupt Status
    0: No interrupt happens.
    1: interrupt happens.
    [Bit3]Read Function ..Key Scan Interrupt Status
    0: No Key Scan interrupt happens.
    1: Key Scan interrupt happens.
    [Bit2]Read Function..Interrupt Status
    0: No interrupt happens.
    1: interrupt happens.
    [Bit1]Read Function..Interrupt Status
    0: No interrupt happens.
    1: interrupt happens.
    [Bit0]Read Function..Interrupt Status
    0: No interrupt happens.
    1: interrupt happens.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    return temp;
}

void Check_Vsync_finished(void)
{
    /*[Bit4]
    Vsync Time base interrupt flag
    Read Function .. Vsync Interrupt Status
    0: No interrupt happens.
    1: interrupt happens.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    LCD_DataWrite(0x10);
    do {
        temp = LCD_DataRead();

    } while ((temp & 0x10) == 0x00);
}

void Clear_Resume_Interrupt_Flag(void)
{
    /*
    Resume Interrupt flag
    Write Function .. Resume Interrupt Clear Bit
    0: No operation.
    1: Clear Resume interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Clear_ExtInterrupt_Input_Flag(void)
{
    /*
    External Interrupt (PS[0] pin) flag
    Write Function .. PS[0] pin edge Interrupt Clear Bit
    0: No operation.
    1: Clear the PS[0] pin edge interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Clear_I2CM_Interrupt_Flag(void)
{
    /*
    I2C master Interrupt flag
    Write Function.. I2C master Interrupt Clear Bit
    0: No operation.
    1: Clear the I2C master interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Clear_Vsync_Interrupt_Flag(void)
{
    /*
    Vsync Time base interrupt flag
    Write Function ..Vsync Interrupt Clear Bit
    0: No operation.
    1: Clear the interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Clear_KeyScan_Interrupt_Flag(void)
{
    /*
    Key Scan Interrupt flag
    Write Function..Key Scan Interrupt Clear Bit
    0: No operation.
    1: Clear the Key Scan interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
unsigned char Chk_Key_Press(void)
{
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();

    if ((temp & 0x08) == 0x08)
        return 1;
    else
        return 0;
}

void Clear_DMA_Draw_BTE_Interrupt_Flag(void)
{
    /*
    Serial flash DMA Complete | Draw task finished | BTE
    Process Complete etc. Interrupt flag
    Write Function.. Interrupt Clear Bit
    0: No operation.
    1: Clear interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Clear_PWM1_Interrupt_Flag(void)
{
    /*
    PWM 1 timer Interrupt flag
    Write Function..Interrupt Clear Bit
    0: No operation.
    1: Clear interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void Clear_PWM0_Interrupt_Flag(void)
{
    /*
    PWM 0 timer Interrupt flag
    Write Function..Interrupt Clear Bit
    0: No operation.
    1: Clear interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
//[0Dh]=========================================================================
void XnINTR_Mask_Resume_Interrupt_Flag(void)
{
    /*
    Mask Resume Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_ExtInterrupt_Input_Flag(void)
{
    /*
    Mask External Interrupt (PS[0] pin) Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_I2CM_Interrupt_Flag(void)
{
    /*
    Mask I2C Master Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_Vsync_Interrupt_Flag(void)
{
    /*
    Mask Vsync time base interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_KeyScan_Interrupt_Flag(void)
{
    /*
    Mask Key Scan Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_DMA_Draw_BTE_Interrupt_Flag(void)
{
    /*
    Mask Serial flash DMA Complete | Draw task finished | BTE
    Process Complete etc. Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_PWM1_Interrupt_Flag(void)
{
    /*
    Mask PWM timer 1 Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void XnINTR_Mask_PWM0_Interrupt_Flag(void)
{
    /*
    Mask PWM timer 0 Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

void XnINTR_Unmask_Resume_Interrupt_Flag(void)
{
    /*
    Mask Resume Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_ExtInterrupt_Input_Flag(void)
{
    /*
    Mask External Interrupt (PS[0] pin) Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_I2CM_Interrupt_Flag(void)
{
    /*
    Mask I2C Master Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_Vsync_Interrupt_Flag(void)
{
    /*
    Mask Vsync time base interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_KeyScan_Interrupt_Flag(void)
{
    /*
    Mask Key Scan Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_DMA_Draw_BTE_Interrupt_Flag(void)
{
    /*
    Mask Serial flash DMA Complete | Draw task finished | BTE
    Process Complete etc. Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_PWM1_Interrupt_Flag(void)
{
    /*
    Mask PWM timer 1 Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void XnINTR_Unmask_PWM0_Interrupt_Flag(void)
{
    /*
    Mask PWM timer 0 Interrupt Flag
    0: Enable.
    1: Mask.
    */
    unsigned char temp;
    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}

//[0Eh]=========================================================================
void Enable_GPIOF_PullUp(void)
{
    /*
    GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Enable_GPIOE_PullUp(void)
{
    /*
    GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Enable_GPIOD_PullUp(void)
{
    /*
    GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Enable_GPIOC_PullUp(void)
{
    /*
    GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
    XnSFCS0, XMISO, XMOSI , XSCK)
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Enable_XDB15_8_PullUp(void)
{
    /*
    XDB[15:8] Pull-Up Enable
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void Enable_XDB7_0_PullUp(void)
{
    /*
    XDB[7:0] Pull-Up Enable
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Disable_GPIOF_PullUp(void)
{
    /*
    GPIO_F[7:0] Pull-Up Enable (XPDAT[23:19, 15:13])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Disable_GPIOE_PullUp(void)
{
    /*
    GPIO_E[7:0] Pull-Up Enable (XPDAT[12:10, 7:3])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Disable_GPIOD_PullUp(void)
{
    /*
    GPIO_D[7:0] Pull-Up Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Disable_GPIOC_PullUp(void)
{
    /*
    GPIO_C[6:0] Pull-Up Enable (XSDA, XSCL, XnSFCS1,
    XnSFCS0, XMISO, XMOSI , XSCK)
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Disable_XDB15_8_PullUp(void)
{
    /*
    XDB[15:8] Pull-Up Enable
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Disable_XDB7_0_PullUp(void)
{
    /*
    XDB[7:0] Pull-Up Enable
    0: Pull-Up Disable
    1: Pull-Up Enable
    */
    unsigned char temp;
    LCD_CmdWrite(0x0E);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
//[0Fh]=========================================================================
void XPDAT18_Set_GPIO_D7(void)
{
    /*
    XPDAT[18] – not scan function select
    0: GPIO-D7
    1: KOUT[4]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void XPDAT18_Set_KOUT4(void)
{
    /*
    XPDAT[18] – not scan function select
    0: GPIO-D7
    1: KOUT[4]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void XPDAT17_Set_GPIO_D5(void)
{
    /*
    XPDAT[17] – not scan function select
    0: GPIO-D5
    1: KOUT[2]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void XPDAT17_Set_KOUT2(void)
{
    /*
    XPDAT[17] – not scan function select
    0: GPIO-D5
    1: KOUT[2]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void XPDAT16_Set_GPIO_D4(void)
{
    /*
    XPDAT[16] – not scan function select
    0: GPIO-D4
    1: KOUT[1]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void XPDAT16_Set_KOUT1(void)
{
    /*
    XPDAT[16] – not scan function select
    0: GPIO-D4
    1: KOUT[1]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void XPDAT9_Set_GPIO_D3(void)
{
    /*
    XPDAT[9] – not scan function select
    0: GPIO-D3
    1: KOUT[3]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void XPDAT9_Set_KOUT3(void)
{
    /*
    XPDAT[9] – not scan function select
    0: GPIO-D3
    1: KOUT[3]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void XPDAT8_Set_GPIO_D2(void)
{
    /*
    XPDAT[8] – not scan function select
    0: GPIO-D2
    1: KIN[3]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void XPDAT8_Set_KIN3(void)
{
    /*
    XPDAT[8] – not scan function select
    0: GPIO-D2
    1: KIN[3]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void XPDAT2_Set_GPIO_D6(void)
{
    /*
    XPDAT[2] – not scan function select
    0: GPIO-D6
    1: KIN[4]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void XPDAT2_Set_KIN4(void)
{
    /*
    XPDAT[2] – not scan function select
    0: GPIO-D6
    1: KIN[4]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void XPDAT1_Set_GPIO_D1(void)
{
    /*
    XPDAT[1] – not scan function select
    0: GPIO-D1
    1: KIN[2]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void XPDAT1_Set_KIN2(void)
{
    /*
    XPDAT[1] – not scan function select
    0: GPIO-D1
    1: KIN[2]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void XPDAT0_Set_GPIO_D0(void)
{
    /*
    XPDAT[0] – not scan function select
    0: GPIO-D0
    1: KIN[1]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void XPDAT0_Set_KIN1(void)
{
    /*
    XPDAT[0] – not scan function select
    0: GPIO-D0
    1: KIN[1]
    */
    unsigned char temp;
    LCD_CmdWrite(0x0F);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[10h]=========================================================================
void Enable_PIP1(void)
{
    /*
    PIP 1 window Enable/Disable
    0 : PIP 1 window disable.
    1 : PIP 1 window enable
    PIP 1 window always on top of PIP 2 window.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Disable_PIP1(void)
{
    /*
    PIP 1 window Enable/Disable
    0 : PIP 1 window disable.
    1 : PIP 1 window enable
    PIP 1 window always on top of PIP 2 window.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Enable_PIP2(void)
{
    /*
    PIP 2 window Enable/Disable
    0 : PIP 2 window disable.
    1 : PIP 2 window enable
    PIP 1 window always on top of PIP 2 window.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Disable_PIP2(void)
{
    /*
    PIP 2 window Enable/Disable
    0 : PIP 2 window disable.
    1 : PIP 2 window enable
    PIP 1 window always on top of PIP 2 window.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Select_PIP1_Parameter(void)
{
    /*
    0: To configure PIP 1’s parameters.
    1: To configure PIP 2’s parameters..
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Select_PIP2_Parameter(void)
{
    /*
    0: To configure PIP 1’s parameters.
    1: To configure PIP 2’s parameters..
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Select_Main_Window_8bpp(void)
{
    /*
    Main Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_Main_Window_16bpp(void)
{
    /*
    Main Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Select_Main_Window_24bpp(void)
{
    /*
    Main Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}

void Select_LCD_Sync_Mode(void)
{
    /*
    To Control panel's synchronous signals
    0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
    1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Select_LCD_DE_Mode(void)
{
    /*
    To Control panel's synchronous signals
    0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
    1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
    */
    unsigned char temp;
    LCD_CmdWrite(0x10);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[11h]=========================================================================
void Select_PIP1_Window_8bpp(void)
{
    /*
    PIP 1 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_PIP1_Window_16bpp(void)
{
    /*
    PIP 1 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Select_PIP1_Window_24bpp(void)
{
    /*
    PIP 1 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp |= cSetb3;
    //    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_PIP2_Window_8bpp(void)
{
    /*
    PIP 2 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Select_PIP2_Window_16bpp(void)
{
    /*
    PIP 2 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Select_PIP2_Window_24bpp(void)
{
    /*
    PIP 2 Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    unsigned char temp;
    LCD_CmdWrite(0x11);
    temp = LCD_DataRead();
    temp |= cSetb1;
    //    temp &= cClrb0;
    LCD_DataWrite(temp);
}

//[12h]=========================================================================
void PCLK_Rising(void)
{
    /*
    PCLK Inversion
    0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
    1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
    */
    unsigned char temp;
    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void PCLK_Falling(void)
{
    /*
    PCLK Inversion
    0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
    1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
    */
    unsigned char temp;
    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Display_ON(void)
{
    /*
    Display ON/OFF
    0b: Display Off.
    1b: Display On.
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}

void Display_OFF(void)
{
    /*
    Display ON/OFF
    0b: Display Off.
    1b: Display On.
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Color_Bar_ON(void)
{
    /*
    Display Test Color Bar
    0b: Disable.
    1b: Enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Color_Bar_OFF(void)
{
    /*
    Display Test Color Bar
    0b: Disable.
    1b: Enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}

void HSCAN_L_to_R(void)
{
    /*
    Horizontal Scan direction
    0 : From Left to Right
    1 : From Right to Left
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void HSCAN_R_to_L(void)
{
    /*
    Horizontal Scan direction
    0 : From Left to Right
    1 : From Right to Left
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void VSCAN_T_to_B(void)
{
    /*
    Vertical Scan direction
    0 : From Top to Bottom
    1 : From bottom to Top
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void VSCAN_B_to_T(void)
{
    /*
    Vertical Scan direction
    0 : From Top to Bottom
    1 : From bottom to Top
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void PDATA_Set_RGB(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    LCD_DataWrite(temp);
}
void PDATA_Set_RBG(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void PDATA_Set_GRB(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void PDATA_Set_GBR(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void PDATA_Set_BRG(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void PDATA_Set_BGR(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb2;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void PDATA_Set_Gray(void)
{
    /*parallel PDATA[23:0] Output Sequence
    000b : RGB
    001b : RBG
    010b : GRB
    011b : GBR
    100b : BRG
    101b : BGR
    110b : Gray
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp &= 0xf8;
    temp |= cSetb2;
    temp |= cSetb1;
    LCD_DataWrite(temp);
}

void PDATA_IDLE_STATE(void)
{
    /*
    111b : XPDAT pins send out PDAT idle state (all 0 or 1, black or white color).
    */
    unsigned char temp;

    LCD_CmdWrite(0x12);
    temp = LCD_DataRead();
    temp |= 0x07;
    LCD_DataWrite(temp);
}

//[13h]=========================================================================
void HSYNC_Low_Active(void)
{
    /*
    HSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void HSYNC_High_Active(void)
{
    /*
    HSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void VSYNC_Low_Active(void)
{
    /*
    VSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void VSYNC_High_Active(void)
{
    /*
    VSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void DE_Low_Active(void)
{
    /*
    DE Polarity
    0 : High active.
    1 : Low active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void DE_High_Active(void)
{
    /*
    DE Polarity
    0 : High active.
    1 : Low active.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Idle_DE_Low(void)
{
    /*
    DE IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “DE” output is low.
    1 : Pin “DE” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Idle_DE_High(void)
{
    /*
    DE IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “DE” output is low.
    1 : Pin “DE” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Idle_PCLK_Low(void)
{
    /*
    PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “PCLK” output is low.
    1 : Pin “PCLK” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Idle_PCLK_High(void)
{
    /*
    PCLK IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “PCLK” output is low.
    1 : Pin “PCLK” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Idle_PDAT_Low(void)
{
    /*
    PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pins “PDAT[23:0]” output is low.
    1 : Pins “PCLK[23:0]” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Idle_PDAT_High(void)
{
    /*
    PDAT IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pins “PDAT[23:0]” output is low.
    1 : Pins “PCLK[23:0]” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Idle_HSYNC_Low(void)
{
    /*
    HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “HSYNC” output is low.
    1 : Pin “HSYNC” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Idle_HSYNC_High(void)
{
    /*
    HSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “HSYNC” output is low.
    1 : Pin “HSYNC” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void Idle_VSYNC_Low(void)
{
    /*
    VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “VSYNC” output is low.
    1 : Pin “VSYNC” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Idle_VSYNC_High(void)
{
    /*
    VSYNC IDLE STATE(When STANDBY or DISPLAY OFF )
    0 : Pin “VSYNC” output is low.
    1 : Pin “VSYNC” output is high.
    */
    unsigned char temp;

    LCD_CmdWrite(0x13);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[14h][15h][1Ah][1Bh]=========================================================================
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX, unsigned short HY)
{
    /*
    [14h] Horizontal Display Width Setting Bit[7:0]
    [15h] Horizontal Display Width Fine Tuning (HDWFT) [3:0]
    The register specifies the LCD panel horizontal display width in
    the unit of 8 pixels resolution.
    Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR

    [1Ah] Vertical Display Height Bit[7:0]
    Vertical Display Height(Line) = VDHR + 1
    [1Bh] Vertical Display Height Bit[10:8]
    Vertical Display Height(Line) = VDHR + 1
    */
    unsigned char temp;
    if (WX < 8) {
        LCD_CmdWrite(0x14);
        LCD_DataWrite(0x00);

        LCD_CmdWrite(0x15);
        LCD_DataWrite(WX);

        temp = HY - 1;
        LCD_CmdWrite(0x1A);
        LCD_DataWrite(temp);

        temp = (HY - 1) >> 8;
        LCD_CmdWrite(0x1B);
        LCD_DataWrite(temp);
    } else {
        temp = (WX / 8) - 1;
        LCD_CmdWrite(0x14);
        LCD_DataWrite(temp);

        temp = WX % 8;
        LCD_CmdWrite(0x15);
        LCD_DataWrite(temp);

        temp = HY - 1;
        LCD_CmdWrite(0x1A);
        LCD_DataWrite(temp);

        temp = (HY - 1) >> 8;
        LCD_CmdWrite(0x1B);
        LCD_DataWrite(temp);
    }
}
//[16h][17h]=========================================================================
void LCD_Horizontal_Non_Display(unsigned short WX)
{
    /*
    [16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
    This register specifies the horizontal non-display period. Also
    called back porch.
    Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR

    [17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
    This register specifies the fine tuning for horizontal non-display
    period; it is used to support the SYNC mode panel. Each level of
    this modulation is 1-pixel.
    Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
    */
    unsigned char temp;
    if (WX < 8) {
        LCD_CmdWrite(0x16);
        LCD_DataWrite(0x00);

        LCD_CmdWrite(0x17);
        LCD_DataWrite(WX);
    } else {
        temp = (WX / 8) - 1;
        LCD_CmdWrite(0x16);
        LCD_DataWrite(temp);

        temp = WX % 8;
        LCD_CmdWrite(0x17);
        LCD_DataWrite(temp);
    }
}
//[18h]=========================================================================
void LCD_HSYNC_Start_Position(unsigned short WX)
{
    /*
    [18h] HSYNC Start Position[4:0]
    The starting position from the end of display area to the
    beginning of HSYNC. Each level of this modulation is 8-pixel.
    Also called front porch.
    HSYNC Start Position(pixels) = (HSTR + 1)x8
    */
    unsigned char temp;
    if (WX < 8) {
        LCD_CmdWrite(0x18);
        LCD_DataWrite(0x00);
    } else {
        temp = (WX / 8) - 1;
        LCD_CmdWrite(0x18);
        LCD_DataWrite(temp);
    }
}
//[19h]=========================================================================
void LCD_HSYNC_Pulse_Width(unsigned short WX)
{
    /*
    [19h] HSYNC Pulse Width(HPW) [4:0]
    The period width of HSYNC.
    HSYNC Pulse Width(pixels) = (HPW + 1)x8
    */
    unsigned char temp;
    if (WX < 8) {
        LCD_CmdWrite(0x19);
        LCD_DataWrite(0x00);
    } else {
        temp = (WX / 8) - 1;
        LCD_CmdWrite(0x19);
        LCD_DataWrite(temp);
    }
}
//[1Ch][1Dh]=========================================================================
void LCD_Vertical_Non_Display(unsigned short HY)
{
    /*
    [1Ch] Vertical Non-Display Period Bit[7:0]
    Vertical Non-Display Period(Line) = (VNDR + 1)

    [1Dh] Vertical Non-Display Period Bit[9:8]
    Vertical Non-Display Period(Line) = (VNDR + 1)
    */
    unsigned short temp;
    temp = HY - 1;
    LCD_CmdWrite(0x1C);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0x1D);
    LCD_DataWrite(temp >> 8);
}
//[1Eh]=========================================================================
void LCD_VSYNC_Start_Position(unsigned short HY)
{
    /*
    [1Eh] VSYNC Start Position[7:0]
    The starting position from the end of display area to the beginning of VSYNC.
    VSYNC Start Position(Line) = (VSTR + 1)
    */
    unsigned char temp;
    temp = HY - 1;
    LCD_CmdWrite(0x1E);
    LCD_DataWrite(temp);
}
//[1Fh]=========================================================================
void LCD_VSYNC_Pulse_Width(unsigned short HY)
{
    /*
    [1Fh] VSYNC Pulse Width[5:0]
    The pulse width of VSYNC in lines.
    VSYNC Pulse Width(Line) = (VPWR + 1)
    */
    unsigned char temp;
    temp = HY - 1;
    LCD_CmdWrite(0x1F);
    LCD_DataWrite(temp);
}
//[20h][21h][22h][23h]=========================================================================
void Main_Image_Start_Address(unsigned long Addr)
{
    /*
    [20h] Main Image Start Address[7:2]
    [21h] Main Image Start Address[15:8]
    [22h] Main Image Start Address [23:16]
    [23h] Main Image Start Address [31:24]
    */
    LCD_CmdWrite(0x20);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x21);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x22);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x23);
    LCD_DataWrite(Addr >> 24);
}
//[24h][25h]=========================================================================
void Main_Image_Width(unsigned short WX)
{
    /*
    [24h] Main Image Width [7:0]
    [25h] Main Image Width [12:8]
    Unit: Pixel.
    It must be divisible by 4. MIW Bit [1:0] tie to “0” internally.
    The value is physical pixel number. Maximum value is 8188 pixels
    */
    LCD_CmdWrite(0x24);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x25);
    LCD_DataWrite(WX >> 8);
}
//[26h][27h][28h][29h]=========================================================================
void Main_Window_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [26h] Main Window Upper-Left corner X-coordinate [7:0]
    [27h] Main Window Upper-Left corner X-coordinate [12:8]
    Reference Main Image coordinate.
    Unit: Pixel
    It must be divisible by 4. MWULX Bit [1:0] tie to “0” internally.
    X-axis coordinate plus Horizontal display width cannot large than 8188.

    [28h] Main Window Upper-Left corner Y-coordinate [7:0]
    [29h] Main Window Upper-Left corner Y-coordinate [12:8]
    Reference Main Image coordinate.
    Unit: Pixel
    Range is between 0 and 8191.
    */
    LCD_CmdWrite(0x26);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x27);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x28);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x29);
    LCD_DataWrite(HY >> 8);
}
//[2Ah][2Bh][2Ch][2Dh]=========================================================================
void PIP_Display_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [2Ah] PIP Window Display Upper-Left corner X-coordinate [7:0]
    [2Bh] PIP Window Display Upper-Left corner X-coordinate [12:8]
    Reference Main Window coordinate.
    Unit: Pixel
    It must be divisible by 4. PWDULX Bit [1:0] tie to “0” internally.
    X-axis coordinate should less than horizontal display width.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.

    [2Ch] PIP Window Display Upper-Left corner Y-coordinate [7:0]
    [2Dh] PIP Window Display Upper-Left corner Y-coordinate [12:8]
    Reference Main Window coordinate.
    Unit: Pixel
    Y-axis coordinate should less than vertical display height.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.
    */
    LCD_CmdWrite(0x2A);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x2B);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x2C);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x2D);
    LCD_DataWrite(HY >> 8);
}
//[2Eh][2Fh][30h][31h]=========================================================================
void PIP_Image_Start_Address(unsigned long Addr)
{
    /*
    [2Eh] PIP Image Start Address[7:2]
    [2Fh] PIP Image Start Address[15:8]
    [30h] PIP Image Start Address [23:16]
    [31h] PIP Image Start Address [31:24]
    */
    LCD_CmdWrite(0x2E);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x2F);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x30);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x31);
    LCD_DataWrite(Addr >> 24);
}
//[32h][33h]=========================================================================
void PIP_Image_Width(unsigned short WX)
{
    /*
    [32h] PIP Image Width [7:0]
    [33h] PIP Image Width [12:8]
    Unit: Pixel.
    It must be divisible by 4. PIW Bit [1:0] tie to “0” internally.
    The value is physical pixel number.
    This width should less than horizontal display width.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.
    */
    LCD_CmdWrite(0x32);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x33);
    LCD_DataWrite(WX >> 8);
}
//[34h][35h][36h][37h]=========================================================================
void PIP_Window_Image_Start_XY(unsigned short WX, unsigned short HY)
{

    /*
    [34h] PIP 1 or 2 Window Image Upper-Left corner X-coordinate [7:0]
    [35h] PIP Window Image Upper-Left corner X-coordinate [12:8]
    Reference PIP Image coordinate.
    Unit: Pixel
    It must be divisible by 4. PWIULX Bit [1:0] tie to “0” internally.
    X-axis coordinate plus PIP image width cannot large than 8188.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.

    [36h] PIP Windows Display Upper-Left corner Y-coordinate [7:0]
    [37h] PIP Windows Image Upper-Left corner Y-coordinate [12:8]
    Reference PIP Image coordinate.
    Unit: Pixel
    Y-axis coordinate plus PIP window height should less than 8191.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.
    */
    LCD_CmdWrite(0x34);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x35);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x36);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x37);
    LCD_DataWrite(HY >> 8);
}
//[38h][39h][3Ah][3Bh]=========================================================================
void PIP_Window_Width_Height(unsigned short WX, unsigned short HY)
{
    /*
    [38h] PIP Window Width [7:0]
    [39h] PIP Window Width [10:8]
    Unit: Pixel.
    It must be divisible by 4. The value is physical pixel number.
    Maximum value is 8188 pixels.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.

    [3Ah] PIP Window Height [7:0]
    [3Bh] PIP Window Height [10:8]
    Unit: Pixel
    The value is physical pixel number. Maximum value is 8191 pixels.
    According to bit of Select Configure PIP 1 or 2 Window’s parameters.
    Function bit will be configured for relative PIP window.
    */
    LCD_CmdWrite(0x38);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x39);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x3A);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x3B);
    LCD_DataWrite(HY >> 8);
}

//[3Ch]=========================================================================
void Enable_Gamma_Correction(void)
{
    /*
    Gamma correction Enable
    0: Disable
    1: Enable
    Gamma correction is the last output stage.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Disable_Gamma_Correction(void)
{
    /*
    Gamma correction Enable
    0: Disable
    1: Enable
    Gamma correction is the last output stage.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Gamma_Table_for_Blue(void)
{
    /*
    Gamma table select for MPU write gamma data
    00b: Gamma table for Blue
    01b: Gamma table for Green
    10b: Gamma table for Red
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Gamma_Table_for_Green(void)
{
    /*
    Gamma table select for MPU write gamma data
    00b: Gamma table for Blue
    01b: Gamma table for Green
    10b: Gamma table for Red
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb6;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Gamma_Table_for_Red(void)
{
    /*
    Gamma table select for MPU write gamma data
    00b: Gamma table for Blue
    01b: Gamma table for Green
    10b: Gamma table for Red
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}

void Enable_Graphic_Cursor(void)
{
    /*
    Graphic Cursor Enable
    0 : Graphic Cursor disable.
    1 : Graphic Cursor enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Disable_Graphic_Cursor(void)
{
    /*
    Graphic Cursor Enable
    0 : Graphic Cursor disable.
    1 : Graphic Cursor enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
//
void Select_Graphic_Cursor_1(void)
{
    /*
    Graphic Cursor Selection Bit
    Select one from four graphic cursor types. (00b to 11b)
    00b : Graphic Cursor Set 1.
    01b : Graphic Cursor Set 2.
    10b : Graphic Cursor Set 3.
    11b : Graphic Cursor Set 4.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_2(void)
{
    /*
    Graphic Cursor Selection Bit
    Select one from four graphic cursor types. (00b to 11b)
    00b : Graphic Cursor Set 1.
    01b : Graphic Cursor Set 2.
    10b : Graphic Cursor Set 3.
    11b : Graphic Cursor Set 4.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_3(void)
{
    /*
    Graphic Cursor Selection Bit
    Select one from four graphic cursor types. (00b to 11b)
    00b : Graphic Cursor Set 1.
    01b : Graphic Cursor Set 2.
    10b : Graphic Cursor Set 3.
    11b : Graphic Cursor Set 4.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_Graphic_Cursor_4(void)
{
    /*
    Graphic Cursor Selection Bit
    Select one from four graphic cursor types. (00b to 11b)
    00b : Graphic Cursor Set 1.
    01b : Graphic Cursor Set 2.
    10b : Graphic Cursor Set 3.
    11b : Graphic Cursor Set 4.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor(void)
{
    /*
    Text Cursor Enable
    0 : Disable.
    1 : Enable.
    Text cursor & Graphic cursor cannot enable simultaneously.
    Graphic cursor has higher priority then Text cursor if enabled simultaneously.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void Disable_Text_Cursor(void)
{
    /*
    Text Cursor Enable
    0 : Disable.
    1 : Enable.
    Text cursor & Graphic cursor cannot enable simultaneously.
    Graphic cursor has higher priority then Text cursor if enabled simultaneously.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
//
void Enable_Text_Cursor_Blinking(void)
{
    /*
    Text Cursor Blinking Enable
    0 : Disable.
    1 : Enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Disable_Text_Cursor_Blinking(void)
{
    /*
    Text Cursor Blinking Enable
    0 : Disable.
    1 : Enable.
    */
    unsigned char temp;

    LCD_CmdWrite(0x3C);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
//[3Dh]=========================================================================
void Blinking_Time_Frames(unsigned char temp)
{
    /*
    Text Cursor Blink Time Setting (Unit: Frame)
    00h : 1 frame time.
    01h : 2 frames time.
    02h : 3 frames time.
    :
    FFh : 256 frames time.
    */
    LCD_CmdWrite(0x3D);
    LCD_DataWrite(temp);
}
//[3Eh]=========================================================================
void Text_Cursor_H_V(unsigned short WX, unsigned short HY)
{
    /*
    [3Eh]
    Text Cursor Horizontal Size Setting[4:0]
    Unit : Pixel
    Zero-based number. Value “0” means 1 pixel.
    Note : When font is enlarged, the cursor setting will multiply the
    same times as the font enlargement.
    [3Fh]
    Text Cursor Vertical Size Setting[4:0]
    Unit : Pixel
    Zero-based number. Value “0” means 1 pixel.
    Note : When font is enlarged, the cursor setting will multiply the
    same times as the font enlargement.
    */
    LCD_CmdWrite(0x3E);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x3F);
    LCD_DataWrite(HY);
}
//[40h][41h][42h][43h]=========================================================================
void Graphic_Cursor_XY(unsigned short WX, unsigned short HY)
{
    /*
    [40h] Graphic Cursor Horizontal Location[7:0]
    [41h] Graphic Cursor Horizontal Location[12:8]
    [42h] Graphic Cursor Vertical Location[7:0]
    [43h] Graphic Cursor Vertical Location[12:8]
    Reference main Window coordinate.
    */
    LCD_CmdWrite(0x40);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x41);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x42);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x43);
    LCD_DataWrite(HY >> 8);
}
//[44h]=========================================================================
void Set_Graphic_Cursor_Color_1(unsigned char temp)
{
    /*
    [44h] Graphic Cursor Color 0 with 256 Colors
    RGB Format [7:0] = RRRGGGBB.
    */
    LCD_RegisterWrite(0x44, temp);
}
//[45h]=========================================================================
void Set_Graphic_Cursor_Color_2(unsigned char temp)
{
    /*
    [45h] Graphic Cursor Color 1 with 256 Colors
    RGB Format [7:0] = RRRGGGBB.
    */
    LCD_RegisterWrite(0x45, temp);
}

//[50h][51h][52h][53h]=========================================================================
void Canvas_Image_Start_address(unsigned long Addr)
{
    /*
    [50h] Start address of Canvas [7:0]
    [51h] Start address of Canvas [15:8]
    [52h] Start address of Canvas [23:16]
    [53h] Start address of Canvas [31:24]
    */
    LCD_CmdWrite(0x50);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x51);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x52);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x53);
    LCD_DataWrite(Addr >> 24);
}
//[54h][55h]=========================================================================
void Canvas_image_width(unsigned short WX)
{
    /*
    [54h] Canvas image width [7:2]
    [55h] Canvas image width [12:8]
    */
    LCD_CmdWrite(0x54);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x55);
    LCD_DataWrite(WX >> 8);
}
//[56h][57h][58h][59h]=========================================================================
void Active_Window_XY(unsigned short WX, unsigned short HY)
{
    /*
    [56h] Active Window Upper-Left corner X-coordinate [7:0]
    [57h] Active Window Upper-Left corner X-coordinate [12:8]
    [58h] Active Window Upper-Left corner Y-coordinate [7:0]
    [59h] Active Window Upper-Left corner Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x56);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x57);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x58);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x59);
    LCD_DataWrite(HY >> 8);
}
//[5Ah][5Bh][5Ch][5Dh]=========================================================================
void Active_Window_WH(unsigned short WX, unsigned short HY)
{
    /*
    [5Ah] Width of Active Window [7:0]
    [5Bh] Width of Active Window [12:8]
    [5Ch] Height of Active Window [7:0]
    [5Dh] Height of Active Window [12:8]
    */
    LCD_CmdWrite(0x5A);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x5B);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x5C);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x5D);
    LCD_DataWrite(HY >> 8);
}
//[5Eh]=========================================================================

void Memory_XY_Mode(void)
{
    /*
    Canvas addressing mode
    0: Block mode (X-Y coordinate addressing)
    1: linear mode
    */
    unsigned char temp;

    LCD_CmdWrite(0x5E);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Memory_Linear_Mode(void)
{
    /*
    Canvas addressing mode
    0: Block mode (X-Y coordinate addressing)
    1: linear mode
    */
    unsigned char temp;

    LCD_CmdWrite(0x5E);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Memory_8bpp_Mode(void)
{
    /*
    Canvas image’s color depth & memory R/W data width
    In Block Mode:
    00: 8bpp
    01: 16bpp
    1x: 24bpp
    In Linear Mode:
    x0: 8-bits memory data read/write.
    x1: 16-bits memory data read/write
    */
    unsigned char temp;

    LCD_CmdWrite(0x5E);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Memory_16bpp_Mode(void)
{
    /*
    Canvas image’s color depth & memory R/W data width
    In Block Mode:
    00: 8bpp
    01: 16bpp
    1x: 24bpp
    In Linear Mode:
    x0: 8-bits memory data read/write.
    x1: 16-bits memory data read/write
    */
    unsigned char temp;

    LCD_CmdWrite(0x5E);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Memory_24bpp_Mode(void)
{
    /*
    Canvas image’s color depth & memory R/W data width
    In Block Mode:
    00: 8bpp
    01: 16bpp
    1x: 24bpp
    In Linear Mode:
    x0: 8-bits memory data read/write.
    x1: 16-bits memory data read/write
    */
    unsigned char temp;

    LCD_CmdWrite(0x5E);
    temp = LCD_DataRead();
    temp |= cSetb1;
    //	temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[5Fh][60h][61h][62h]=========================================================================
void Goto_Pixel_XY(unsigned short WX, unsigned short HY)
{
    /*
    Set Graphic Read/Write position

    REG[5Eh] bit3, Select to read back Graphic Read/Write position.
    When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
    When DPRAM Active window mode:Graphic Read/Write
    Horizontal Position [12:8][7:0],
    Vertical Position [12:8][7:0].
    Reference Canvas image coordinate. Unit: Pixel
    */
    LCD_CmdWrite(0x5F);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x60);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x61);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x62);
    LCD_DataWrite(HY >> 8);
}
void Goto_Linear_Addr(unsigned long Addr)
{
    /*
    Set Graphic Read/Write position

    REG[5Eh] bit3, Select to read back Graphic Read/Write position.
    When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
    When DPRAM Active window mode:Graphic Read/Write
    Horizontal Position [12:8][7:0],
    Vertical Position [12:8][7:0].
    Reference Canvas image coordinate. Unit: Pixel
    */
    LCD_CmdWrite(0x5F);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x60);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x61);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x62);
    LCD_DataWrite(Addr >> 24);
}

//[63h][64h][65h][66h]=========================================================================
void Goto_Text_XY(unsigned short WX, unsigned short HY)
{
    /*
    Set Text Write position
    Text Write X-coordinate [12:8][7:0]
    Text Write Y-coordinate [12:8][7:0]
    Reference Canvas image coordinate.
    Unit: Pixel
    */
    LCD_CmdWrite(0x63);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x64);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x65);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x66);
    LCD_DataWrite(HY >> 8);
}

//[67h]=========================================================================
/*
[bit7]Draw Line / Triangle Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit5]Fill function for Triangle Signal
0 : Non fill.
1 : Fill.
[bit1]Draw Triangle or Line Select Signal
0 : Draw Line
1 : Draw Triangle
*/
void Draw_Enable_AA(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x67);
    temp = LCD_DataRead();

    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Draw_Disable_AA(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x67);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Start_Line(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x67);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb7;
    LCD_DataWrite(temp);

    Check_2D_Busy();
}
void Start_Triangle(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x67);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb5;
    //	LCD_DataWrite(temp);
    temp |= cSetb7;
    LCD_DataWrite(temp);

    Check_2D_Busy();
}
void Start_Triangle_Fill(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x67);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb5;
    //	LCD_DataWrite(temp);
    temp |= cSetb7;
    LCD_DataWrite(temp);

    Check_2D_Busy();
}
//[68h][69h][6Ah][6Bh]=========================================================================
// 線起點
void Line_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [68h] Draw Line/Square/Triangle Start X-coordinate [7:0]
    [69h] Draw Line/Square/Triangle Start X-coordinate [12:8]
    [6Ah] Draw Line/Square/Triangle Start Y-coordinate [7:0]
    [6Bh] Draw Line/Square/Triangle Start Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x68);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x69);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6A);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6B);
    LCD_DataWrite(HY >> 8);
}
//[6Ch][6Dh][6Eh][6Fh]=========================================================================
// 線終點
void Line_End_XY(unsigned short WX, unsigned short HY)
{
    /*
    [6Ch] Draw Line/Square/Triangle End X-coordinate [7:0]
    [6Dh] Draw Line/Square/Triangle End X-coordinate [12:8]
    [6Eh] Draw Line/Square/Triangle End Y-coordinate [7:0]
    [6Fh] Draw Line/Square/Triangle End Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x6C);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x6D);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6E);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6F);
    LCD_DataWrite(HY >> 8);
}
//[68h]~[73h]=========================================================================
// 三角-點1
void Triangle_Point1_XY(unsigned short WX, unsigned short HY)
{
    /*
    [68h] Draw Line/Square/Triangle Start X-coordinate [7:0]
    [69h] Draw Line/Square/Triangle Start X-coordinate [12:8]
    [6Ah] Draw Line/Square/Triangle Start Y-coordinate [7:0]
    [6Bh] Draw Line/Square/Triangle Start Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x68);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x69);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6A);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6B);
    LCD_DataWrite(HY >> 8);
}
// 三角-點2
void Triangle_Point2_XY(unsigned short WX, unsigned short HY)
{
    /*
    [6Ch] Draw Line/Square/Triangle End X-coordinate [7:0]
    [6Dh] Draw Line/Square/Triangle End X-coordinate [12:8]
    [6Eh] Draw Line/Square/Triangle End Y-coordinate [7:0]
    [6Fh] Draw Line/Square/Triangle End Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x6C);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x6D);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6E);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6F);
    LCD_DataWrite(HY >> 8);
}
// 三角-點3
void Triangle_Point3_XY(unsigned short WX, unsigned short HY)
{
    /*
    [70h] Draw Triangle Point 3 X-coordinate [7:0]
    [71h] Draw Triangle Point 3 X-coordinate [12:8]
    [72h] Draw Triangle Point 3 Y-coordinate [7:0]
    [73h] Draw Triangle Point 3 Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x70);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x71);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x72);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x73);
    LCD_DataWrite(HY >> 8);
}
// 方起點
void Square_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [68h] Draw Line/Square/Triangle Start X-coordinate [7:0]
    [69h] Draw Line/Square/Triangle Start X-coordinate [12:8]
    [6Ah] Draw Line/Square/Triangle Start Y-coordinate [7:0]
    [6Bh] Draw Line/Square/Triangle Start Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x68);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x69);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6A);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6B);
    LCD_DataWrite(HY >> 8);
}
// 方終點
void Square_End_XY(unsigned short WX, unsigned short HY)
{
    /*
    [6Ch] Draw Line/Square/Triangle End X-coordinate [7:0]
    [6Dh] Draw Line/Square/Triangle End X-coordinate [12:8]
    [6Eh] Draw Line/Square/Triangle End Y-coordinate [7:0]
    [6Fh] Draw Line/Square/Triangle End Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x6C);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x6D);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x6E);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x6F);
    LCD_DataWrite(HY >> 8);
}
//[76h]=========================================================================
/*
[bit7]
Draw Circle / Ellipse / Square /Circle Square Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit6]
Fill the Circle / Ellipse / Square / Circle Square Signal
0 : Non fill.
1 : fill.
[bit5 bit4]
Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
00 : Draw Circle / Ellipse
01 : Draw Circle / Ellipse Curve
10 : Draw Square.
11 : Draw Circle Square.
[bit1 bit0]
Draw Circle / Ellipse Curve Part Select
00 : bottom-left Ellipse Curve
01 : upper-left Ellipse Curve
10 : upper-right Ellipse Curve
11 : bottom-right Ellipse Curve
*/
void Start_Circle_or_Ellipse(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0x80); // B1000_XXXX

    Check_2D_Busy();
}
void Start_Circle_or_Ellipse_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xC0); // B1100_XXXX

    Check_2D_Busy();
}
//
void Start_Left_Down_Curve(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0x90); // B1001_XX00

    Check_2D_Busy();
}
void Start_Left_Up_Curve(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0x91); // B1001_XX01

    Check_2D_Busy();
}
void Start_Right_Up_Curve(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0x92); // B1001_XX10

    Check_2D_Busy();
}
void Start_Right_Down_Curve(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0x93); // B1001_XX11

    Check_2D_Busy();
}
//
void Start_Left_Down_Curve_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xD0); // B1101_XX00

    Check_2D_Busy();
}
void Start_Left_Up_Curve_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xD1); // B1101_XX01

    Check_2D_Busy();
}
void Start_Right_Up_Curve_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xD2); // B1101_XX10

    Check_2D_Busy();
}
void Start_Right_Down_Curve_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xD3); // B1101_XX11

    Check_2D_Busy();
}
//
void Start_Square(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xA0); // B1010_XXXX

    Check_2D_Busy();
}
void Start_Square_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xE0); // B1110_XXXX

    Check_2D_Busy();
}
void Start_Circle_Square(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xB0); // B1011_XXXX

    Check_2D_Busy();
}
void Start_Circle_Square_Fill(void)
{
    LCD_CmdWrite(0x76);
    LCD_DataWrite(0xF0); // B1111_XXXX

    Check_2D_Busy();
}
//[77h]~[7Eh]=========================================================================
// 圓半徑
void Circle_Radius_R(unsigned short WX)
{
    /*
    [77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
    [78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
    [79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
    [7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
    */
    LCD_CmdWrite(0x77);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x78);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x79);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x7A);
    LCD_DataWrite(WX >> 8);
}

// 橢圓半徑
void Ellipse_Radius_RxRy(unsigned short WX, unsigned short HY)
{
    /*
    [77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
    [78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
    [79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
    [7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
    */
    LCD_CmdWrite(0x77);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x78);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x79);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x7A);
    LCD_DataWrite(HY >> 8);
}

// 方形轉角半徑
void Circle_Square_Radius_RxRy(unsigned short WX, unsigned short HY)
{
    /*
    [77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
    [78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
    [79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
    [7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
    */
    LCD_CmdWrite(0x77);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x78);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x79);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x7A);
    LCD_DataWrite(HY >> 8);
}

// 圓中心
void Circle_Center_XY(unsigned short WX, unsigned short HY)
{
    /*
    [7Bh] Draw Circle/Ellipse/Circle Square Center X-coordinate [7:0]
    [7Ch] Draw Circle/Ellipse/Circle Square Center X-coordinate [12:8]
    [7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordinate [7:0]
    [7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x7B);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x7C);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x7D);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x7E);
    LCD_DataWrite(HY >> 8);
}
// 橢圓中心
void Ellipse_Center_XY(unsigned short WX, unsigned short HY)
{
    /*
    [7Bh] Draw Circle/Ellipse/Circle Square Center X-coordinate [7:0]
    [7Ch] Draw Circle/Ellipse/Circle Square Center X-coordinate [12:8]
    [7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordinate [7:0]
    [7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordinate [12:8]
    */
    LCD_CmdWrite(0x7B);
    LCD_DataWrite(WX);

    LCD_CmdWrite(0x7C);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0x7D);
    LCD_DataWrite(HY);

    LCD_CmdWrite(0x7E);
    LCD_DataWrite(HY >> 8);
}

//[84h]=========================================================================
void Set_PWM_Prescaler_1_to_256(unsigned short WX)
{
    /*
    PWM Prescaler Register
    These 8 bits determine prescaler value for Timer 0 and 1.
    Time base is “Core_Freq / (Prescaler + 1)”
    */
    LCD_CmdWrite(0x84);
    LCD_DataWrite(WX - 1);
}
//[85h]=========================================================================
void Select_PWM1_Clock_Divided_By_1(void)
{
    /*
    Select MUX input for PWM Timer 1.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_2(void)
{
    /*
    Select MUX input for PWM Timer 1.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_4(void)
{
    /*
    Select MUX input for PWM Timer 1.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Select_PWM1_Clock_Divided_By_8(void)
{
    /*
    Select MUX input for PWM Timer 1.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_1(void)
{
    /*
    Select MUX input for PWM Timer 0.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_2(void)
{
    /*
    Select MUX input for PWM Timer 0.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_4(void)
{
    /*
    Select MUX input for PWM Timer 0.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Select_PWM0_Clock_Divided_By_8(void)
{
    /*
    Select MUX input for PWM Timer 0.
    00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
    */
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
//[85h].[bit3][bit2]=========================================================================
/*
XPWM[1] pin function control
0X:	XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth insufficient + Memory access out of range)
10:	XPWM[1] enabled and controlled by PWM timer 1
11:	XPWM[1] output oscillator clock
//If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
*/
void Select_PWM1_is_ErrorFlag(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Select_PWM1(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Select_PWM1_is_Osc_Clock(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
//[85h].[bit1][bit0]=========================================================================
/*
XPWM[0] pin function control
0X: XPWM[0] becomes GPIO-C[7]
10: XPWM[0] enabled and controlled by PWM timer 0
11: XPWM[0] output core clock
*/
void Select_PWM0_is_GPIO_C7(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Select_PWM0(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Select_PWM0_is_Core_Clock(void)
{
    unsigned char temp;

    LCD_CmdWrite(0x85);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
//[86h]=========================================================================
//[86h]PWM1
void Enable_PWM1_Inverter(void)
{
    /*
    PWM Timer 1 output inverter on/off.
    Determine the output inverter on/off for Timer 1.
    0 = Inverter off
    1 = Inverter on for PWM1
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Disable_PWM1_Inverter(void)
{
    /*
    PWM Timer 1 output inverter on/off.
    Determine the output inverter on/off for Timer 1.
    0 = Inverter off
    1 = Inverter on for PWM1
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Auto_Reload_PWM1(void)
{
    /*
    PWM Timer 1 auto reload on/off
    Determine auto reload on/off for Timer 1.
    0 = One-shot
    1 = Interval mode(auto reload)
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void One_Shot_PWM1(void)
{
    /*
    PWM Timer 1 auto reload on/off
    Determine auto reload on/off for Timer 1.
    0 = One-shot
    1 = Interval mode(auto reload)
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Start_PWM1(void)
{
    /*
    PWM Timer 1 start/stop
    Determine start/stop for Timer 1.
    0 = Stop
    1 = Start for Timer 1
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Stop_PWM1(void)
{
    /*
    PWM Timer 1 start/stop
    Determine start/stop for Timer 1.
    0 = Stop
    1 = Start for Timer 1
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void)
{
    /*
    PWM Timer 0 Dead zone enable
    Determine the dead zone operation. 0 = Disable. 1 = Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Disable_PWM0_Dead_Zone(void)
{
    /*
    PWM Timer 0 Dead zone enable
    Determine the dead zone operation. 0 = Disable. 1 = Enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}
void Enable_PWM0_Inverter(void)
{
    /*
    PWM Timer 0 output inverter on/off
    Determine the output inverter on/off for Timer 0.
    0 = Inverter off
    1 = Inverter on for PWM0
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Disable_PWM0_Inverter(void)
{
    /*
    PWM Timer 0 output inverter on/off
    Determine the output inverter on/off for Timer 0.
    0 = Inverter off
    1 = Inverter on for PWM0
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Auto_Reload_PWM0(void)
{
    /*
    PWM Timer 0 auto reload on/off
    Determine auto reload on/off for Timer 0.
    0 = One-shot
    1 = Interval mode(auto reload)
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}
void One_Shot_PWM0(void)
{
    /*
    PWM Timer 1 auto reload on/off
    Determine auto reload on/off for Timer 1.
    0 = One-shot
    1 = Interval mode(auto reload)
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void Start_PWM0(void)
{
    /*
    PWM Timer 0 start/stop
    Determine start/stop for Timer 0.
    0 = Stop
    1 = Start for Timer 0
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Stop_PWM0(void)
{
    /*
    PWM Timer 0 start/stop
    Determine start/stop for Timer 0.
    0 = Stop
    1 = Start for Timer 0
    */
    unsigned char temp;
    LCD_CmdWrite(0x86);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
//[87h]=========================================================================
void Set_Timer0_Dead_Zone_Length(unsigned char temp)
{
    /*
    Timer 0 Dead zone length register
    These 8 bits determine the dead zone length. The 1 unit time of
    the dead zone length is equal to that of timer 0.
    */
    LCD_CmdWrite(0x87);
    LCD_DataWrite(temp);
}
//[88h][89h]=========================================================================
void Set_Timer0_Compare_Buffer(unsigned short WX)
{
    /*
    Timer 0 compare buffer register
    Compare buffer register total has 16 bits.
    When timer counter equal or less than compare buffer register will cause PWM out
    high level if inv_on bit is off.
    */
    LCD_CmdWrite(0x88);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x89);
    LCD_DataWrite(WX >> 8);
}
//[8Ah][8Bh]=========================================================================
void Set_Timer0_Count_Buffer(unsigned short WX)
{
    /*
    Timer 0 count buffer register
    Count buffer register total has 16 bits.
    When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
    It may read back timer counter’s real time value when PWM timer start.
    */
    LCD_CmdWrite(0x8A);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x8B);
    LCD_DataWrite(WX >> 8);
}
//[8Ch][8Dh]=========================================================================
void Set_Timer1_Compare_Buffer(unsigned short WX)
{
    /*
    Timer 0 compare buffer register
    Compare buffer register total has 16 bits.
    When timer counter equal or less than compare buffer register will cause PWM out
    high level if inv_on bit is off.
    */
    LCD_CmdWrite(0x8C);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x8D);
    LCD_DataWrite(WX >> 8);
}
//[8Eh][8Fh]=========================================================================
void Set_Timer1_Count_Buffer(unsigned short WX)
{
    /*
    Timer 0 count buffer register
    Count buffer register total has 16 bits.
    When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
    It may read back timer counter’s real time value when PWM timer start.
    */
    LCD_CmdWrite(0x8E);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x8F);
    LCD_DataWrite(WX >> 8);
}




//[90h]~[B5h]=========================================================================

//[90h]=========================================================================
void BTE_Enable(void)
{
    /*
    BTE Function Enable
    0 : BTE Function disable.
    1 : BTE Function enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}

//[90h]=========================================================================
void BTE_Disable(void)
{
    /*
    BTE Function Enable
    0 : BTE Function disable.
    1 : BTE Function enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}

//[90h]=========================================================================
void Check_BTE_Busy(void)
{
    /*
    case1: [90h]BTE Function Control Register_bit4
    Read
    0 : BTE function is idle.
    1 : BTE function is busy.

    case2: Status Register_bit3
    0 : BTE function is idle.
    1 : BTE function is busy.
    */
    unsigned char temp;

    // case1: using BTE Function Control Register
    do {
        LCD_CmdWrite(0x90);
        temp = LCD_DataRead();
    } while (temp & 0x10);

    // case2: using status register
    do {
        temp = LCD_StatusRead();
    } while (temp & 0x08);
}
//[90h]=========================================================================
void Pattern_Format_8X8(void)
{
    /*
    Pattern Format
    0 : 8X8
    1 : 16X16
    */
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
//[90h]=========================================================================
void Pattern_Format_16X16(void)
{
    /*
    Pattern Format
    0 : 8X8
    1 : 16X16
    */
    unsigned char temp;
    LCD_CmdWrite(0x90);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[91h]=========================================================================
void BTE_ROP_Code(unsigned char setx)
{
    /*
    BTE ROP Code[Bit7:4]

    0000 : 0(Blackness)
    0001 : ~S0.~S1 or ~ ( S0+S1 )
    0010 : ~S0.S1
    0011 : ~S0
    0100 : S0.~S1
    0101 : ~S1
    0110 : S0^S1
    0111 : ~S0+~S1 or ~ ( S0.S1 )
    1000 : S0.S1
    1001 : ~ ( S0^S1 )
    1010 : S1
    1011 : ~S0+S1
    1100 : S0
    1101 : S0+~S1
    1110 : S0+S1
    1111 : 1 ( Whiteness )
    */
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0x0f;
    temp |= (setx << 4);
    LCD_DataWrite(temp);
}

//[91h]=========================================================================
void BTE_Operation_Code(unsigned char setx)
{
    /*
    BTE Operation Code[Bit3:0]

    0000/ 0 : MPU Write BTE with ROP.
    0001    : NA
    0010/ 2 : Memory copy (move) BTE in positive direction with ROP.
    0011    : NA
    0100/ 4 : MPU Write with chroma keying (w/o ROP)
    0101/ 5 : Memory copy (move) with chroma keying (w/o ROP)
    0110/ 6 : Pattern Fill with ROP
    0111/ 7 : Pattern Fill with chroma keying (w/o ROP)
    1000/ 8 : MPU Write with Color Expansion (w/o ROP)
    1001/ 9 : MPU Write with Color Expansion and chroma keying (w/o ROP)
    1010/10 : Memory write with opacity (w/o ROP)
    1011/11 : MPU Write with opacity (w/o ROP)
    1100/12 : Solid Fill (w/o ROP)
    1101    : NA
    1110/14 : Memory write with Color Expansion (w/o ROP)
    1111/15 : Memory write with Color Expansion and chroma keying (w/o ROP)
    */
    unsigned char temp;
    LCD_CmdWrite(0x91);
    temp = LCD_DataRead();
    temp &= 0xf0;
    temp |= setx;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S0_Color_8bpp(void)
{
    /*
    S0 Color Depth
    00 : 256 Color
    01 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S0_Color_16bpp(void)
{
    /*
    S0 Color Depth
    00 : 256 Color
    01 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb6;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S0_Color_24bpp(void)
{
    /*
    S0 Color Depth
    00 : 256 Color
    01 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb6;
    // temp |= cSetb5 ;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_8bpp(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_16bpp(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_24bpp(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp |= cSetb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_Constant(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb4;
    temp |= cSetb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_8bit_Alpha(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb4;
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_S1_Color_16bit_Alpha(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb4;
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_S1_Color_32bit_Alpha(void)
{
    /*
    S1 Color Depth
    000 : 256 Color
    001 : 64k Color
    010 : 16M Color
    011 : Constant Color
    100 : 8 bit pixel alpha blending
    101 : 16 bit pixel alpha blending
    110 : 32bit ARGB mode
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb4;
    temp |= cSetb3;
    temp &= cClrb2;

    LCD_DataWrite(temp);
}

//[92h]=========================================================================
void BTE_Destination_Color_8bpp(void)
{
    /*
    Destination Color Depth
    00 : 256 Color
    01 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_Destination_Color_16bpp(void)
{
    /*
    Destination Color Depth
    00 : 256 Color
    01 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
//[92h]=========================================================================
void BTE_Destination_Color_24bpp(void)
{
    /*
    Destination Color Depth
    00 : 256 Color
    10 : 64k Color
    1x : 16M Color
    */
    unsigned char temp;
    LCD_CmdWrite(0x92);
    temp = LCD_DataRead();
    temp |= cSetb1;
    // temp |= cSetb0 ;
    LCD_DataWrite(temp);
}

//[93h][94h][95h][96h]=========================================================================
void BTE_S0_Memory_Start_Address(unsigned long Addr)
{
    /*
    [93h] BTE S0 Memory Start Address [7:0]
    [94h] BTE S0 Memory Start Address [15:8]
    [95h] BTE S0 Memory Start Address [23:16]
    [96h] BTE S0 Memory Start Address [31:24]
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0x93, Addr);
    LCD_RegisterWrite(0x94, Addr >> 8);
    LCD_RegisterWrite(0x95, Addr >> 16);
    LCD_RegisterWrite(0x96, Addr >> 24);
}

//[97h][98h]=========================================================================
void BTE_S0_Image_Width(unsigned short WX)
{
    /*
    [97h] BTE S0 Image Width [7:0]
    [98h] BTE S0 Image Width [12:8]
    Unit: Pixel.
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0x97, WX);
    LCD_RegisterWrite(0x98, WX >> 8);
}

//[99h][9Ah][9Bh][9Ch]=========================================================================
void BTE_S0_Window_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [99h] BTE S0 Window Upper-Left corner X-coordinate [7:0]
    [9Ah] BTE S0 Window Upper-Left corner X-coordinate [12:8]
    [9Bh] BTE S0 Window Upper-Left corner Y-coordinate [7:0]
    [9Ch] BTE S0 Window Upper-Left corner Y-coordinate [12:8]
    */

    LCD_RegisterWrite(0x99, WX);
    LCD_RegisterWrite(0x9A, WX >> 8);

    LCD_RegisterWrite(0x9B, HY);
    LCD_RegisterWrite(0x9C, HY >> 8);
}

//[9Dh][9Eh][9Fh][A0h]=========================================================================
void BTE_S1_Memory_Start_Address(unsigned long Addr)
{
    /*
    [9Dh] BTE S1 Memory Start Address [7:0]
    [9Eh] BTE S1 Memory Start Address [15:8]
    [9Fh] BTE S1 Memory Start Address [23:16]
    [A0h] BTE S1 Memory Start Address [31:24]
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0x9D, Addr);
    LCD_RegisterWrite(0x9E, Addr >> 8);
    LCD_RegisterWrite(0x9F, Addr >> 16);
    LCD_RegisterWrite(0xA0, Addr >> 24);
}

// Input data format:R3G3B2
void S1_Constant_color_256(unsigned char temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp << 3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp << 6);
}

// Input data format:R5G6B6
void S1_Constant_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp >> 3);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp << 3);
}

// Input data format:R8G8B8
void S1_Constant_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0x9D);
    LCD_DataWrite(temp >> 16);

    LCD_CmdWrite(0x9E);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0x9F);
    LCD_DataWrite(temp);
}

//[A1h][A2h]=========================================================================
void BTE_S1_Image_Width(unsigned short WX)
{
    /*
    [A1h] BTE S1 Image Width [7:0]
    [A2h] BTE S1 Image Width [12:8]
    Unit: Pixel.
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0xA1, WX);
    LCD_RegisterWrite(0xA2, WX >> 8);
}

//[A3h][A4h][A5h][A6h]=========================================================================
void BTE_S1_Window_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [A3h] BTE S1 Window Upper-Left corner X-coordinate [7:0]
    [A4h] BTE S1 Window Upper-Left corner X-coordinate [12:8]
    [A5h] BTE S1 Window Upper-Left corner Y-coordinate [7:0]
    [A6h] BTE S1 Window Upper-Left corner Y-coordinate [12:8]
    */

    LCD_RegisterWrite(0xA3, WX);
    LCD_RegisterWrite(0xA4, WX >> 8);

    LCD_RegisterWrite(0xA5, HY);
    LCD_RegisterWrite(0xA6, HY >> 8);
}

//[A7h][A8h][A9h][AAh]=========================================================================
void BTE_Destination_Memory_Start_Address(unsigned long Addr)
{
    /*
    [A7h] BTE Destination Memory Start Address [7:0]
    [A8h] BTE Destination Memory Start Address [15:8]
    [A9h] BTE Destination Memory Start Address [23:16]
    [AAh] BTE Destination Memory Start Address [31:24]
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0xA7, Addr);
    LCD_RegisterWrite(0xA8, Addr >> 8);
    LCD_RegisterWrite(0xA9, Addr >> 16);
    LCD_RegisterWrite(0xAA, Addr >> 24);
}

//[ABh][ACh]=========================================================================
void BTE_Destination_Image_Width(unsigned short WX)
{
    /*
    [ABh] BTE Destination Image Width [7:0]
    [ACh] BTE Destination Image Width [12:8]
    Unit: Pixel.
    Bit [1:0] tie to “0” internally.
    */
    LCD_RegisterWrite(0xAB, WX);
    LCD_RegisterWrite(0xAC, WX >> 8);
}

//[ADh][AEh][AFh][B0h]=========================================================================
void BTE_Destination_Window_Start_XY(unsigned short WX, unsigned short HY)
{
    /*
    [ADh] BTE Destination Window Upper-Left corner X-coordinate [7:0]
    [AEh] BTE Destination Window Upper-Left corner X-coordinate [12:8]
    [AFh] BTE Destination Window Upper-Left corner Y-coordinate [7:0]
    [B0h] BTE Destination Window Upper-Left corner Y-coordinate [12:8]
    */

    LCD_RegisterWrite(0xAD, WX);
    LCD_RegisterWrite(0xAE, WX >> 8);

    LCD_RegisterWrite(0xAF, HY);
    LCD_RegisterWrite(0xB0, HY >> 8);
}

//[B1h][B2h][B3h][B4h]===============================================================

void BTE_Window_Size(unsigned short WX, unsigned short HY)

{
    /*
    [B1h] BTE Window Width [7:0]
    [B2h] BTE Window Width [12:8]

    [B3h] BTE Window Height [7:0]
    [B4h] BTE Window Height [12:8]
    */
    LCD_RegisterWrite(0xB1, WX);
    LCD_RegisterWrite(0xB2, WX >> 8);

    LCD_RegisterWrite(0xB3, HY);
    LCD_RegisterWrite(0xB4, HY >> 8);
}

//[B5h]=========================================================================
void BTE_Alpha_Blending_Effect(unsigned char temp)
{
    /*
    Window Alpha Blending effect for S0 & S1
    The value of alpha in the color code ranges from 0.0 to 1.0,
    where 0.0 represents a fully transparent color, and 1.0
    represents a fully opaque color.
    00h: 0
    01h: 1/32
    02h: 2/32
    :
    1Eh: 30/32
    1Fh: 31/32
    2Xh: 1
    Output Effect = (S0 image x (1 - alpha setting value)) + (S1 image x alpha setting value)
    */
    LCD_CmdWrite(0xB5);
    LCD_DataWrite(temp);
}

//[B6h]=========================================================================

void Select_SFI_Quad_Mode_Dummy_8T_6Bh(void) //
{
    /*[B6h]bit7-6=
    00b: [B7h] B3-0
    01b: 4x read command code – 6Bh.
    10b: 4x read command code – EBh.
    11b:NA
    */
    unsigned char temp;
    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    temp |= 0x40;
    LCD_DataWrite(temp);
}
void Select_SFI_Quad_Mode_Dummy_4T_EBh(void) //
{
    /*[B6h]bit7-6=
    00b: [B7h] B3-0
    01b: 4x read command code – 6Bh.
    10b: 4x read command code – EBh.
    11b:NA
    */
    unsigned char temp;
    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    temp |= 0x80;
    LCD_DataWrite(temp);
}

void Start_SFI_DMA(void)
{
    /*[bit0]
    Write Function: DMA Start Bit
    Set to 1 by MPU and reset to 0 automatically
    It cannot start with fontwr_busy as 1. And if DMA enabled also
    cannot set as text mode & send character code.
    */
    unsigned char temp;
    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

void Check_Busy_SFI_DMA(void)
{
    /*[bit0]
    Read Function: DMA Busy Check Bit
    0: Idle
    1: Busy
    */
    unsigned char temp;

    // case1:  using DMA Function Control Register
    LCD_CmdWrite(0xB6);
    do {
        temp = LCD_DataRead();
    } while (temp & 0x01);
}

//[B7h]=========================================================================

void Select_SFI_Font_Mode(void)
{
    /*[bit6]
    Serial Flash /ROM Access Mode
    0: Font mode – for external cgrom
    1: DMA mode – for cgram , pattern , bootstart image or osd
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Select_SFI_DMA_Mode(void)
{
    /*[bit6]
    Serial Flash /ROM Access Mode
    0: Font mode – for external cgrom
    1: DMA mode – for cgram , pattern , bootstart image or osd
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Select_SFI_24bit_Address(void)
{
    /*[bit5]
    Serial Flash/ROM Address Mode
    0: 24 bits address mode
    1: 32 bits address mode
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void Select_SFI_32bit_Address(void)
{
    /*[bit5]
    Serial Flash/ROM Address Mode
    0: 24 bits address mode
    1: 32 bits address mode
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Select_standard_SPI_Mode0_or_Mode3(void)
{
    /*[bit4]
    RA8875 compatible mode
    0: standard SPI mode 0 or mode 3 timing
    1: Follow RA8875 mode 0 & mode 3 timing
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Select_RA8875_SPI_Mode0_and_Mode3(void)
{
    /*[bit4]
    RA8875 compatible mode
    0: standard SPI mode 0 or mode 3 timing
    1: Follow RA8875 mode 0 & mode 3 timing
    */
    unsigned char temp;
    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}

void Select_SFI_Single_Mode_Dummy_0T_03h(void)
{
    /*
    (1) [B6h]bit7-6=0x00
    (2) [B7h]bit3-0=
    000xb: 1x read command code = 03h. Without dummy cycle between address and data.
    010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    LCD_DataWrite(temp);
}
void Select_SFI_Single_Mode_Dummy_8T_0Bh(void)
{
    /*
    (1) [B6h]bit7-6=0x00
    (2) [B7h]bit3-0=
    000xb: 1x read command code = 03h. Without dummy cycle between address and data.
    010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Select_SFI_Single_Mode_Dummy_16T_1Bh(void)
{
    /*
    (1) [B6h]bit7-6=0x00
    (2) [B7h]bit3-0=
    000xb: 1x read command code = 03h. Without dummy cycle between address and data.
    010xb: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    1x0xb: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode_Dummy_8T_3Bh(void)
{
    /*
    (1) [B6h]bit7-6=0x00
    (2) [B7h]bit3-0=
    xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
    xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
    */
    unsigned char temp;

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x02;
    LCD_DataWrite(temp);
}
void Select_SFI_Dual_Mode_Dummy_4T_BBh(void)
{
    /*
    (1) [B6h]bit7-6=0x00
    (2) [B7h]bit3-0=
    xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
    xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
    */
    unsigned char temp;

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3F;
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x03;
    LCD_DataWrite(temp);
}

//[B8h]=========================================================================
// REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR)
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data)
{
    unsigned char temp;
    LCD_CmdWrite(0xB8);
    LCD_DataWrite(Data);
    do {
        temp = Tx_FIFO_Empty_Flag();
    } while (temp == 0);
    //	while(Tx_FIFO_Empty_Flag()==0);	//空了再執行下一筆
    temp = SPI_Master_FIFO_Data_Get();

    return temp;
}

unsigned char SPI_Master_FIFO_Data_Get(void)
{
    unsigned char temp;

    while (Rx_FIFO_Empty_Flag() == 1)
        ; // 不是空的往下執行
    LCD_CmdWrite(0xB8);
    temp = LCD_DataRead();
    // while(Rx_FIFO_full_flag()); //連續寫入16筆資料才需要
    return temp;
}

//[B9h]=========================================================================
// REG[B9h] SPI master Control Register (SPIMCR2)

void Enable_SPI_Master_Interrupt(void)
{
    /*
    SPI Master Interrupt enable
    0: Disable interrupt.
    1: Enable interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Disable_SPI_Master_Interrupt(void)
{
    /*
    SPI Master Interrupt enable
    0: Disable interrupt.
    1: Enable interrupt.
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}

// 0: inactive (nSS port will goes high)
void nSS_Inactive(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
// 1: active (nSS port will goes low)
void nSS_Active(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}

void Mask_FIFO_overflow_error_Interrupt(void)
{
    /*
    Mask interrupt for FIFO overflow error [OVFIRQEN]
    0: unmask
    1: mask
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}
void Unmask_FIFO_overflow_error_Interrupt(void)
{
    /*
    Mask interrupt for FIFO overflow error [OVFIRQEN]
    0: unmask
    1: mask
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb3;
    LCD_DataWrite(temp);
}

void Mask_EMTIRQEN_Interrupt(void)
{
    /*
    Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
    0: unmask
    1: mask
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Unmask_EMTIRQEN_Interrupt(void)
{
    /*
    Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
    0: unmask
    1: mask
    */
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);
}

/*
SPI operation mode
Only support mode 0 & mode 3, when enable serial flash’s DMA
or access Getop’s character serial ROM device.
mode / CPOL:Clock Polarity bit / CPHA:Clock Phase bit
    0	0	0
    1	0	1
    2	1	0
    3	1	1
*/
void Reset_CPOL(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}

void Set_CPOL(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}

void Reset_CPHA(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);
}

void Set_CPHA(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

// REG[BAh] SPI master Status Register (SPIMSR)
unsigned char Tx_FIFO_Empty_Flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x80) == 0x80)
        return 1;
    else
        return 0;
}

unsigned char Tx_FIFO_Full_Flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x40) == 0x40)
        return 1;
    else
        return 0;
}

unsigned char Rx_FIFO_Empty_Flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x20) == 0x20)
        return 1;
    else
        return 0;
}

unsigned char Rx_FIFO_full_flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x10) == 0x10)
        return 1;
    else
        return 0;
}

unsigned char OVFI_Flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x08) == 0x08)
        return 1;
    else
        return 0;
}

void Clear_OVFI_Flag(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xBA);
    temp = LCD_DataRead();
    temp |= cSetb3;
    LCD_DataWrite(temp);
}

unsigned char EMTI_Flag(void)
{
    LCD_CmdWrite(0xBA);
    if ((LCD_DataRead() & 0x04) == 0x04)
        return 1;
    else
        return 0;
}

void Clear_EMTI_Flag(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xBA);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);
}

// REG[BB] SPI Clock period (SPIDIV)
void SPI_Clock_Period(unsigned char temp)
{
    /*
    SPI_clock= CORE_FREQ / ((Divisor+1)x2)
    */
    LCD_CmdWrite(0xBB);
    LCD_DataWrite(temp);
}

//[BCh][BDh][BEh][BFh]=========================================================================
void SFI_DMA_Source_Start_Address(unsigned long Addr)
{
    /*
    DMA Source START ADDRESS
    This bits index serial flash address [7:0][15:8][23:16][31:24]
    */
    LCD_CmdWrite(0xBC);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0xBD);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0xBE);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0xBF);
    LCD_DataWrite(Addr >> 24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Start_Address(unsigned long Addr)
{
    /*
    DMA Destination START ADDRESS
    [1:0]Fix at 0
    This bits index SDRAM address [7:0][15:8][23:16][31:24]
    */
    LCD_CmdWrite(0xC0);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0xC1);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0xC2);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0xC3);
    LCD_DataWrite(Addr >> 24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX, unsigned short HY)
{
    /*
    C0h
    This register defines DMA Destination Window Upper-Left corner
    X-coordinate [7:0] on Canvas area.
    When REG DMACR bit 1 = 1 (Block Mode)
    This register defines Destination address [7:2] in SDRAM.
    C1h
    When REG DMACR bit 1 = 0 (Linear Mode)
    This register defines DMA Destination Window Upper-Left corner
    X-coordinate [12:8] on Canvas area.
    When REG DMACR bit 1 = 1 (Block Mode)
    This register defines Destination address [15:8] in SDRAM.
    C2h
    When REG DMACR bit 1 = 0 (Linear Mode)
    This register defines DMA Destination Window Upper-Left corner
    Y-coordinate [7:0] on Canvas area.
    When REG DMACR bit 1 = 1 (Block Mode)
    This register defines Destination address [23:16] in SDRAM.
    C3h
    When REG DMACR bit 1 = 0 (Linear Mode)
    This register defines DMA Destination Window Upper-Left corner
    Y-coordinate [12:8] on Canvas area.
    When REG DMACR bit 1 = 1 (Block Mode)
    This register defines Destination address [31:24] in SDRAM.
    */

    LCD_CmdWrite(0xC0);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xC1);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0xC2);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0xC3);
    LCD_DataWrite(HY >> 8);
}

//[C6h][C7h][C8h][C9h]=========================================================================
void SFI_DMA_Transfer_Number(unsigned long Num)
{
    /*
    When REG DMACR bit 1 = 0 (Linear Mode)
    DMA Transfer Number [7:0][15:8][23:16][31:24]

    When REG DMACR bit 1 = 1 (Block Mode)
    DMA Block Width [7:0][15:8]
    DMA Block HIGH[7:0][15:8]
    */
    LCD_CmdWrite(0xC6);
    LCD_DataWrite(Num);
    LCD_CmdWrite(0xC7);
    LCD_DataWrite(Num >> 8);
    LCD_CmdWrite(0xC8);
    LCD_DataWrite(Num >> 16);
    LCD_CmdWrite(0xC9);
    LCD_DataWrite(Num >> 24);
}
void SFI_DMA_Transfer_Width_Height(unsigned short WX, unsigned short HY)
{
    /*
    When REG DMACR bit 1 = 0 (Linear Mode)
    DMA Transfer Number [7:0][15:8][23:16][31:24]

    When REG DMACR bit 1 = 1 (Block Mode)
    DMA Block Width [7:0][15:8]
    DMA Block HIGH[7:0][15:8]
    */
    LCD_CmdWrite(0xC6);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xC7);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0xC8);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0xC9);
    LCD_DataWrite(HY >> 8);
}
//[CAh][CBh]=========================================================================
void SFI_DMA_Source_Width(unsigned short WX)
{
    /*
    DMA Source Picture Width [7:0][12:8]
    Unit: pixel
    */
    LCD_CmdWrite(0xCA);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xCB);
    LCD_DataWrite(WX >> 8);
}

//[CCh]=========================================================================

void Font_Select_UserDefine_Mode(void)
{
    /*[bit7-6]
    User-defined Font /CGROM Font Selection Bit in Text Mode
    00 : Internal CGROM
    01 : Genitop serial flash
    10 : User-defined Font
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void CGROM_Select_Internal_CGROM(void)
{
    /*[bit7-6]
    User-defined Font /CGROM Font Selection Bit in Text Mode
    00 : Internal CGROM
    01 : Genitop serial flash
    10 : User-defined Font
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void CGROM_Select_Genitop_FontROM(void)
{
    /*[bit7-6]
    User-defined Font /CGROM Font Selection Bit in Text Mode
    00 : Internal CGROM
    01 : Genitop serial flash
    10 : User-defined Font
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    LCD_DataWrite(temp);
}

void Font_Select_8x16_16x16(void)
{
    //[bit5-4]
    // Font Height Setting
    // 00b : 8x16 / 16x16.
    // 01b : 12x24 / 24x24.
    // 10b : 16x32 / 32x32.
    //*** User-defined Font width is decided by font code.
    // Genitop serial flash’s font width is decided by font code or GT Font ROM control register.

    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
} //*/
void Font_Select_12x24_24x24(void)
{
    /*[bit5-4]
    Font Height Setting
    00b : 8x16 / 16x16.
    01b : 12x24 / 24x24.
    10b : 16x32 / 32x32.
    *** User-defined Font width is decided by font code. Genitop
    serial flash’s font width is decided by font code or GT Font ROM
    control register.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Font_Select_16x32_32x32(void)
{
    //[bit5-4]
    // Font Height Setting
    // 00b : 8x16 / 16x16.
    // 01b : 12x24 / 24x24.
    // 10b : 16x32 / 32x32.
    //*** User-defined Font width is decided by font code.
    // Genitop serial flash’s font width is decided by font code or GT Font ROM control register.

    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
    LCD_DataWrite(temp);
} //*/

void Internal_CGROM_Select_ISOIEC8859_1(void)
{
    /*
    Font Selection for internal CGROM
    When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
    8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
    which supports English and most of European country languages.
    00b : ISO/IEC 8859-1.
    01b : ISO/IEC 8859-2.
    10b : ISO/IEC 8859-4.
    11b : ISO/IEC 8859-5.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_2(void)
{
    /*
    Font Selection for internal CGROM
    When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
    8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
    which supports English and most of European country languages.
    00b : ISO/IEC 8859-1.
    01b : ISO/IEC 8859-2.
    10b : ISO/IEC 8859-4.
    11b : ISO/IEC 8859-5.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_4(void)
{
    /*
    Font Selection for internal CGROM
    When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
    8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
    which supports English and most of European country languages.
    00b : ISO/IEC 8859-1.
    01b : ISO/IEC 8859-2.
    10b : ISO/IEC 8859-4.
    11b : ISO/IEC 8859-5.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Internal_CGROM_Select_ISOIEC8859_5(void)
{
    /*
    Font Selection for internal CGROM
    When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
    8x16 character sets with the standard coding of ISO/IEC 8859-1,2,4,5,
    which supports English and most of European country languages.
    00b : ISO/IEC 8859-1.
    01b : ISO/IEC 8859-2.
    10b : ISO/IEC 8859-4.
    11b : ISO/IEC 8859-5.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCC);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
//[CDh]=========================================================================
void Enable_Font_Alignment(void)
{
    /*
    Full Alignment Selection Bit
    0 : Full alignment disable.
    1 : Full alignment enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Disable_Font_Alignment(void)
{
    /*
    Full Alignment Selection Bit
    0 : Full alignment disable.
    1 : Full alignment enable.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Font_Background_select_Original_Canvas(void)
{
    /*
    Font Transparency
    0 : Character?s background displayed with specified color.
    1 : Character?s background displayed with original canvas background.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}
void Font_Background_select_Color(void)
{
    /*
    Font Transparency
    0 : Character?s background displayed with specified color.
    1 : Character?s background displayed with original canvas background.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb6;
    LCD_DataWrite(temp);
}
void Font_0_degree(void)
{
    /*
    Font Rotation
    0 : Normal
    Text direction from left to right then from top to bottom
    1 : Counterclockwise 90 degree & horizontal flip
    Text direction from top to bottom then from left to right
    (it should accommodate with set VDIR as 1)
    This attribute can be changed only when previous font write
    finished (core_busy = 0)
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);
}
void Font_90_degree(void)
{
    /*
    Font Rotation
    0 : Normal
    Text direction from left to right then from top to bottom
    1 : Counterclockwise 90 degree & horizontal flip
    Text direction from top to bottom then from left to right
    (it should accommodate with set VDIR as 1)
    This attribute can be changed only when previous font write
    finished (core_busy = 0)
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);
}
void Font_Width_X1(void)
{
    /*
    Horizontal Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Font_Width_X2(void)
{
    /*
    Horizontal Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Font_Width_X3(void)
{
    /*
    Horizontal Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
    LCD_DataWrite(temp);
}
void Font_Width_X4(void)
{
    /*
    Horizontal Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb3;
    temp |= cSetb2;
    LCD_DataWrite(temp);
}
void Font_Height_X1(void)
{
    /*
    Vertical Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Font_Height_X2(void)
{
    /*
    Vertical Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}
void Font_Height_X3(void)
{
    /*
    Vertical Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
    LCD_DataWrite(temp);
}
void Font_Height_X4(void)
{
    /*
    Vertical Font Enlargement
    00b : X1.
    01b : X2.
    10b : X3.
    11b : X4.
    */
    unsigned char temp;
    LCD_CmdWrite(0xCD);
    temp = LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
    LCD_DataWrite(temp);
}

//[CEh]=========================================================================
void GTFont_Select_GT21L16T1W(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT30L16U2W(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb6;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT30L24T3Y(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT30L24M1Z(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb6;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT30L32S4W(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT20L24F6Y(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb6;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void GTFont_Select_GT21L24S1W(void)
{
    /*
    GT Serial Font ROM Select
    000b: GT21L16T1W
    001b: GT30L16U2W
    010b: GT30L24T3Y
    011b: GT30L24M1Z
    100b: GT30L32S4W
    101b: GT20L24F6Y
    110b: GT21L24S1W
    */
    unsigned char temp;
    LCD_CmdWrite(0xCE);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb6;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}

//[CFh]=========================================================================
void Set_GTFont_Decoder(unsigned char temp)
{
    /*
    [bit7-3]
    FONT ROM Coding Setting
    For specific GT serial Font ROM, the coding method must be set for decoding.
    00000b: GB2312
    00001b: GB12345/GB18030
    00010b: BIG5
    00011b: UNICODE
    00100b: ASCII
    00101b: UNI-Japanese
    00110b: JIS0208
    00111b: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
    01000b: Korea
    10001b: ISO-8859-1
    10010b: ISO-8859-2
    10011b: ISO-8859-3
    10100b: ISO-8859-4
    10101b: ISO-8859-5
    10110b: ISO-8859-6
    10111b: ISO-8859-7
    11000b: ISO-8859-8
    11001b: ISO-8859-9
    11010b: ISO-8859-10
    11011b: ISO-8859-11
    11100b: ISO-8859-12
    11101b: ISO-8859-13
    11110b: ISO-8859-14
    11111b: ISO-8859-15

    [bit1-0]
    ASCII / Latin/Greek/ Cyrillic / Arabic
            (ASCII)		(Latin/Greek/Cyrillic)	(Arabic)
    00b		Normal			Normal 					NA
    01b		Arial 		Variable Width 			Presentation Forms-A
    10b		Roman 			NA 					Presentation Forms-B
    11b		Bold			NA 						NA
    */
    LCD_CmdWrite(0xCF);
    LCD_DataWrite(temp);
}
//[D0h]=========================================================================
void Font_Line_Distance(unsigned char temp)
{
    /*[bit4-0]
    Font Line Distance Setting
    Setting the font character line distance when setting memory font
    write cursor auto move. (Unit: pixel)
    */
    LCD_CmdWrite(0xD0);
    LCD_DataWrite(temp);
}
//[D1h]=========================================================================
void Set_Font_to_Font_Width(unsigned char temp)
{
    /*[bit5-0]
    Font to Font Width Setting (Unit: pixel)
    */
    LCD_CmdWrite(0xD1);
    LCD_DataWrite(temp);
}
//[D2h]~[D4h]=========================================================================
void Foreground_RGB(unsigned char RED, unsigned char GREEN, unsigned char BLUE)
{
    /*
    [D2h] Foreground Color - Red, for draw, text or color expansion
    [D3h] Foreground Color - Green, for draw, text or color expansion
    [D4h] Foreground Color - Blue, for draw, text or color expansion
    */
    LCD_CmdWrite(0xD2);
    LCD_DataWrite(RED);

    LCD_CmdWrite(0xD3);
    LCD_DataWrite(GREEN);

    LCD_CmdWrite(0xD4);
    LCD_DataWrite(BLUE);
}

// Input data format:R3G3B2
void Foreground_color_256(unsigned char temp)
{
    LCD_CmdWrite(0xD2);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xD3);
    LCD_DataWrite(temp << 3);

    LCD_CmdWrite(0xD4);
    LCD_DataWrite(temp << 6);
}

// Input data format:R5G6B5
void Foreground_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD2);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0xD3);
    LCD_DataWrite(temp >> 3);

    LCD_CmdWrite(0xD4);
    LCD_DataWrite(temp << 3);
}

// Input data format:R8G8B8
void Foreground_color_16M(unsigned long temp)
{

    LCD_CmdWrite(0xD2);
    LCD_DataWrite(temp >> 16);

    LCD_CmdWrite(0xD3);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0xD4);
    LCD_DataWrite(temp);
}

//[D5h]~[D7h]=========================================================================
/*
[D5h] Background Color - Red, for Text or color expansion
[D6h] Background Color - Green, for Text or color expansion
[D7h] Background Color - Blue, for Text or color expansion
*/
void Background_RGB(unsigned char RED, unsigned char GREEN, unsigned char BLUE)
{

    LCD_CmdWrite(0xD5);
    LCD_DataWrite(RED);

    LCD_CmdWrite(0xD6);
    LCD_DataWrite(GREEN);

    LCD_CmdWrite(0xD7);
    LCD_DataWrite(BLUE);
}

// Input data format:R3G3B2
void Background_color_256(unsigned char temp)
{
    LCD_CmdWrite(0xD5);
    LCD_DataWrite(temp);

    LCD_CmdWrite(0xD6);
    LCD_DataWrite(temp << 3);

    LCD_CmdWrite(0xD7);
    LCD_DataWrite(temp << 6);
}

// Input data format:R5G6B6
void Background_color_65k(unsigned short temp)
{
    LCD_CmdWrite(0xD5);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0xD6);
    LCD_DataWrite(temp >> 3);

    LCD_CmdWrite(0xD7);
    LCD_DataWrite(temp << 3);
}

// Input data format:R8G8B8
void Background_color_16M(unsigned long temp)
{
    LCD_CmdWrite(0xD5);
    LCD_DataWrite(temp >> 16);

    LCD_CmdWrite(0xD6);
    LCD_DataWrite(temp >> 8);

    LCD_CmdWrite(0xD7);
    LCD_DataWrite(temp);
}

//[DBh]~[DEh]=========================================================================
void CGRAM_Start_address(unsigned long Addr)
{
    /*
    CGRAM START ADDRESS [31:0]
    */
    LCD_CmdWrite(0xDB);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0xDC);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0xDD);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0xDE);
    LCD_DataWrite(Addr >> 24);
}

//[DFh]=========================================================================
/*
[bit7] Enter Power saving state
0: Normal state.
1: Enter power saving state.
[bit1][bit0] Power saving Mode definition
00: NA
01: Standby Mode
10: Suspend Mode
11: Sleep Mode
*/
void Power_Normal_Mode(void)
{
    LCD_CmdWrite(0xDF);
    LCD_DataWrite(0x00);
    Check_IC_ready();
}
void Power_Saving_Standby_Mode(void)
{
    LCD_CmdWrite(0xDF);
    //	LCD_DataWrite(0x01);
    //	LCD_CmdWrite(0xDF);
    LCD_DataWrite(0x81);
}
void Power_Saving_Suspend_Mode(void)
{
    LCD_CmdWrite(0xDF);
    //	LCD_DataWrite(0x02);
    //	LCD_CmdWrite(0xDF);
    LCD_DataWrite(0x82);
}
void Power_Saving_Sleep_Mode(void)
{
    LCD_CmdWrite(0xDF);
    LCD_DataWrite(0x03);
    LCD_CmdWrite(0xDF);
    LCD_DataWrite(0x83);
}

//[E5h]~[E6h]=========================================================================
void RA8889_I2CM_Clock_Prescale(unsigned short WX)
{
    /*
    I2C Master Clock Pre-scale [7:0]
    I2C Master Clock Pre-scale [15:8]
    XSCL = CCLK / (5*(prescale + 2))
    */
    LCD_CmdWrite(0xE5);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xE6);
    LCD_DataWrite(WX >> 8);
}
//[E7h]=========================================================================
void RA8889_I2CM_Transmit_Data(unsigned char temp)
{
    /*
    I2C Master Transmit[7:0]
    */
    LCD_CmdWrite(0xE7);
    LCD_DataWrite(temp);
}
//[E8h]=========================================================================
unsigned char RA8889_I2CM_Receiver_Data(void)
{
    /*
    I2C Master Receiver [7:0]
    */
    unsigned char temp;
    LCD_CmdWrite(0xE8);
    temp = LCD_DataRead();
    return temp;
}
//[E9h]=========================================================================
/*
[bit7] START
    Generate (repeated) start condition and be cleared by hardware automatically
    Note : This bit is always read as 0.
[bit6] STOP
    Generate stop condition and be cleared by hardware automatically
    Note : This bit is always read as 0.
[bit5] READ
    READ(READ and WRITE can’t be used simultaneously)
    Read form slave and be cleared by hardware automatically
    Note : This bit is always read as 0.
[bit4] WRITE
    WRITE(READ and WRITE can’t be used simultaneously)
    Write to slave and be cleared by hardware automatically
    Note : This bit is always read as 0.
[bit3] ACKNOWLEDGE
    When as a I2C master receiver
    0 : Sent ACK.
    1 : Sent NACK.
    Note : This bit is always read as 0.
[bit0] Noise Filter
    0 : Enable.
    1 : Disable.
*/

//[bit7] START
void RA8889_I2CM_Write_With_Start(void)
{
    LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter
    LCD_DataWrite(0x90);
#endif

#ifdef Enable_I2CM_Noise_Filter
    LCD_DataWrite(0x91);
#endif
}

//[bit6] STOP
void RA8889_I2CM_Stop(void)
{
    LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter
    LCD_DataWrite(0x40);
#endif

#ifdef Enable_I2CM_Noise_Filter
    LCD_DataWrite(0x41);
#endif
}

//[bit5] READ
void RA8889_I2CM_Read_With_Ack(void)
{
    LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter
    LCD_DataWrite(0x20);
#endif
#ifdef Enable_I2CM_Noise_Filter
    LCD_DataWrite(0x21);
#endif
}

void RA8889_I2CM_Read_With_Nack(void)
{
    LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter
    LCD_DataWrite(0x68);
#endif
#ifdef Enable_I2CM_Noise_Filter
    LCD_DataWrite(0x69);
#endif
}

//[bit4] WRITE
void RA8889_I2CM_Write(void)
{
    LCD_CmdWrite(0xE9);
#ifdef Disable_I2CM_Noise_Filter
    LCD_DataWrite(0x10);
#endif
#ifdef Enable_I2CM_Noise_Filter
    LCD_DataWrite(0x11);
#endif
}

//[EAh]=========================================================================

unsigned char RA8889_I2CM_Check_Slave_ACK(void)
{
    /*[bit7]
    Received acknowledge from slave
    0 : Acknowledge received.
    1 : No Acknowledge received.
    */
    unsigned char temp;

    LCD_CmdWrite(0xEA);
    temp = LCD_DataRead();
    if ((temp & 0x80) == 0x80)
        return 1;
    else
        return 0;
}

unsigned char RA8889_I2CM_Bus_Busy(void)
{
    /*[bit6]
    I2C Bus is Busy
    0 : Idle.
    1 : Busy.
    */
    unsigned char temp;

    LCD_CmdWrite(0xEA);
    temp = LCD_DataRead();
    if ((temp & 0x40) == 0x40)
        return 1;
    else
        return 0;
}

unsigned char RA8889_I2CM_transmit_Progress(void)
{
    /*[bit6]
     0=Complete
     1=Transferring
    */
    unsigned char temp;

    LCD_CmdWrite(0xEA);
    temp = LCD_DataRead();
    if ((temp & 0x02) == 0x02)
        return 1;
    else
        return 0;
}

unsigned char RA8889_I2CM_Arbitration(void)
{
    /*[bit6]
    I2C Bus is Busy
    0 : Idle.
    1 : Busy.
    */
    unsigned char temp;

    LCD_CmdWrite(0xEA);
    temp = LCD_DataRead();
    temp &= 0x01;
    return temp;
}

//[F0h]=========================================================================
void Set_GPIO_A_In_Out(unsigned char temp)
{
    /*
    GPO-A_dir[7:0] : General Purpose I/O direction control.
    0: Output
    1: Input
    */
    LCD_CmdWrite(0xF0);
    LCD_DataWrite(temp);
}
//[F1h]=========================================================================
void Write_GPIO_A_7_0(unsigned char temp)
{
    /*
    GPI-A[7:0] : General Purpose Input, share with DB[15:8]
    GPO-A[7:0] : General Purpose Output, share with DB[15:8]
    */
    LCD_CmdWrite(0xF1);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_A_7_0(void)
{
    /*
    GPI-A[7:0] : General Purpose Input, share with DB[15:8]
    GPO-A[7:0] : General Purpose Output, share with DB[15:8]
    */
    unsigned char temp;
    LCD_CmdWrite(0xF1);
    temp = LCD_DataRead();
    return temp;
}
//[F2h]=========================================================================
void Write_GPIO_B_7_4(unsigned char temp)
{
    /*
    Bit [7:4] is port B's General Purpose Output bit [7:4]. They are share with XKOUT[3:0]
    Bit [3:0] is not writable.
    */
    LCD_CmdWrite(0xF2);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_B_7_0(void)
{
    /*
    Bit[7:0] are share with {XKIN[3:0], XA0, XnWR, XnRD, XnCS}
    Bit[3:0] are only available in serial MPU I/F, otherwise fix at 0.
    */
    unsigned char temp;
    LCD_CmdWrite(0xF2);
    temp = LCD_DataRead();
    return temp;
}

//[F3h]=========================================================================
void Set_GPIO_C_In_Out(unsigned char temp)
{
    /*
    GPIO-C_dir[7:0] : General Purpose I/O direction control.
    0: Output
    1: Input
    */
    LCD_CmdWrite(0xF3);
    LCD_DataWrite(temp);
}
//[F4h]=========================================================================
void Write_GPIO_C_7_0(unsigned char temp)
{
    /*
    REG[F4h] GPIO-C
    GPIO-C[7] : XPWM0,
    GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
    */
    LCD_CmdWrite(0xF4);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_C_7_0(void)
{
    /*
    REG[F4h] GPIO-C
    GPIO-C[7] : XPWM0,
    GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
    */
    unsigned char temp;
    LCD_CmdWrite(0xF4);
    temp = LCD_DataRead();
    return temp;
}
//[F5h]=========================================================================
void Set_GPIO_D_In_Out(unsigned char temp)
{
    /*
    GPIO-D_dir[7:0] : General Purpose I/O direction control.
    0: Output
    1: Input
    */
    LCD_CmdWrite(0xF5);
    LCD_DataWrite(temp);
}
//[F6h]=========================================================================
void Write_GPIO_D_7_0(unsigned char temp)
{
    /*
    GPIO-D[7:0] : General Purpose Input/Output
    */
    LCD_CmdWrite(0xF6);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_D_7_0(void)
{
    /*
    GPIO-D[7:0] : General Purpose Input/Output
    */
    unsigned char temp;
    LCD_CmdWrite(0xF6);
    temp = LCD_DataRead();
    return temp;
}
//[F7h]=========================================================================
void Set_GPIO_E_In_Out(unsigned char temp)
{
    /*
    GPIO-E_dir[7:0] : General Purpose I/O direction control.
    0: Output
    1: Input
    */
    LCD_CmdWrite(0xF7);
    LCD_DataWrite(temp);
}
//[F8h]=========================================================================
void Write_GPIO_E_7_0(unsigned char temp)
{
    /*
    GPIO-E[7:0] : General Purpose Input/Output.
    share with {PDAT[23:19], PDAT[15:13]}
    */
    LCD_CmdWrite(0xF8);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_E_7_0(void)
{
    /*
    GPIO-E[7:0] : General Purpose Input/Output.
    share with {PDAT[23:19], PDAT[15:13]}
    */
    unsigned char temp;
    LCD_CmdWrite(0xF8);
    temp = LCD_DataRead();
    return temp;
}
//[F9h]=========================================================================
void Set_GPIO_F_In_Out(unsigned char temp)
{
    /*
    GPIO-F_dir[7:0] : General Purpose I/O direction control.
    0: Output
    1: Input
    */
    LCD_CmdWrite(0xF9);
    LCD_DataWrite(temp);
}
//[FAh]=========================================================================
void Write_GPIO_F_7_0(unsigned char temp)
{
    /*
    GPIO-F[7:0] : General Purpose Input/Output.
    share with {XPDAT[12:10], XPDAT[7:3]}
    */
    LCD_CmdWrite(0xFA);
    LCD_DataWrite(temp);
}
unsigned char Read_GPIO_F_7_0(void)
{
    /*
    GPIO-F[7:0] : General Purpose Input/Output.
    share with {XPDAT[12:10], XPDAT[7:3]}
    */
    unsigned char temp;
    LCD_CmdWrite(0xFA);
    temp = LCD_DataRead();
    return temp;
}

//[FBh]=========================================================================

void Long_Key_enable(void)
{
    /*
   Key-Scan Control Register 1
   [bit6]		LongKey Enable Bit
   1 : Enable. Long key period is set by KSCR2 bit4-2.
   0 : Disable.
   */
    unsigned char temp;
    LCD_CmdWrite(0xFB);
    temp = LCD_DataRead();
    temp |= cSetb6;
    LCD_DataWrite(temp);
}

void Key_Scan_Freg(unsigned char setx)
{
    /*KF2-0: Key-Scan Frequency */
    unsigned char temp;
    LCD_CmdWrite(0xFB);
    temp = LCD_DataRead();
    temp &= 0xf0;
    temp |= (setx & 0x07);
    LCD_DataWrite(temp);
}

//[FCh]=========================================================================

void Key_Scan_Wakeup_Function_Enable(void)
{
    /*
    Key-Scan Controller Register 2
    [bit7]
    Key-Scan Wakeup Function Enable Bit
    0: Key-Scan Wakeup function is disabled.
    1: Key-Scan Wakeup function is enabled.
    */
    unsigned char temp;
    LCD_CmdWrite(0xFC);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}

void Long_Key_Timing_Adjustment(unsigned char setx)
{
    /*Long Key Timing Adjustment*/
    unsigned char temp, temp1;
    temp = setx & 0x1c;
    LCD_CmdWrite(0xFC);
    temp1 = LCD_DataRead();
    temp1 |= temp;
    LCD_DataWrite(temp1);
}

unsigned char Numbers_of_Key_Hit(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xFC);
    temp = LCD_DataRead(); // read key touch number
    temp = temp & 0x03;    // 確認有幾個按鍵被按下
    return temp;
}

//[FDh][FEh][FFh]=========================================================================
unsigned char Read_Key_Strobe_Data_0(void)
{
    /*
    Key Strobe Data 0
    The corresponding key code 0 that is pressed.
    */
    unsigned char temp;
    LCD_CmdWrite(0xFD);
    temp = LCD_DataRead();
    return temp;
}
unsigned char Read_Key_Strobe_Data_1(void)
{
    /*
    Key Strobe Data 1
    The corresponding key code 1 that is pressed.
    */
    unsigned char temp;
    LCD_CmdWrite(0xFE);
    temp = LCD_DataRead();
    return temp;
}
unsigned char Read_Key_Strobe_Data_2(void)
{
    /*
    Key Strobe Data 2
    The corresponding key code 2 that is pressed.
    */
    unsigned char temp;
    LCD_CmdWrite(0xFF);
    temp = LCD_DataRead();
    return temp;
}

/*------------------------------------//------------------------------------
//------------------------------------//----------------------------------*/

void Set_LCD_Panel(void)
{
#ifdef EJ080NA_04B // ##INNOLUX_1024x768_EJ080NA-04B

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    //	Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
    Select_LCD_DE_Mode(); // XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 768); // INNOLUX 1024x768。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(96); // Blank=90~376, TYP.=320。
    LCD_HSYNC_Start_Position(96);   // Start Position or Front-Porch
    LCD_HSYNC_Pulse_Width(128);     // Pulse Width

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(10); // Non Display or Back-Porch  //Blank=10~77, TYP.=38。
    LCD_VSYNC_Start_Position(10); // Start Position or Front-Porch
    LCD_VSYNC_Pulse_Width(16);    // Pulse Width

#endif

#ifdef LQ150X1LGN2C // ##SHARP_LQ150X1LGN2C_LVDS1 & LVDS2

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 768); // SHARP1024x768。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(30); // SHARP1024x768，Blank=32~320~696。
    LCD_HSYNC_Start_Position(10);   // SHARP1024x768。
    LCD_HSYNC_Pulse_Width(10);      // SHARP1024x768。

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(20); // SHARP1024x768，Blank=5~38~222。
    LCD_VSYNC_Start_Position(10); // SHARP1024x768。
    LCD_VSYNC_Pulse_Width(10);    // SHARP1024x768。
#endif

#ifdef N070ICG_LD1 // ##INNOLUX_1280x800_N070ICG-LD1

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1280, 800); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(50); //
    LCD_HSYNC_Start_Position(32);   //
    LCD_HSYNC_Pulse_Width(80);      //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(24); //
    LCD_VSYNC_Start_Position(4);  //
    LCD_VSYNC_Pulse_Width(24);    //

#endif

#ifdef B116XW03_V0 // ##AUO1366x768_B116XW03 V0

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    //	HSYNC_Low_Active();
    //	HSYNC_High_Active();

    //	VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1366, 768); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(10); //
    LCD_HSYNC_Start_Position(10);   //
    LCD_HSYNC_Pulse_Width(100);     // 刻意加大給 76+ 用

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(4); //
    LCD_VSYNC_Start_Position(4); //
    LCD_VSYNC_Pulse_Width(50);   // 刻意加大給 76+ 用
#endif

#ifdef G121XN01_V0 // ##AUO1366x768_B116XW03 V0

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    //	HSYNC_Low_Active();
    //	HSYNC_High_Active();

    //	VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 768); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(60); //
    LCD_HSYNC_Start_Position(60);   //
    LCD_HSYNC_Pulse_Width(240);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(8); //
    LCD_VSYNC_Start_Position(8); //
    LCD_VSYNC_Pulse_Width(16);   //
#endif

#ifdef LQ121S1LG81 // ##SHARP_LQ121S1LG81

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(800, 600); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(88); //
    LCD_HSYNC_Start_Position(40);   //
    LCD_HSYNC_Pulse_Width(128);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(23); //
    LCD_VSYNC_Start_Position(39); //
    LCD_VSYNC_Pulse_Width(4);     //

#endif

#ifdef LQ035NC111

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    // PCLK_Falling();
    PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    // HSYNC_High_Active();
    // VSYNC_High_Active();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    // DE_High_Active();
    DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(320, 240); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(28); //
    LCD_HSYNC_Start_Position(20);   //
    LCD_HSYNC_Pulse_Width(40);      //	non_display + pulse width = 68

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(8); //
    LCD_VSYNC_Start_Position(2); //
    LCD_VSYNC_Pulse_Width(8);    //

#endif

#ifdef EJ080NA_05B

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(800, 600); // INNOLUX 800x600。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(38); // INNOLUX800x600，46。
    LCD_HSYNC_Start_Position(210);  // INNOLUX800x600，16~354。
    LCD_HSYNC_Pulse_Width(8);       // INNOLUX800x600，1~40C

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(15); // INNOLUX800x600，23。
    LCD_VSYNC_Start_Position(12); // INNOLUX800x600，1~77。
    LCD_VSYNC_Pulse_Width(8);     // INNOLUX800x600，1~20C

#endif

#ifdef AWY_800480T70N02

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(800, 480);

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(40);
    LCD_HSYNC_Start_Position(40);
    LCD_HSYNC_Pulse_Width(48);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(24);
    LCD_VSYNC_Start_Position(13);
    LCD_VSYNC_Pulse_Width(8);

#endif

#ifdef DH320x960 // ##

    //**[10h]**//
    //			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
    Select_LCD_DE_Mode(); // XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(320, 960);

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(110); // Blank=204x2。
    LCD_HSYNC_Start_Position(110);   //
    LCD_HSYNC_Pulse_Width(110);      //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(15); // Blank=42。
    LCD_VSYNC_Start_Position(15); //
    LCD_VSYNC_Pulse_Width(12);    //

#endif

#ifdef AWT_1024600L7N03

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(1024, 600);

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(80);
    LCD_HSYNC_Start_Position(80);
    LCD_HSYNC_Pulse_Width(160);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(8);
    LCD_VSYNC_Start_Position(5);
    LCD_VSYNC_Pulse_Width(8);

#endif

#ifdef AT043TN25

    //**[10h]**//
    //			Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
    Select_LCD_DE_Mode(); // XVSYNC & XHSYNC in idle state.

    //	PCLK_Rising();
    PCLK_Falling();
    VSCAN_T_to_B();
    PDATA_Set_RGB();

    //**[13h]**//
    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    //	DE_Low_Active();
    //------------------------------------
    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(480, 272); // LCD Resolution 480x272。
    //**[16h][17h]**//
    LCD_Horizontal_Non_Display(38); // 480x272，HSYNC Back Porch=38。
    //**[18h]**//
    LCD_HSYNC_Start_Position(20); // 480x272 HSYNC Front Porch = 20。
    //**[19h]**//
    LCD_HSYNC_Pulse_Width(20); // 480x272，HSYNC Pulse Width=5~30,set 20。
    //**[1Ch][1Dh]**//
    LCD_Vertical_Non_Display(4); // 480x272，VSYNC Back Porch =4。
    //**[1Eh]**//
    LCD_VSYNC_Start_Position(15); // 480x272，VSYNC Front Porch =15。
    //**[1Fh]**//
    LCD_VSYNC_Pulse_Width(3); // 480x272，VSYNC Pulse Width =3。

#endif

#ifdef AT070TN92

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    //		VSCAN_B_to_T();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(800, 480); // INNOLUX 800x480。
    LCD_Horizontal_Non_Display(30);               // INNOLUX800x600，46。
    LCD_HSYNC_Start_Position(210);                // INNOLUX800x600，16~354。
    LCD_HSYNC_Pulse_Width(16);                    // INNOLUX800x600，1~40。
    LCD_Vertical_Non_Display(13);                 // INNOLUX800x600，23。
    LCD_VSYNC_Start_Position(22);                 // INNOLUX800x600，1~147。
    LCD_VSYNC_Pulse_Width(10);                    // INNOLUX800x600，1~20。

#endif
	

#ifdef L10600R70

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 600); // 1024x600

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(40); // Blank=320。
    LCD_HSYNC_Start_Position(48);   //
    LCD_HSYNC_Pulse_Width(120);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(9); // Blank=35。
    LCD_VSYNC_Start_Position(7); //
    LCD_VSYNC_Pulse_Width(14);   //

#endif

#ifdef L80480R70

	//**[10h]**//
	Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
//	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.
	
	PCLK_Falling();
//	PCLK_Rising();

	VSCAN_T_to_B();
	PDATA_Set_RGB();

	HSYNC_Low_Active();
	VSYNC_Low_Active();
	DE_High_Active();
//	DE_Low_Active();

	LCD_HorizontalWidth_VerticalHeight(800,480);
	LCD_Horizontal_Non_Display(64);
	LCD_HSYNC_Start_Position(208);
	LCD_HSYNC_Pulse_Width(24);
	LCD_Vertical_Non_Display(16);
	LCD_VSYNC_Start_Position(22);
	LCD_VSYNC_Pulse_Width(16);

#endif

#ifdef ET101000DM6 // ##EDT_1024x600_ET101000DM6

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 600); // EDT_1024x600???

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(160); // EDT_1024x600???Blank=90~376@320???
    LCD_HSYNC_Start_Position(160);   // EDT_1024x600???Blank=90~376@320???
    LCD_HSYNC_Pulse_Width(60);       // EDT_1024x600???Blank=90~376@320???

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(23); // EDT_1024x600???Blank=10~200@35???
    LCD_VSYNC_Start_Position(12); // EDT_1024x600???Blank=10~200@35???
    LCD_VSYNC_Pulse_Width(5);     // EDT_1024x600???Blank=10~200@35???

#endif

#ifdef AT043TN25

    //**[10h]**//
    //	Select_LCD_Sync_Mode();	// Enable XVSYNC, XHSYNC, XDE.
    Select_LCD_DE_Mode(); // XVSYNC & XHSYNC in idle state.

    //	PCLK_Rising();
    PCLK_Falling();
    VSCAN_T_to_B();
    PDATA_Set_RGB();

    //**[13h]**//
    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    //	DE_Low_Active();
    //------------------------------------
    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(480, 272); // LCD Resolution 480x272。
    //**[16h][17h]**//
    LCD_Horizontal_Non_Display(38); // 480x272，HSYNC Back Porch=38。
    //**[18h]**//
    LCD_HSYNC_Start_Position(20); // 480x272 HSYNC Front Porch = 20。
    //**[19h]**//
    LCD_HSYNC_Pulse_Width(20); // 480x272，HSYNC Pulse Width=5~30,set 20。
    //**[1Ch][1Dh]**//
    LCD_Vertical_Non_Display(4); // 480x272，VSYNC Back Porch =4。
    //**[1Eh]**//
    LCD_VSYNC_Start_Position(15); // 480x272，VSYNC Front Porch =15。
    //**[1Fh]**//
    LCD_VSYNC_Pulse_Width(3); // 480x272，VSYNC Pulse Width =3。

#endif

#ifdef LQ035NC111

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Rising();
    //	PCLK_Falling();
    VSCAN_T_to_B();
    PDATA_Set_RGB();

    //**[13h]**//
    HSYNC_Low_Active();
    VSYNC_Low_Active();
    //	DE_High_Active();
    DE_Low_Active();
    //------------------------------------
    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(320, 240); // LCD Resolution 320x240。
    //**[16h][17h]**//
    LCD_Horizontal_Non_Display(38); // 320x240，HSYNC Back Porch=38。
    //**[18h]**//
    LCD_HSYNC_Start_Position(20); // 320x240 HSYNC Front Porch = 20。
    //**[19h]**//
    LCD_HSYNC_Pulse_Width(20); // 320x240，HSYNC Pulse Width=5~30,set 20。
    //**[1Ch][1Dh]**//
    LCD_Vertical_Non_Display(4); // 320x240，VSYNC Back Porch =4。
    //**[1Eh]**//
    LCD_VSYNC_Start_Position(15); // 320x240，VSYNC Front Porch =15。
    //**[1Fh]**//
    LCD_VSYNC_Pulse_Width(3); // 320x240，VSYNC Pulse Width =3。

#endif

#ifdef ET101000DM6 // ##EDT_1024x600_ET101000DM6

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 600); // EDT_1024x600。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(160); // EDT_1024x600，Blank=90~376@320。
    LCD_HSYNC_Start_Position(160);   // EDT_1024x600，Blank=90~376@320。
    LCD_HSYNC_Pulse_Width(60);       // EDT_1024x600，Blank=90~376@320。

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(23); // EDT_1024x600，Blank=10~200@35。
    LCD_VSYNC_Start_Position(12); // EDT_1024x600，Blank=10~200@35。
    LCD_VSYNC_Pulse_Width(5);     // EDT_1024x600，Blank=10~200@35。

#endif

#ifdef G190SVT01 // ##AUO_1680x342_G190SVT01

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1680, 350); // 1680x342

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(30); // Blank=288。
    LCD_HSYNC_Start_Position(30);   //
    LCD_HSYNC_Pulse_Width(30);      //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(6); // Blank=16。
    LCD_VSYNC_Start_Position(6); //
    LCD_VSYNC_Pulse_Width(6);    //

#endif

#ifdef ZJ070NA_01B // ##INNOLUX_1024x600_ZJ070NA_01B

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1024, 600); // 1024x600

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(50); // Blank=320。
    LCD_HSYNC_Start_Position(50);   //
    LCD_HSYNC_Pulse_Width(220);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(8); // Blank=35。
    LCD_VSYNC_Start_Position(7); //
    LCD_VSYNC_Pulse_Width(20);   //

#endif

#ifdef LQ190E1LW52 // ##SHARP_1280x1024_LQ190E1LW52

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1280, 1024); // 1024x600

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(110); // Blank=204x2。
    LCD_HSYNC_Start_Position(110);   //
    LCD_HSYNC_Pulse_Width(110);      //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(15); // Blank=42。
    LCD_VSYNC_Start_Position(15); //
    LCD_VSYNC_Pulse_Width(12);    //

#endif

#ifdef HJ070IA_02F // ##INNOLUX_1280x800_HJ070IA_02F

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1280, 800); // 1280x800

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(30); // Blank=160。
    LCD_HSYNC_Start_Position(30);   //
    LCD_HSYNC_Pulse_Width(100);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(3); // Blank=23。
    LCD_VSYNC_Start_Position(3); //
    LCD_VSYNC_Pulse_Width(18);   //
#endif

#ifdef LQ156M1LG21 // ##SHARP_1920x1080_LQ156M1LG21

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    //	HSYNC_Low_Active();
    HSYNC_High_Active();

    //	VSYNC_Low_Active();
    VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1920, 1080); // 1280x800

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(40); // Blank min.=140。
    LCD_HSYNC_Start_Position(20);   //
    LCD_HSYNC_Pulse_Width(100);     //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(3); // Blank min.=31。
    LCD_VSYNC_Start_Position(3); //
    LCD_VSYNC_Pulse_Width(20);   //
#endif

#ifdef LQ201U1LW32 // ##SHARP_1600x1200_LQ201U1LW32

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    //	HSYNC_Low_Active();
    HSYNC_High_Active();

    //	VSYNC_Low_Active();
    VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1600, 1200); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(15);
    LCD_HSYNC_Start_Position(15);
    LCD_HSYNC_Pulse_Width(40);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(5);
    LCD_VSYNC_Start_Position(5);
    LCD_VSYNC_Pulse_Width(5);
#endif

#ifdef WF52ATLASDNN0 // ## 480x128 WF52ATLASDNN0

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(480, 272); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(27); //
    LCD_HSYNC_Start_Position(16);   //
    LCD_HSYNC_Pulse_Width(16);      //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(8); //
    LCD_VSYNC_Start_Position(8); //
    LCD_VSYNC_Pulse_Width(4);    //

#endif

#ifdef WF39ATIASDNN0 // ## 480x128 WF39ATIASDNN0

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(480, 272);

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(32); //
    LCD_HSYNC_Start_Position(8);    //
    LCD_HSYNC_Pulse_Width(8);       //

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(4); //
    LCD_VSYNC_Start_Position(4); //
    LCD_VSYNC_Pulse_Width(4);    //

#endif

#ifdef ATM1040L3A_T1 // ## 800x600 ATM1040L3A-T1

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(800, 600); // ATM1040L3A-T1 800x600。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(38); // ATM1040L3A-T1 800x600，46。
    LCD_HSYNC_Start_Position(210);  // ATM1040L3A-T1 800x600，16~354。
    LCD_HSYNC_Pulse_Width(8);       // ATM1040L3A-T1 800x600，1~40C

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(15); // ATM1040L3A-T1 800x600，23。
    LCD_VSYNC_Start_Position(12); // ATM1040L3A-T1 800x600，1~77。
    LCD_VSYNC_Pulse_Width(8);     // ATM1040L3A-T1 800x600，1~20C

#endif

#ifdef G121I1_L01 // INNOLUX_1280x800_G121I1_L01

    //**[03h]**//
    LVDS_Format1(); //(VESA format)//only for RA8877
                    //	LVDS_Format2();//(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    HSYNC_Low_Active();
    //	HSYNC_High_Active();

    VSYNC_Low_Active();
    //	VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(1280, 800); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(40); // 80~160~
    LCD_HSYNC_Start_Position(40);
    LCD_HSYNC_Pulse_Width(40);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(7); // 10~23~
    LCD_VSYNC_Start_Position(7);
    LCD_VSYNC_Pulse_Width(7);
#endif

#ifdef BA104S01_100 // BOE_800x600(京東方)// 18bpp LVDS

    //**[03h]**//
    //	LVDS_Format1();//(VESA format)//only for RA8877
    LVDS_Format2(); //(JEIDA format) //only for RA8877

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
                            //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    //**[12h]**//
    //	PCLK_Rising();
    PCLK_Falling();

    VSCAN_T_to_B();
    //	VSCAN_B_to_T();

    PDATA_Set_RGB();
    //	PDATA_Set_RBG();
    //	PDATA_Set_GRB();
    //	PDATA_Set_GBR();
    //	PDATA_Set_BRG();
    //	PDATA_Set_BGR();
    //	PDATA_Set_Gray();
    //	PDATA_IDLE_STATE();

    //**[13h]**//
    //	HSYNC_Low_Active();
    HSYNC_High_Active();

    //	VSYNC_Low_Active();
    VSYNC_High_Active();

    //	DE_Low_Active();
    DE_High_Active();

    //**[14h][15h][1Ah][1Bh]**//
    LCD_HorizontalWidth_VerticalHeight(800, 600); //

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(70); // 200
    LCD_HSYNC_Start_Position(70);
    LCD_HSYNC_Pulse_Width(70);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(20); // 60
    LCD_VSYNC_Start_Position(20);
    LCD_VSYNC_Pulse_Width(20);
#endif

#ifdef ET0700B3DMA

    //**[10h]**//
    Select_LCD_Sync_Mode(); // Enable XVSYNC, XHSYNC, XDE.
    //	Select_LCD_DE_Mode();	// XVSYNC & XHSYNC in idle state.

    PCLK_Falling();
    // PCLK_Rising();

    VSCAN_T_to_B();
    PDATA_Set_RGB();

    HSYNC_Low_Active();
    VSYNC_Low_Active();
    DE_High_Active();
    // DE_Low_Active();

    LCD_HorizontalWidth_VerticalHeight(800, 480); // 800x480。

    /*	[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR
        [18h] :		 Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1)x8
        [19h] :		 Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1)x8
    */
    LCD_Horizontal_Non_Display(46);
    LCD_HSYNC_Start_Position(210);
    LCD_HSYNC_Pulse_Width(10);

    /*	[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
        [1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
        [1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
    */
    LCD_Vertical_Non_Display(25);
    LCD_VSYNC_Start_Position(20);
    LCD_VSYNC_Pulse_Width(10);

#endif
}

void Set_Serial_Flash_IF(void)
{

    //(*) Using Serial Flash
#ifdef MX25L25635E
    Enable_SFlash_SPI();
#endif
#ifdef W25Q256FV
    Enable_SFlash_SPI();
#endif
    //(*) Using GT_ROM
#ifdef GT21L16T1W
    GTFont_Select_GT21L16T1W();
    Enable_SFlash_SPI();
#endif
#ifdef GT30L16U2W
    GTFont_Select_GT30L16U2W();
    Enable_SFlash_SPI();
#endif
#ifdef GT30L24T3Y
    GTFont_Select_GT30L24T3Y();
    Enable_SFlash_SPI();
#endif
#ifdef GT30L24M1Z
    GTFont_Select_GT30L24M1Z();
    Enable_SFlash_SPI();
#endif
#ifdef GT30L32S4W
    GTFont_Select_GT30L32S4W();
    Enable_SFlash_SPI();
#endif
#ifdef GT20L24F6Y
    GTFont_Select_GT20L24F6Y();
    Enable_SFlash_SPI();
#endif
#ifdef GT21L24S1W
    GTFont_Select_GT21L24S1W();
    Enable_SFlash_SPI();
#endif

    //(*)
    Select_SFI_24bit_Address();
    //	Select_SFI_32bit_Address();

    //(*)
    Select_standard_SPI_Mode0_or_Mode3();
    // Select_RA8875_SPI_Mode0_and_Mode3();

    //(*)
    Select_SFI_Single_Mode_Dummy_0T_03h();
    //	Select_SFI_Single_Mode_Dummy_8T_0Bh();
    //	Select_SFI_Single_Mode_Dummy_16T_1Bh();
    //	Select_SFI_Dual_Mode_Dummy_8T_3Bh();
    //	Select_SFI_Dual_Mode_Dummy_4T_BBh();
    //	Select_SFI_Quad_Mode_Dummy_8T_6Bh();
    //	Select_SFI_Quad_Mode_Dummy_4T_EBh();

    //(*)

    Set_CPOL();
    Set_CPHA();

    Reset_CPOL();
    Reset_CPHA();

    //(*) clock speed
    SPI_Clock_Period(3);

    /******************************/
}

void RA8889_SDRAM_initial(void)
{
    unsigned char CAS_Latency;
    unsigned short Auto_Refresh;
    //
    // Auto_Refresh=(64*DRAM_FREQ*1000)/(8192);

    CAS_Latency  = 3;
    Auto_Refresh = (64 * DRAM_FREQ * 1000) / (4096);
    Auto_Refresh = Auto_Refresh - 2; // 提前啟動refresh，避免剛好碰到極限範圍。// Start [refresh] in advance to avoid just reaching the limits.

    LCD_RegisterWrite(0xe0, 0x29);
    LCD_RegisterWrite(0xe1, CAS_Latency); // CAS:2=0x02，CAS:3=0x03
    LCD_RegisterWrite(0xe2, Auto_Refresh);
    LCD_RegisterWrite(0xe3, Auto_Refresh >> 8);
    LCD_RegisterWrite(0xe4, 0x01);

    Check_SDRAM_Ready();
}

/*------------------------------------//------------------------------------
//------------------------------------//----------------------------------*/
void RA8889_PLL(unsigned short DRAM_clock, unsigned short CORE_clock, unsigned short SCAN_clock)
{
    /*
    [A]
    DRAM_clock maximum 166 MHz
    CORE_clock maximum 133 MHz
    SCAN_clock maximum 100 MHz

    [B]
    (1) 10MHz <= OSC_FREQ <= 15MHz
    (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
    (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 500MHz
    (4) In addition, please pay special attention to:
        [DRAM_clock] >= [CORE_clock],
        [CORE_clock] >= 2x[SCAN_clock].
    PLLDIVM:0
    PLLDIVN:1~63
    PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
    ex:
     OSC_FREQ = 10MHz
     Set X_DIVK=2
     Set X_DIVM=0
     => (X_DIVN+1)=(XPLLx4)/10
    */

    unsigned short x_Divide, PLLC1, PLLC2;
    unsigned short pll_m_lo, pll_m_hi;
    unsigned char temp;

    // Set DRAM clock
    if ((DRAM_clock >= 125) && (DRAM_clock <= 166)) {
        LCD_RegisterWrite(0x07, 0x02); // PLL Divided by 2
        LCD_RegisterWrite(0x08, (DRAM_clock * 2 / OSC_FREQ) - 1);
    } else if ((DRAM_clock >= 63) && (DRAM_clock <= 124)) {
        LCD_RegisterWrite(0x07, 0x04); // PLL Divided by 4
        LCD_RegisterWrite(0x08, (DRAM_clock * 4 / OSC_FREQ) - 1);
    } else if ((DRAM_clock >= 32) && (DRAM_clock <= 62)) {
        LCD_RegisterWrite(0x07, 0x06); // PLL Divided by 8
        LCD_RegisterWrite(0x08, (DRAM_clock * 8 / OSC_FREQ) - 1);
    } else // if(DRAM_clock<32)
    {
        LCD_RegisterWrite(0x07, 0x06);                    // PLL Divided by 8
        LCD_RegisterWrite(0x08, (32 * 8 / OSC_FREQ) - 1); //
    }

    // Set Core clock
    if ((CORE_clock >= 125) && (CORE_clock <= 133)) {
        LCD_RegisterWrite(0x09, 0x02); // PLL Divided by 2
        LCD_RegisterWrite(0x0A, (CORE_clock * 2 / OSC_FREQ) - 1);
    } else if ((CORE_clock >= 63) && (CORE_clock <= 124)) {
        LCD_RegisterWrite(0x09, 0x04); // PLL Divided by 4
        LCD_RegisterWrite(0x0A, (CORE_clock * 4 / OSC_FREQ) - 1);
    } else if ((CORE_clock >= 32) && (CORE_clock <= 62)) {
        LCD_RegisterWrite(0x09, 0x06); // PLL Divided by 8
        LCD_RegisterWrite(0x0A, (CORE_clock * 8 / OSC_FREQ) - 1);
    } else // if(CORE_clock<32)
    {
        LCD_RegisterWrite(0x09, 0x06);                    // PLL Divided by 8
        LCD_RegisterWrite(0x0A, (32 * 8 / OSC_FREQ) - 1); //
    }

    // Set pixel clock
    if ((SCAN_clock >= 63) && (SCAN_clock <= 100)) {
        LCD_RegisterWrite(0x05, 0x04); // PLL Divided by 4
        LCD_RegisterWrite(0x06, (SCAN_clock * 4 / OSC_FREQ) - 1);
    } else if ((SCAN_clock >= 32) && (SCAN_clock <= 62)) {
        LCD_RegisterWrite(0x05, 0x06); // PLL Divided by 8
        LCD_RegisterWrite(0x06, (SCAN_clock * 8 / OSC_FREQ) - 1);
    } else if ((SCAN_clock >= 16) && (SCAN_clock <= 31)) {
        LCD_RegisterWrite(0x05, 0x16); // PLL Divided by 16
        LCD_RegisterWrite(0x06, (SCAN_clock * 16 / OSC_FREQ) - 1);
    } else if ((SCAN_clock >= 8) && (SCAN_clock <= 15)) {
        LCD_RegisterWrite(0x05, 0x26); // PLL Divided by 32
        LCD_RegisterWrite(0x06, (SCAN_clock * 32 / OSC_FREQ) - 1);
    } else if ((SCAN_clock > 0) && (SCAN_clock <= 7)) {
        LCD_RegisterWrite(0x05, 0x36); // PLL Divided by 64
        LCD_RegisterWrite(0x06, (SCAN_clock * 64 / OSC_FREQ) - 1);
    } else // if out of range, set 32MHz for debug.
    {
        LCD_RegisterWrite(0x05, 0x06);
        LCD_RegisterWrite(0x06, (32 * 8 / OSC_FREQ) - 1);
    }

    Enable_PLL();
    delay_ms(100);
}

//[46h]=========================================================================
void SPIM_Clock_Divided_2(void) //(default)
{
    /*
    REG[46h] [bit2] Spi master frequency eliminator
    0: SPIM_clock = ( Core clock / 2 ) / ( frequency eliminator )
    1: SPIM_clock = ( Core clock / 1 ) / ( frequency eliminator )
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp &= cClrb2;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void SPIM_Clock_Divided_1(void) //
{
    /*
    REG[46h] [bit2] Spi master frequency eliminator

    0: SPIM_clock = ( Core clock / 2 ) / ( frequency eliminator )
    1: SPIM_clock = ( Core clock / 1 ) / ( frequency eliminator )
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp |= cSetb2;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void SPIM_Selectable_Bus_mode(void) //(default)
{
    /*
    REG[46h] [bit1] SPI_Master_Separate
    0: bus ???(可選擇)
    1: bus ???(Bus0: CS0 CS1 / Bus1: CS2 CS3)
    */
    unsigned char temp;
    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp &= cClrb1;
    LCD_DataWrite(temp);
}
void SPIM_Fixed_Bus_mode(void) //
{
    /*
    REG[46h] [bit1] SPI_Master_Separate
    0: bus ???(可選擇)
    1: bus ???(Bus0: CS0 CS1 / Bus1: CS2 CS3)
    */
    unsigned char temp;
    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp |= cSetb1;
    LCD_DataWrite(temp);
}

void REG_Switch_to_REG0(void) //(default)
{
    /*
    REG[46h] [bit0] Page switch, and SPIM bus switch.
    0: page 0, SPIM bus 0.
    1: page 1, SPIM bus 1.
    */
    unsigned char temp;
    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);

    delay_ms(1); // for test
}
void REG_Switch_to_REG1(void) //
{
    /*
    REG[46h] [bit0] Page switch, and SPIM bus switch.
    0: page 0, SPIM bus 0.
    1: page 1, SPIM bus 1.
    */
    unsigned char temp;
    LCD_CmdWrite(0x46);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);

    delay_ms(1); // for test
}

//[B7h]=========================================================================
void Select_SFI_0(void)
{
    /*[bit7]
    Serial Flash/ROM I/F # Select
    0: Serial Flash/ROM 0 I/F is selected.
    1: Serial Flash/ROM 1 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Select_SFI_1(void)
{
    /*[bit7]
    Serial Flash/ROM I/F # Select
    0: Serial Flash/ROM 0 I/F is selected.
    1: Serial Flash/ROM 1 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}
void Select_SFI_2(void)
{
    /*[bit7]
    Serial Flash/ROM I/F # Select
    0: Serial Flash/ROM 0 I/F is selected.
    1: Serial Flash/ROM 1 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb7;
    LCD_DataWrite(temp);
}
void Select_SFI_3(void)
{
    /*[bit7]
    Serial Flash/ROM I/F # Select
    0: Serial Flash/ROM 0 I/F is selected.
    1: Serial Flash/ROM 1 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb7;
    LCD_DataWrite(temp);
}

//[B9h]=========================================================================
// REG[B9h] SPI master Control Register (SPIMCR2)

void Select_nSS_drive_on_xnsfcs0(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}

void Select_nSS_drive_on_xnsfcs1(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp &= cClrb7;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}
void Select_nSS_drive_on_xnsfcs2(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp &= cClrb5;
    LCD_DataWrite(temp);
}

void Select_nSS_drive_on_xnsfcs3(void)
{
    unsigned char temp;
    LCD_CmdWrite(0xB9);
    temp = LCD_DataRead();
    temp |= cSetb7;
    temp |= cSetb5;
    LCD_DataWrite(temp);
}

//[REG PAGE 1]
//[0Bh]=========================================================================
void Disable_IDEC_Interrupt(void)
{
    /* [REG PAGE 1], [0Bh]
    [bit0] IDEC Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void Enable_IDEC_Interrupt(void)
{
    /* [REG PAGE 1], [0Bh]
    [bit0] IDEC Interrupt Enable Bit
    0: Disable Interrupt.
    1: Enable Interrupt.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x0B);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[0Ch]=========================================================================
void Clear_IDEC_Interrupt(void)
{
    /* [REG PAGE 1], [0Ch]
    [bit0] IDEC Interrupt flag
    Write Function?膏nterrupt Clear Bit
    0: No operation.
    1: Clear IDEC interrupt.
    Read Function?膏nterrupt Status
    0: No IDEC interrupt happens.
    1: IDEC interrupt happens.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x0C);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[0Dh]=========================================================================
void Unmask_IDEC_Interrupt(void)
{
    /* [REG PAGE 1], [0Dh]
    [bit0] Mask IDEC Interrupt Flag
    0: Unmask.
    1: Mask.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp &= cClrb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void Mask_IDEC_Interrupt(void)
{
    /* [REG PAGE 1], [0Dh]
    [bit0] Mask IDEC Interrupt Flag
    0: Unmask.
    1: Mask.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x0D);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[2Eh]~[31h]=========================================================================
// AVI shadow pip start address
void AVI_Shadow_PIP_Start_Address(unsigned long Addr)
{
    /*[REG PAGE 1]
    [2Eh] AVI shadow PIP Start Address[7:2]
    [2Fh] AVI shadow PIP Start Address[15:8]
    [30h] AVI shadow PIP Start Address [23:16]
    [31h] AVI shadow PIP Start Address [31:24]
    */

    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0x2E);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x2F);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x30);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x31);
    LCD_DataWrite(Addr >> 24);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[A0h] – Video Control (VC)=========================================================================
unsigned char MediaFile_ErrorFlag(void)
{
    /*[REG PAGE 1], REG[A0h],
    [bit7] Media error, indicate either the unsupported image format or header format error.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA0);
    temp = LCD_DataRead();
    temp &= 0x80;

    REG_Switch_to_REG0();

    return temp;
}

void Check_Busy_Media_Decode(void)
{
    /*[REG PAGE 1], REG[A0h],
    [bit6] Media decode busy
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    // case4: check AVI
    LCD_CmdWrite(0xA0);
    do {
        temp = LCD_DataRead();
    } while (temp & 0x40);

    REG_Switch_to_REG0();
}

unsigned char Read_Media_Decode_Busy(void)
{
    /*[REG PAGE 1], REG[A0h],
    [bit6] Media decode busy
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    // case4: check AVI
    LCD_CmdWrite(0xA0);
    temp = LCD_DataRead();

    REG_Switch_to_REG0();

    return temp;
}

unsigned char Check_Media_Fifo_Flag(void)
{
    /*[REG PAGE 1], REG[A0h],
    [bit5] Media fifo empty
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA0);
    temp = LCD_DataRead();
    temp &= 0x20;

    REG_Switch_to_REG0();

    return temp;
}

//[REG PAGE 1]
//[A1h]~[A8h]=========================================================================
unsigned short Read_Media_Height(void)
{
    /*[REG PAGE 1],
    REG[A1h]	Extracted from media (BMP/JPEG/AVI) header Height[15:8]
    REG[A2h]	Extracted from media (BMP/JPEG/AVI) header Height[7:0]
    */
    unsigned short temp, Media_height;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA1);
    temp = LCD_DataRead();

    LCD_CmdWrite(0xA2);
    Media_height = LCD_DataRead();

    Media_height = (temp << 8) + Media_height;

    REG_Switch_to_REG0();

    return Media_height;
}
unsigned short Read_Media_Width(void)
{
    /*[REG PAGE 1],
    REG[A3h]	Extracted from media (BMP/JPEG/AVI) header Width[15:8]
    REG[A4h]	Extracted from media (BMP/JPEG/AVI) header Width[7:0]
    */
    unsigned short temp, Media_Width;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA3);
    temp = LCD_DataRead();

    LCD_CmdWrite(0xA4);
    Media_Width = LCD_DataRead();

    Media_Width = (temp << 8) + Media_Width;

    REG_Switch_to_REG0();

    return Media_Width;
}

unsigned long Read_Media_Data_Amount(void)
{
    /*[REG PAGE 1],
    REG[A5h]	Extracted from AVI header VFPB[31:24]
    REG[A6h]	Extracted from AVI header VFPB[23:16]
    REG[A7h]	Extracted from AVI header VFPB[15:8]
    REG[A8h]	Extracted from AVI header VFPB[7:0]
    */
    unsigned char temp1, temp2, temp3, temp4;
    unsigned long Media_Data_Amount;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA5);
    temp4 = LCD_DataRead();

    LCD_CmdWrite(0xA6);
    temp3 = LCD_DataRead();

    LCD_CmdWrite(0xA7);
    temp2 = LCD_DataRead();

    LCD_CmdWrite(0xA8);
    temp1 = LCD_DataRead();

    Media_Data_Amount = (temp4 << 24) + (temp3 << 16) + (temp2 << 8) + temp1;

    REG_Switch_to_REG0();

    return Media_Data_Amount;
}

//[REG PAGE 1]
//[A9h]=========================================================================
void Reset_IDEC(void)
{
    /*[REG PAGE 1], REG[A9h], Idec reset , clear Idec circuit
    1: no active
    0: reset
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xA9);
    LCD_DataWrite(0x02);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[B6h]=========================================================================
void IDEC_Select_SFI_0(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit7-6]
    00: Serial Flash/ROM 0 I/F is selected.
    01: Serial Flash/ROM 1 I/F is selected.
    10: Serial Flash/ROM 2 I/F is selected.
    11: Serial Flash/ROM 3 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3E; // if bit0=1, IDEC will start .
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void IDEC_Select_SFI_1(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit7-6]
    00: Serial Flash/ROM 0 I/F is selected.
    01: Serial Flash/ROM 1 I/F is selected.
    10: Serial Flash/ROM 2 I/F is selected.
    11: Serial Flash/ROM 3 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3E; // if bit0=1, IDEC will start .
    temp |= 0x40;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_2(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit7-6]
    00: Serial Flash/ROM 0 I/F is selected.
    01: Serial Flash/ROM 1 I/F is selected.
    10: Serial Flash/ROM 2 I/F is selected.
    11: Serial Flash/ROM 3 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3E; // if bit0=1, IDEC will start .
    temp |= 0x80;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_3(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit7-6]
    00: Serial Flash/ROM 0 I/F is selected.
    01: Serial Flash/ROM 1 I/F is selected.
    10: Serial Flash/ROM 2 I/F is selected.
    11: Serial Flash/ROM 3 I/F is selected.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x3E; // if bit0=1, IDEC will start .
    temp |= 0xC0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[B6h][bit4-3]====
void FONT_DMA_Select_Bus_0(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit4] FONT/DMA serial flash sck and data bus select
    0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
    1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xEE; // if bit0=1, IDEC will start .
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void FONT_DMA_Select_Bus_1(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit4] FONT/DMA serial flash sck and data bus select
    0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
    1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xEE; // if bit0=1, IDEC will start .
    temp |= 0x10;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_Bus_0(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit3]
    IDEC sck and data bus select
    0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
    1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xF6; // if bit0=1, IDEC will start .
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_Bus_1(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit3]
    IDEC sck and data bus select
    0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
    1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xF6; // if bit0=1, IDEC will start .
    temp |= 0x08;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[B6h][bit2-1]====
void IDEC_Destination_Color_8bpp(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit2-1]
    IDEC destination Color depth:
    00: 8bit
    01: 16 bit
    10: 24bit
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xF8; // if bit0=1, IDEC will start .
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Destination_Color_16bpp(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit2-1]
    IDEC destination Color depth:
    00: 8bit
    01: 16 bit
    10: 24bit
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xF8; // if bit0=1, IDEC will start .
    temp |= 0x02;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Destination_Color_24bpp(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit2-1]
    IDEC destination Color depth:
    00: 8bit
    01: 16 bit
    10: 24bit
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0xF8; // if bit0=1, IDEC will start .
    temp |= 0x04;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[B6h][bit0]====
void IDEC_Starts_Decoding(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit0]
    Write Function: IDEC Start Bit
    Set to 1 by MPU and reset to 0 automatically
    It cannot start when fontwr_busy is 1. And if IDEC is enabled,
    serial flash I/F can’t be set as text mode & send character code.
    Read Function: IDEC Busy Check Bit
    0: Idle
    1: Busy
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp |= cSetb0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void Check_Busy_IDEC(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit0]
    Write Function: IDEC Start Bit
    Set to 1 by MPU and reset to 0 automatically
    It cannot start when fontwr_busy is 1. And if IDEC is enabled,
    serial flash I/F can’t be set as text mode & send character code.
    Read Function: IDEC Busy Check Bit
    0: Idle
    1: Busy
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    do {
        LCD_CmdWrite(0xB6);
        temp = LCD_DataRead();
    } while (temp & 0x01);

    REG_Switch_to_REG0();
}

unsigned char Read_IDEC_Busy(void)
{
    /*[REG PAGE 1], REG[B6h],
    [bit0]
    Write Function: IDEC Start Bit
    Set to 1 by MPU and reset to 0 automatically
    It cannot start when fontwr_busy is 1. And if IDEC is enabled,
    serial flash I/F can’t be set as text mode & send character code.
    Read Function: IDEC Busy Check Bit
    0: Idle
    1: Busy
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB6);
    temp = LCD_DataRead();
    temp &= 0x01;
    REG_Switch_to_REG0();

    return temp;
}

//[REG PAGE 1]
//[B7h]=========================================================================

void IDEC_Select_SFI_24bit_Address(void)
{
    /*[REG PAGE 1],[B7h] [bit5]
    IDEC Serial Flash/ROM Address Mode
    0: 24 bits address mode
    1: 32 bits address mode
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb5;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_32bit_Address(void)
{
    /*[REG PAGE 1],[B7h] [bit5]
    IDEC Serial Flash/ROM Address Mode
    0: 24 bits address mode
    1: 32 bits address mode
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb5;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void IDEC_Select_standard_SPI_Mode0_or_Mode3(void)
{
    /*[REG PAGE 1],[B7h] [bit4]
    IDEC Serial Interface
    0: standard SPI mode 0 or mode 3 timing
    1: Follow RA8875 mode 0 & mode 3 timing
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= cClrb4;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_RA8875_SPI_Mode0_and_Mode3(void)
{
    /*[REG PAGE 1],[B7h] [bit4]
    IDEC Serial Interface
    0: standard SPI mode 0 or mode 3 timing
    1: Follow RA8875 mode 0 & mode 3 timing
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp |= cSetb4;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void IDEC_Select_SFI_Single_Mode_Dummy_0T_03h(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    0000b: 1x read command code = 03h. Without dummy cycle between address and data.
    0010b: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    0100b: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_Single_Mode_Dummy_8T_0Bh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    0000b: 1x read command code = 03h. Without dummy cycle between address and data.
    0010b: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    0100b: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x02;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_Single_Mode_Dummy_16T_1Bh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    0000b: 1x read command code = 03h. Without dummy cycle between address and data.
    0010b: 1x read command code = 0Bh. 8 dummy cycles inserted between address and data.
    0100b: 1x read command code = 1Bh. 16 dummy cycles inserted between address and data.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x04;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_Dual_Mode_Dummy_8T_3Bh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    0110b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
    1000b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x06;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_Dual_Mode_Dummy_4T_BBh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    0110b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
    1000b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x08;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

void IDEC_Select_SFI_Quad_Mode_Dummy_8T_6Bh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    1010b: 4x read command code – 6Bh.
    1100b: 4x read command code – EBh.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x0A;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}
void IDEC_Select_SFI_Quad_Mode_Dummy_4T_EBh(void) //
{
    /*[REG PAGE 1],[B7h]
    [bit3-0]
    1010b: 4x read command code – 6Bh.
    1100b: 4x read command code – EBh.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xB7);
    temp = LCD_DataRead();
    temp &= 0xF0;
    temp |= 0x0C;
    LCD_DataWrite(temp);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[BBh]=========================================================================
void IDEC_SPI_Clock_Divide(unsigned char IDEC_SPIclock_divide) //
{
    /*[REG PAGE 1],[BBh] idec SF Clock divide
    [bit 1-0]
    00: idec_SF clock = cclk
    01: idec_SF clock = cclk/2
    10: idec SF clock  = cclk/4
    11: reverse

    NOTE: IDEC_clock must be more than 2 times OSC clock. IDEC_clock must be > 2 x OSC_clock.
    */
    unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xBB);
    LCD_DataWrite(IDEC_SPIclock_divide);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[BCh]~[BFh]=========================================================================
void IDEC_Source_Start_Address(unsigned long Addr) // AVI,JPG,BMP	//
{
    /*[REG PAGE 1]
    [BCh] Serial flash IDEC Source START ADDRESS [7:0]
    [BDh] Serial flash IDEC Source START ADDRESS [15:8]
    [BEh] Serial flash IDEC Source START ADDRESS [23:16]
    [BFh] Serial flash IDEC Source START ADDRESS [31:24]
    */

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xBC);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0xBD);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0xBE);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0xBF);
    LCD_DataWrite(Addr >> 24);

    REG_Switch_to_REG0();
}
//[REG PAGE 1]
//[C0h][C1h][C2h][C3h]=========================================================================
void IDEC_Destination_Upper_Left_Corner(unsigned short WX, unsigned short HY)
{
    /*
    [C0h][C1h]: IDEC_Destination X-coordinates [7:0][15:8]
    [C2h][C3h]: IDEC_Destination Y-coordinates [7:0][15:8]
    */

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xC0);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xC1);
    LCD_DataWrite(WX >> 8);

    LCD_CmdWrite(0xC2);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0xC3);
    LCD_DataWrite(HY >> 8);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[C5h]=========================================================================
void IDEC_AVI_Decoding_to_PIP1_and_Shadow(void)
{
    /*[REG PAGE 1],[C5h]
    [bit1-0]
    00b: AVI_Decoding_to_PIP1_and_Shadow
    01b: AVI_Decoding_to_PIP2_and_Shadow
    1xb: AVI_Decoding_to_PIP1_no_Shadow
    */
    REG_Switch_to_REG1();

    LCD_CmdWrite(0xC5);
    LCD_DataWrite(0x00);

    REG_Switch_to_REG0();
}
void IDEC_AVI_Decoding_to_PIP2_and_Shadow(void)
{
    /*[REG PAGE 1],[C5h]
    [bit1-0]
    00b: AVI_Decoding_to_PIP1_and_Shadow
    01b: AVI_Decoding_to_PIP2_and_Shadow
    1xb: AVI_Decoding_to_PIP1_no_Shadow
    */
    REG_Switch_to_REG1();

    LCD_CmdWrite(0xC5);
    LCD_DataWrite(0x01);

    REG_Switch_to_REG0();
}
void IDEC_AVI_Decoding_to_PIP1_no_Shadow(void)
{
    /*[REG PAGE 1],[C5h]
    [bit1-0]
    00b: AVI_Decoding_to_PIP1_and_Shadow
    01b: AVI_Decoding_to_PIP2_and_Shadow
    1xb: AVI_Decoding_to_PIP1_no_Shadow
    */
    REG_Switch_to_REG1();

    LCD_CmdWrite(0xC5);
    LCD_DataWrite(0x02);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[C6h][C7h][C8h][C9h]=========================================================================
void IDEC_Transfer_Number(unsigned long Num)
{
    /*
    IDEC Transfer Number [7:0][15:8][23:16][31:24]
    */
    REG_Switch_to_REG1();

    LCD_CmdWrite(0xC6);
    LCD_DataWrite(Num);
    LCD_CmdWrite(0xC7);
    LCD_DataWrite(Num >> 8);
    LCD_CmdWrite(0xC8);
    LCD_DataWrite(Num >> 16);
    LCD_CmdWrite(0xC9);
    LCD_DataWrite(Num >> 24);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[CAh][CBh][CCh][CDh]=========================================================================
void IDEC_Destination_Start_Address(unsigned long Addr) // ,JPG,BMP	//
{
    /*[REG PAGE 1]
    REG[CAh] IDEC (JPG/BMP) Destination start addr 0 [7:0]
    REG[CBh] IDEC (JPG/BMP) Destination start addr 1 [15:8]
    REG[CCh] IDEC (JPG/BMP) Destination start addr 2 [23:16]
    REG[CDh] IDEC (JPG/BMP) Destination start addr 3 [31:24]
    Note: just only for JPG/BMP
    */

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xCA);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0xCB);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0xCC);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0xCD);
    LCD_DataWrite(Addr >> 24);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[CEh][CFh]=========================================================================
void IDEC_Destination_Image_Width(unsigned short WX) // ,JPG,BMP	//
{
    /*[REG PAGE 1]
    REG[CEh] IDEC (JPG/BMP) Destination Image_Width [7:0]
    REG[CFh] IDEC (JPG/BMP) Destination Image_Width [12:8]
    Note: just only for JPG/BMP
    */

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xCE);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0xCF);
    LCD_DataWrite(WX >> 8);

    REG_Switch_to_REG0();
}

//[REG PAGE 0]
//[C5h]=========================================================================
void SPIM_Select_Bus_0(void)
{
    /*[REG PAGE 0],[C5h]
    [bit7] SPI master bus select
    0b:  Bus 0
    1b:  Bus 1
    */
    unsigned char temp;

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xC5);
    temp = LCD_DataRead();
    temp &= 0x7F; // 設定REG c5h, bit 7=0
    LCD_DataWrite(temp);
}
void SPIM_Select_Bus_1(void) //
{
    /*[REG PAGE 0],[C5h]
    [bit7] SPI master bus select
    0b:  Bus 0
    1b:  Bus 1
    */
    unsigned char temp;

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xC5);
    temp = LCD_DataRead();
    temp |= 0x80; // 設定REG c5h, bit 7=1
    LCD_DataWrite(temp);
}

void SPI_Master_LatchEdge_Select_Rising(void)
{
    /*[REG PAGE 0],[C5h]
    [bit5] SPI master rx register latch edge
    0: cclk rising edge
    1: cclk falling edge
    */
    unsigned char temp;

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xC5);
    temp = LCD_DataRead();
    temp &= 0xDF; // 設定REG c5h, bit 5=0, B1101_1111.
    LCD_DataWrite(temp);
}
void SPI_Master_LatchEdge_Select_Falling(void)
{
    /*[REG PAGE 0],[C5h]
    [bit5] SPI master rx register latch edge
    0: cclk rising edge
    1: cclk falling edge
    */
    unsigned char temp;

    REG_Switch_to_REG0();

    LCD_CmdWrite(0xC5);
    temp = LCD_DataRead();
    temp |= 0x20; // 設定REG c5h, bit 5=1, B0010_0000.
    LCD_DataWrite(temp);
}

//[REG PAGE 1]
//[D3h]=========================================================================
void Press_AVI_Pause(void)
{
    /*[REG PAGE 1], REG[D3h],
    [bit0] Pause, the video will be paused when the bit is set
    Write	:	1: entire pause / exit pause.
    Read	:	1: AVI pause;
                0:AVI display.
    */
    //	unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xD3);
    LCD_DataWrite(0x01);

    REG_Switch_to_REG0();
}

//[REG PAGE 1]
//[D4h]=========================================================================
void Stop_AVI(void)
{
    /*[REG PAGE 1], REG[D4h],
    [bit0] Stop, the video will be stopped and exited when the bit is set
    1: stop enable
    0: no operation
    */
    //	unsigned char temp;

    REG_Switch_to_REG1();

    LCD_CmdWrite(0xD4);
    LCD_DataWrite(0x01);

    REG_Switch_to_REG0();
}

