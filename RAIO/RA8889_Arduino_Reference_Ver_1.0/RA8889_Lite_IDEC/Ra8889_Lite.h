//=========================================//
/*
File Name : Ra8889_Lite.h                                   
Author    : RAiO Application Team                             
Edit Date : 04/21/2020
Version   : v1.0

*/
//=========================================//
//========== COPYRIGHT 2020 RAiO ==========//
//=========================================//

#include "Arduino.h"
#include "Print.h"
#include "stdio.h" 

#ifndef _RA8889_LITE.H
#define _RA8889_LITE.H

typedef signed char  rs8;
typedef signed short rs16;
typedef signed long  rs32;
typedef unsigned char  ru8; 
typedef unsigned short ru16;
typedef unsigned long  ru32;

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

#define RA8889_SPI_CMDWRITE    0x00
#define RA8889_SPI_DATAWRITE   0x80
#define RA8889_SPI_DATAREAD    0xc0
#define RA8889_SPI_STATUSREAD  0x40

/*==== [SW_(1)]  PLL  =====*/
//Crystal resonator for RA8889, suggested 10MHz
//SDRAMaccess clock,suggested 50~160MHz
//RA8889 system core clock, suggested 50~130MHz
//TFTdriving clock PCLK,refer to LCD SPEC specified PCLK frequency requirements 
//DRAM_FREQ>= CORE_FREQ   
//CORE_FREQ>= 2 * SCAN_FREQ

#define OSC_FREQ	10  // OSC clock frequency, unit: MHz.
#define DRAM_FREQ	140  // SDRAM clock frequency, unit: MHz. 
#define CORE_FREQ	120  // Core (system) clock frequency, unit: MHz. 
#define SCAN_FREQ	35  // Panel Scan clock frequency, unit: MHz.


/*TFT timing configure*/
#define TFT_MODE   0  //0:SYNC_mode(SYNC+DE mode), 1: DE mode  //if sync only mode do not connect DE signal or XDE_INV = 1
#define XHSYNC_INV  0 // 0:no inversion, 1:inversion 
#define XVSYNC_INV  0 // 0:no inversion, 1:inversion 
#define XDE_INV     0 // 0:no inversion, 1:inversion 
#define XPCLK_INV  1  // 0:no inversion, 1:inversion 
#define HPW       8   //
#define HND       38
#define HDW       800
#define HST       16
#define VPW       8
#define VND       15
#define VDH       600
#define VST       12


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define COLOR_DEPTH_24BPP 
//#define COLOR_DEPTH_16BPP 



/*Page(image buffer) configure*/
/*The maximum number of pages is based on SDRAM capacity and color depth and width and height of one page*/
/*For example we used 128Mbit SDRAM that capacity =  16Mbyte */
/*The SDRAM is divided into several image buffers and the maximum number of image buffers is limited by the 
memory size. For example : page_size = 800*600*2byte(16bpp) = 960000byte, maximum number = 16/0.96 = 16.6 */
/*vertical mulit page application*/

#ifdef COLOR_DEPTH_16BPP
#define PAGE1_START_ADDR  0
#define PAGE2_START_ADDR  800*600*2
#define PAGE3_START_ADDR  800*600*2*2
#define PAGE4_START_ADDR  800*600*2*3
#define PAGE5_START_ADDR  800*600*2*4
#define PAGE6_START_ADDR  800*600*2*5
#define PAGE7_START_ADDR  800*600*2*6
#define PAGE8_START_ADDR  800*600*2*7
#define PAGE9_START_ADDR  800*600*2*8
#define PAGE10_START_ADDR  800*600*2*9


#define COLOR65K_BLACK         0x0000
#define COLOR65K_WHITE         0xffff
#define COLOR65K_RED           0xf800
#define COLOR65K_LIGHTRED      0xfc10
#define COLOR65K_DARKRED       0x8000
#define COLOR65K_GREEN         0x07e0
#define COLOR65K_LIGHTGREEN    0x87f0
#define COLOR65K_DARKGREEN     0x0400
#define COLOR65K_BLUE          0x001f
#define COLOR65K_BLUE2         0x051f
#define COLOR65K_LIGHTBLUE     0x841f
#define COLOR65K_DARKBLUE      0x0010
#define COLOR65K_YELLOW        0xffe0
#define COLOR65K_LIGHTYELLOW   0xfff0
#define COLOR65K_DARKYELLOW    0x8400
#define COLOR65K_CYAN          0x07ff
#define COLOR65K_LIGHTCYAN     0x87ff
#define COLOR65K_DARKCYAN      0x0410
#define COLOR65K_MAGENTA       0xf81f
#define COLOR65K_LIGHTMAGENTA  0xfc1f
#define COLOR65K_DARKMAGENTA   0x8010
#define COLOR65K_BROWN         0xA145


#define COLOR65K_GRAYSCALE1    2113*2
#define COLOR65K_GRAYSCALE2    2113*4
#define COLOR65K_GRAYSCALE3    2113*6
#define COLOR65K_GRAYSCALE4    2113*8
#define COLOR65K_GRAYSCALE5    2113*10
#define COLOR65K_GRAYSCALE6    2113*12
#define COLOR65K_GRAYSCALE7    2113*14
#define COLOR65K_GRAYSCALE8    2113*16
#define COLOR65K_GRAYSCALE9    2113*18
#define COLOR65K_GRAYSCALE10   2113*20
#define COLOR65K_GRAYSCALE11   2113*22
#define COLOR65K_GRAYSCALE12   2113*24
#define COLOR65K_GRAYSCALE13   2113*26
#define COLOR65K_GRAYSCALE14   2113*28


#endif


#ifdef COLOR_DEPTH_24BPP

#define PAGE1_START_ADDR  0
#define PAGE2_START_ADDR  800*600*3
#define PAGE3_START_ADDR  800*600*3*2
#define PAGE5_START_ADDR  800*600*3*4
#define PAGE4_START_ADDR  800*600*3*3
#define PAGE6_START_ADDR  800*600*3*5
#define PAGE7_START_ADDR  800*600*3*6
#define PAGE8_START_ADDR  800*600*3*7
#define PAGE9_START_ADDR  800*600*3*8
#define PAGE10_START_ADDR  800*600*3*9


#define COLOR16M_BLACK         0x000000
#define COLOR16M_WHITE         0xffffff
#define COLOR16M_RED           0xff0000
#define COLOR16M_LIGHTRED      0xff5151
#define COLOR16M_DARKRED       0xAE0000
#define COLOR16M_GREEN         0x00ff00
#define COLOR16M_LIGHTGREEN    0x53ff53
#define COLOR16M_DARKGREEN     0x007500
#define COLOR16M_BLUE          0x0000ff
#define COLOR16M_BLUE2         0x00e3e3
#define COLOR16M_LIGHTBLUE     0x4a4aff
#define COLOR16M_DARKBLUE      0x004b97
#define COLOR16M_YELLOW        0xffff00
#define COLOR16M_LIGHTYELLOW   0xffff93
#define COLOR16M_DARKYELLOW    0xe1e100
#define COLOR16M_CYAN          0x00ffff
#define COLOR16M_LIGHTCYAN     0xa6ffff
#define COLOR16M_DARKCYAN      0x009393
#define COLOR16M_MAGENTA       0xff00ff
#define COLOR16M_LIGHTMAGENTA  0xffa6ff
#define COLOR16M_DARKMAGENTA   0x930093
#define COLOR16M_BROWN         0x642100

