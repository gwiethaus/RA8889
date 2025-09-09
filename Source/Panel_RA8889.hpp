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

//SPI
#define RA8889_SPI_CMDWRITE    0x00            //Write Command for SPI
#define RA8889_SPI_DATAWRITE   0x80            //Write Data for SPI
#define RA8889_SPI_DATAREAD    0xc0            //Read Data from SPI 
#define RA8889_SPI_STATUSREAD  0x40            //Read Status from SPI

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

//==== RA8889 frequency configuration [SW_(1)] PLL =====
#define OSC_FREQ     10	                       // Crystal clock
#define DRAM_FREQ    133                       // SDRAM clock frequency, unti: MHz		  
#define CORE_FREQ    120                       // Core (system) clock frequency, unit: MHz 
#define SCAN_FREQ    34                        // PSCK Panel Scan clock frequency, unit: MHz

//==== Valores tipicos do fabricante da tela de display LCD =====
//O RA8889 usa os valores adequados apra envio ao display LCD par a exibição correta das imagam
//Cada fabricante de tela possui as especificações e caracteristicas configuradas nesta constantes abaixo
//Consulte o Datasheet do fabricante de tela para entender estes valores 
//Exemplo de alguns Fabricnates
//https://www.twscreen.com/index.php/lcdpanel/search?brand=19&size=3

//===== LCD screen model =====
//#define ILI6122                                //ILITEK ILI6122 800x480 7.0" TFT-LCD, https://www.buydisplay.com/7-tft-lcd-touch-screen-display-module-800x480-for-mp4-gps-tablet-pc
//#define ST7277                                 //Sitronix ST7277 800x480 7.0" TFT-LCD, IPS, https://www.buydisplay.com/wide-angle-7-inch-800x480-color-ips-tft-display-st7277-controller
//#define HX8282                                 //HiMAX HX8282
//#define HX8264                                 //HiMAX HX8264 800x480 7.0"
//#define HX8664                                 //HiMAX HX8664 800x480 7.0"
//#define AWY_800480T70N02                       //AWY_800480T70N02
//#define LQ035NC111                             //Innolux LQ035NC111 320x240
//#define AWT_1024600L7N03                       //AWT_1024600L7N03
#define EK9713                                 //Fitipower EK9713 800x600/800x480, https://www.buydisplay.com/7-inch-lcd-screen-tft-display-module-wvga-800x480-at070tn90-at070tn92
//#define EK7330                                 //Fitipower EK7330 800x480 7.0" TFT
//#define EK73002                                //Fitipower EK73002
//#define L80480R70		                         //L80480R70 800x480
//#define AT070TN90                              //Innolux AT070TN90
//#define AT070TN92                              //Innolux AT070TN92 800X480 7"
//#define AT070TN94                              //Innolux AT070TN94
//#define AT070TN92_V1_TP                        //Innolux AT070TN92 V.1 TP 800X480 7.0" TFT-LCD 
//#define EJ080NA-04A                            //Innolux EJ080NA-04A 1024X768 8.0" TFT-LCD
//#define EJ080NA_04B                            //Innolux EJ080NA-04B 1024X768 8.0" TFT-LCD
//#define EJ080NA_04C                            //Innolux EJ080NA-04C 1024X768 8.0" TFT-LCD
//#define EJ080NA_05B                            //Innolux EJ080NA-05B 800x600 8.0" TFT-LCD
//#define NJ070NA-23A                            //Innolux NJ070NA-23A 1024X600 7" TFT-LCD
//#define ZJ070NA-01B                            //Innolux ZJ070NA-01B 1024x600
//#define AT070TN90                              //CMO AT070TN90 800x480 7.0" TFT-LCD 
//#define ET101000DM6                            //EDT ET101000DM6 1024x600 
//#define B116XW03_V0                            //AUO B116XW03 V0 1366x768
//#define G190SVT01                              //AUO G190SVT01 1680x342 
//#define G070VW01_V1                            //AUO G070VW01 V1 800X480 7" TFT-LCD 
//#define G070VW01_V0                            //AUO G070VW01 V0 800X480 7" TFT-LCD 
//#define LQ150X1LGN2C                           //SHARP LQ150X1LGN2C
//#define LQ190E1LW52                            //SHARP LQ190E1LW52 1280x1024 
//#define LQ121S1LG81                            //SHARP LQ121S1LG81
//#define LQ156M1LG21                            //SHARP LQ156M1LG21 1920x1080
//#define LQ201U1LW32                            //SHARP LQ201U1LW32 1600x1200
//#define LQ150X1LGN2C_LVDS1                     //SHARP LQ150X1LGN2C_LVDS1 1024x768
//#define LQ150X1LGN2C_LVDS2                     //SHARP LQ150X1LGN2C_LVDS2 1024x768

