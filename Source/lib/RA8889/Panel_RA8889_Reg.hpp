//Conjunto de registradores de Page0 e Page1
//Para alternar o conjunto de registradores acessiveis entre Page0 e Page1 precisa escrever ou ler o registrador 0x46
//este mesmo registrador esta presente dentro do page 0 e page 1. o bit 0 deste registrador 0x46 é o mesmo da page 0 e page 1 no entanto são diferentes os bits 1-7
//Alguns bits tem funcionalidades especificas do registrador 0x46 por isso precisa ler este registrador 0x46 apra se saber qual pagina de registradores está sendo usado
//faça a leitura do registrador 0x46 para saber qual page está ativa e entao poder escrever adequadamente o registrador 0x46 se houver necessidade.
//Em duas paginas page 0 e page 1 podem ter o mesmo valor de registrador, mas com funcionalidade diferentes ou mesma funcionalidade.

//Status Register
//Deve envair ums solicitacao 0x40 via SPI/I2C
#define REG_STSR                               0x00            //Status Register (STSR)

//Chip Configuration Registers
#define REG_SRR                                0x00            //Software Reset Register (SRR)
#define REG_CCR                                0x01            //Chip Configuration Register (CCR)
#define REG_MACR                               0x02            //Memory Access Control Register (MACR)
#define REG_ICR                                0x03            //Input Control Register (ICR) 
#define REG_MRWDP                              0x04            //Memory Data Read/Write Port (MRWDP)
#define REG_PPLLC1                             0x05            //SCLK PLL Control Register 1 (PPLLC1)
#define REG_PPLLC2                             0x06            //SCLK PLL Control Register 2 (PPLLC2) 
#define REG_MPLLC1                             0x07            //MCLK PLL Control Register 1 (MPLLC1)
#define REG_MPLLC2                             0x08            //MCLK PLL Control Register 2 (MPLLC2)
#define REG_SPLLC1                             0x09            //CCLK PLL Control Register 1 (SPLLC1)
#define REG_SPLLC2                             0x0a            //CCLK PLL Control Register 2 (SPLLC2)

//Interrupt Control Registers
#define REG_INTEN                              0x0b            //Page 0 and 1 Interrupt Enable Register (INTEN)
#define REG_INTF                               0x0c            //Page 0 and 1 Interrupt Event Flag Register (INTF)
#define REG_MINTFR                             0x0d            //Page 0 and 1 Mask Interrupt Flag Register (MINTFR)
#define REG_PUENR                              0x0e            //Page 0 Pull- high control Register (PUENR)
#define REG_PSFSR                              0x0f            //Page 0 PDAT for PIO/Key Function Select Register (PSFSR)