#define COLOR16M_GRAYSCALE1    0x272727
#define COLOR16M_GRAYSCALE2    0x3c3c3c
#define COLOR16M_GRAYSCALE3    0x4f4f4f
#define COLOR16M_GRAYSCALE4    0x5b5b5b
#define COLOR16M_GRAYSCALE5    0x6c6c6c
#define COLOR16M_GRAYSCALE6    0x7b7b7b
#define COLOR16M_GRAYSCALE7    0x8e8e8e
#define COLOR16M_GRAYSCALE8    0x9d9d9d
#define COLOR16M_GRAYSCALE9    0xadadad
#define COLOR16M_GRAYSCALE10   0xbebebe
#define COLOR16M_GRAYSCALE11   0xd0d0d0
#define COLOR16M_GRAYSCALE12   0xe0e0e0
#define COLOR16M_GRAYSCALE13   0xf0f0f0
#define COLOR16M_GRAYSCALE14   0xfcfcfc

#endif



#define STRING_LINE1  0
#define STRING_LINE2  32
#define STRING_LINE3  32*2
#define STRING_LINE4  32*3
#define STRING_LINE5  32*4
#define STRING_LINE6  32*5
#define STRING_LINE7  32*6
#define STRING_LINE8  32*7
#define STRING_LINE9  32*8
#define STRING_LINE10  32*9
#define STRING_LINE11  32*10
#define STRING_LINE12  32*11
#define STRING_LINE13  32*12
#define STRING_LINE14  32*13
#define STRING_LINE15  32*14
#define STRING_LINE16  32*15
#define STRING_LINE17  32*16
#define STRING_LINE18  32*17
#define STRING_LINE19  32*18

/*RA8889 register & bit*/
/*page0 register*/
#define RA8889_SRR  0x00
#define RA8889_SOFTWARE_RESET  0xD7

#define RA8889_CCR  0x01
#define RA8889_PLL_DISABLE  0  
#define RA8889_PLL_ENABLE   1
#define RA8889_WAIT_NO_MASK  0
#define RA8889_WAIT_MASK  1  
#define RA8889_KEY_SCAN_DISABLE  0
#define RA8889_KEY_SCAN_ENABLE  1
#define RA8889_TFT_OUTPUT24  0
#define RA8889_TFT_OUTPUT18  1
#define RA8889_TFT_OUTPUT16  3
#define RA8889_I2C_MASTER_DISABLE  0
#define RA8889_I2C_MASTER_ENABLE  1
#define RA8889_SERIAL_IF_DISABLE  0
#define RA8889_SERIAL_IF_ENABLE  1
#define RA8889_HOST_DATA_BUS_8BIT  0
#define RA8889_HOST_DATA_BUS_SERIAL  0
#define RA8889_HOST_DATA_BUS_16BIT  1

#define RA8889_MACR  0x02
#define RA8889_DIRECT_WRITE  0  // (for all 8 bits MPU I/F, 16 bits MPU I/F with 8bpp data mode 1 & 2, 16 bits MPU I/F with 16/24-bpp data mode 1 & serial host interface)
#define RA8889_MASK_HIGH_BYTE  2  // (ex. 16 bit MPU I/F with 8-bpp data mode 1)
#define RA8889_MASK_HIGH_BYTE_EVEN_DATA 3  //(ex. 16 bit MPU I/F with 24-bpp data mode 2)
#define RA8889_READ_MEMORY_LRTB  0
#define RA8889_READ_MEMORY_RLTB  1
#define RA8889_READ_MEMORY_TBLR  2
#define RA8889_READ_MEMORY_BTLR  3
#define RA8889_WRITE_MEMORY_LRTB  0
#define RA8889_WRITE_MEMORY_RLTB  1
#define RA8889_WRITE_MEMORY_TBLR  2
#define RA8889_WRITE_MEMORY_BTLR  3

#define RA8889_ICR  0x03
#define RA8889_OUTPUT_INT_LEVEL_LOW  0
#define RA8889_OUTPUT_INT_LEVEL_HIGH  1
#define RA8889_EXT_INT_WITHOUT_DEBOUNCE  0
#define RA8889_EXT_INT_WITH_DEBOUNCE  1
#define RA8889_EXT_INT_LOW_LEVEL_TRIGGER  0
#define RA8889_EXT_INT_FALLING_EDGE_TRIGGER  1
#define RA8889_EXT_INT_HIGH_LEVEL_TRIGGER  2
#define RA8889_EXT_INT_RISING_EDGE_TRIGGER  3
#define RA8889_GRAPHIC_MODE  0
#define RA8889_TEXT_MODE  1
#define RA8889_MEMORY_SELECT_IMAGE  0
#define RA8889_MEMORY_SELECT_GAMMA  1
#define RA8889_MEMORY_SELECT_CURSOR_RAM  2
#define RA8889_MEMORY_SELECT_PALETTE_RAM  3

#define RA8889_MRWDP  0x04

#define RA8889_PPLLC1  0x05
#define RA8889_PPLLC2  0x06
#define RA8889_MPLLC1  0x07
#define RA8889_MPLLC2  0x08
#define RA8889_SPLLC1  0x09
#define RA8889_SPLLC2  0x0A

#define RA8889_INTEN  0x0B
#define RA8889_INTF   0x0C
#define RA8889_MINTFR 0x0D
#define RA8889_PUENR  0x0E
#define RA8889_PSFSR  0x0F

#define RA8889_MPWCTR  0x10
#define RA8889_PIP1_WINDOW_DISABLE 0
#define RA8889_PIP1_WINDOW_ENABLE  1
#define RA8889_PIP2_WINDOW_DISABLE 0
#define RA8889_PIP2_WINDOW_ENABLE  1
#define RA8889_SELECT_CONFIG_PIP1  0
#define RA8889_SELECT_CONFIG_PIP2  1
#define RA8889_IMAGE_COLOCR_DEPTH_8BPP  0
#define RA8889_IMAGE_COLOCR_DEPTH_16BPP 1
#define RA8889_IMAGE_COLOCR_DEPTH_24BPP 2
#define RA8889_PANEL_SYNC_MODE 0
#define RA8889_PANEL_DE_MODE 1

#define RA8889_PIPCDEP  0x11
#define RA8889_PIP1_COLOR_DEPTH_8BPP  0
#define RA8889_PIP1_COLOR_DEPTH_16BPP 1
#define RA8889_PIP1_COLOR_DEPTH_24BPP 2
#define RA8889_PIP2_COLOR_DEPTH_8BPP  0
#define RA8889_PIP2_COLOR_DEPTH_16BPP  1
#define RA8889_PIP2_COLOR_DEPTH_24BPP  2

#define RA8889_DPCR  0x12
#define RA8889_PCLK_NO_INVERSION  0
#define RA8889_PCLK_INVERSION  1
#define RA8889_DISPLAY_OFF  0
#define RA8889_DISPLAY_ON  1
#define RA8889_COLOR_BAR_DISABLE  0
#define RA8889_COLOR_BAR_ENABLE  1
#define RA8889_HDIR_LR  0
#define RA8889_HDIR_RL  1
#define RA8889_VDIR_TB  0
#define RA8889_VDIR_BT  1
#define RA8889_OUTPUT_RGB  0
#define RA8889_OUTPUT_RBG  1
#define RA8889_OUTPUT_GRB  2
#define RA8889_OUTPUT_GBR  3
#define RA8889_OUTPUT_BRG  4
#define RA8889_OUTPUT_BGR  5
#define RA8889_OUTPUT_GRAY  6
#define RA8889_OUTPUT_IDLE  7