//Valores foi fornecido pela BuyDisplay
#ifdef EK9713                                  //Fitipower EK9713 800x600/800x480
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         20                  //HS Back Porch (Blanking)
  #define LCD_HFPD         160                 //HS Front Porch
  #define LCD_HSPW         5                   //HS Pulse Width
  #define LCD_VBPD         20                  //VS Back Porch (Blanking)
  #define LCD_VFPD         12                  //VS Front Porch
  #define LCD_VSPW         3                   //VS Pulse Width
#endif

#ifdef EJ080NA_05B                             //Innolux EJ080NA-05B 800x600 8.0" TFT-LCD
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           600                 //Vertical Height
  #define LCD_HBPD         38                  //HS Back Porch (Blanking) - 46
  #define LCD_HFPD         210                 //HS Front Porch - 16~354
  #define LCD_HSPW         8                   //HS Pulse Width - 1~40C
  #define LCD_VBPD         15                  //VS Back Porch (Blanking) - 23
  #define LCD_VFPD         12                  //VS Front Porch - 1~77
  #define LCD_VSPW         8                   //VS Pulse Width - 1~20C
#endif

#ifdef AT070TN92                               //Innolux AT070TN92 800X480 7"  TFT-LCD
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         30                  //HS Back Porch (Blanking) - 46
  #define LCD_HFPD         210                 //HS Front Porch - 16~354
  #define LCD_HSPW         16                  //HS Pulse Width - 1~40C
  #define LCD_VBPD         13                  //VS Back Porch (Blanking) - 23
  #define LCD_VFPD         22                  //VS Front Porch - 7~147
  #define LCD_VSPW         10                  //VS Pulse Width - 1~20
#endif


//Troca de pagina de registrador
enum class PageReg : uint8_t {
  Page0 = 0x00,                                //Page 0 Register Set
  Page1 = cSetb0,                              //Page 1 Register Set
}


//Tipo de seleção de destino da porta de memória do RA8889
enum class MemoryPortDest : uint8_t {          //bit 1-0
  SDRAM            = 0x0,
  GammaTable       = cSetb0,
  GraphicCursorRAM = cSetb1,
  ColorPaletteRAM  = cSetb0 | cSetb1
};

//Interface de saída do TFT com o RA8889
enum class TFTInterface : uint8_t {            //bit 4-3
  IF_24BIT = 0x0,                              //Saída TFT 24-bits 
  IF_18BIT = cSetb3,                           //Saída TFT 18-bits 
  IF_16BIT = cSetb4,                           //Saída TFT 16-bits         
  IF_NONE  = cSetb4 | cSetb3                   //Sem Saída TFT
};

//Host Read/Write Memory Direction
enum class MemoryDirection : uint8_t {
  LeftRight_TopBotom = 0b00,
  RightLeft_TopBotom = 0b01,
  TopBotom_LeftRight = 0b10,        
  BotomTop_LeftRight = 0b11
}

//Horizontal Scan Direction
enum class HSCANDir : uint8_t {                //bit 4
  LeftToRight        = 0x0,                    
  RightToLeft        = cSetb4                  
}

//Vertical Scan Direction
enum class VSCANDir : uint8_t {                //bit 3
  TopToBottom        = 0x0,                    
  BottomToTop        = cSetb3                  
}

//Parallel XDATA[23~0] Color Format
enum class PDATAColorFmt : uint8_t {          //bit [2~0]
  RGB               = 0x0,
  RBG               = 0x1,
  GRB               = 0x2,
  GBR               = 0x3,
  BRG               = 0x4,
  BGR               = 0x5,
  Gray              = 0x6,
  BW                = 0x7
}

//Panel Scan Clock PCLK Edge Type
enum class PCLKEdge : uint8_t {                //bit [7]
  Rising            = 0x0,                     //Borda de subida
  Falling           = cSetb7                   //Borda de descida
}

enum class HSYNCPolarity : uint8_t {           //bit [6]
  Low              = 0x0,
  High             = cSetb7
}