//LCD Display Control Registers
#define REG_MPWCTR                             0x10            //Page 0 Main/PIP Window Control Register (MPWCTR)
#define REG_PIPCDEP                            0x11            //Page 0 PIP Window Color Depth Setting (PIPCDEP)
#define REG_DPCR                               0x12            //Page 0 Display Configuration Register (DPCR)
#define REG_PCSR                               0x13            //Page 0 Panel scan Clock and Data Setting Register (PCSR)
#define REG_HDWR                               0x14            //Page 0 Horizontal Display Width Register (HDWR)
#define REG_HDWFTR                             0x15            //Page 0 Horizontal Display Width Fine Tune Register (HDWFTR)
#define REG_HNDR                               0x16            //Page 0 Horizontal Non-Display Period Register (HNDR)
#define REG_HNDFTR                             0x17            //Page 0 Horizontal Non-Display Period Fine Tune Register (HNDFTR)
#define REG_HSTR                               0x18            //Page 0 HSYNC Start Position Register (HSTR)
#define REG_HPWR                               0x19            //Page 0 HSYNC Pulse Width Register (HPWR)
#define REG_VDHR0                              0x1a            //Page 0 Vertical Display Height Register 0(VDHR0)
#define REG_VDHR1                              0x1b            //Page 0 Vertical Display Height Register 1 (VDHR1)
#define REG_VNDR0                              0x1c            //Page 0 Vertical Non-Display Period Register 0(VNDR0)
#define REG_VNDR1                              0x1d            //Page 0 Vertical Non-Display Period Register 1(VNDR1)
#define REG_VSTR                               0x1e            //Page 0 VSYNC Start Position Register (VSTR)
#define REG_VPWR                               0x1f            //Page 0 VSYNC Pulse Width Register (VPWR)
#define REG_MISA0                              0x20            //Page 0 Main Image Start Address 0 (MISA0)
#define REG_MISA1                              0x21            //Page 0 Main Image Start Address 1 (MISA1)
#define REG_MISA2                              0x22            //Page 0 Main Image Start Address 2 (MISA2)
#define REG_MISA3                              0x23            //Page 0 Main Image Start Address 3 (MISA3)
#define REG_MIW0                               0x24            //Page 0 Main Image Width 0 (MIW0)
#define REG_MIW1                               0x25            //Page 0 Main Image Width 1 (MIW1)
#define REG_MWULX0                             0x26            //Page 0 Main Window Upper-Left corner X-coordinates 0 (MWULX0)
#define REG_MWULX1                             0x27            //Page 0 Main Window Upper-Left corner X-coordinates 1 (MWULX1)
#define REG_MWULY0                             0x28            //Page 0 Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
#define REG_MWULY1                             0x29            //Page 0 Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
#define REG_PWDULX0                            0x2a            //Page 0 PIP 1 or 2 Window Display Upper-Left corner X-coordinates 0 (PWDULX0)
#define REG_PWDULX1                            0x2b            //Page 0 PIP 1 or 2 Window Display Upper-Left corner X-coordinates 1 (PWDULX1)
#define REG_PWDULY0                            0x2c            //Page 0 PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 0 (PWDULY0)
#define REG_PWDULY1                            0x2d            //Page 0 PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 1 (PWDULY1)
#define REG_PISA0                              0x2e            //Page 0 PIP 1 or 2 Image Start Address 0 (PISA0)
#define REG_PISA1                              0x2f            //Page 0 PIP 1 or 2 Image Start Address 1 (PISA1)
#define REG_PISA2                              0x30            //Page 0 PIP 1 or 2 Image Start Address 2 (PISA2)
#define REG_PISA3                              0x31            //Page 0 PIP 1 or 2 Image Start Address 3 (PISA3)
#define REG_PIW0                               0x32            //Page 0 PIP 1 or 2 Image Width 0 (PIW0)
#define REG_PIW1                               0x33            //Page 0 PIP 1 or 2 Image Width 1 (PIW1)
#define REG_PWIULX0                            0x34            //Page 0 PIP 1 or 2 Window Image Upper-Left corner X-coordinates 0 (PWIULX0)
#define REG_PWIULX1                            0x35            //Page 0 PIP 1 or 2 Window Image Upper-Left corner X-coordinates 1 (PWIULX1)
#define REG_PWIULY0                            0x36            //Page 0 PIP 1 or 2 Window Image Upper-Left corner Y-coordinates (PWIULY0)
#define REG_PWIULY1                            0x37            //Page 0 PIP 1 or 2 Window Image Upper-Left corner Y-coordinates 1 (PWIULY1)
#define REG_PWW0                               0x38            //Page 0 PIP 1 or 2 Window Width 0 (PWW0)
#define REG_PWW1                               0x39            //Page 0 PIP 1 or 2 Window Width 1 (PWW1)
#define REG_PWH0                               0x3a            //Page 0 PIP 1 or 2 Window Height 0 (PWH0)
#define REG_PWH1                               0x3b            //Page 0 PIP 1 or 2 Windows Height 1 (PWH1)
#define REG_GTCCR                              0x3c            //Page 0 Graphic / Text Cursor Control Register (GTCCR)
#define REG_BTCR                               0x3d            //Page 0 Blink Time Control Register (BTCR)
#define REG_CURHS                              0x3e            //Page 0 Text Cursor Horizontal Size Register (CURHS)
#define REG_CURVS                              0x3f            //Page 0 Text Cursor Vertical Size Register (CURVS)
#define REG_GCHP0                              0x40            //Page 0 Graphic Cursor Horizontal Position Register 0 (GCHP0)
#define REG_GCHP1                              0x41            //Page 0 Graphic Cursor Horizontal Position Register 1 (GCHP1)
#define REG_GCVP0                              0x42            //Page 0 Graphic Cursor Vertical Position Register 0 (GCVP0)
#define REG_GCVP1                              0x43            //Page 0 Graphic Cursor Vertical Position Register 1 (GCVP1)
#define REG_GCC0                               0x44            //Page 0 Graphic Cursor Color 0 (GCC0)
#define REG_GCC1                               0x45            //Page 0 Graphic Cursor Color 1 (GCC1)

#define REG_PAGE_SWITCH                        0x46            //Page 0 / Page 1 PAGE Switch

//Geomatric Engine Control Registers
#define REG_CVSSA0                             0x50            //Page 0 Canvas Start address 0 (CVSSA0)
#define REG_CVSSA1                             0x51            //Page 0 Canvas Start address 1 (CVSSA1)
#define REG_CVSSA2                             0x52            //Page 0 Canvas Start address 2 (CVSSA2)
#define REG_CVSSA3                             0x53            //Page 0 Canvas Start address 3 (CVSSA3)
#define REG_CVS_IMWTH0                         0x54            //Page 0 Canvas image width 0 (CVS_IMWTH0)
#define REG_CVS_IMWTH1                         0x55            //Page 0 Canvas image width 1 (CVS_IMWTH1)
#define REG_AWUL_X0                            0x56            //Page 0 Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
#define REG_AWUL_X1                            0x57            //Page 0 Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
#define REG_AWUL_Y0                            0x58            //Page 0 Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
#define REG_AWUL_Y1                            0x59            //Page 0 Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
#define REG_AW_WTH0                            0x5a            //Page 0 Active Window Width 0 (AW_WTH0)
#define REG_AW_WTH1                            0x5b            //Page 0 Active Window Width 1 (AW_WTH1)
#define REG_AW_HT0                             0x5c            //Page 0 Active Window Height 0 (AW_HT0)
#define REG_AW_HT1                             0x5d            //Page 0 Active Window Height 1 (AW_HT1)
#define REG_AW_COLOR                           0x5e            //Page 0 Color Depth of Canvas & Active Window (AW_COLOR)
    #define BIT_CANVAS_BLOCK_MODE              0x00
    #define BIT_CANVAS_LINEAR_MODE             0x04
    #define BIT_CANVAS_COLOR_DEPTH_8BPP        0x00
    #define BIT_CANVAS_COLOR_DEPTH_16BPP       0x01
    #define BIT_CANVAS_COLOR_DEPTH_24BPP       0x02
