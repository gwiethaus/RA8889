/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [gwiethaus](https://github.com/gwiethaus)
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "Panel_Device.hpp"

//Conjunto de registradores de Page0 e Page1
//Para alternar o conjunto de registradores acessiveis entre Page0 e Page1 precisa escrever ou ler o registrador 0x46
//este mesmo registrador esta presente dentro do page 0 e page 1. o bit 0 deste registrador 0x46 é o mesmo da page 0 e page 1 no entanto são diferentes os bits 1-7
//Alguns bits tem funcionalidades especificas do registrador 0x46 por isso precisa ler este registrador 0x46 apra se saber qual pagina de registradores está sendo usado
//faça a leitura do registrador 0x46 para saber qual page está ativa e entao poder escrever adequadamente o registrador 0x46 se houver necessidade.
//Em duas paginas page 0 e page 1 podem ter o mesmo valor de registrador, mas com funcionalidade diferentes ou mesma funcionalidade.

//Status Register
//Deve envair ums solicitacao 0x40 via SPI/I2C
#define REG_STSR               0x00            //Status Register (STSR)

//Chip Configuration Registers
#define REG_SRR                0x00            //Software Reset Register (SRR)
#define REG_CCR                0x01            //Chip Configuration Register (CCR)
#define REG_MACR               0x02            //Memory Access Control Register (MACR)
#define REG_ICR                0x03            //Input Control Register (ICR) 
#define REG_MRWDP              0x04            //Memory Data Read/Write Port (MRWDP)
#define REG_PPLLC1             0x05            //SCLK PLL Control Register 1 (PPLLC1)
#define REG_PPLLC2             0x06            //SCLK PLL Control Register 2 (PPLLC2) 
#define REG_MPLLC1             0x07            //MCLK PLL Control Register 1 (MPLLC1)
#define REG_MPLLC2             0x08            //MCLK PLL Control Register 2 (MPLLC2)
#define REG_SPLLC1             0x09            //CCLK PLL Control Register 1 (SPLLC1)
#define REG_SPLLC2             0x0a            //CCLK PLL Control Register 2 (SPLLC2)

//Interrupt Control Registers
#define REG_PG0_INTEN          0x0b            //Page 0 Interrupt Enable Register (INTEN)
#define REG_PG0_INTF           0x0c            //Page 0 Interrupt Event Flag Register (INTF)
#define REG_PG0_MINTFR         0x0d            //Page 0 Mask Interrupt Flag Register (MINTFR)
#define REG_PUENR              0x0e            //Page 0 Pull- high control Register (PUENR)
#define REG_PSFSR              0x0f            //Page 0 PDAT for PIO/Key Function Select Register (PSFSR)

//LCD Display Control Registers
#define REG_MPWCTR             0x10            //Page 0 Main/PIP Window Control Register (MPWCTR)
#define REG_PIPCDEP            0x11            //Page 0 PIP Window Color Depth Setting (PIPCDEP)
#define REG_DPCR               0x12            //Page 0 Display Configuration Register (DPCR)
#define REG_PCSR               0x13            //Page 0 Panel scan Clock and Data Setting Register (PCSR)
#define REG_HDWR               0x14            //Page 0 Horizontal Display Width Register (HDWR)
#define REG_HDWFTR             0x15            //Page 0 Horizontal Display Width Fine Tune Register (HDWFTR)
#define REG_HNDR               0x16            //Page 0 Horizontal Non-Display Period Register (HNDR)
#define REG_HNDFTR             0x17            //Page 0 Horizontal Non-Display Period Fine Tune Register (HNDFTR)
#define REG_HSTR               0x18            //Page 0 HSYNC Start Position Register (HSTR)
#define REG_HPWR               0x19            //Page 0 HSYNC Pulse Width Register (HPWR)
#define REG_VDHR0              0x1a            //Page 0 Vertical Display Height Register 0(VDHR0)
#define REG_VDHR1              0x1b            //Page 0 Vertical Display Height Register 1 (VDHR1)
#define REG_VNDR0              0x1c            //Page 0 Vertical Non-Display Period Register 0(VNDR0)
#define REG_VNDR1              0x1d            //Page 0 Vertical Non-Display Period Register 1(VNDR1)
#define REG_VSTR               0x1e            //Page 0 VSYNC Start Position Register (VSTR)
#define REG_VPWR               0x1f            //Page 0 VSYNC Pulse Width Register (VPWR)
#define REG_MISA0              0x20            //Page 0 Main Image Start Address 0 (MISA0)
#define REG_MISA1              0x21            //Page 0 Main Image Start Address 1 (MISA1)
#define REG_MISA2              0x22            //Page 0 Main Image Start Address 2 (MISA2)
#define REG_MISA3              0x23            //Page 0 Main Image Start Address 3 (MISA3)
#define REG_MIW0               0x24            //Page 0 Main Image Width 0 (MIW0)
#define REG_MIW1               0x25            //Page 0 Main Image Width 1 (MIW1)
#define REG_MWULX0             0x26            //Page 0 Main Window Upper-Left corner X-coordinates 0 (MWULX0)
#define REG_MWULX1             0x27            //Page 0 Main Window Upper-Left corner X-coordinates 1 (MWULX1)
#define REG_MWULY0             0x28            //Page 0 Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
#define REG_MWULY1             0x29            //Page 0 Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
#define REG_PWDULX0            0x2a            //Page 0 PIP 1 or 2 Window Display Upper-Left corner X-coordinates 0 (PWDULX0)
#define REG_PWDULX1            0x2b            //Page 0 PIP 1 or 2 Window Display Upper-Left corner X-coordinates 1 (PWDULX1)
#define REG_PWDULY0            0x2c            //Page 0 PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 0 (PWDULY0)
#define REG_PWDULY1            0x2d            //Page 0 PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 1 (PWDULY1)
#define REG_PISA0              0x2e            //Page 0 PIP 1 or 2 Image Start Address 0 (PISA0)
#define REG_PISA1              0x2f            //Page 0 PIP 1 or 2 Image Start Address 1 (PISA1)
#define REG_PISA2              0x30            //Page 0 PIP 1 or 2 Image Start Address 2 (PISA2)
#define REG_PISA3              0x31            //Page 0 PIP 1 or 2 Image Start Address 3 (PISA3)
#define REG_PIW0               0x32            //Page 0 PIP 1 or 2 Image Width 0 (PIW0)
#define REG_PIW1               0x33            //Page 0 PIP 1 or 2 Image Width 1 (PIW1)
#define REG_PWIULX0            0x34            //Page 0 PIP 1 or 2 Window Image Upper-Left corner X-coordinates 0 (PWIULX0)
#define REG_PWIULX1            0x35            //Page 0 PIP 1 or 2 Window Image Upper-Left corner X-coordinates 1 (PWIULX1)
#define REG_PWIULY0            0x36            //Page 0 PIP 1 or 2 Window Image Upper-Left corner Y-coordinates (PWIULY0)
#define REG_PWIULY1            0x37            //Page 0 PIP 1 or 2 Window Image Upper-Left corner Y-coordinates 1 (PWIULY1)
#define REG_PWW0               0x38            //Page 0 PIP 1 or 2 Window Width 0 (PWW0)
#define REG_PWW1               0x39            //Page 0 PIP 1 or 2 Window Width 1 (PWW1)
#define REG_PWH0               0x3a            //Page 0 PIP 1 or 2 Window Height 0 (PWH0)
#define REG_PWH1               0x3b            //Page 0 PIP 1 or 2 Windows Height 1 (PWH1)
#define REG_GTCCR              0x3c            //Page 0 Graphic / Text Cursor Control Register (GTCCR)
#define REG_BTCR               0x3d            //Page 0 Blink Time Control Register (BTCR)
#define REG_CURHS              0x3e            //Page 0 Text Cursor Horizontal Size Register (CURHS)
#define REG_CURVS              0x3f            //Page 0 Text Cursor Vertical Size Register (CURVS)
#define REG_GCHP0              0x40            //Page 0 Graphic Cursor Horizontal Position Register 0 (GCHP0)
#define REG_GCHP1              0x41            //Page 0 Graphic Cursor Horizontal Position Register 1 (GCHP1)
#define REG_GCVP0              0x42            //Page 0 Graphic Cursor Vertical Position Register 0 (GCVP0)
#define REG_GCVP1              0x43            //Page 0 Graphic Cursor Vertical Position Register 1 (GCVP1)
#define REG_GCC0               0x44            //Page 0 Graphic Cursor Color 0 (GCC0)
#define REG_GCC1               0x45            //Page 0 Graphic Cursor Color 1 (GCC1)

#define REG_PAGE_SWITCH        0x46            //Page 0 / Page 1 PAGE Switch

//Geomatric Engine Control Registers
#define REG_CVSSA0             0x50            //Page 0 Canvas Start address 0 (CVSSA0)
#define REG_CVSSA1             0x51            //Page 0 Canvas Start address 1 (CVSSA1)
#define REG_CVSSA2             0x52            //Page 0 Canvas Start address 2 (CVSSA2)
#define REG_CVSSA3             0x53            //Page 0 Canvas Start address 3 (CVSSA3)
#define REG_CVS_IMWTH0         0x54            //Page 0 Canvas image width 0 (CVS_IMWTH0)
#define REG_CVS_IMWTH1         0x55            //Page 0 Canvas image width 1 (CVS_IMWTH1)
#define REG_AWUL_X0            0x56            //Page 0 Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
#define REG_AWUL_X1            0x57            //Page 0 Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
#define REG_AWUL_Y0            0x58            //Page 0 Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
#define REG_AWUL_Y1            0x59            //Page 0 Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
#define REG_AW_WTH0            0x5a            //Page 0 Active Window Width 0 (AW_WTH0)
#define REG_AW_WTH1            0x5b            //Page 0 Active Window Width 1 (AW_WTH1)
#define REG_AW_HT0             0x5c            //Page 0 Active Window Height 0 (AW_HT0)
#define REG_AW_HT1             0x5d            //Page 0 Active Window Height 1 (AW_HT1)
#define REG_AW_COLOR           0x5e            //Page 0 Color Depth of Canvas & Active Window (AW_COLOR)
#define REG_CURH0              0x5f            //Page 0 Graphic Read/Write position Horizontal Position Register 0 (CURH0)
#define REG_CURH1              0x60            //Page 0 Graphic Read/Write position Horizontal Position Register 1 (CURH1)
#define REG_CURV0              0x61            //Page 0 Graphic Read/Write position Vertical Position Register 0 (CURV0)
#define REG_CURV1              0x62            //Page 0 Graphic Read/Write position Vertical Position Register 1 (CURV1)
#define REG_F_CURX0            0x63            //Page 0 Text Write X-coordinates Register 0 (F_CURX0)
#define REG_F_CURX1            0x64            //Page 0 Text Write X-coordinates Register 1 (F_CURX1)
#define REG_F_CURY0            0x65            //Page 0 Text Write Y-coordinates Register 0 (F_CURY0)
#define REG_F_CURY1            0x66            //Page 0 Text Write Y-coordinates Register 1 (F_CURY1)
#define REG_DCR0               0x67            //Page 0 Draw Line / Triangle Control Register 0 (DCR0)
#define REG_DLHSR0             0x68            //Page 0 Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
#define REG_DLHSR1             0x69            //Page 0 Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
#define REG_DLVSR0             0x6a            //Page 0 Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
#define REG_DLVSR1             0x6b            //Page 0 Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
#define REG_DLHER0             0x6c            //Page 0 Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
#define REG_DLHER1             0x6d            //Page 0 Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
#define REG_DLVER0             0x6e            //Page 0 Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
#define REG_DLVER1             0x6f            //Page 0 Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
#define REG_DTPH0              0x70            //Page 0 Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
#define REG_DTPH1              0x71            //Page 0 Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
#define REG_DTPV0              0x72            //Page 0 Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
#define REG_DTPV1              0x73            //Page 0 Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
#define REG_DCR1               0x76            //Page 0 Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
#define REG_ELL_A0             0x77            //Page 0 Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
#define REG_ELL_A1             0x78            //Page 0 Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
#define REG_ELL_B0             0x79            //Page 0 Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
#define REG_ELL_B1             0x7a            //Page 0 Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
#define REG_DEHR0              0x7b            //Page 0 Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
#define REG_DEHR1              0x7c            //Page 0 Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
#define REG_DEVR0              0x7d            //Page 0 Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
#define REG_DEVR1              0x7e            //Page 0 Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)

//PWM Timer Control Registers
#define REG_PSCLR              0x84            //Page 0 PWM Prescaler Register (PSCLR)
#define REG_PMUXR              0x85            //Page 0 PWM clock Mux Register (PMUXR)
#define REG_PCFGR              0x86            //Page 0 PWM Configuration Register (PCFGR)
#define REG_DZ_LENGTH          0x87            //Page 0 Timer 0 Dead zone length register [DZ_LENGTH]
#define REG_TCMPB0L            0x88            //Page 0 Timer 0 compare buffer register [TCMPB0L]
#define REG_TCMPB0H            0x89            //Page 0 Timer 0 compare buffer register [TCMPB0H]
#define REG_TCNTB0L            0x8a            //Page 0 Timer 0 count buffer register [TCNTB0L]
#define REG_TCNTB0H            0x8b            //Page 0 Timer 0 count buffer register [TCNTB0H]
#define REG_TCMPB1L            0x8c            //Page 0 Timer 1 compare buffer register [TCMPB1L]
#define REG_TCMPB1H            0x8d            //Page 0 Timer 1 compare buffer register [TCMPB1H]
#define REG_TCNTB1L            0x8e            //Page 0 Timer 1 count buffer register [TCNTB1L]
#define REG_TCNTB1H            0x8f            //Page 0 Timer 1 count buffer register [TCNTB1H]

//Block Transfer Engine (BTE) Control Registers
#define REG_BTE_CTRL0          0x90            //Page 0 BTE Function Control Register 0 (BTE_CTRL0)
#define REG_BTE_CTRL1          0x91            //Page 0 BTE Function Control Register1 (BTE_CTRL1)
#define REG_BTE_COLR           0x92            //Page 0 Source 0/1 & Destination Color Depth (BTE_COLR)
#define REG_S0_STR0            0x93            //Page 0 Source 0 memory start address 0 (S0_STR0)
#define REG_S0_STR1            0x94            //Page 0 Source 0 memory start address 1 (S0_STR1)
#define REG_S0_STR2            0x95            //Page 0 Source 0 memory start address 2 (S0_STR2)
#define REG_S0_STR3            0x96            //Page 0 Source 0 memory start address 3 (S0_STR3)
#define REG_S0_WTH0            0x97            //Page 0 Source 0 image width 0 (S0_WTH0)
#define REG_S0_WTH1            0x98            //Page 0 Source 0 image width 1 (S0_WTH1)
#define REG_S0_X0              0x99            //Page 0 Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
#define REG_S0_X1              0x9a            //Page 0 Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
#define REG_S0_Y0              0x9b            //Page 0 Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
#define REG_S0_Y1              0x9c            //Page 0 Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
#define REG_S1_STR0_RED        0x9d            //Page 0 Source 1 memory start address 0 (S1_STR0)/S1 constant color – Red element (S1_RED)
#define REG_S1_STR1_GREEN      0x9e            //Page 0 Source 1 memory start address 1 (S1_STR1)/S1 constant color – Green element (S1_GREEN)
#define REG_S1_STR2_BLUE       0x9f            //Page 0 Source 1 memory start address 2 (S1_STR2)/S1 constant color – Blue element (S1_BLUE)
#define REG_S1_STR3            0xa0            //Page 0 Source 1 memory start address 3 (S1_STR3)
#define REG_S1_WTH0            0xa1            //Page 0 Source 1 image width 0 (S1_WTH0)
#define REG_S1_WTH1            0xa2            //Page 0 Source 1 image width 1 (S1_WTH1)
#define REG_S1_X0              0xa3            //Page 0 Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
#define REG_S1_X1              0xa4            //Page 0 Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
#define REG_S1_Y0              0xa5            //Page 0 Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
#define REG_S1_Y1              0xa6            //Page 0 Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
#define REG_DT_STR0            0xa7            //Page 0 Destination memory start address 0 (DT_STR0)
#define REG_DT_STR1            0xa8            //Page 0 Destination memory start address 1 (DT_STR1)
#define REG_DT_STR2            0xa9            //Page 0 Destination memory start address 2 (DT_STR2)
#define REG_DT_STR3            0xaa            //Page 0 Destination memory start address 3 (DT_STR3)
#define REG_DT_WTH0            0xab            //Page 0 Destination image width 0 (DT_WTH0)
#define REG_DT_WTH1            0xac            //Page 0 Destination image width 1 (DT_WTH1)
#define REG_DT_X0              0xad            //Page 0 Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
#define REG_DT_X1              0xae            //Page 0 Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
#define REG_DT_Y0              0xaf            //Page 0 Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
#define REG_DT_Y1              0xb0            //Page 0 Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
#define REG_BTE_WTH0           0xb1            //Page 0 BTE Window Width 0 (BTE_WTH0)
#define REG_BTE_WTH1           0xb2            //Page 0 BTE Window Width 1 (BTE_WTH1)
#define REG_BTE_HIG0           0xb3            //Page 0 BTE Window Height 0 (BTE_HIG0)
#define REG_BTE_HIG1           0xb4            //Page 0 BTE Window Height 1 (BTE_HIG1)
#define REG_APB_CTRL           0xb5            //Page 0 Alpha Blending (APB_CTRL)

//Serial Flash & SPI Master Control Registers
#define REG_DMA_CTRL           0xb6            //Page 0 Serial flash DMA Controller REG (DMA_CTRL)
#define REG_SFL_CTRL           0xb7            //Page 0 Serial Flash/ROM Controller Register (SFL_CTRL)
#define REG_SPIDR              0xb8            //Page 0 SPI master Tx /Rx FIFO Data Register (SPIDR)
#define REG_SPIMCR2            0xb9            //Page 0 SPI master Control Register (SPIMCR2)
#define REG_SPIMSR             0xba            //Page 0 SPI master Status Register (SPIMSR)
#define REG_SPI_DIVSOR         0xbb            //Page 0 SPI Clock period (SPI_DIVSOR)
#define REG_DMA_SSTR0          0xbc            //Page 0 Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
#define REG_DMA_SSTR1          0xbd            //Page 0 Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
#define REG_DMA_SSTR2          0xbe            //Page 0 Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
#define REG_DMA_SSTR3          0xbf            //Page 0 Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
#define REG_DMA_DX0            0xc0            //Page 0 DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
#define REG_DMA_DX1            0xc1            //Page 0 DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
#define REG_DMA_DY0            0xc2            //Page 0 DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
#define REG_DMA_DY1            0xc3            //Page 0 DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
#define REG_SPIMBS             0xc5            //Page 0 SPI Master Bus Select (SPIMBS)
#define REG_DMAW_WTH0          0xc6            //Page 0 DMA Block Width 0 (DMAW_WTH0)
#define REG_DMAW_WTH1          0xc7            //Page 0 DMA Block Width 1 (DMAW_WTH1)
#define REG_DMAW_HIGH0         0xc8            //Page 0 DMA Block Height 0 (DMAW_HIGH0)
#define REG_DMAW_HIGH1         0xc9            //Page 0 DMA Block Height 1 (DMAW_HIGH1)
#define REG_DMA_SWTH0          0xca            //Page 0 DMA Source Picture Width 0 (DMA_SWTH0)
#define REG_DMA_SWTH1          0xcb            //Page 0 DMA Source Picture Width 0 (DMA_SWTH1)

//Text Engine
#define REG_CCR0               0xcc            //Page 0 Character Control Register 0 (CCR0)
#define REG_CCR1               0xcd            //Page 0 Character Control Register 1 (CCR1)
#define REG_GTFNT_SEL          0xce            //Page 0 GT Character ROM Select (GTFNT_SEL)
#define REG_GTFNT_CR           0xcf            //Page 0 GT Character ROM Control register (GTFNT_CR)
#define REG_FLDR               0xd0            //Page 0 Character Line gap Setting Register (FLDR)
#define REG_F2FSSR             0xd1            //Page 0 Character to Character Space Setting Register (F2FSSR)
#define REG_FGCR               0xd2            //Page 0 Foreground Color Register - Red (FGCR)
#define REG_FGCG               0xd3            //Page 0 Foreground Color Register - Green (FGCG)
#define REG_FGCB               0xd4            //Page 0 Foreground Color Register - Blue (FGCB)
#define REG_BGCR               0xd5            //Page 0 Background Color Register - Red (BGCR)
#define REG_BGCG               0xd6            //Page 0 Background Color Register - Green (BGCG)
#define REG_BGCB               0xd7            //Page 0 Background Color Register - Blue (BGCB)
#define REG_CGRAM_STR0         0xdb            //Page 0 CGRAM Start Address 0 (CGRAM_STR0)
#define REG_CGRAM_STR1         0xdc            //Page 0 CGRAM Start Address 1 (CGRAM_STR1)
#define REG_CGRAM_STR2         0xdd            //Page 0 CGRAM Start Address 2 (CGRAM_STR2)
#define REG_CGRAM_STR3         0xde            //Page 0 CGRAM Start Address 3 (CGRAM_STR3)

//Power Management Control Register
#define REG_PMU                0xdf            //Page 0 Power Management register (PMU)

//SDRAM Control Register
#define REG_SDRAR              0xe0            //Page 0 SDRAM attribute register (SDRAR)
#define REG_SDRMD              0xe1            //Page 0 SDRAM mode register & extended mode register (SDRMD)
#define REG_SDR_REF_ITVL0      0xe2            //Page 0 SDRAM auto refresh interval (SDR_REF_ITVL0)
#define REG_SDR_REF_ITVL1      0xe3            //Page 0 SDRAM auto refresh interval (SDR_REF_ITVL1)
#define REG_SDRCR              0xe4            //Page 0 SDRAM Control register (SDRCR)
#define REG_SDR_TP1            0xe0            //Page 0 SDRAM timing parameter 1
#define REG_SDR_TP2            0xe1            //Page 0 SDRAM timing parameter 2
#define REG_SDR_TP3            0xe2            //Page 0 SDRAM timing parameter 3
#define REG_SDR_TP4            0xe3            //Page 0 SDRAM timing parameter 4

//IIC Master Registers
#define REG_IICMCPR0           0xe5            //Page 0 IIC Master Clock Pre-scale Register 0 (IICMCPR0)
#define REG_IICMCPR1           0xe6            //Page 0 IIC Master Clock Pre-scale Register 1 (IICMCPR1)
#define REG_IICMTXR            0xe7            //Page 0 IIC Master Transmit Register (IICMTXR)
#define REG_IICMRXR            0xe8            //Page 0 IIC Master Receiver Register (IICMRXR)
#define REG_IICMCMDR           0xe9            //Page 0 IIC Master Command Register (IICMCMDR)
#define REG_IICMSTUR           0xea            //Page 0 IIC Master Status Register (IICMSTUR)

//GPI & GPO Register
#define REG_GPIOAD             0xf0            //Page 0 GPIO-A direction (GPIOAD)
#define REG_GPIOA              0xf1            //Page 0 GPIO-A (GPIOA)
#define REG_GPIOB              0xf2            //Page 0 GPIO-B (GPIOB)
#define REG_GPIOCD             0xf3            //Page 0 GPIO-C direction (GPIOCD)
#define REG_GPIOC              0xf4            //Page 0 GPIO-C (GPIOC)
#define REG_GPIODD             0xf5            //Page 0 GPIO-D direction (GPIODD)
#define REG_GPIOD              0xf6            //Page 0 GPIO-D (GPIOD)
#define REG_GPIOED             0xf7            //Page 0 GPIO-E direction (GPIOED)
#define REG_GPIOE              0xf8            //Page 0 GPIO-E (GPIOE)
#define REG_GPIOFD             0xf9            //Page 0 GPIO-F direction (GPIOFD)
#define REG_GPIOF              0xfa            //Page 0 GPIO-F (GPIOF)

//Key-Scan Control Registers
#define REG_KSCR1              0xfb            //Page 0 Key-Scan Control Register 1 (KSCR1)
#define REG_KSCR2              0xfc            //Page 0 Key-Scan Controller Register 2 (KSCR2)
#define REG_KSDR0              0xfd            //Page 0 Key-Scan Data Register (KSDR0)
#define REG_KSDR1              0xfe            //Page 0 Key-Scan Data Register (KSDR1)
#define REG_KSDR2              0xff            //Page 0 Key-Scan Data Register (KSDR2)

//Media Decoder Relative Registers
#define REG_PG1_INTEN          0x0b            //Page 1 Interrupt Enable Register (INTEN)                              
#define REG_PG1_INTF           0x0c            //Page 1 Interrupt Event Flag Register (INTF)                              
#define REG_MINTFR             0x0d            //Page 1 Mask Interrupt Flag Register (MINTFR)
#define REG_AVI_SPIP_SADR0     0x2e            //Page 1 AVI shadow pip start address 0 (avi_spip_sadr0)
#define REG_AVI_SPIP_SADR1     0x2f            //Page 1 AVI shadow pip start address 1 (avi_spip_sadr1)
#define REG_AVI_SPIP_SADR2     0x30            //Page 1 AVI shadow pip start address 2 (avi_spip_sadr2)
#define REG_AVI_SPIP_SADR2     0x31            //Page 1 AVI shadow pip start address 3 (avi_spip_sadr3)
#define REG_VC1                0xa0            //Page 1 Video Control (VC) 1
#define REG_MIHH               0xa1            //Page 1 Media Image Height High Byte (MIHH)
#define REG_MIHL               0xa2            //Page 1 Media Image Height Low Byte (MIHL)
#define REG_MIWH               0xa3            //Page 1 Media Image Width High Byte (MIWH)
#define REG_MIWL               0xa4            //Page 1 Media Image Width Low Byte (MIWL)
#define REG_VFPB3              0xa5            //Page 1 Video Frame Period Byte3 (VFPB3)
#define REG_VFPB2              0xa6            //Page 1 Video Frame Period Byte2 (VFPB2)
#define REG_VFPB1              0xa7            //Page 1 Video Frame Period Byte1 (VFPB1)
#define REG_VFPB0              0xa8            //Page 1 Video Frame Period Byte0 (VFPB0)
#define REG_VC2                0xa9            //Page 1 Video Control (VC) 2
#define REG_IDEC_CTRL0         0xb6            //Page 1 Serial flash AVI/JPG/BMP (IDEC_CTRL)                                   
#define REG_IDEC_CTRL1         0xb7            //Page 1 Serial flash AVI/JPG/BMP (IDEC_CTRL)
#define REG_IDEC_CLKDIV        0xbb            //Page 1 IDEC Clock divide
#define REG_IDEC_SADR0         0xbc            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 0 (IDEC_SADR0)
#define REG_IDEC_SADR1         0xbd            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 1 (IDEC_SADR1)
#define REG_IDEC_SADR2         0xbe            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 2 (IDEC_SADR2)
#define REG_IDEC_SADR3         0xbf            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 3 (IDEC_SADR3)
#define REG_IDEC_DX0           0xc0            //Page 1 IDEC (JPG/BMP)Destination Window Upper-Left corner X-coordinates 0 (IDEC_DX0)
#define REG_IDEC_DX1           0xc1            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 1 (IDEC_DX1)
#define REG_IDEC_DY0           0xc2            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 0 (IDEC_DY0)
#define REG_IDEC_DY1           0xc3            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 1 (IDEC_DY1)
#define REG_IDEC_PIP           0xc5            //Page 1 IDEC AVI PIP controller (IDEC_PIP)
#define REG_IDEC_TF0           0xc6            //Page 1 IDEC (AVI/JPG/BMP) transfer number 0 (IDEC_TF0)
#define REG_IDEC_TF1           0xc7            //Page 1 IDEC (AVI/JPG/BMP) transfer number 1 (IDEC_TF1)
#define REG_IDEC_TF2           0xc8            //Page 1 IDEC (AVI/JPG/BMP) transfer number 2 (IDEC_TF2)
#define REG_IDEC_TF3           0xc9            //Page 1 IDEC (AVI/JPG/BMP) transfer number 3 (IDEC_TF3)
#define REG_IDEC_DADR0         0xca            //Page 1 IDEC (JPG/BMP) Destination memory start addr 0 (IDEC_DADR0)
#define REG_IDEC_DADR1         0xcb            //Page 1 IDEC (JPG/BMP) Destination memory start addr 1 (IDEC_DADR1)
#define REG_IDEC_DADR2         0xcc            //Page 1 IDEC (JPG/BMP) Destination memory start addr 2 (IDEC_DADR2)
#define REG_IDEC_DADR3         0xcd            //Page 1 IDEC (JPG/BMP) Destination memory start addr 3 (IDEC_DADR3)
#define REG_IDEC_DWTH0         0xce            //Page 1 IDEC (JPG/BMP) Destination Image Width 0 (IDEC_DWTH0)
#define REG_IDEC_DWTH1         0xcf            //Page 1 IDEC (JPG/BMP) Destination Image Width 1 (IDEC_DWTH1)
#define REG_AVI_PAUSE          0xd3            //Page 1 AVI pause
#define REG_AVI_STOP           0xd4            //Page 1 AVI stop                        

//uso para Set bits (or apply)
#define cSetb0    0x01
#define cSetb1    0x02
#define cSetb2    0x04
#define cSetb3    0x08
#define cSetb4    0x10
#define cSetb5    0x20
#define cSetb6    0x40
#define cSetb7    0x80

//Uso para Clear bits (and apply)
#define cClrb0    0xfe
#define cClrb1    0xfd
#define cClrb2    0xfb
#define cClrb3    0xf7
#define cClrb4    0xef
#define cClrb5    0xdf
#define cClrb6    0xbf
#define cClrb7    0x7f

 //==== [SW_(1)]  PLL  =====
#define OSC_FREQ     10	  // crystal clcok
#define DRAM_FREQ    133 // SDRAM clock frequency, unti: MHz		  
#define CORE_FREQ    120  // Core (system) clock frequency, unit: MHz 
#define SCAN_FREQ     34 // Panel Scan clock frequency, unit: MHz

class Panel_RA8889 {
	
	public
		Panel_RA8889(uint8_t cs, uint8_t rst);
		uint8_t init();
		void DisplayOn(bool on);
		void GraphicMode(void);
		void TextMode(void);
		
	private
        void RegisterWrite(uint8_t Cmd, uint8_t Data);
	    uint8_t RegisterRead(uint8_t Cmd);
		uint8_t StatusRead(void);
		void HardwareReset(void);

	protected
		uint8_t _cs;	      //chip select pin
		uint8_t _rst;	      //chip reset pin
		unsigned int _width;  //lardura do display
		unsigned int _height; //altura do display
		
		void SPISetCS(bool active);
		uint8_t SPIRwByte(uint8_t value);
		void SPI_CmdWrite(int cmd);
	    void SPI_DataWrite(int data);
		void SPI_DataWrite_Pixel(int data);
		int SPI_DataRead(void);
        void SPI_Init();
		
		void PLL_WaitReady(void);
		void PLL_ConfigClocks(void);		
		void SDRAM_WaitReady(void);
		void SDRAM_Init(void);
		
}













//----------------------------------------------------------------------------

  struct Panel_RA8875: public Panel_Device
  {
    Panel_RA8875(void)
    {
      _cfg.memory_width  = _cfg.panel_width  = 800;
      _cfg.memory_height = _cfg.panel_height = 480;
      _cfg.dummy_read_pixel = 16;
      _cfg.dummy_read_bits  =  0;
    }

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(uint_fast8_t r) override;

    void writeCommand(uint32_t, uint_fast8_t) override;
    void writeData(uint32_t, uint_fast8_t) override;

    void waitDisplay(void) override;
    bool displayBusy(void) override;

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t len) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;

    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;


    void setInvert(bool invert) override {}; // Not yet implemented.
    void setSleep(bool flg) override {}; // Not yet implemented.
    void setPowerSave(bool flg) override {}; // Not yet implemented.
    uint32_t readCommand(uint_fast16_t cmd, uint_fast8_t index, uint_fast8_t len) override { return 0; }; // Not yet implemented.
    uint32_t readData(uint_fast8_t index, uint_fast8_t len)  override { return 0; }; // Not yet implemented.

  protected:

    uint32_t _latestcolor = 0;
    uint16_t _colstart = 0;
    uint16_t _rowstart = 0;
    uint16_t _xpos = UINT16_MAX;
    uint16_t _ypos = UINT16_MAX;
    bool _in_transaction = false;
    bool _flg_serialbus = false;
    bool _flg_memorywrite = false;
    uint8_t _reg_0x51[16] = {0};

    const uint8_t* getInitCommands(uint8_t listno) const override;
    void begin_transaction(void);
    void end_transaction(void);
    void write_bytes(const uint8_t* data, uint32_t len, bool use_dma);
    void update_madctl(void);

    void _set_write_pos(uint_fast16_t x, uint_fast16_t y);
    bool _wait_busy( uint32_t timeout = 1000);
    void _write_reg(uint_fast16_t reg, uint_fast16_t data);
    void _write_reg_0x51(uint8_t reg, uint8_t data);
    void _start_memorywrite(void);
  };

//----------------------------------------------------------------------------
 }
}