enum class VSYNCPolarity : uint8_t {           //bit [6]
  Low              = 0x0,
  High             = cSetb6
}

enum class DEPolarity : uint8_t {              //bit [5]
  High             = 0x0,
  Low              = cSetb5
}

enum class PIPSelect : uint8_t {              //bit [4]
  PIP1            = 0x00,                     //Picture-to-Picute (PIP 1) 
  PIP2            = cSetb4                    //Picture-to-Picute (PIP 2)
}


//Supported Panel Resolution (Horiz x Vert)
//Note:  The actual panel resolution depends on the pixel clock and color depth.
//When RA8889 TFT Output supports 24bpp 
//CCLK Max. = 120MHz 
//SCLK Max. = 60MHz 
//Required LCD clock ≒ LCD vertical Pixel * LCD horizontal Pixel * 60(Hz) * 1.1 
//If Required LCD Clock > SCLK, the LCD refresh rate (Refresh Rate or VSYNC rate) will be lower than 
//60Hz under this application condition.
enum class PanelResolution : uint8_t {
  r320x240,                       //QVGA:  320 x 240 x 16/18/24-bit LCD panel 
  r480x272,                       //WQVGA: 480 x 272 x 16/18/24-bit LCD panel
  r640x480,                       //VGA:   640 x 480 x 16/18/24-bit LCD panel 
  r800x480,                       //WVGA:  800 x 480 x 16/18/24-bit LCD panel   
  r800x600,                       //SVGA:  800 x 600 x 16/18/24-bit LCD panel
  r960x540,                       //QHD:   960 x 540 x 16/18/24-bit LCD panel
  r1024x600,                      //WSVGA: 1024 x 600 x 16/18/24-bit LCD panel
  r1024x768,                      //XGA:   1024 x 768 x 16/18/24-bit LCD panel 
  r1280x768,                      //WXGA:  1280 x 768 x 16/18/24-bit LCD panel
  r1280x800,                      //WXGA:  1280 x 800 x 16/18/24-bit LCD panel 
  r1366x768                       //WXGA:  1366 x 768 x 16/18/24-bit LCD panel
}

/*Page(image buffer) configure*/
/*The maximum number of pages is based on SDRAM capacity and color depth and width and height of one page*/
/*For example we used 128Mbit SDRAM that capacity =  16Mbyte */
/*The SDRAM is divided into several image buffers and the maximum number of image buffers is limited by the 
memory size. For example : page_size = 800*600*2byte(16bpp) = 960000byte, maximum number = 16/0.96 = 16.6 */
/*vertical mulit page application*/
#define MAX_LAYER 8     //numero maxim o de layer para endereçar

//se tornar obsoleto
#define LAYER1_START_ADDR  800*480*2*0
#define LAYER2_START_ADDR  800*480*2*1
#define LAYER3_START_ADDR  800*480*2*2
#define LAYER4_START_ADDR  800*480*2*3
#define LAYER5_START_ADDR  800*480*2*4
#define LAYER6_START_ADDR  800*480*2*5
#define LAYER7_START_ADDR  800*480*2*6
#define LAYER8_START_ADDR  800*480*2*7
#define LAYER9_START_ADDR  800*480*2*8
#define LAYER10_START_ADDR 800*480*2*9

#define COLOR_DEPH_8BPP   8
#define COLOR_DEPH_16BPP  16
#define COLOR_DEPH_24BPP  24

//minha nova impelemtnacao, colocar isso de forma ajustada no sistema apra selecao do COLOR DEPTH 8/16/24 bpp
enum class eColorDepthBPP : uint8_t {
  bpp8  = COLOR_DEPH_8BPP,       //Color Depth 8bpp
  bpp16 = COLOR_DEPH_16BPP,      //Color Depth 16bpp
  bpp24 = COLOR_DEPH_24BPP       //Color Depth 24bpp
}


enum class Color : uint32_t {
  clBlack  =  0x00000000,
  clWhite  =  0x0000ffff,
  clRed    =  0x0000f800,
  clGreen  =  0x000007e0,
  clBlue   =  0x0000001f,
  clYellow = clRed | clGreen,
  clCyan   =  clGreen | clBlue,
  clPurple = clRed | clBlue
}


enum class FontSource : uint8_t {
  InternalCGROM =  0x00                             //0b00 Select internal CGROM Character.
  ExternalCGROM =  cSetb6                           //0b01 Select external CGROM Character. (Genitop serial flash) 
  UserDefined   =  cSetb7                           //0b10 Select user-defined Character.
}