#define REG_CURH0                              0x5f            //Page 0 Graphic Read/Write position Horizontal Position Register 0 (CURH0)
#define REG_CURH1                              0x60            //Page 0 Graphic Read/Write position Horizontal Position Register 1 (CURH1)
#define REG_CURV0                              0x61            //Page 0 Graphic Read/Write position Vertical Position Register 0 (CURV0)
#define REG_CURV1                              0x62            //Page 0 Graphic Read/Write position Vertical Position Register 1 (CURV1)
#define REG_F_CURX0                            0x63            //Page 0 Text Write X-coordinates Register 0 (F_CURX0)
#define REG_F_CURX1                            0x64            //Page 0 Text Write X-coordinates Register 1 (F_CURX1)
#define REG_F_CURY0                            0x65            //Page 0 Text Write Y-coordinates Register 0 (F_CURY0)
#define REG_F_CURY1                            0x66            //Page 0 Text Write Y-coordinates Register 1 (F_CURY1)
#define REG_DCR0                               0x67            //Page 0 Draw Line / Triangle Control Register 0 (DCR0)
#define REG_DLHSR0                             0x68            //Page 0 Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
#define REG_DLHSR1                             0x69            //Page 0 Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
#define REG_DLVSR0                             0x6a            //Page 0 Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
#define REG_DLVSR1                             0x6b            //Page 0 Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
#define REG_DLHER0                             0x6c            //Page 0 Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
#define REG_DLHER1                             0x6d            //Page 0 Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
#define REG_DLVER0                             0x6e            //Page 0 Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
#define REG_DLVER1                             0x6f            //Page 0 Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
#define REG_DTPH0                              0x70            //Page 0 Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
#define REG_DTPH1                              0x71            //Page 0 Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
#define REG_DTPV0                              0x72            //Page 0 Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
#define REG_DTPV1                              0x73            //Page 0 Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
#define REG_DCR1                               0x76            //Page 0 Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
#define REG_ELL_A0                             0x77            //Page 0 Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
#define REG_ELL_A1                             0x78            //Page 0 Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
#define REG_ELL_B0                             0x79            //Page 0 Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
#define REG_ELL_B1                             0x7a            //Page 0 Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
#define REG_DEHR0                              0x7b            //Page 0 Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
#define REG_DEHR1                              0x7c            //Page 0 Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
#define REG_DEVR0                              0x7d            //Page 0 Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
#define REG_DEVR1                              0x7e            //Page 0 Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)

//PWM Timer Control Registers
#define REG_PSCLR                              0x84            //Page 0 PWM Prescaler Register (PSCLR)
#define REG_PMUXR                              0x85            //Page 0 PWM clock Mux Register (PMUXR)
    #define BIT_PWM1_TIMER_DIV1                0x00            //clock divider’s MUX input for PWM Timer 1,  1
    #define BIT_PWM1_TIMER_DIV2                0x40            //clock divider’s MUX input for PWM Timer 1,  1/2
    #define BIT_PWM1_TIMER_DIV4                0x80            //clock divider’s MUX input for PWM Timer 1,  1/4
    #define BIT_PWM1_TIMER_DIV8                0xC0            //clock divider’s MUX input for PWM Timer 1,  1/8
    #define BIT_PWM0_TIMER_DIV1                0x00            //clock divider’s MUX input for PWM Timer 0,  1
    #define BIT_PWM0_TIMER_DIV2                0x10            //clock divider’s MUX input for PWM Timer 0,  1/2
    #define BIT_PWM0_TIMER_DIV4                0x20            //clock divider’s MUX input for PWM Timer 0,  1/4
    #define BIT_PWM0_TIMER_DIV8                0x30            //clock divider’s MUX input for PWM Timer 0,  1/8
    #define BIT_XPWM1_OUTPUT_ERROR_FLAG        0x00            //XPWM[1] output system error flag
    #define BIT_XPWM1_OUTPUT_PWM_TIMER1        0x08            //XPWM[1] output PWM timer 1 event or invert of PWM timer 0
    #define BIT_XPWM1_OUTPUT_OSC_CLK           0x0C            //XPWM[1] output oscillator clock
    #define BIT_XPWM0_GPIO_C7                  0x00            //XPWM[0] becomes GPIO-C[7]
    #define BIT_XPWM0_OUTPUT_PWM_TIMER0        0x02            //XPWM[0] output PWM timer 0
    #define BIT_XPWM0_OUTPUT_CORE_CLK          0x03            //XPWM[0] output core clock 
#define REG_PCFGR                              0x86            //Page 0 PWM Configuration Register (PCFGR)
#define REG_DZ_LENGTH                          0x87            //Page 0 Timer 0 Dead zone length register [DZ_LENGTH]
#define REG_TCMPB0L                            0x88            //Page 0 Timer 0 compare buffer register [TCMPB0L]
#define REG_TCMPB0H                            0x89            //Page 0 Timer 0 compare buffer register [TCMPB0H]
#define REG_TCNTB0L                            0x8a            //Page 0 Timer 0 count buffer register [TCNTB0L]
#define REG_TCNTB0H                            0x8b            //Page 0 Timer 0 count buffer register [TCNTB0H]
#define REG_TCMPB1L                            0x8c            //Page 0 Timer 1 compare buffer register [TCMPB1L]
#define REG_TCMPB1H                            0x8d            //Page 0 Timer 1 compare buffer register [TCMPB1H]
#define REG_TCNTB1L                            0x8e            //Page 0 Timer 1 count buffer register [TCNTB1L]
#define REG_TCNTB1H                            0x8f            //Page 0 Timer 1 count buffer register [TCNTB1H]

//Block Transfer Engine (BTE) Control Registers
#define REG_BTE_CTRL0                          0x90            //Page 0 BTE Function Control Register 0 (BTE_CTRL0)
    #define BIT_BTE_ENABLE                     0x10