#define RA8889_PCSR  0x13
#define RA8889_XHSYNC_LOW_ACTIVE  0
#define RA8889_XHSYNC_HIGH_ACTIVE  1
#define RA8889_XVSYNC_LOW_ACTIVE  0
#define RA8889_XVSYNC_HIGH_ACTIVE  1
#define RA8889_XDE_HIGH_ACTIVE  0
#define RA8889_XDE_LOW_ACTIVE  1
#define RA8889_XDE_IDLE_LOW  0
#define RA8889_XDE_IDLE_HIGH  1
#define RA8889_XPCLK_IDLE_LOW  0
#define RA8889_XPCLK_IDLE_HIGH  1
#define RA8889_XPDAT_IDLE_LOW  0
#define RA8889_XPDAT_IDLE_HIGH  1
#define RA8889_XHSYNC_IDLE_LOW  0
#define RA8889_XHSYNC_IDLE_HIGH  1
#define RA8889_XVSYNC_IDLE_LOW  0
#define RA8889_XVSYNC_IDLE_HIGH  1

#define RA8889_HDWR    0x14
#define RA8889_HDWFTR  0x15
#define RA8889_HNDR    0x16
#define RA8889_HNDFTR  0x17
#define RA8889_HSTR    0x18
#define RA8889_HPWR    0x19

#define RA8889_VDHR0  0x1A
#define RA8889_VDHR1  0x1B
#define RA8889_VNDR0  0x1C
#define RA8889_VNDR1  0x1D
#define RA8889_VSTR   0x1E
#define RA8889_VPWR   0x1F

#define RA8889_MISA0   0x20
#define RA8889_MISA1   0x21
#define RA8889_MISA2   0x22
#define RA8889_MISA3   0x23

#define RA8889_MIW0    0x24
#define RA8889_MIW1    0x25
#define RA8889_MWULX0  0x26
#define RA8889_MWULX1  0x27
#define RA8889_MWULY0  0x28
#define RA8889_MWULY1  0x29

#define RA8889_PWDULX0  0x2A
#define RA8889_PWDULX1  0x2B
#define RA8889_PWDULY0  0x2C
#define RA8889_PWDULY1  0x2D
#define RA8889_PISA0    0x2E
#define RA8889_PISA1    0x2F
#define RA8889_PISA2    0x30
#define RA8889_PISA3    0x31
#define RA8889_PIW0     0x32
#define RA8889_PIW1     0x33
#define RA8889_PWIULX0  0x34
#define RA8889_PWIULX1  0x35
#define RA8889_PWIULY0  0x36
#define RA8889_PWIULY1  0x37
#define RA8889_PWW0     0x38
#define RA8889_PWW1     0x39
#define RA8889_PWH0     0x3A
#define RA8889_PWH1     0x3B

#define RA8889_GTCCR    0x3C
#define RA8889_BTCR     0x3D
#define RA8889_CURHS    0x3E
#define RA8889_CURVS    0x3F

#define RA8889_GCHP0    0x40
#define RA8889_GCHP1    0x41
#define RA8889_GCVP0    0x42
#define RA8889_GCVP1    0x43
#define RA8889_GCC0     0x44
#define RA8889_GCC1     0x45

#define RA8889_CMC   0x46
#define RA8889_REGISTER_PAGE0  0
#define RA8889_REGISTER_PAGE1  1

#define RA8889_CVSSA0   0x50
#define RA8889_CVSSA1   0x51
#define RA8889_CVSSA2   0x52
#define RA8889_CVSSA3   0x53
#define RA8889_CVS_IMWTH0  0x54
#define RA8889_CVS_IMWTH1  0x55

#define RA8889_AWUL_X0  0x56
#define RA8889_AWUL_X1  0x57
#define RA8889_AWUL_Y0  0x58
#define RA8889_AWUL_Y1  0x59
#define RA8889_AW_WTH0  0x5A
#define RA8889_AW_WTH1  0x5B
#define RA8889_AW_HT0   0x5C
#define RA8889_AW_HT1   0x5D

#define RA8889_AW_COLOR  0x5E
#define RA8889_CANVAS_BLOCK_MODE  0
#define RA8889_CANVAS_LINEAR_MODE  1
#define RA8889_CANVAS_COLOR_DEPTH_8BPP  0
#define RA8889_CANVAS_COLOR_DEPTH_16BPP  1
#define RA8889_CANVAS_COLOR_DEPTH_24BPP  2

#define RA8889_CURH0  0x5F
#define RA8889_CURH1  0x60
#define RA8889_CURV0  0x61
#define RA8889_CURV1  0x62

#define RA8889_F_CURX0  0x63
#define RA8889_F_CURX1  0x64
#define RA8889_F_CURY0  0x65
#define RA8889_F_CURY1  0x66

#define RA8889_DCR0  0x67
#define RA8889_DRAW_LINE  0x80
#define RA8889_DRAW_TRIANGLE  0x82 
#define RA8889_DRAW_TRIANGLE_FILL  0xA2 

#define RA8889_DLHSR0  0x68
#define RA8889_DLHSR1  0x69
#define RA8889_DLVSR0  0x6A
#define RA8889_DLVSR1  0x6B
#define RA8889_DLHER0  0x6C
#define RA8889_DLHER1  0x6D
#define RA8889_DLVER0  0x6E
#define RA8889_DLVER1  0x6F
#define RA8889_DTPH0   0x70
#define RA8889_DTPH1   0x71
#define RA8889_DTPV0   0x72
#define RA8889_DTPV1   0x73

#define RA8889_DCR1   0x76
#define RA8889_DRAW_CIRCLE   0x80
#define RA8889_DRAW_CIRCLE_FILL  0xC0
#define RA8889_DRAW_ELLIPSE 0x80
#define RA8889_DRAW_ELLIPSE_FILL  0xC0
#define RA8889_DRAW_BOTTOM_LEFT_CURVE  0x90
#define RA8889_DRAW_BOTTOM_LEFT_CURVE_FILL  0xD0
#define RA8889_DRAW_UPPER_LEFT_CURVE  0x91
#define RA8889_DRAW_UPPER_LEFT_CURVE_FILL  0xD1
#define RA8889_DRAW_UPPER_RIGHT_CURVE  0x92
#define RA8889_DRAW_UPPER_RIGHT_CURVE_FILL  0xD2
#define RA8889_DRAW_BOTTOM_RIGHT_CURVE  0x93
#define RA8889_DRAW_BOTTOM_RIGHT_CURVE_FILL  0xD3
#define RA8889_DRAW_SQUARE  0xA0
#define RA8889_DRAW_SQUARE_FILL  0xE0
#define RA8889_DRAW_CIRCLE_SQUARE  0xB0
#define RA8889_DRAW_CIRCLE_SQUARE_FILL  0xF0

#define RA8889_ELL_A0  0x77
#define RA8889_ELL_A1  0x78
#define RA8889_ELL_B0  0x79
#define RA8889_ELL_B1  0x7A
#define RA8889_DEHR0   0x7B
#define RA8889_DEHR1   0x7C
#define RA8889_DEVR0   0x7D
#define RA8889_DEVR1   0x7E

