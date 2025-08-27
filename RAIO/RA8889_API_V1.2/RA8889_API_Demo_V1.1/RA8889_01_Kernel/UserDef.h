
  /*************** RAIO Technology Inc. ***************
  * file		: UserDef.h
  * author		: RAIO Application Team 
  * version		: V1.0 
  * date		: 2020/03 
  * brief		: 	
	* RAIO copyright
  ****************************************************/	

#ifndef __UserDef_H
#define __UserDef_H


//------------------------------------------------------------------------------
// Hardware Setting
//------------------------------------------------------------------------------
/*==== [HW_(1)]   MCU   =====*/
//#define MEGAWIN_82G516A
//#define STM_32



/*==== [HW_(2)] MCU I/F   =====*/
	#define Parallel_8080
//	#define Parallel_6800
//	#define Serial_3
//	#define Serial_4
//	#define Serial_I2C
  
/*==== [HW_(3)] SDRAM  =====*/


/*==== [HW_(4)] Panel   =====*/
//	#define EJ080NA_04B		//##INNOLUX_1024x768_EJ080NA-04B
//	#define LQ150X1LGN2C	//##SHARP_LQ150X1LGN2C_LVDS1 & LVDS2
//	#define N070ICG_LD1		//##INNOLUX_1280x800_N070ICG-LD1
//	#define B116XW03_V0		//##AUO1366x768_B116XW03 V0
//	#define G121XN01_V0		//##AUO1024x768 G121XN01 V0
//	#define LQ121S1LG81		//##SHARP_LQ121S1LG81
//	#define LQ035NC111
//	#define EJ080NA_05B		//##INNOLUX_800x600
	#define AT070TN92		//##INNOLUX_800x480
//	#define AWY_800480T70N02
// 	#define AWT_1024600L7N03
//	#define ET101000DM6		//##EDT_1024x600_ET101000DM6
//	#define G190SVT01		//##AUO_1680x342_G190SVT01
//	#define ZJ070NA_01B		//##INNOLUX_1024x600_ZJ070NA_01B
//	#define	LQ190E1LW52		//##SHARP_1280x1024_LQ190E1LW52
//	#define HJ070IA_02F		//##INNOLUX_1280x800_HJ070IA_02F
//	#define LQ156M1LG21		//##SHARP_1920x1080_LQ156M1LG21
//	#define LQ201U1LW32		//##SHARP_1600x1200_LQ201U1LW32
//	#define WF52ATLASDNN0	//## 480x128 WF52ATLASDNN0
//	#define WF39ATIASDNN0	//## 480x128 WF39ATIASDNN0
//	#define ATM1040L3A_T1	//## 800x600 ATM1040L3A-T1

//	#define G121I1_L01		//INNOLUX_1280x800_G121I1_L01
//	#define BA104S01_100	//BOE_800x600(¨ÊªF¤è)// 18bpp LVDS

//	#define LCD_1280x800	//for test

//	#define LCD_1366x768	//for test
	#define ET0700B3DMA		// Same as RA8875 800x480 

/*==== [HW_(5)] Serial Flash Memory */ 
//	#define MX25L25635E
	#define W25Q256FV


/*==== [HW_(6)]  GT_Font ROM*/ 
//	#define GT21L16T1W
//	#define GT30L16U2W
	#define GT30L24T3Y
//	#define GT30L24M1Z
//	#define GT30L32S4W
//	#define GT20L24F6Y
//	#define GT21L24S1W


#define canvas_image_width  4800 //set RAM=(800x6)x(480x2) for RA8889 API.C 
#define main_image_width  4800  //set RAM=(800x6)x(480x2) for RA8889 API.C 
#define LCD_width  800 
#define LCD_legth  480 


//------------------------------------------------------------------------------
// Software Setting
//------------------------------------------------------------------------------

/*==== [SW_(1)]  PLL  =====*/
#define OSC_FREQ	10	// OSC clock frequency, unit: MHz.

#define DRAM_FREQ	140	// SDRAM clock frequency, unit: MHz. 
#define CORE_FREQ	120	// Core (system) clock frequency, unit: MHz. 
#define SCAN_FREQ	30	// Panel Scan clock frequency, unit: MHz.