#define REG_BTE_CTRL1                          0x91            //Page 0 BTE Function Control Register1 (BTE_CTRL1)
    #define BIT_BTE_MPU_WRITE_WITH_ROP         0x00
    #define BIT_BTE_MEMORY_COPY_WITH_ROP       0x02
    #define BIT_BTE_MPU_WRITE_WITH_CHROMA      0x04
    #define BIT_BTE_MEMORY_COPY_WITH_CHROMA    0x05
    #define BIT_BTE_PATTERN_FILL_WITH_ROP      0x06
    #define BIT_BTE_PATTERN_FILL_WITH_CHROMA   0x07  
    #define BIT_BTE_MPU_WRITE_COLOR_EXPANSION  0x08
    #define BIT_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA  0x09
    #define BIT_BTE_MEMORY_COPY_WITH_OPACITY   0xa0
    #define BIT_BTE_ROP_CODE_0                 0x00            //0 ( Blackness ) 
    #define BIT_BTE_ROP_CODE_1                 0x10            //~S0・~S1 or ~ ( S0+S1 ) 
    #define BIT_BTE_ROP_CODE_2                 0x20            //~S0・S1
    #define BIT_BTE_ROP_CODE_3                 0x30            //~S0
    #define BIT_BTE_ROP_CODE_4                 0x40            //S0・~S1
    #define BIT_BTE_ROP_CODE_5                 0x50            //~S1
    #define BIT_BTE_ROP_CODE_6                 0x60            //S0^S1
    #define BIT_BTE_ROP_CODE_7                 0x70            //~S0+~S1 or ~ ( S0・S1 ) 
    #define BIT_BTE_ROP_CODE_8                 0x80            //S0・S1
    #define BIT_BTE_ROP_CODE_9                 0x90            //~ ( S0^S1 ) 
    #define BIT_BTE_ROP_CODE_10                0xa0            //S1
    #define BIT_BTE_ROP_CODE_11                0xb0            //~S0+S1
    #define BIT_BTE_ROP_CODE_12                0xc0            //S0
    #define BIT_BTE_ROP_CODE_13                0xd0            //S0+~S1
    #define BIT_BTE_ROP_CODE_14                0xe0            //S0+S1
    #define BIT_BTE_ROP_CODE_15                0xf0            //1 ( Whiteness ) 
    #define BIT_BTE_ROP_BUS_WIDTH8             0x70
    #define BIT_BTE_ROP_BUS_WIDTH16            0xf0
#define REG_BTE_COLR                           0x92            //Page 0 Source 0/1 & Destination Color Depth (BTE_COLR)
    #define BIT_S0_COLOR_DEPTH_8BPP            0x00
    #define BIT_S0_COLOR_DEPTH_16BPP           0x20
    #define BIT_S0_COLOR_DEPTH_24BPP           0x40
    #define BIT_S1_COLOR_DEPTH_8BPP            0x00
    #define BIT_S1_COLOR_DEPTH_16BPP           0x04
    #define BIT_S1_COLOR_DEPTH_24BPP           0x08
    #define BIT_S1_CONSTANT_COLOR              0x0b
    #define BIT_S1_8BIT_PIXEL_ALPHA_BLENDING   0x10
    #define BIT_S1_16BIT_PIXEL_ALPHA_BLENDING  0x14
    #define BIT_S1_32BIT_ARGB_ALPHA_BLENDING   0x18
    #define BIT_DESTINATION_COLOR_DEPTH_8BPP   0x00
    #define BIT_DESTINATION_COLOR_DEPTH_16BPP  0x01
    #define BIT_DESTINATION_COLOR_DEPTH_24BPP  0x02
#define REG_S0_STR0                            0x93            //Page 0 Source 0 memory start address 0 (S0_STR0)
#define REG_S0_STR1                            0x94            //Page 0 Source 0 memory start address 1 (S0_STR1)
#define REG_S0_STR2                            0x95            //Page 0 Source 0 memory start address 2 (S0_STR2)
#define REG_S0_STR3                            0x96            //Page 0 Source 0 memory start address 3 (S0_STR3)
#define REG_S0_WTH0                            0x97            //Page 0 Source 0 image width 0 (S0_WTH0)
#define REG_S0_WTH1                            0x98            //Page 0 Source 0 image width 1 (S0_WTH1)
#define REG_S0_X0                              0x99            //Page 0 Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
#define REG_S0_X1                              0x9a            //Page 0 Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
#define REG_S0_Y0                              0x9b            //Page 0 Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
#define REG_S0_Y1                              0x9c            //Page 0 Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
#define REG_S1_STR0_RED                        0x9d            //Page 0 Source 1 memory start address 0 (S1_STR0)/S1 constant color – Red element (S1_RED)
#define REG_S1_STR1_GREEN                      0x9e            //Page 0 Source 1 memory start address 1 (S1_STR1)/S1 constant color – Green element (S1_GREEN)
#define REG_S1_STR2_BLUE                       0x9f            //Page 0 Source 1 memory start address 2 (S1_STR2)/S1 constant color – Blue element (S1_BLUE)
#define REG_S1_STR3                            0xa0            //Page 0 Source 1 memory start address 3 (S1_STR3)
#define REG_S1_WTH0                            0xa1            //Page 0 Source 1 image width 0 (S1_WTH0)
#define REG_S1_WTH1                            0xa2            //Page 0 Source 1 image width 1 (S1_WTH1)
#define REG_S1_X0                              0xa3            //Page 0 Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
#define REG_S1_X1                              0xa4            //Page 0 Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
#define REG_S1_Y0                              0xa5            //Page 0 Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
#define REG_S1_Y1                              0xa6            //Page 0 Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
#define REG_DT_STR0                            0xa7            //Page 0 Destination memory start address 0 (DT_STR0)
#define REG_DT_STR1                            0xa8            //Page 0 Destination memory start address 1 (DT_STR1)
#define REG_DT_STR2                            0xa9            //Page 0 Destination memory start address 2 (DT_STR2)
#define REG_DT_STR3                            0xaa            //Page 0 Destination memory start address 3 (DT_STR3)
#define REG_DT_WTH0                            0xab            //Page 0 Destination image width 0 (DT_WTH0)
#define REG_DT_WTH1                            0xac            //Page 0 Destination image width 1 (DT_WTH1)
#define REG_DT_X0                              0xad            //Page 0 Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
#define REG_DT_X1                              0xae            //Page 0 Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
#define REG_DT_Y0                              0xaf            //Page 0 Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
#define REG_DT_Y1                              0xb0            //Page 0 Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
#define REG_BTE_WTH0                           0xb1            //Page 0 BTE Window Width 0 (BTE_WTH0)
#define REG_BTE_WTH1                           0xb2            //Page 0 BTE Window Width 1 (BTE_WTH1)
#define REG_BTE_HIG0                           0xb3            //Page 0 BTE Window Height 0 (BTE_HIG0)
#define REG_BTE_HIG1                           0xb4            //Page 0 BTE Window Height 1 (BTE_HIG1)
#define REG_APB_CTRL                           0xb5            //Page 0 Alpha Blending (APB_CTRL)