#define RA8889_FGCR   0xD2
#define RA8889_FGCG   0xD3
#define RA8889_FGCB   0xD4

#define RA8889_PSCLR   0x84
#define RA8889_PRESCALER  0x03
#define RA8889_PMUXR  0x85
#define RA8889_PWM_TIMER_DIV1   0
#define RA8889_PWM_TIMER_DIV2   1
#define RA8889_PWM_TIMER_DIV4   2
#define RA8889_PWM_TIMER_DIV8   3
#define RA8889_XPWM1_OUTPUT_ERROR_FLAG   0
#define RA8889_XPWM1_OUTPUT_PWM_TIMER1   2
#define RA8889_XPWM1_OUTPUT_OSC_CLK   3
#define RA8889_XPWM0_GPIO_C7   0
#define RA8889_XPWM0_OUTPUT_PWM_TIMER0   2
#define RA8889_XPWM0_OUTPUT_CORE_CLK   3

#define RA8889_PCFGR   0x86
#define RA8889_PWM_TIMER1_INVERTER_OFF  0
#define RA8889_PWM_TIMER1_INVERTER_ON   1
#define RA8889_PWM_TIMER1_ONE_SHOT   0
#define RA8889_PWM_TIMER1_AUTO_RELOAD   1
#define RA8889_PWM_TIMER1_STOP  0
#define RA8889_PWM_TIMER1_START  1
#define RA8889_PWM_TIMER0_DEAD_ZONE_DISABLE   0
#define RA8889_PWM_TIMER0_DEAD_ZONE_ENABLE   1
#define RA8889_PWM_TIMER0_INVERTER_OFF   0
#define RA8889_PWM_TIMER0_INVERTER_ON   1
#define RA8889_PWM_TIMER0_ONE_SHOT   0
#define RA8889_PWM_TIMER0_AUTO_RELOAD   1
#define RA8889_PWM_TIMER0_STOP  0
#define RA8889_PWM_TIMER0_START  1

#define RA8889_DZ_LENGTH   0x87
#define RA8889_TCMPB0L     0x88
#define RA8889_TCMPB0H     0x89
#define RA8889_TCNTB0L     0x8A
#define RA8889_TCNTB0H     0x8B
#define RA8889_TCMPB1L     0x8C
#define RA8889_TCMPB1H     0x8D
#define RA8889_TCNTB1L     0x8E
#define RA8889_TCNTB1F     0x8F

#define RA8889_BTE_CTRL0   0x90
#define RA8889_BTE_ENABLE   1
#define RA8889_PATTERN_FORMAT8X8   0
#define RA8889_PATTERN_FORMAT16X16   1

#define RA8889_BTE_CTRL1  0x91
#define RA8889_BTE_ROP_CODE_0     0   //0 ( Blackness ) 
#define RA8889_BTE_ROP_CODE_1     1   //~S0・~S1 or ~ ( S0+S1 ) 
#define RA8889_BTE_ROP_CODE_2     2   //~S0・S1
#define RA8889_BTE_ROP_CODE_3     3   //~S0
#define RA8889_BTE_ROP_CODE_4     4   //S0・~S1
#define RA8889_BTE_ROP_CODE_5     5   //~S1
#define RA8889_BTE_ROP_CODE_6     6   //S0^S1
#define RA8889_BTE_ROP_CODE_7     7   //~S0+~S1 or ~ ( S0・S1 ) 
#define RA8889_BTE_ROP_CODE_8     8   //S0・S1
#define RA8889_BTE_ROP_CODE_9     9   //~ ( S0^S1 ) 
#define RA8889_BTE_ROP_CODE_10    10  //S1
#define RA8889_BTE_ROP_CODE_11    11  //~S0+S1
#define RA8889_BTE_ROP_CODE_12    12  //S0
#define RA8889_BTE_ROP_CODE_13    13  //S0+~S1
#define RA8889_BTE_ROP_CODE_14    14  //S0+S1
#define RA8889_BTE_ROP_CODE_15    15  //1 ( Whiteness ) 
#define RA8889_BTE_ROP_BUS_WIDTH8  7
#define RA8889_BTE_ROP_BUS_WIDTH16  15

#define RA8889_BTE_MPU_WRITE_WITH_ROP    0  
#define RA8889_BTE_MEMORY_COPY_WITH_ROP   2
#define RA8889_BTE_MPU_WRITE_WITH_CHROMA   4
#define RA8889_BTE_MEMORY_COPY_WITH_CHROMA  5
#define RA8889_BTE_PATTERN_FILL_WITH_ROP  6
#define RA8889_BTE_PATTERN_FILL_WITH_CHROMA  7
#define RA8889_BTE_MPU_WRITE_COLOR_EXPANSION   8
#define RA8889_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA   9
#define RA8889_BTE_MEMORY_COPY_WITH_OPACITY  10
#define RA8889_BTE_MPU_WRITE_WITH_OPACITY  11
#define RA8889_BTE_SOLID_FILL  12
#define RA8889_BTE_MEMORY_COPY_WITH_COLOR_EXPANSION  14
#define RA8889_BTE_MEMORY_COPY_WITH_COLOR_EXPANSION_CHROMA  15

#define RA8889_BTE_COLR  0x92
#define RA8889_S0_COLOR_DEPTH_8BPP  0
#define RA8889_S0_COLOR_DEPTH_16BPP  1
#define RA8889_S0_COLOR_DEPTH_24BPP   2
#define RA8889_S1_COLOR_DEPTH_8BPP  0
#define RA8889_S1_COLOR_DEPTH_16BPP  1
#define RA8889_S1_COLOR_DEPTH_24BPP  2
#define RA8889_S1_CONSTANT_COLOR   3
#define RA8889_S1_8BIT_PIXEL_ALPHA_BLENDING  4
#define RA8889_S1_16BIT_PIXEL_ALPHA_BLENDING  5
#define RA8889_S1_32BIT_ARGB_ALPHA_BLENDING  6

#define RA8889_DESTINATION_COLOR_DEPTH_8BPP  0
#define RA8889_DESTINATION_COLOR_DEPTH_16BPP  1
#define RA8889_DESTINATION_COLOR_DEPTH_24BPP  2

#define RA8889_S0_STR0   0x93
#define RA8889_S0_STR1   0x94
#define RA8889_S0_STR2   0x95
#define RA8889_S0_STR3   0x96
#define RA8889_S0_WTH0   0x97
#define RA8889_S0_WTH1   0x98
#define RA8889_S0_X0     0x99
#define RA8889_S0_X1     0x9A
#define RA8889_S0_Y0     0x9B
#define RA8889_S0_Y1     0x9C
#define RA8889_S1_STR0   0x9D
#define RA8889_S1_STR1   0x9E
#define RA8889_S1_STR2   0x9F
#define RA8889_S1_RED    0x9D
#define RA8889_S1_GREEN  0x9E
#define RA8889_S1_BLUE   0x9F
#define RA8889_S1_STR3   0xA0
#define RA8889_S1_WTH0   0xA1
#define RA8889_S1_WTH1   0xA2