// Enum para os padrões ISO/IEC 8859 suportados pela CGROM interna
enum class InternalCGROM_ISO8859 : uint8_t {
    ISO8859_1 = 0x0,                           // 0b00: Character ISO/IEC 8859-1
    ISO8859_2 = cSetb0,                        // 0b01: Character ISO/IEC 8859-2.
    ISO8859_4 = cSetb1,                        // 0b10: Character ISO/IEC 8859-4.
    ISO8859_5 = cSetb1 | cSetb0                // 0b11: Character ISO/IEC 8859-5.
};


// Enum para alturas de fonte suportadas
enum class FontHeight : uint8_t
{
  H16 = 16,                                    // 8x16 / 16x16
  H24 = 24,                                    // 12x24 / 24x24
  H32 = 32                                     // 16x32 / 32x32
};


// Enum Font Horizontal/Vertical Enlagement Factors
enum class FontEnlargFactor : uint8_t
{
  X1 = 0,                           //factor 1x
  X2 = 1,                           //factor 2x
  X3 = 2,                           //factor 3x
  X4 = 3                            //factor 4x
}


enum class InterruptLevel : uint8_t {
  Low  = 0x0,                      //Reset bit 7
  High = cSetb7                    //Set bit 7
}


enum class eInterrupLevelTrigger : uint8_t {
  Low      = 0b00                              //low level trigger    
  Falling  = 0b01                              //falling edge trigger
  High     = 0b10                              //high level trigger
  rising   = 0b11                              //rising edge trigger
}


enum class eNSS_Channel : uint8_t {
  XNSFCS0 = 0,                   
  XNSFCS1 = 1,                 
  XNSFCS2 = 2,               
  XNSFCS3 = 3                
};


enum class eDividerClock : uint8_t {
  X1 = 0b00,                                   //divided by 1, 1/1 
  X2 = 0b01,                                   //divided by 2, 1/2  
  X4 = 0b10,                                   //divided by 4, 1/4
  X8 = 0b11                                    //divided by 8, 1/8
};


//Uso com BTE Operation Code
enum class eBTEOpCode : uint8_t {
  MPU_WROP            = 0b0000,      //MPU Write BTE with ROP.
  MPU_RNOP            = 0b0001,      //MPU Read BTE without ROP.
  MEM_COPY_POS_ROP    = 0b0010,      //Memory copy (move) BTE in positive direction with ROP.
  MEM_COPY_NEG_ROP    = 0b0011,      //Memory copy (move) BTE in negative direction with ROP.
  MPU_TWRITE          = 0b0100,      //MPU Transparent Write BTE. (w/o ROP.)
  MPU_T_COPY_POS      = 0b0101,      //Transparent Memory copy (move) BTE in positive direction (w/o ROP.)
  PAT_ROP             = 0b0110,      //Pattern Fill with ROP.
  PAT_CHROMA          = 0b0111,      //Pattern Fill with key-chroma
  COLOR_EXP           = 0b1000,      //Color Expansion
  COLOR_EXP_T         = 0b1001,      //Color Expansion with transparency
  MOVE_POS_ALPHA      = 0b1010,      //Move BTE in positive direction with Alpha blending
  MPU_WALPHA          = 0b1011,      //MPU Write BTE with Alpha blending
  SOLID_FILL          = 0b1100       //Solid Fill
}


/**
 * @brief BTE ROP (Raster Operation) Codes for RA8889
 *
 * These codes define how the source(s) (S0, S1) are combined to produce
 * the destination (D). They are used with BTE operations that support ROP.
 */
enum class eBTEROPCode : uint8_t {
    Blackness       = 0b0000,  // 0
    NotS0_And_NotS1 = 0b0001,  // ~S0・~S1  or  ~(S0 + S1)
    NotS0_And_S1    = 0b0010,  // ~S0・S1
    NotS0           = 0b0011,  // ~S0
    S0_And_NotS1    = 0b0100,  // S0・~S1
    NotS1           = 0b0101,  // ~S1
    S0_Xor_S1       = 0b0110,  // S0 ^ S1
    NotS0_Or_NotS1  = 0b0111,  // ~S0 + ~S1  or  ~(S0・S1)
    S0_And_S1       = 0b1000,  // S0・S1
    Not_S0_Xor_S1   = 0b1001,  // ~(S0 ^ S1)
    S1              = 0b1010,  // S1
    NotS0_Or_S1     = 0b1011,  // ~S0 + S1
    S0              = 0b1100,  // S0
    S0_Or_NotS1     = 0b1101,  // S0 + ~S1
    S0_Or_S1        = 0b1110,  // S0 + S1
    Whiteness       = 0b1111   // 1
};