//Serial Flash & SPI Master Control Registers
#define REG_DMA_CTRL                           0xb6            //Page 0 Serial flash DMA Controller REG (DMA_CTRL)
    #define BIT_DMA_START                      0x01
#define REG_SFL_CTRL                           0xb7            //Page 0 Serial Flash/ROM Controller Register (SFL_CTRL)
    #define BIT_SERIAL_FLASH_SELECT0           0x00            //Bit 7, Serial Flash/ROM 0 I/F is selected
    #define BIT_SERIAL_FLASH_SELECT1           0x80            //Bit 7, Serial Flash/ROM 1 I/F is selected.
    #define BIT_SERIAL_FLASH_SELECT2           0x00            //when page1 B7h bit 7 = 1
    #define BIT_SERIAL_FLASH_SELECT3           0x80            //when page1 B7h bit 7 = 1
    #define BIT_SERIAL_FLASH_FONT_MODE         0x00            //Bit 6
    #define BIT_SERIAL_FLASH_DMA_MODE          0x40            //Bit 6 
    #define BIT_SERIAL_FLASH_ADDR_24BIT        0x00            //Bit 5
    #define BIT_SERIAL_FLASH_ADDR_32BIT        0x20            //Bit 5
    #define BIT_STANDARD_SPI_MODE              0x00
    #define BIT_FOLLOW_RA8875_MODE             0x10            //Bit 4 
    #define BIT_SPI_NORMAL_READ                0x00            //Bit 3:0 COMMAND 03h
    #define BIT_SPI_FAST_READ_8DUMMY           0x04            //Bit 3:0 COMMAND 0Bh
    #define BIT_SPI_FAST_READ_16DUMMY          0x08            //Bit 3:0 COMMAND 1Bh
    #define BIT_SPI_DUAL_READ_8DUMMY           0x02            //Bit 3:0 COMMAND 3Bh
    #define BIT_2XIO_READ_MODE_4DUMMY          0x03            //Bit 3:0 COMMAND BBh    
#define REG_SPIDR                              0xb8            //Page 0 SPI master Tx /Rx FIFO Data Register (SPIDR)
#define REG_SPIMCR2                            0xb9            //Page 0 SPI master Control Register (SPIMCR2)
#define REG_SPIMSR                             0xba            //Page 0 SPI master Status Register (SPIMSR)
#define REG_SPI_DIVSOR                         0xbb            //Page 0 SPI Clock period (SPI_DIVSOR)
#define REG_DMA_SSTR0                          0xbc            //Page 0 Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
#define REG_DMA_SSTR1                          0xbd            //Page 0 Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
#define REG_DMA_SSTR2                          0xbe            //Page 0 Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
#define REG_DMA_SSTR3                          0xbf            //Page 0 Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
#define REG_DMA_DX0                            0xc0            //Page 0 DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
#define REG_DMA_DX1                            0xc1            //Page 0 DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
#define REG_DMA_DY0                            0xc2            //Page 0 DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
#define REG_DMA_DY1                            0xc3            //Page 0 DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
#define REG_SPIMBS                             0xc5            //Page 0 SPI Master Bus Select (SPIMBS)
    #define BIT_SPIM_SELECT_BUS0               0x00            //SPI master bus select BUS 0
    #define BIT_SPIM_SELECT_BUS1               0x80            //SPI master bus select BUS 1
    #define BIT_SPIM_RX_LATCH_RISING_EDGE      0x00            //SPI master rx register latch edge, cclk rising edge
    #define BIT_SPIM_RX_LATCH_FALLING_EDGE     0x20            //SPI master rx register latch edge, cclk falling edge, suggested
#define REG_DMAW_WTH0                          0xc6            //Page 0 DMA Block Width 0 (DMAW_WTH0)
#define REG_DMAW_WTH1                          0xc7            //Page 0 DMA Block Width 1 (DMAW_WTH1)
#define REG_DMAW_HIGH0                         0xc8            //Page 0 DMA Block Height 0 (DMAW_HIGH0)
#define REG_DMAW_HIGH1                         0xc9            //Page 0 DMA Block Height 1 (DMAW_HIGH1)
#define REG_DMA_SWTH0                          0xca            //Page 0 DMA Source Picture Width 0 (DMA_SWTH0)
#define REG_DMA_SWTH1                          0xcb            //Page 0 DMA Source Picture Width 0 (DMA_SWTH1)