#define RA8889_S1_X0   0xA3
#define RA8889_S1_X1   0xA4
#define RA8889_S1_Y0   0xA5
#define RA8889_S1_Y1   0xA6
#define RA8889_DT_STR0  0xA7
#define RA8889_DT_STR1   0xA8
#define RA8889_DT_STR2   0xA9
#define RA8889_DT_STR3   0xAA
#define RA8889_DT_WTH0   0xAB
#define RA8889_DT_WTH1   0xAC
#define RA8889_DT_X0     0xAD
#define RA8889_DT_X1     0xAE
#define RA8889_DT_Y0     0xAF
#define RA8889_DT_Y1     0xB0
#define RA8889_BTE_WTH0  0xB1
#define RA8889_BTE_WTH1  0xB2
#define RA8889_BTE_HIG0  0xB3
#define RA8889_BTE_HIG1  0xB4
#define RA8889_APB_CTRL  0xB5

#define RA8889_DMA_CTRL  0xB6
#define RA8889_B7H_B3_0 0
#define RA8889_SPI_QUAD_MODE_6B 1 //COMMAND 6Bh
#define RA8889_SPI_QUAD MODE EB 2 //COMMAND EBh
#define RA8889_DMA_START  1

#define RA8889_SFL_CTRL  0xB7
#define RA8889_SERIAL_FLASH_SELECT0    0
#define RA8889_SERIAL_FLASH_SELECT1    1
#define RA8889_SERIAL_FLASH_SELECT2    0 //when page1 B7h bit 7 = 1
#define RA8889_SERIAL_FLASH_SELECT3    1 //when page1 B7h bit 7 = 1
#define RA8889_SERIAL_FLASH_FONT_MODE  0
#define RA8889_SERIAL_FLASH_DMA_MODE   1
#define RA8889_SERIAL_FLASH_ADDR_24BIT  0
#define RA8889_SERIAL_FLASH_ADDR_32BIT  1
#define RA8889_STANDARD_SPI_MODE  0
#define RA8889_FOLLOW_RA8875_MODE  1
#define RA8889_SPI_NORMAL_READ   0   //COMMAND 03h
#define RA8889_SPI_FAST_READ_8DUMMY   4   //COMMAND 0Bh
#define RA8889_SPI_FAST_READ_16DUMMY  8   //COMMAND 1Bh
#define RA8889_SPI_DUAL_READ_8DUMMY   2   //COMMAND 3Bh
#define RA8889_2XIO_READ_MODE_4DUMMY  3   //COMMAND BBh

#define RA8889_SPIDR  0xB8
#define RA8889_SPIMCR2   0xB9

#define RA8889_SPIM_NSS_SELECT_2   1  //B7 and B5 = 10b: nSS drive on xnsfcs[2]
#define RA8889_SPIM_NSS_SELECT_3   1  //B7 and B5 = 11b: nSS drive on xnsfcs[3]

#define RA8889_SPIM_INT_DISABLE   0
#define RA8889_SPIM_INT_ENABLE    1
#define RA8889_SPIM_NSS_SELECT_0  0
#define RA8889_SPIM_NSS_SELECT_1  1

#define RA8889_SPIM_NSS_INACTIVE  0  //nSS port will goes high
#define RA8889_SPIM_NSS_ACTIVE    1  //nSS port will goes low
#define RA8889_SPIM_OVFIRQMSK_UNMASK  0
#define RA8889_SPIM_OVFIRQMSK_MASK    1 
#define RA8889_SPIM_EMTIRQMSK_UNMASK  0
#define RA8889_SPIM_EMTIRQMSK_MASK    1
#define RA8889_SPIM_MODE0   0
#define RA8889_SPIM_MODE1   1
#define RA8889_SPIM_MODE2   2
#define RA8889_SPIM_MODE3   3

#define RA8889_SPIMSR  0xBA

#define RA8889_SPI_DIVSOR 0xBB
#define RA8889_SPI_DIV2   0
#define RA8889_SPI_DIV4   1
#define RA8889_SPI_DIV6   2
#define RA8889_SPI_DIV8   3
#define RA8889_SPI_DIV10  4

#define RA8889_DMA_SSTR0  0xBC
#define RA8889_DMA_SSTR1  0xBD
#define RA8889_DMA_SSTR2  0xBE
#define RA8889_DMA_SSTR3  0xBF

#define RA8889_DMA_DX0  0xC0
#define RA8889_DMA_DX1  0xC1
#define RA8889_DMA_DY0  0xC2
#define RA8889_DMA_DY1  0xC3

#define RA8889_SPIMBS   0xC5
#define SPIM_SELECT_BUS0 0  
#define SPIM_SELECT_BUS1 1 
#define SPIM_RX_LATCH_RISING_EDGE 0
#define SPIM_RX_LATCH_FALLING_EDGE 1 //suggested

#define RA8889_DMAW_WTH0   0xC6
#define RA8889_DMAW_WTH1   0xC7
#define RA8889_DMAW_HIGH0  0xC8
#define RA8889_DMAW_HIGH1  0xC9
#define RA8889_DMA_SWTH0   0xCA
#define RA8889_DMA_SWTH1   0xCB

#define RA8889_CCR0   0xCC
#define RA8889_SELECT_INTERNAL_CGROM  0
#define RA8889_SELECT_EXTERNAL_CGROM  1  //Genitop serial flash
#define RA8889_SELECT_USER_DEFINED  2
#define RA8889_CHAR_HEIGHT_16  0
#define RA8889_CHAR_HEIGHT_24  1
#define RA8889_CHAR_HEIGHT_32  2
#define RA8889_SELECT_8859_1   0
#define RA8889_SELECT_8859_2   1
#define RA8889_SELECT_8859_4   2
#define RA8889_SELECT_8859_5   3

#define RA8889_CCR1   0xCD
#define RA8889_TEXT_FULL_ALIGN_DISABLE   0
#define RA8889_TEXT_FULL_ALIGN_ENABLE    1
#define RA8889_TEXT_CHROMA_KEY_DISABLE   0
#define RA8889_TEXT_CHROMA_KEY_ENABLE    1
#define RA8889_TEXT_NO_ROTATION  0
#define RA8889_TEXT_ROTATION     1  
#define RA8889_TEXT_WIDTH_ENLARGEMENT_X1  0
#define RA8889_TEXT_WIDTH_ENLARGEMENT_X2  1
#define RA8889_TEXT_WIDTH_ENLARGEMENT_X3  2
#define RA8889_TEXT_WIDTH_ENLARGEMENT_X4  3
#define RA8889_TEXT_HEIGHT_ENLARGEMENT_X1  0
#define RA8889_TEXT_HEIGHT_ENLARGEMENT_X2  1
#define RA8889_TEXT_HEIGHT_ENLARGEMENT_X3  2
#define RA8889_TEXT_HEIGHT_ENLARGEMENT_X4  3

#define RA8889_GTFNT_SEL   0xCE
#define RA8889_GT21L16T1W   0
#define RA8889_GT30L16U2W   1
#define RA8889_GT30L24T3Y   2
#define RA8889_GT30L24M1Z   3
#define RA8889_GT30L32S4W   4
#define RA8889_GT20L24F6Y   5
#define RA8889_GT21L24S1W   6