//#ifdef WF52ATLASDNN0
//
//#define DRAM_FREQ	100	// SDRAM clock frequency, unit: MHz. 
//#define CORE_FREQ	100	// Core (system) clock frequency, unit: MHz. 
//#define SCAN_FREQ	9	// Panel Scan clock frequency, unit: MHz.
//
//#endif

/*==== [SW_(2)]  select MCU and color depth  =====*/
//	#define MCU_8bit_ColorDepth_8bpp			  
//	#define MCU_8bit_ColorDepth_16bpp		
//	#define MCU_8bit_ColorDepth_24bpp
//	#define MCU_16bit_ColorDepth_16bpp		
	#define MCU_16bit_ColorDepth_24bpp_Mode_1
//	#define MCU_16bit_ColorDepth_24bpp_Mode_2




/*==== [SW_()]  Master I2C  =====*/
#define Enable_I2CM_Noise_Filter
//#define Disable_I2CM_Noise_Filter



/*==== [SW_()]    =====*/

	#define RAIO_AE

	#define Check_While_Busy




/*==== [SW_()]    =====*/









//------------------------------------------------------------------------------
// I/O Setting
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//User Parameter Defines
//------------------------------------------------------------------------------
//	typedef unsigned char uchar;
//	typedef unsigned int uint;
//	typedef unsigned short ushort;
//	typedef unsigned long ulong;

//--

//#define true		1
//#define	false		0

//--

#define color256_black   0x00
#define color256_white   0xff
#define color256_red     0xe0
#define color256_green   0x1c
#define color256_blue    0x03
#define color256_yellow  color256_red|color256_green
#define color256_cyan    color256_green|color256_blue
#define color256_purple  color256_red|color256_blue
 
#define color65k_black   0x0000
#define color65k_white   0xffff
#define color65k_red     0xf800
#define color65k_green   0x07e0
#define color65k_blue    0x001f
#define color65k_yellow  color65k_red|color65k_green
#define color65k_cyan    color65k_green|color65k_blue
#define color65k_purple  color65k_red|color65k_blue
 
#define color16M_black   0x00000000
#define color16M_white   0x00ffffff
#define color16M_red     0x00ff0000
#define color16M_green   0x0000ff00
#define color16M_blue    0x000000ff
#define color16M_yellow  color16M_red|color16M_green
#define color16M_cyan    color16M_green|color16M_blue
#define color16M_purple  color16M_red|color16M_blue


#define color65k_grayscale1    2113
#define color65k_grayscale2    2113*2
#define color65k_grayscale3    2113*3
#define color65k_grayscale4    2113*4
#define color65k_grayscale5    2113*5
#define color65k_grayscale6    2113*6
#define color65k_grayscale7    2113*7
#define color65k_grayscale8    2113*8
#define color65k_grayscale9    2113*9
#define color65k_grayscale10   2113*10
#define color65k_grayscale11   2113*11
#define color65k_grayscale12   2113*12
#define color65k_grayscale13   2113*13
#define color65k_grayscale14   2113*14
#define color65k_grayscale15   2113*15
#define color65k_grayscale16   2113*16
#define color65k_grayscale17   2113*17
#define color65k_grayscale18   2113*18
#define color65k_grayscale19   2113*19
#define color65k_grayscale20   2113*20
#define color65k_grayscale21   2113*21
#define color65k_grayscale22   2113*22
#define color65k_grayscale23   2113*23
#define color65k_grayscale24   2113*24
#define color65k_grayscale25   2113*25
#define color65k_grayscale26   2113*26
#define color65k_grayscale27   2113*27
#define color65k_grayscale28   2113*28
#define color65k_grayscale29   2113*29
#define color65k_grayscale30   2113*30


//--

#define	cSetb0		0x01
#define	cSetb1		0x02
#define	cSetb2		0x04
#define	cSetb3		0x08
#define	cSetb4		0x10
#define	cSetb5		0x20
#define	cSetb6		0x40
#define	cSetb7		0x80

#define	cClrb0		0xfe
#define	cClrb1		0xfd
#define	cClrb2		0xfb
#define	cClrb3		0xf7
#define	cClrb4		0xef
#define	cClrb5		0xdf
#define	cClrb6		0xbf
#define	cClrb7		0x7f

//--




#endif /*__UserDef_H*/