//Text Engine
#define REG_CCR0                               0xcc            //Page 0 Character Control Register 0 (CCR0)
    #define BIT_SELECT_INTERNAL_CGROM          0x00
    #define BIT_SELECT_EXTERNAL_CGROM          0x40            //Genitop serial flash
    #define BIT_SELECT_USER_DEFINED            0x80
    #define BIT_CHAR_HEIGHT_16                 0x00            //16: ex. 8x16 / 16x16 / variable character width x 16
    #define BIT_CHAR_HEIGHT_24                 0x10            //24; ex. 12x24 / 24x24 / variable character width x 24
    #define BIT_CHAR_HEIGHT_32                 0x20            //32; ex. 16x32 / 32x32 / variable character width x 32
    #define BIT_SELECT_8859_1                  0x00            //ISO/IEC 8859-1
    #define BIT_SELECT_8859_2                  0x01            //ISO/IEC 8859-2
    #define BIT_SELECT_8859_4                  0x02            //ISO/IEC 8859-4
    #define BIT_SELECT_8859_5                  0x03            //ISO/IEC 8859-5
#define REG_CCR1                               0xcd            //Page 0 Character Control Register 1 (CCR1)
    #define BIT_TEXT_FULL_ALIGN_DISABLE        0x00            //
    #define BIT_TEXT_FULL_ALIGN_ENABLE         0x80            //
    #define BIT_TEXT_CHROMA_KEY_DISABLE        0x00            //Character’s background displayed with specified color
    #define BIT_TEXT_CHROMA_KEY_ENABLE         0x40            //Character’s background displayed with original canvas’ background (transparency)
    #define BIT_TEXT_NO_ROTATION               0x00
    #define BIT_TEXT_ROTATION                  0x10  
    #define BIT_TEXT_WIDTH_ENLARGEMENT_X1      0x00
    #define BIT_TEXT_WIDTH_ENLARGEMENT_X2      0x04
    #define BIT_TEXT_WIDTH_ENLARGEMENT_X3      0x08
    #define BIT_TEXT_WIDTH_ENLARGEMENT_X4      0x0c
    #define BIT_TEXT_HEIGHT_ENLARGEMENT_X1     0x00
    #define BIT_TEXT_HEIGHT_ENLARGEMENT_X2     0x01
    #define BIT_TEXT_HEIGHT_ENLARGEMENT_X3     0x02
    #define BIT_TEXT_HEIGHT_ENLARGEMENT_X4     0x03
#define REG_GTFNT_SEL                          0xce            //Page 0 GT Character ROM Select (GTFNT_SEL)
    #define BIT_CGROM_GT21L16T1W               0x00            //Integrate Circuit External CGROM GT21L16T1W
    #define BIT_CGROM_GT30L16U2W               0x20            //Integrate Circuit External CGROM GT30L16U2W
    #define BIT_CGROM_GT30L24T3Y               0x40            //Integrate Circuit External CGROM GT30L24T3Y
    #define BIT_CGROM_GT30L24M1Z               0x60            //Integrate Circuit External CGROM GT30L24M1Z
    #define BIT_CGROM_GT30L32S4W               0x80            //Integrate Circuit External CGROM GT30L32S4W
    #define BIT_CGROM_GT20L24F6Y               0xA0            //Integrate Circuit External CGROM GT20L24F6Y
    #define BIT_CGROM_GT21L24S1W               0xC0            //Integrate Circuit External CGROM GT21L24S1W
#define REG_GTFNT_CR                           0xcf            //Page 0 GT Character ROM Control register (GTFNT_CR)
    #define BIT_GB2312                         0x00            //bit 7-3, 
    #define BIT_GB12345                        0x08            //bit 7-3, 
	#define BIT_GB18030                        0x08            //bit 7-3, 
    #define BIT_BIG5                           0x10            //bit 7-3, 
    #define BIT_UNICODE                        0x18            //bit 7-3, 
    #define BIT_ASCII                          0x20            //bit 7-3, 
    #define BIT_UNI_JAPANESE                   0x28            //bit 7-3, 
    #define BIT_JIS0208                        0x30            //bit 7-3, 
    #define BIT_LGCATH                         0x38            //bit 7-3, Latin (L), Greek (G), Cyrillic (C), Arabic (A), Thai (T), Hebrew (H)
    #define BIT_ISO8859_1_ASCII                0x88            //bit 7-3, ISO-8859-1 and ASCII code
    #define BIT_ISO8859_2_ASCII                0x90            //bit 7-3, ISO-8859-2 and ASCII code
    #define BIT_ISO8859_3_ASCII                0x98            //bit 7-3, ISO-8859-3 and ASCII code
    #define BIT_ISO8859_4_ASCII                0xA0            //bit 7-3, ISO-8859-4 and ASCII code
    #define BIT_ISO8859_5_ASCII                0xA8            //bit 7-3, ISO-8859-5 and ASCII code
    #define BIT_ISO8859_7_ASCII                0xB0            //bit 7-3, ISO-8859-7 and ASCII code
    #define BIT_ISO8859_8_ASCII                0xB8            //bit 7-3, ISO-8859-8 and ASCII code
    #define BIT_ISO8859_9_ASCII                0xC0            //bit 7-3, ISO-8859-9 and ASCII code
    #define BIT_ISO8859_10_ASCII               0xC8            //bit 7-3, ISO-8859-10 and ASCII code
    #define BIT_ISO8859_11_ASCII               0xD0            //bit 7-3, ISO-8859-11 and ASCII code
    #define BIT_ISO8859_13_ASCII               0xD8            //bit 7-3, ISO-8859-13 and ASCII code
    #define BIT_ISO8859_14_ASCII               0xE0            //bit 7-3, ISO-8859-14 and ASCII code
    #define BIT_ISO8859_15_ASCII               0xE8            //bit 7-3, ISO-8859-15 and ASCII code
    #define BIT_ISO8859_16_ASCII               0xF0            //bit 7-3, ISO-8859-16 and ASCII code
    #define BIT_GT_FIXED_WIDTH                 0x00            //bit 1-0, Fixed width
    #define BIT_GT_VARIABLE_WIDTH_ARIAL        0x01            //bit 1-0, Variable width for Arial
    #define BIT_GT_VARIABLE_FIXED_WIDTH_ROMAN  0x02            //bit 1-0, Variable and fixed width for Roman
    #define BIT_GT_BOLD                        0x03            //bit 1-0, Fonte Bold