#define RA8889_GTFNT_CR   0xCF
#define RA8889_GB2312             0
#define RA8889_GB12345_GB18030    1
#define RA8889_BIG5               2
#define RA8889_ASCII              4
#define RA8889_UNICODE            3
#define RA8889_UNI_JAPANESE       5
#define RA8889_JIS0208            6
#define RA8889_LATIN_GREEK_CYRILLIC_ARABIC_THAI_HEBREW   7
#define RA8889_ISO_8859_1_AND_ASCII   17
#define RA8889_ISO_8859_2_AND_ASCII   18
#define RA8889_ISO_8859_3_AND_ASCII   19
#define RA8889_ISO_8859_4_AND_ASCII   20
#define RA8889_ISO_8859_5_AND_ASCII   21
#define RA8889_ISO_8859_7_AND_ASCII   22
#define RA8889_ISO_8859_8_AND_ASCII   23
#define RA8889_ISO_8859_9_AND_ASCII   24
#define RA8889_ISO_8859_10_AND_ASCII  25
#define RA8889_ISO_8859_11_AND_ASCII  26
#define RA8889_ISO_8859_13_AND_ASCII  27
#define RA8889_ISO_8859_14_AND_ASCII  28
#define RA8889_ISO_8859_15_AND_ASCII  29
#define RA8889_ISO_8859_16_AND_ASCII  30
#define RA8889_GT_FIXED_WIDTH  0
#define RA8889_GT_VARIABLE_WIDTH_ARIAL  1
#define RA8889_GT_VARIABLE_FIXED_WIDTH_ROMAN  2
#define RA8889_GT_BOLD  3

#define RA8889_FLDR     0xD0
#define RA8889_F2FSSR   0xD1
#define RA8889_FGCR     0xD2
#define RA8889_FGCG     0xD3
#define RA8889_FGCB     0xD4
#define RA8889_BGCR     0xD5
#define RA8889_BGCG     0xD6
#define RA8889_BGCB     0xD7

#define RA8889_CGRAM_STR0   0xDB
#define RA8889_CGRAM_STR1   0xDC
#define RA8889_CGRAM_STR2   0xDD
#define RA8889_CGRAM_STR3   0xDE

#define RA8889_PMU   0xDF
#define RA8889_POWER_NORMAL_STATE  0x00
#define RA8889_WAKE_UP       0x00
#define RA8889_STANDBY_MODE  0x81
#define RA8889_SUSPEND_MODE  0x82
#define RA8889_SLEEP_MODE    0x83

#define RA8889_SDRAR   0xE0
#define RA8889_SDRAM_POWER_DOWN   0
#define RA8889_SDRAM_SELF_REFRESH   1
#define RA8889_SDRAM_SDR   0
#define RA8889_SDRAM_MOBILE   1
#define RA8889_SDRAM_2BANKS   0
#define RA8889_SDRAM_4BANKS   1
#define RA8889_SDRAM_ROW_ADDR_2K  0 
#define RA8889_SDRAM_ROW_ADDR_4K  1
#define RA8889_SDRAM_ROW_ADDR_8K  2
#define RA8889_SDRAM_COLUMN_256  0
#define RA8889_SDRAM_COLUMN_512  1
#define RA8889_SDRAM_COLUMN_1024  2
#define RA8889_SDRAM_COLUMN_2048  3
#define RA8889_SDRAM_COLUMN_4096  4

#define RA8889_SDRMD   0xE1
#define RA8889_SDRAM_FULL_ARRAY   0
#define RA8889_SDRAM_HALF_ARRAY   1
#define RA8889_SDRAM_QUARTER_ARRAY   2
#define RA8889_SDRAM_ONE_EIGHTH_ARRAY   5
#define RA8889_SDRAM_ONE_SIXTEENTH_ARRAY   6
#define RA8889_SDRAM_FULL_STRENGTH_DRIVER   0
#define RA8889_SDRAM_HALF_STRENGTH_DRIVER   1
#define RA8889_SDRAM_QUARTER_STRENGTH_DRIVER   2
#define RA8889_SDRAM_ONE_EIGHT_STRENGTH_DRIVER  3
#define RA8889_SDRAM_CAS_LATENCY_2   2
#define RA8889_SDRAM_CAS_LATENCY_3   3

#define RA8889_SDR_REF_ITVL0   0xE2
#define RA8889_SDR_REF_ITVL1    0xE3

#define RA8889_SDRCR   0xE4
#define RA8889_SDRAM_BREAK_LEN_256   0
#define RA8889_SDRAM_BREAK_LEN_128   1
#define RA8889_SDRAM_BREAK_LEN_64    2
#define RA8889_SDRAM_BREAK_LEN_32    3
#define RA8889_SDRAM_BUS_WIDTH_16    0
#define RA8889_SDRAM_BUS_WIDTH_32    1
#define RA8889_SDRAM_XMCKE_DISABLE   0
#define RA8889_SDRAM_XMCKE_ENABLE    1
#define RA8889_SDRAM_DISABLE_WARNING  0
#define RA8889_SDRAM_ENABLE_WARNING   1
#define RA8889_SDRAM_TIMING_PARA_DISABLE   0
#define RA8889_SDRAM_TIMING_PARA_ENABLE   1
#define RA8889_SDRAM_ENTER_POWER_SAVING   1 //0 to 1 transition will enter power saving mode
#define RA8889_SDRAM_EXIT_POWER_SAVING   0  //1 to 0 transition will exit power saving mode
#define RA8889_SDRAM_INITIALIZE   1  // An 0 to 1 transition will execute SDRAM initialization procedure.
#define RA8889_SDRAM_PARAMETER1   0xE0
#define RA8889_SDRAM_PARAMETER2   0xE1
#define RA8889_SDRAM_PARAMETER3   0xE2
#define RA8889_SDRAM_PARAMETER4   0xE3

#define RA8889_I2CMCPR0   0xE5
#define RA8889_I2CMCPR1   0xE6
#define RA8889_I2CMTXR   0xE7
#define RA8889_I2CMRXR   0xE8
#define RA8889_I2CMCMDR   0xE9
#define RA8889_I2CMSTUR   0xEA

#define RA8889_GPIOAD   0xF0
#define RA8889_GPIOA    0xF1
#define RA8889_GPIOB    0xF2
#define RA8889_GPIOCD   0xF3
#define RA8889_GPIOC    0xF4
#define RA8889_GPIODD   0xF5
#define RA8889_GPIOD    0xF6
#define RA8889_GPIOED   0xF7
#define RA8889_GPIOE    0xF8
#define RA8889_GPIOFD   0xF9
#define RA8889_GPIOF    0xFA

#define RA8889_KSCR1    0xFB
#define RA8889_KSCR2    0xFC
#define RA8889_KSDR0    0xFD
#define RA8889_KSDR1    0xFE
#define RA8889_KSDR2    0xFF

//=============================
/*page1 register*/
#define RA8889_INTEN     0x0B
#define RA8889_IDEC_INT_DISBLAY 0 
#define RA8889_IDEC_INT_ENABLE 1 

#define RA8889_INTF      0x0C
#define RA8889_MINTFR    0x0D

#define RA8889_AVI_SPIP_SADR0   0x2E
#define RA8889_AVI_SPIP_SADR1   0x2F
#define RA8889_AVI_SPIP_SADR2   0x30
#define RA8889_AVI_SPIP_SADR3   0x31

#define RA8889_VIDEO_CONTROL   0xA0
#define RA8889_MIHH    0xA1
#define RA8889_MIHL    0xA2
#define RA8889_MIWH    0xA3
#define RA8889_MIWL    0xA4
#define RA8889_VFPB3    0xA5
#define RA8889_VFPB2    0xA6
#define RA8889_VFPB1    0xA7
#define RA8889_VFPB0    0xA8