class Panel_RA8889 {
	
	public
		Panel_RA8889(uint8_t cs, uint8_t rst);
		uint8_t init();
		void DisplayOn(bool on);
		void GraphicMode(void);     //overload
		bool GraphicMode(void);     //overload
		void TextMode(void);
		uint16_t Width(void);
		uint16_t Height(void);
		
		unsigned long LayerStartAddr(uint8_t layer)
		void MainImage_StartAddress(unsigned long addr);
		void MainImage_Width(uint16_t wx);
		void MainWindow_StartXY(uint16_t wx, uint16_t hy);
		void CanvasImage_StartAddr(unsigned long addr);
		void CanvasImage_Width(uint16_t wx);
		void ActiveWindow_XY(uint16_t wx, uint16_t hy);
		void ActiveWindow_WidhtHeight(uint16_t wx, uint16_t hy); 
		
		void ForegroundColor_RGB(uint8_t red, uint8_t green, uint8_t blue);
		void ForegroundColor_256(uint8_t color);
		void ForegroundColor_65k(uint16_t color);
		void ForegroundColor_16M(uint32_t color);
		void BackgroundColor_RGB(uint8_t red, uint8_t green, uint8_t blue);
		void BackgroundColor_256(uint8_t color);
		void BackgroundColor_65k(uint16_t color);
		void BackgroundColor_16M(uint32_t color);
		