#define REG_FLDR                               0xd0            //Page 0 Character Line gap Setting Register (FLDR)
#define REG_F2FSSR                             0xd1            //Page 0 Character to Character Space Setting Register (F2FSSR)
#define REG_FGCR                               0xd2            //Page 0 Foreground Color Register - Red (FGCR)
#define REG_FGCG                               0xd3            //Page 0 Foreground Color Register - Green (FGCG)
#define REG_FGCB                               0xd4            //Page 0 Foreground Color Register - Blue (FGCB)
#define REG_BGCR                               0xd5            //Page 0 Background Color Register - Red (BGCR)
#define REG_BGCG                               0xd6            //Page 0 Background Color Register - Green (BGCG)
#define REG_BGCB                               0xd7            //Page 0 Background Color Register - Blue (BGCB)
#define REG_CGRAM_STR0                         0xdb            //Page 0 CGRAM Start Address 0 (CGRAM_STR0)
#define REG_CGRAM_STR1                         0xdc            //Page 0 CGRAM Start Address 1 (CGRAM_STR1)
#define REG_CGRAM_STR2                         0xdd            //Page 0 CGRAM Start Address 2 (CGRAM_STR2)
#define REG_CGRAM_STR3                         0xde            //Page 0 CGRAM Start Address 3 (CGRAM_STR3)

//Power Management Control Register
#define REG_PMU                                0xdf            //Page 0 Power Management register (PMU)

//SDRAM Control Register
#define REG_SDRAR                              0xe0            //Page 0 SDRAM attribute register (SDRAR)
#define REG_SDRMD                              0xe1            //Page 0 SDRAM mode register & extended mode register (SDRMD)
#define REG_SDR_REF_ITVL0                      0xe2            //Page 0 SDRAM auto refresh interval (SDR_REF_ITVL0)
#define REG_SDR_REF_ITVL1                      0xe3            //Page 0 SDRAM auto refresh interval (SDR_REF_ITVL1)
#define REG_SDRCR                              0xe4            //Page 0 SDRAM Control register (SDRCR)
#define REG_SDR_TP1                            0xe0            //Page 0 SDRAM timing parameter 1
#define REG_SDR_TP2                            0xe1            //Page 0 SDRAM timing parameter 2
#define REG_SDR_TP3                            0xe2            //Page 0 SDRAM timing parameter 3
#define REG_SDR_TP4                            0xe3            //Page 0 SDRAM timing parameter 4

//IIC Master Registers
#define REG_IICMCPR0                           0xe5            //Page 0 IIC Master Clock Pre-scale Register 0 (IICMCPR0)
#define REG_IICMCPR1                           0xe6            //Page 0 IIC Master Clock Pre-scale Register 1 (IICMCPR1)
#define REG_IICMTXR                            0xe7            //Page 0 IIC Master Transmit Register (IICMTXR)
#define REG_IICMRXR                            0xe8            //Page 0 IIC Master Receiver Register (IICMRXR)
#define REG_IICMCMDR                           0xe9            //Page 0 IIC Master Command Register (IICMCMDR)
#define REG_IICMSTUR                           0xea            //Page 0 IIC Master Status Register (IICMSTUR)

//GPI & GPO Register
#define REG_GPIOAD                             0xf0            //Page 0 GPIO-A direction (GPIOAD)
#define REG_GPIOA                              0xf1            //Page 0 GPIO-A (GPIOA)
#define REG_GPIOB                              0xf2            //Page 0 GPIO-B (GPIOB)
#define REG_GPIOCD                             0xf3            //Page 0 GPIO-C direction (GPIOCD)
#define REG_GPIOC                              0xf4            //Page 0 GPIO-C (GPIOC)
#define REG_GPIODD                             0xf5            //Page 0 GPIO-D direction (GPIODD)
#define REG_GPIOD                              0xf6            //Page 0 GPIO-D (GPIOD)
#define REG_GPIOED                             0xf7            //Page 0 GPIO-E direction (GPIOED)
#define REG_GPIOE                              0xf8            //Page 0 GPIO-E (GPIOE)
#define REG_GPIOFD                             0xf9            //Page 0 GPIO-F direction (GPIOFD)
#define REG_GPIOF                              0xfa            //Page 0 GPIO-F (GPIOF)

//Key-Scan Control Registers
#define REG_KSCR1                              0xfb            //Page 0 Key-Scan Control Register 1 (KSCR1)
#define REG_KSCR2                              0xfc            //Page 0 Key-Scan Controller Register 2 (KSCR2)
#define REG_KSDR0                              0xfd            //Page 0 Key-Scan Data Register (KSDR0)
#define REG_KSDR1                              0xfe            //Page 0 Key-Scan Data Register (KSDR1)
#define REG_KSDR2                              0xff            //Page 0 Key-Scan Data Register (KSDR2)