#define RA8889_VC   0xA9
#define RA8889_IDEC_RESET  0

#define RA8889_IDEC_CTRL  0xB6
#define RA8889_IDEC_SFI_SELECT_0   0
#define RA8889_IDEC_SFI_SELECT_1   1
#define RA8889_IDEC_SFI_SELECT_2   2
#define RA8889_IDEC_SFI_SELECT_3   3
#define RA8889_FONT_DMA_SELECT_BUS0  0
#define RA8889_FONT_DMA_SELECT_BUS1  1
#define RA8889_IDEC_SELECT_BUS0  0
#define RA8889_IDEC_SELECT_BUS1  1
#define RA8889_IDEC_COLOR_DEPTH_8BPP 0
#define RA8889_IDEC_COLOR_DEPTH_16BPP 1
#define RA8889_IDEC_COLOR_DEPTH_24BPP 2
#define RA8889_IDEC_START 1

#define RA8889_IDEC_CTRL   0xB7
#define RA8889_IDEC_FONT_DMA_SELECT_SFI2  0
#define RA8889_IDEC_FONT_DMA_SELECT_SFI3  1
#define RA8889_IDEC_SFI_24BIT  0
#define RA8889_IDEC_SFI_32BIT  1
#define RA8889_IDEC_STANDARD_SPI_MODE  0
#define RA8889_IDEC_FOLLOW_RA8875_MODE  1
#define RA8889_IDEC_SPI_QUAD_MODE_6BH   10   
#define RA8889_IDEC_SPI_QUAD_MODE_EBH  12  

#define RA8889_IDEC_CLOCK_DIVIDE   0xBB

#define RA8889_IDEC_SADR0  0xBC
#define RA8889_IDEC_SADR1  0xBD
#define RA8889_IDEC_SADR2  0xBE
#define RA8889_IDEC_SADR3  0xBF

#define RA8889_IDEC_DX0  0xC0
#define RA8889_IDEC_DX1  0xC1
#define RA8889_IDEC_DY0  0xC2
#define RA8889_IDEC_DY1  0xC3

#define RA8889_IDEC_PIP  0xC5
#define RA8889_AVI_DIS_BUFFER_USE_PIP1_AND_SHADOW 0
#define RA8889_AVI_DIS_BUFFER_USE_PIP2_AND_SHADOW 1

#define RA8889_IDEC_TF0  0xC6
#define RA8889_IDEC_TF1  0xC7
#define RA8889_IDEC_TF2  0xC8
#define RA8889_IDEC_TF3  0xC9

#define RA8889_IDEC_DADR0  0xCA
#define RA8889_IDEC_DADR1  0xCB
#define RA8889_IDEC_DADR2  0xCC
#define RA8889_IDEC_DADR3  0xCD

#define RA8889_IDEC_DWTH0  0xCE
#define RA8889_IDEC_DWTH1  0xCF

#define RA8889_AVI_PAUSE   0xD3
#define RA8889_AVI_PAUSE_ENTER_EXIT 0

#define RA8889_AVI_STOP  0xD4
#define RA8889_AVI_STOP_ENABLE 1



class Ra8889_Lite
{
 private:
 int _xnscs, _xnreset;
 public:
 Ra8889_Lite(int xnscs, int xnreset);
 boolean begin(void);
  
 boolean ra8889Initialize(void); 
 boolean ra8889PllInitial(ru8 dram_clk,ru8 core_clk,ru8 scan_clk); 
 boolean ra8889SdramInitial(void);

 /*access*/
 void lcdRegWrite(ru8 reg);
 void lcdDataWrite(ru8 data);
 ru8 lcdDataRead(void);
 ru8 lcdStatusRead(void);
 void lcdRegDataWrite(ru8 reg,ru8 data);
 ru8 lcdRegDataRead(ru8 reg);
 void lcdDataWrite16bbp(ru16 data); 
 /*Staus*/
 void checkWriteFifoNotFull(void);
 void checkWriteFifoEmpty(void);
 void checkReadFifoNotFull(void);
 void checkReadFifoFull(void);
 void checkReadFifoNotEmpty(void);
 void check2dBusy(void);
 boolean checkSdramReady(void);
 ru8 powerSavingStatus(void);
 boolean checkIcReady(void);//
 
 void displayOn(boolean on);
 void lcdHorizontalWidthVerticalHeight(ru16 width,ru16 height);
 void lcdHorizontalNonDisplay(ru16 numbers);
 void lcdHsyncStartPosition(ru16 numbers);
 void lcdHsyncPulseWidth(ru16 numbers);
 void lcdVerticalNonDisplay(ru16 numbers);
 void lcdVsyncStartPosition(ru16 numbers);
 void lcdVsyncPulseWidth(ru16 numbers);
 void displayImageStartAddress(ru32 addr);
 void displayImageWidth(ru16 width);
 void displayWindowStartXY(ru16 x0,ru16 y0);
 void canvasImageStartAddress(ru32 addr);
 void canvasImageWidth(ru16 width);
 void activeWindowXY(ru16 x0,ru16 y0);
 void activeWindowWH(ru16 width,ru16 height);
 
 void bte_Source0_MemoryStartAddr(ru32 addr);
 void bte_Source0_ImageWidth(ru16 width);
 void bte_Source0_WindowStartXY(ru16 x0,ru16 y0);
 void bte_Source1_MemoryStartAddr(ru32 addr);
 void bte_Source1_ImageWidth(ru16 width);
 void bte_Source1_WindowStartXY(ru16 x0,ru16 y0);
 void bte_DestinationMemoryStartAddr(ru32 addr);
 void bte_DestinationImageWidth(ru16 width);
 void bte_DestinationWindowStartXY(ru16 x0,ru16 y0);
 void bte_WindowSize(ru16 width, ru16 height);
 
 /*PWM function*/
 void pwm_Prescaler(ru8 prescaler);
 void pwm_ClockMuxReg(ru8 pwm1_clk_div, ru8 pwm0_clk_div, ru8 xpwm1_ctrl, ru8 xpwm0_ctrl);
 void pwm_Configuration(ru8 pwm1_inverter,ru8 pwm1_auto_reload,ru8 pwm1_start,ru8 
                       pwm0_dead_zone, ru8 pwm0_inverter, ru8 pwm0_auto_reload,ru8 pwm0_start);
 
 void pwm0_ClocksPerPeriod(ru16 clocks_per_period);
 void pwm0_Duty(ru16 duty);
 void pwm1_ClocksPerPeriod(ru16 clocks_per_period);
 void pwm1_Duty(ru16 duty);
  		
 void ramAccessPrepare(void);
 void foreGroundColor16bpp(ru16 color);
 void backGroundColor16bpp(ru16 color);
 void foreGroundColor24bpp(ru32 color);
 void backGroundColor24bpp(ru32 color);

 /*graphic function*/
 void graphicMode(boolean on);
 void setPixelCursor(ru16 x,ru16 y);