		void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
		void DrawSquare (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t forecolor, bfill = false);
		void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint32_t forecolor, bool bfill = false);
        void DrawCircle (uint16_t x1, uint16_t y1, uint16_t R, uint32_t forecolor, bfill = false);
		void DrawEllipse (uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
        void DrawCurveLeftUp(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
        void DrawCurveRightDown(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
        void DrawCurveRightUp(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
		void DrawCurveLeftDown(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
		void DrawCircleSquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
		
		void DrawPicture(uint16_t Wx, uint16_t Hy, uint16_t width, uint16_t height, const uint8_t *datap);
		
	private
		uint8_t StatusRead(void);
        void RegisterWrite(uint8_t Cmd, uint8_t Data);
	    uint8_t RegisterRead(uint8_t Cmd);
		void HardwareReset(void);
        void CoreTask_WaitReady(void);
		void Draw_WaitReady(void);
		bool IC_WaitReady(void);
		void Wait_WriteFIFO_NotFull(void);
		void Wait_WriteFIFO_Empty(void);
		void Wait_ReadFIFO_NotFull(void);
		void Wait_ReadFIFO_NotEmpty(void);
		void GotoLinearAddr(uint32_t addr);
		void GotoPixel_Linear(uint32_t addr);
		void GotoPixel_XY(uint16_t Wx, uint16_t Hy);
		void GotoText_XY(uint16_t Wx, uint16_t Hy);
		void ShowPicture(uint32_t size, const uint8_t *datap)
		
	protected
		uint8_t _cs;	       //chip select pin
		uint8_t _rst;	       //chip reset pin
		uint16_t _width;       //lardura do display
		uint16_t _height;      //altura do display
		uint8_t _bpp           //color depht 8/16/24 bit per pixel (bpp)
		uint8_t _colorfmt      //formato da cor RGB, RBG, GRB, GBR, ....
		
		void PanelResolution(PanelResolution resolution);
		void PageSwitch(PageReg pr);
		
		void SPISetCS(bool active);
		uint8_t SPIRwByte(uint8_t value);
		void SPI_CmdWrite(uint8_t cmd);
	    void SPI_DataWrite(uint8_t data);
		void SPI_DataWrite_Pixel(uint16_t data);
		uint8_t SPI_DataRead(void);
        void SPI_Init();
		
		void PLL_WaitReady(void);
		void PLL_ConfigClocks(void);		
		bool SDRAM_WaitReady(void);
		void SDRAM_Init(void);
		
		void MemorySelect_SDRAM(void);
		void MemorySelect_GammaTable(void);
		void MemorySelect_GraphicCursorRAM(void);
		void MemorySelect_ColorPaletteRAM(void);
		void MemoryPort_Select(MemoryPortDest dest);
		
		void Interrupt_Resume_Enable(bool b);
		void ExtInterrupt_Input_Enable(bool b);
		void Interrupt_I2CM_Enable(bool b);
		void Interrupt_VSync_Enable(bool b);
		void Interrupt_KeyScan_Enable(bool b);
		void Interrupt_ClearMultiEventTask_Enable(bool b);
		void Interrupt_PWM1_Enable(bool b);
		void Interrupt_PWM0_Enable(bool b);
		uint8_t Interrupt_Status(void);
		void VSYNC_WaitReady(void);
		void Interrupt_ClearResume_Flag(void);
		void ExtInterrupt_ClearInput_Flag(void);
		void Interrupt_ClearI2CM_Flag(void);
		void Interrupt_ClearVSync_Flag(void);
		void Interrupt_ClearKeyScan_Flag(void);
		bool Interrupt_IsKeyPressed(void);
		void Interrupt_ClearMultiEventTask_Flag(void);
		void Interrupt_ClearPWM0_Flag(void);
		void Interrupt_ClearPWM1_Flag(void);
		
		void Enable_PIP1(bool b);
		void Enable_PIP2(bool b);
		void Select_PIP_Parameter(PIPSelect pip);
		void Select_MainWindow_8bpp(void);
		void Select_MainWindow_16bpp(void);
		void Select_MainWindow_24bpp(void);
		void Select_LCD_SyncMode(void);
		void Select_LCD_DEMode(void);
		
		void TFT_24bit(void);
		void TFT_18bit(void);
		void TFT_16bit(void);
		void TFT_Without(void);
		void TFT_Interface(TFTInterface mode);
		
		void HostDataBus_Select_8bit(void);
		void HostDataBus_Select_16bit(void);
        void SFlashSPI_Enable(bool b);
		
        void HostColorDepthFormat(uint_t type);
		void HostReadMemoryDirection(MemoryDirection direction);
		void HostWriteMemoryDirection(MemoryDirection direction);
		
		void HScanDirection_LeftToRight(void);
		void HScanDirection_RightToLeft(void);
		void HorizontalScanDirection(HSCANDir direction);
		void VScanDirection_TopToBottom(void);
		void VScanDirection_BottomToTop(void);
		void VerticalScanDirection(VSCANDir direction);
		void PDATA_ColorFmt(PDATAColorFmt: fmt);
		void PCLK_Rising(void);
		void PCLK_Falling(void);
		void PCLK_EdgeType(PCLKEdge edge);

        void HSYNC_PolarityLow(void);
		void HSYNC_PolarityHigh(void);
		void HSYNC_Polarity(HSYNCPolarity val);
		void VSYNC_PolarityLow(void);
		void VSYNC_PolarityHigh(void);
		void VSYNC_Polarity(VSYNCPolarity val);
        void DE_PolarityLow(void);
        void DE_PolarityHigh(void);
        void DE_Polarity(DEPolarity val);
		
		void HorizontalWidth_VerticalHeight(uint16_t WX, uint16_t HY);
		void Horizontal_NonDisplay(uint16_t hbpd);
		void HSYNC_StartPosition(uint16_t hfpd);
		void HSYNC_PulseWidth(uint16_t hspw);
		void Vertical_NonDisplay(uint16_t vbpd);
		void VSYNC_StartPosition(uint16_t vfpd);
		void VSYNC_PulseWidth(uint8_t vspw);
		void LCD_SetPanel(void);
		
		void Memory_BlockMode(void);
		bool Memory_BlockMode(void);
		void Memory_XYMode(void);
		bool Memory_XYMode(void);
		void Memory_LinearMode(void);
		bool Memory_LinearMode(void);
		void Memory_8bpp_BlockMode(void);
		void Memory_16bpp_BlockMode(void);
		void Memory_24bpp_BlockMode(void);
		
		uint8_t SPIM_TxRxFIFOData_Get(void);
		uint8_t SPIM_TxRxFIFOData_Put(uint8_t data);
		bool SPIM_TxFIFO_Empty(void);
        bool SPIM_TxFIFO_Full(void);
		bool SPIM_RxFIFO_Empty(void);
		bool SPIM_RxFIFO_full(void);
		bool Interrupt_Overflow_Flag(void);
		void Interrupt_ClearOverflow_Flag(void);
		bool EMTI_Flag(void);
		void EMTI_Clear_Flag(void);
		
		void SPI_Clock_Period(uint8_t divisor);
		
		void Interrupt_ActiveLevel(InterruptLevel level);
		void ExtInterrupt_Debounce(void)
		void ExtInterrupt_NoDebounce(void)
		void ExtInterrupt_InputLevelTrigger(InterrupLevelTrigger leveltrg);
		void LVDS_DataFormat_VESA(void);
		void LVDS_DataFormat_JEIDA(void);
		
		void nSS_Select_Channel(NSS_Channel channel);
		void Interrupt_SPIM_Enable(bool b);
		void nSS_Inactive(void);
		void nSS_Active(void);
		void Interrupt_FIFOOverflow_Enable(bool b);
		void Interrupt_EMTIRQEN_Enable(bool b);
		void Reset_CPOL(void)
		void Set_CPOL(void)
		void Reset_CPHA(void)
		void Set_CPHA(void)/
		
		void DrawEnable_AA(bool b);             //Verificar se funcao do RA8876/RA8877
		void LineMode_Start(void);
        void TriangleMode_Start(bool fill);
        void CircleMode_Start(bool fill);
		void EllipseMode_Start(bool fill);
		void CurveLeftDownMode_Start(bool fill)
		void CurveLeftUpMode_Start(bool fill);
		void CurveRightUpMode_Start(bool fill);
		void CurveRightDownMode_Start(bool fill);
		void SquareMode_Start(bool fill);
		void CircleSquareMode_Start(bool fill);
		
        void Point1_XY(uint16_t wx, uint16_t hy);
		void Point2_XY(uint16_t wx, uint16_t hy);
		void Point3_XY(uint16_t wx, uint16_t hy);
		void Line_Point1XY(uint16_t wx, uint16_t hy);            //Mesmo que Point1_XY()
		void Line_Point2XY(uint16_t wx, uint16_t hy);            //Mesmo que Point2_XY()
		void Triangle_Point1XY(uint16_t wx, uint16_t hy);        //Mesmo que Point1_XY()
		void Triangle_Point2XY(uint16_t wx, uint16_t hy);        //Mesmo que Point2_XY()
		void Triangle_Point3XY(uint16_t wx, uint16_t hy);        //Mesmo que Point3_XY()
		void Square_Point1XY(uint16_t wx, uint16_t hy);          //Mesmo que Point1_XY()
		void Square_Point2XY(uint16_t wx, uint16_t hy);          //Mesmo que Point2_XY()
        void Radius_RxRy(uint16_t Rx, uint16_t Ry);
		void CircleRadius_R(uint16_t R);                         //Adaptado, mesmo que Radius_RxRy()
        void EllipseRadius_RxRy(uint16_t Rx, uint16_t Ry);       //Mesmo que Radius_RxRy()
        void CircleSquareRadius_RxRy(uint16_t Rx, uint16_t Ry);  //Mesmo que Radius_RxRy()
		void Center_XY(uint16_t Wx, uint16_t Hy);
		void CircleCenter_XY(uint16_t Wx, uint16_t Hy);          //Mesmo que Center_XY()
        void EllipseCenter_XY(uint16_t Wx, uint16_t Hy);         //Mesmo que Center_XY()

        void Font_UseUserDefined(void);
        void Font_UseInternalCGROM(void);
		void Font_UseExternalCGROM(void);
		void Font_SetSource(FontSource source);
        void Font_SetHeight_16(void);
		void Font_SetHeight_24(void);
		void Font_SetHeight_32(void);
		void Font_SetHeight(FontHeight height);
        void Select_Internal_CGROM_ISOIEC8859_1(void);
		void Select_Internal_CGROM_ISOIEC8859_2(void);
		void Select_Internal_CGROM_ISOIEC8859_4(void);
		void Select_Internal_CGROM_ISOIEC8859_5(void);
        void Select_Internal_CGROM_ISO8859(InternalCGROM_ISO8859 iso);
		void GTFont_Select_GT21L16T1W(void);
		void GTFont_Select_GT30L16U2W(void);
		void GTFont_Select_GT30L24T3Y(void);
		void GTFont_Select_GT30L24M1Z(void);
		void GTFont_Select_GT30L32S4W(void);
		void GTFont_Select_GT20L24F6Y(void);
		void GTFont_Select_GT21L24S1W(void);
		void GTFont_SetDecoder(uint8_t temp);
		void Font_LineDistance(uint8_t gap);
		void Font_toFontWidthSetting(uint8_t pixels);
		
		void Font_FullAlignmentEnable(void);
		void Font_FullAlignmentDisable(void);
        void Font_UseBackgroundTransparency(void);
        void Font_UseBackgroundColor(void);
        void Font_0degree(void);
		void Font_90degree(void);
		void Font_WidthEnlargFactor(FontEnlargFactor factor);
		void Font_HeightEnlargFactor(FontEnlargFactor factor);
		void CGRAM_StartAddress(uint32_t addr);
		
		void SFI_DMA_WaitReady(void);
		void SFI_DMA_Start(void);
		void Select_SFI_FontMode(void);
		void Select_SFI_DMAMode(void);
		void Select_SFI_24bitAddress(void);
		void Select_SFI_32bitAddress(void);
		
		void Select_SFI_SingleData_03h(void);
		void Select_SFI_SingleData_0Bh(void);
		void Select_SFI_SingleData_1Bh(void);
        void Select_SFI_DualData_3Bh(void);
		void Select_SFI_DualData_BBh(void);               //Only RA8876/RA8877
		void Select_SFI_QuadData_6Bh(void);
		void Select_SFI_QuadData_EBh(void);
		
		void SFI_Select_ROM0(void);
		void SFI_Select_ROM1(void);
		
		void IDEC_SPI_Select_StandardMode0orMode3(void);  //Only RA8876/RA8887
		void IDEC_RA8875_SPI_Select_Mode0andMode3(void);  //Only RA8876/RA8887
		void SFI_Select_WaveformMode0(void);              //Only RA8876/RA8887
		void Select_SFI_WaveformMode3(void);              //Only RA8876/RA8887
		
		void PWM_Prescaler(uint8_t prescaler);
        void PWM0_ClockDividedBy(DividerClock divider);
		void PWM1_ClockDividedBy(DividerClock divider);
		void PWM1_Select_ErrorFlag(void);
		void PWM1_Select(void);
		void PWM1_Select_OscillatorClock(void);
		void PWM0_Select_GPIOC7(void);
		void PWM0_Select(void);
		void PWM0_Select_CoreClock(void);
		
		void PWM1_InverterOn(boolean on);
		void PWM1_Select_AutoReload(void);
		void PWM1_Select_OneShot(void);
		void PWM1_StartTimer(void);
		void PWM1_StopTimer(void);
		void PWM0_DeadZoneEnable(bool b);
		void PWM0_InverterOn(bool on);
		void PWM0_Select_AutoReload(void);
		void PWM0_Select_OneShot(void);
		void PWM0_StartTimer(void);
		void PWM0_StopTimer(void);
        void PWM0_DeadZoneLength(uint8_t len);
		void PWM0_SetCompareBuffer(uint16_t Wx);
		void PWM0_SetCountBuffer(uint16_t Wx);
		void PWM1_SetCompareBuffer(uint16_t Wx);
		void PWM1_SetCountBuffer(uint16_t Wx);
		
		void SFI_SetDMASourceAddress(uint32_t addr);
		void SFI_SetDMADestinationAddress(uint32_t addr);
		void SFI_DMA_DestinationUpperLeftCorner(uint16_t Wx, uint16_t Hy);
		void SFI_DMA_TransferNumber(uint32_t addr);
		void SFI_DMA_TransferWidthHeight(uint16_t Wx, uint16_t Hy);
		void SFI_DMA_SourceWidth(uint16_t Wx);
				
		void Power_NormalMode(void);
		void Power_SavingStandbyMode(void);
		void Power_SavingSuspendMode(void);
		void Power_SavingSleepMode(void);
		void I2CM_ClockPrescale(uint16_t prescale);
		void I2CM_TransmitData(uint8_t data);
		uint8_t I2CM_Receiver_Data(void);
		void I2CM_SetFrequency(uint32_t xscl_hz);
		
		void BTE_WaitReady(void);
		void BTE_DualWaitReady(void);
		void BTE_PatternFormat8X8(void);
		void BTE_PatternFormat16X16(void);
		void BTE_Enable(bool b);
		void BTE_OperationCode(eBTEOpCode opcode);
		void BTE_S0_ColorDeph(eColorDepthBPP bpp)
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