//Media Decoder Relative Registers
#define REG_AVI_SPIP_SADR0                     0x2e            //Page 1 AVI shadow pip start address 0 (avi_spip_sadr0)
#define REG_AVI_SPIP_SADR1                     0x2f            //Page 1 AVI shadow pip start address 1 (avi_spip_sadr1)
#define REG_AVI_SPIP_SADR2                     0x30            //Page 1 AVI shadow pip start address 2 (avi_spip_sadr2)
#define REG_AVI_SPIP_SADR3                     0x31            //Page 1 AVI shadow pip start address 3 (avi_spip_sadr3)
#define REG_VC1                                0xa0            //Page 1 Video Control (VC) 1
#define REG_MIHH                               0xa1            //Page 1 Media Image Height High Byte (MIHH)
#define REG_MIHL                               0xa2            //Page 1 Media Image Height Low Byte (MIHL)
#define REG_MIWH                               0xa3            //Page 1 Media Image Width High Byte (MIWH)
#define REG_MIWL                               0xa4            //Page 1 Media Image Width Low Byte (MIWL)
#define REG_VFPB3                              0xa5            //Page 1 Video Frame Period Byte3 (VFPB3)
#define REG_VFPB2                              0xa6            //Page 1 Video Frame Period Byte2 (VFPB2)
#define REG_VFPB1                              0xa7            //Page 1 Video Frame Period Byte1 (VFPB1)
#define REG_VFPB0                              0xa8            //Page 1 Video Frame Period Byte0 (VFPB0)
#define REG_VC2                                0xa9            //Page 1 Video Control (VC) 2
#define REG_IDEC_CTRL0                         0xb6            //Page 1 Serial flash AVI/JPG/BMP (IDEC_CTRL)                                   
#define REG_IDEC_CTRL1                         0xb7            //Page 1 Serial flash AVI/JPG/BMP (IDEC_CTRL)
#define REG_IDEC_CLKDIV                        0xbb            //Page 1 IDEC Clock divide
    #define BIT_SPI_DIV2                       0x00
    #define BIT_SPI_DIV4                       0x01
    #define BIT_SPI_DIV6                       0x02
    #define BIT_SPI_DIV8                       0x03
    #define BIT_SPI_DIV10                      0x04
#define REG_IDEC_SADR0                         0xbc            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 0 (IDEC_SADR0)
#define REG_IDEC_SADR1                         0xbd            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 1 (IDEC_SADR1)
#define REG_IDEC_SADR2                         0xbe            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 2 (IDEC_SADR2)
#define REG_IDEC_SADR3                         0xbf            //Page 1 Serial flash AVI/JPG/BMP Source Starting Address 3 (IDEC_SADR3)
#define REG_IDEC_DX0                           0xc0            //Page 1 IDEC (JPG/BMP)Destination Window Upper-Left corner X-coordinates 0 (IDEC_DX0)
#define REG_IDEC_DX1                           0xc1            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 1 (IDEC_DX1)
#define REG_IDEC_DY0                           0xc2            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 0 (IDEC_DY0)
#define REG_IDEC_DY1                           0xc3            //Page 1 IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 1 (IDEC_DY1)
#define REG_IDEC_PIP                           0xc5            //Page 1 IDEC AVI PIP controller (IDEC_PIP)
#define REG_IDEC_TF0                           0xc6            //Page 1 IDEC (AVI/JPG/BMP) transfer number 0 (IDEC_TF0)
#define REG_IDEC_TF1                           0xc7            //Page 1 IDEC (AVI/JPG/BMP) transfer number 1 (IDEC_TF1)
#define REG_IDEC_TF2                           0xc8            //Page 1 IDEC (AVI/JPG/BMP) transfer number 2 (IDEC_TF2)
#define REG_IDEC_TF3                           0xc9            //Page 1 IDEC (AVI/JPG/BMP) transfer number 3 (IDEC_TF3)
#define REG_IDEC_DADR0                         0xca            //Page 1 IDEC (JPG/BMP) Destination memory start addr 0 (IDEC_DADR0)
#define REG_IDEC_DADR1                         0xcb            //Page 1 IDEC (JPG/BMP) Destination memory start addr 1 (IDEC_DADR1)
#define REG_IDEC_DADR2                         0xcc            //Page 1 IDEC (JPG/BMP) Destination memory start addr 2 (IDEC_DADR2)
#define REG_IDEC_DADR3                         0xcd            //Page 1 IDEC (JPG/BMP) Destination memory start addr 3 (IDEC_DADR3)
#define REG_IDEC_DWTH0                         0xce            //Page 1 IDEC (JPG/BMP) Destination Image Width 0 (IDEC_DWTH0)
#define REG_IDEC_DWTH1                         0xcf            //Page 1 IDEC (JPG/BMP) Destination Image Width 1 (IDEC_DWTH1)
#define REG_AVI_PAUSE                          0xd3            //Page 1 AVI pause
#define REG_AVI_STOP                           0xd4            //Page 1 AVI stop                        

//SPI
#define RA8889_SPI_CMDWRITE                    0x00            //Write Command for SPI
#define RA8889_SPI_DATAWRITE                   0x80            //Write Data for SPI
#define RA8889_SPI_DATAREAD                    0xc0            //Read Data from SPI 
#define RA8889_SPI_STATUSREAD                  0x40            //Read Status from SPI