 #ifdef COLOR_DEPTH_16BPP 
 void putPixel_16bpp(ru16 x,ru16 y,ru16 color);
 void putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height);
 void putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned char *data);
 void putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned short *data);
 #endif

 #ifdef COLOR_DEPTH_24BPP 
 void putPixel_24bpp(ru16 x,ru16 y,ru32 color);
 void putPicture_24bpp(ru16 x,ru16 y,ru16 width, ru16 height);
 void putPicture_24bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned char *data);
 #endif

 
 /*text function*/
 void textMode(boolean on);
 void textColor(ru32 foreground_color,ru32 background_color);
 void setTextCursor(ru16 x,ru16 y);
 void setTextParameter1(ru8 source_select,ru8 size_select,ru8 iso_select);//cch
 void setTextParameter2(ru8 align, ru8 chroma_key, ru8 width_enlarge, ru8 height_enlarge);//cdh
 
 void genitopCharacterRomParameter(ru8 scs_select, ru8 clk_div, ru8 rom_select, ru8 character_select, ru8 gt_width);//b7h,bbh,ceh,cfh
 
 void putString(ru16 x0,ru16 y0, char *str);
 void putDec(ru16 x0,ru16 y0,rs32 vaule,ru8 len,const char *flag);
 void putFloat(ru16 x0,ru16 y0,double vaule,ru8 len,ru8 precision, const char *flag);
 void putHex(ru16 x0,ru16 y0,ru32 vaule,ru8 len,const char *flag); 
 
 /*draw function*/
 void drawLine(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color);
 void drawSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color);
 void drawSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color);
 void drawCircleSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru32 color);
 void drawCircleSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru32 color);
 void drawTriangle(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru32 color);
 void drawTriangleFill(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru32 color);
 void drawCircle(ru16 x0,ru16 y0,ru16 r,ru32 color);
 void drawCircleFill(ru16 x0,ru16 y0,ru16 r,ru32 color);
 void drawEllipse(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru32 color);
 void drawEllipseFill(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru32 color);
 
 /*BTE function*/
 void bteMemoryCopy(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 des_addr,ru16 des_image_width, 
                    ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height);
 void bteMemoryCopyWithROP(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,
                            ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height,ru8 rop_code);
 void bteMemoryCopyWithChromaKey(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,
                                ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height,ru32 chromakey_color);
                                
 void bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
                         ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned char *data);
  void bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
                         ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned short *data);
 void bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
                         ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code);                     
 void bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color,
                              const unsigned char *data);
 void bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color,
                              const unsigned short *data);
 void bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color);
 void bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color,const unsigned char *data);
 void bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color);
 void bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,
                                             ru32 foreground_color,ru32 background_color,const unsigned char *data);
 void bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,
                                             ru16 width,ru16 height,ru32 foreground_color,ru32 background_color);

 void bteMemoryCopyWith_ARGB8888(ru32 s1_addr,ru16 s1_image_width,
ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height);


/*DMA function*/
 void dma_24bitAddressBlockMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr);
 void dma_32bitAddressBlockMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr);
 void dma_24bitAddressLinearMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru32 source_addr,ru32 des_address,ru32 number);
 void dma_32bitAddressLinearMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru32 source_addr,ru32 des_address,ru32 number);
/*IDEC function*/

#ifdef COLOR_DEPTH_16BPP
 void idec_24bitAddressQuadMode6B_16bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr);
 void idec_32bitAddressQuadMode6B_16bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr);
 
 void idec_24bitAddressQuadMode6B_16bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width);
 void idec_32bitAddressQuadMode6B_16bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width);
#endif

#ifdef COLOR_DEPTH_24BPP
 void idec_24bitAddressQuadMode6B_24bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr);
 void idec_32bitAddressQuadMode6B_24bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr);
 
 void idec_24bitAddressQuadMode6B_24bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width);
 void idec_32bitAddressQuadMode6B_24bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width);
#endif


 
/*register page switch*/
 void registerPageSwitch(ru8 page);
 
/*read id code*/ 
 ru8 readIdCode(void);
 
 void fontDmaSelect_bus0(void);
 void fontDmaSelect_bus1(void);
 
 void fontDmaSelectSFI_0(void);	
 void fontDmaSelectSFI_1(void);
 void fontDmaSelectSFI_2(void);
 void fontDmaSelectSFI_3(void);
 

  
 boolean getMediaFileErrorFlag(void);
 void checkMediaDecodeBusy(void);
 boolean getMediaDecodeBusyFlag(void);
 void resetIdec(void);
 void idecSelectSfi_0(void);
 void idecSelectSfi_1(void);
 void idecSelectSfi_2(void);
 void idecSelectSfi_3(void);
 void idecSelectBus0(void);
 void idecSelectBus1(void);
 void idecDestinationColor16bpp(void);
 void idecDestinationColor24bpp(void);
 void idecStartsDecoding(void);
 void checkIdecBusy(void);
 boolean getIdecBusyFlag(void);
 void idecSelectSfi24bitAddress(void);
 void idecSelectSfi32bitAddress(void);
 void idecSelectStandardSpiMode0AndMode3(void);
 void idecSelectRA8875SpiMode0AndMode3(void);
 void idecSelectSfiNormalRead_03h(void);
 void idecSelectSfiFastRead_0Bh(void);
 void idecSelectSfiFastRead_1Bh(void);
 void idecSelectSfiDualRead_3Bh(void);
 void idecSelectSfiDualRead_BBh(void);
 void idecSelectSfiQuadRead_6Bh(void);
 void idecSelectSfiQuadRead_EBh(void);
 void idecClockDivide(ru8 divide);
 void idecSourceStartAddress(ru32 Addr);
 void idecDestinationUpperLeftCorner(ru16 WX,ru16 HY);	
 void idecAviDisplayBufferUsePipAndShadowPip(ru8 pip_select);

 void idecTransferNumber(ru32 Num);
 void idecDestinationStartAddress(ru32 Addr);
 void idecDestinationImageWidth(ru16 WX); 

 /*IDEC with MDU function*/
 void aviShadowPipStartAddress(ru32 Addr);
 void setAviPause(void);
 boolean getAviPauseFlag(void);
 void stopAvi(void);
 void aviWindowOn(boolean enable);
 
 
/*spim function*/ 
 void spimDataPut(ru8 data);
 ru8 getSpimTxFifoEmptyFlag();
 
 void spimSelectNssDriveOn_xnsfcs0(void);
 void spimSelectNssDriveOn_xnsfcs1(void);
 void spimSelectNssDriveOn_xnsfcs2(void);
 void spimSelectNssDriveOn_xnsfcs3(void);
 void spimNssHigh(void);
 void spimNssLow(void);
 void spimSelectBus(ru8 bus_select);	
 void spimClockPeriod(ru8 divide);
 
 void spimSetSerialFlash4BytesMode(ru8 bus_select,ru8 scs_select);//
 void spimSetSerialFlashQuadMode(ru8 bus_select,ru8 scs_select,ru8 flash_select,ru8 data1,ru8 data2);
 
 void pip1_Enable(void);
 void pip1_Disable(void);
 void pip2_Enable(void);
 void pip2_Disable(void);
 void pip1_ParameterSelect(void);
 void pip2_ParameterSelect(void);
 void pip1_SelectWindow16bpp(void);
 void pip1_SelectWindow24bpp(void);
 void pip2_SelectWindow16bpp(void);
 void pip2_SelectWindow24bpp(void);
 void pipWindowDisplay_XY(ru16 WX,ru16 HY);
 void pipImageStartAddress(ru32 Addr);
 void pipImageWidth(ru16 WX);
 void pipWindowImageStartXY(ru16 WX,ru16 HY);
 void pipWindowWidthHeight(ru16 WX,ru16 HY);


 	
};

#endif
