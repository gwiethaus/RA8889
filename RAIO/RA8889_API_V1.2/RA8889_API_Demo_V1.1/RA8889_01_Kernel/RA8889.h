
  /*************** RAIO Technology Inc. ***************
  * product		: RA8889
  * file		: RA8889.h
  * author		: Henry 
  * version		: ver 0.0 
  * date		: 2020/03/30 
  RAIO Technology Inc. copyright all rights reserved.
  ****************************************************/	

#ifndef __RA8889_H
#define __RA8889_H


//8051
//void LCD_CmdWrite(unsigned char cmd);
//void LCD_DataWrite(unsigned char Data);
//unsigned char LCD_StatusRead(void);
//unsigned char LCD_DataRead(void);



//--
//**Start**//
//
void RA8889_HW_Reset(void);
//
void System_Check_Temp(void);
//Initial RA8889
void RA8889_Initial(void);
void Set_LCD_Panel(void);
void Set_Serial_Flash_IF(void);

//
void LCD_RegisterWrite(unsigned char Cmd,unsigned char Data);
unsigned char LCD_RegisterRead(unsigned char Cmd);

//

//**Staus**//
void Check_Mem_WR_FIFO_not_Full(void);
void Check_Mem_WR_FIFO_Empty(void);
void Check_Mem_RD_FIFO_not_Full(void);
void Check_Mem_RD_FIFO_Full(void);
void Check_Mem_RD_FIFO_not_Empty(void);
void Check_2D_Busy(void);
void Check_SDRAM_Ready(void);
unsigned char Power_Saving_Status(void);
void Check_IC_ready(void);//是 Normal mode 就離開
void Check_Power_is_Saving(void);//是 Saving mode 就離開
void Check_NO_Interrupt(void);//沒有 Interrupt 就離開
void Check_Interrupt_Occur(void);//有 Interrupt 就離開


//**[00h]**//
void RA8889_SW_Reset(void);
//**[01h]**//
void Enable_PLL(void);

void Sent_XnWAIT_Status_When_CS_Low(void);
void Sent_XnWAIT_Status_At_Any_Time(void);
void TFT_24bit(void);
void TFT_18bit(void);
void TFT_16bit(void);
void Without_TFT(void);
void Key_Scan_Enable(void);
void Key_Scan_Disable(void);
void RA8889_I2CM_Enable(void);
void RA8889_I2CM_Disable(void);
void Enable_SFlash_SPI(void);
void Disable_SFlash_SPI(void);
void Host_Bus_8bit(void);
void Host_Bus_16bit(void);
//**[02h]**//		 
void Data_Format_8b_8bpp(void);
void Data_Format_8b_16bpp(void);
void Data_Format_8b_24bpp(void);

void Data_Format_16b_8bpp(void);
void Data_Format_16b_16bpp(void);
void Data_Format_16b_24bpp_mode1(void);
void Data_Format_16b_24bpp_mode2(void);

void MemRead_Left_Right_Top_Down(void);
void MemRead_Right_Left_Top_Down(void);
void MemRead_Top_Down_Left_Right(void);
void MemRead_Down_Top_Left_Right(void);

void MemWrite_Left_Right_Top_Down(void);
void MemWrite_Right_Left_Top_Down(void);
void MemWrite_Top_Down_Left_Right(void);
void MemWrite_Down_Top_Left_Right(void);
//**[03h]**//
void Interrupt_Active_Low(void);
void Interrupt_Active_High(void);
void ExtInterrupt_Debounce(void);
void ExtInterrupt_Nodebounce(void);
void ExtInterrupt_Input_Low_Level_Trigger(void);
void ExtInterrupt_Input_High_Level_Trigger(void);
void ExtInterrupt_Input_Falling_Edge_Trigger(void);
void ExtInterrupt_Input_Rising_Edge_Trigger(void);
void LVDS_Format1(void);//RA8877 only
void LVDS_Format2(void);//RA8877 only
void Graphic_Mode(void);
void Text_Mode(void);
void Memory_Select_SDRAM(void);
void Memory_Select_Gamma_Table(void);
void Memory_Select_Graphic_Cursor_RAM(void);
void Memory_Select_Color_Palette_RAM(void);
//**[05h][06h][07h][08h][09h][0Ah]**//
//void RA8889_PLL (void);//Set PLL
void RA8889_PLL(unsigned short DRAM_clock, unsigned short CORE_clock, unsigned short SCAN_clock);
//**[0Bh]**//
void Enable_Resume_Interrupt(void);
void Disable_Resume_Interrupt(void);
void Enable_ExtInterrupt_Input(void);
void Disable_ExtInterrupt_Input(void);
void Enable_I2CM_Interrupt(void);
void Disable_I2CM_Interrupt(void);
void Enable_Vsync_Interrupt(void);
void Disable_Vsync_Interrupt(void);
void Enable_KeyScan_Interrupt(void);
void Disable_KeyScan_Interrupt(void);
void Enable_DMA_Draw_BTE_Interrupt(void);
void Disable_DMA_Draw_BTE_Interrupt(void);
void Enable_PWM1_Interrupt(void);
void Disable_PWM1_Interrupt(void);
void Enable_PWM0_Interrupt(void);
void Disable_PWM0_Interrupt(void);
//**[0Ch]**//
unsigned char Read_Interrupt_status(void);
void Check_Vsync_finished(void);
void Clear_Resume_Interrupt_Flag(void);
void Clear_ExtInterrupt_Input_Flag(void);
void Clear_I2CM_Interrupt_Flag(void);
void Clear_Vsync_Interrupt_Flag(void);
void Clear_KeyScan_Interrupt_Flag(void);
unsigned char Chk_Key_Press(void);
void Clear_DMA_Draw_BTE_Interrupt_Flag(void);
void Clear_PWM1_Interrupt_Flag(void);
void Clear_PWM0_Interrupt_Flag(void);
//**[0Dh]**//
void XnINTR_Mask_Resume_Interrupt_Flag(void);
void XnINTR_Mask_ExtInterrupt_Input_Flag(void);
void XnINTR_Mask_I2CM_Interrupt_Flag(void);
void XnINTR_Mask_Vsync_Interrupt_Flag(void);
void XnINTR_Mask_KeyScan_Interrupt_Flag(void);
void XnINTR_Mask_DMA_Draw_BTE_Interrupt_Flag(void);
void XnINTR_Mask_PWM1_Interrupt_Flag(void);
void XnINTR_Mask_PWM0_Interrupt_Flag(void);
//
void XnINTR_Unmask_Resume_Interrupt_Flag(void);
void XnINTR_Unmask_ExtInterrupt_Input_Flag(void);
void XnINTR_Unmask_I2CM_Interrupt_Flag(void);
void XnINTR_Unmask_Vsync_Interrupt_Flag(void);
void XnINTR_Unmask_KeyScan_Interrupt_Flag(void);
void XnINTR_Unmask_DMA_Draw_BTE_Interrupt_Flag(void);
void XnINTR_Unmask_PWM1_Interrupt_Flag(void);
void XnINTR_Unmask_PWM0_Interrupt_Flag(void);
//**[0Eh]**//
void Enable_GPIOF_PullUp(void);
void Enable_GPIOE_PullUp(void);
void Enable_GPIOD_PullUp(void);
void Enable_GPIOC_PullUp(void);
void Enable_XDB15_8_PullUp(void);
void Enable_XDB7_0_PullUp(void);
void Disable_GPIOF_PullUp(void);
void Disable_GPIOE_PullUp(void);
void Disable_GPIOD_PullUp(void);
void Disable_GPIOC_PullUp(void);
void Disable_XDB15_8_PullUp(void);
void Disable_XDB7_0_PullUp(void);

//**[0Fh]**//
void XPDAT18_Set_GPIO_D7(void);
void XPDAT18_Set_KOUT4(void);
void XPDAT17_Set_GPIO_D5(void);
void XPDAT17_Set_KOUT2(void);
void XPDAT16_Set_GPIO_D4(void);
void XPDAT16_Set_KOUT1(void);
void XPDAT9_Set_GPIO_D3(void);
void XPDAT9_Set_KOUT3(void);
void XPDAT8_Set_GPIO_D2(void);
void XPDAT8_Set_KIN3(void);
void XPDAT2_Set_GPIO_D6(void);
void XPDAT2_Set_KIN4(void);
void XPDAT1_Set_GPIO_D1(void);
void XPDAT1_Set_KIN2(void);
void XPDAT0_Set_GPIO_D0(void);
void XPDAT0_Set_KIN1(void);

//**[10h]**//
void Enable_PIP1(void);
void Disable_PIP1(void);
void Enable_PIP2(void);
void Disable_PIP2(void);
void Select_PIP1_Parameter(void);
void Select_PIP2_Parameter(void);
void Select_Main_Window_8bpp(void);
void Select_Main_Window_16bpp(void);
void Select_Main_Window_24bpp(void);
void Select_LCD_Sync_Mode(void);
void Select_LCD_DE_Mode(void);
//**[11h]**//
void Select_PIP1_Window_8bpp(void);
void Select_PIP1_Window_16bpp(void);
void Select_PIP1_Window_24bpp(void);
void Select_PIP2_Window_8bpp(void);
void Select_PIP2_Window_16bpp(void);
void Select_PIP2_Window_24bpp(void);
//**[12h]**//
void PCLK_Rising(void);
void PCLK_Falling(void);
void Display_ON(void);
void Display_OFF(void);
void Color_Bar_ON(void);
void Color_Bar_OFF(void);

void HSCAN_L_to_R(void);
void HSCAN_R_to_L(void);
void VSCAN_T_to_B(void);
void VSCAN_B_to_T(void);
void PDATA_Set_RGB(void);
void PDATA_Set_RBG(void);
void PDATA_Set_GRB(void);
void PDATA_Set_GBR(void);
void PDATA_Set_BRG(void);
void PDATA_Set_BGR(void);
void PDATA_Set_Gray(void);
void PDATA_IDLE_STATE(void);

//**[13h]**//
void HSYNC_Low_Active(void);
void HSYNC_High_Active(void);
void VSYNC_Low_Active(void);
void VSYNC_High_Active(void);
void DE_Low_Active(void);
void DE_High_Active(void);
void Idle_DE_Low(void);
void Idle_DE_High(void);
void Idle_PCLK_Low(void);
void Idle_PCLK_High(void);
void Idle_PDAT_Low(void);
void Idle_PDAT_High(void);
void Idle_HSYNC_Low(void);
void Idle_HSYNC_High(void);
void Idle_VSYNC_Low(void);
void Idle_VSYNC_High(void);
//**[14h][15h][1Ah][1Bh]**//
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY);
//**[16h][17h]**//
void LCD_Horizontal_Non_Display(unsigned short WX);
//**[18h]**//
void LCD_HSYNC_Start_Position(unsigned short WX);
//**[19h]**//
void LCD_HSYNC_Pulse_Width(unsigned short WX);
//**[1Ch][1Dh]**//
void LCD_Vertical_Non_Display(unsigned short HY);
//**[1Eh]**//
void LCD_VSYNC_Start_Position(unsigned short HY);
//**[1Fh]**//
void LCD_VSYNC_Pulse_Width(unsigned short HY);
//**[20h][21h][22h][23h]**//
void Main_Image_Start_Address(unsigned long Addr);
//**[24h][25h]**//					
void Main_Image_Width(unsigned short WX);							
//**[26h][27h][28h][29h]**//
void Main_Window_Start_XY(unsigned short WX,unsigned short HY);	
//**[2Ah][2Bh][2Ch][2Dh]**//
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY);
//**[2Eh][2Fh][30h][31h]**//
void PIP_Image_Start_Address(unsigned long Addr);
//**[32h][33h]**//
void PIP_Image_Width(unsigned short WX);
//**[34h][35h][36h][37h]**//
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY);
//**[38h][39h][3Ah][3Bh]**//
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY);
//**[3C]**//
void Enable_Gamma_Correction(void);
void Disable_Gamma_Correction(void);
void Gamma_Table_for_Blue(void);
void Gamma_Table_for_Green(void);
void Gamma_Table_for_Red(void);

void Enable_Graphic_Cursor(void);
void Disable_Graphic_Cursor(void);
void Select_Graphic_Cursor_1(void);
void Select_Graphic_Cursor_2(void);
void Select_Graphic_Cursor_3(void);
void Select_Graphic_Cursor_4(void);
void Enable_Text_Cursor(void);
void Disable_Text_Cursor(void);
void Enable_Text_Cursor_Blinking(void);
void Disable_Text_Cursor_Blinking(void);
//**[3D]**//
void Blinking_Time_Frames(unsigned char temp);
//**[3E][3Fh]**//
void Text_Cursor_H_V(unsigned short WX,unsigned short HY);
//**[40h][41h][42h][43h]**//
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY);
//**[44h]**//
void Set_Graphic_Cursor_Color_1(unsigned char temp);
//**[45h]**//
void Set_Graphic_Cursor_Color_2(unsigned char temp);




//**[50h][51h][52h][53h]**//
void Canvas_Image_Start_address(unsigned long Addr);
//**[54h][55h]**//
void Canvas_image_width(unsigned short WX);
//**[56h][57h][58h][59h]**//
void Active_Window_XY(unsigned short WX,unsigned short HY);
//**[5Ah][5Bh][5Ch][5Dh]**//
void Active_Window_WH(unsigned short WX,unsigned short HY);
//**[5E]**//
void Memory_XY_Mode(void);
void Memory_Linear_Mode(void);
void Memory_8bpp_Mode(void);
void Memory_16bpp_Mode(void);
void Memory_24bpp_Mode(void);
//**[5Fh][60h][61h][62h]**//
void Goto_Pixel_XY(unsigned short WX,unsigned short HY);
void Goto_Linear_Addr(unsigned long Addr);
//**[63h][64h][65h][66h]**//
void Goto_Text_XY(unsigned short WX,unsigned short HY);

////////////////////////////////////////////////////////////////////////
////**** [ Function : Draw ] ****////
//**[67h]**//
void Draw_Enable_AA(void);
void Draw_Disable_AA(void);
void Start_Line(void);
void Start_Triangle(void);
void Start_Triangle_Fill(void);
//**[68h]~[73h]**//
void Line_Start_XY(unsigned short WX,unsigned short HY);		//線起點
void Line_End_XY(unsigned short WX,unsigned short HY);			//線終點
void Triangle_Point1_XY(unsigned short WX,unsigned short HY);	//三角-點1
void Triangle_Point2_XY(unsigned short WX,unsigned short HY);	//三角-點2
void Triangle_Point3_XY (unsigned short WX,unsigned short HY);	//三角-點3
void Square_Start_XY(unsigned short WX,unsigned short HY);		//方起點
void Square_End_XY(unsigned short WX,unsigned short HY);		//方終點
//**[76h]**//
void Start_Circle_or_Ellipse(void);
void Start_Circle_or_Ellipse_Fill(void);
void Start_Left_Down_Curve(void);
void Start_Left_Up_Curve(void);
void Start_Right_Up_Curve(void);
void Start_Right_Down_Curve(void);
void Start_Left_Down_Curve_Fill(void);
void Start_Left_Up_Curve_Fill(void);
void Start_Right_Up_Curve_Fill(void);
void Start_Right_Down_Curve_Fill(void);
void Start_Square(void);
void Start_Square_Fill(void);
void Start_Circle_Square(void);
void Start_Circle_Square_Fill(void);
//**[77h]~[7Eh]**//
void Circle_Center_XY(unsigned short WX,unsigned short HY);				//圓中心
void Ellipse_Center_XY(unsigned short WX,unsigned short HY);			//橢圓中心
void Circle_Radius_R(unsigned short WX);								//圓半徑
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY);			//橢圓半徑
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY);	//方形轉角半徑


////////////////////////////////////////////////////////////////////////
////**** [ Function : PWM ] ****////
//**[84h]**//
void Set_PWM_Prescaler_1_to_256(unsigned short WX);
//**[85h]**//
void Select_PWM1_Clock_Divided_By_1(void);
void Select_PWM1_Clock_Divided_By_2(void);
void Select_PWM1_Clock_Divided_By_4(void);
void Select_PWM1_Clock_Divided_By_8(void);
void Select_PWM0_Clock_Divided_By_1(void);
void Select_PWM0_Clock_Divided_By_2(void);
void Select_PWM0_Clock_Divided_By_4(void);
void Select_PWM0_Clock_Divided_By_8(void);
//[85h].[bit3][bit2]
void Select_PWM1_is_ErrorFlag(void);
void Select_PWM1(void);
void Select_PWM1_is_Osc_Clock(void);
//[85h].[bit1][bit0]
void Select_PWM0_is_GPIO_C7(void);
void Select_PWM0(void);
void Select_PWM0_is_Core_Clock(void);
//**[86h]**//
//[86h]PWM1
void Enable_PWM1_Inverter(void);
void Disable_PWM1_Inverter(void);
void Auto_Reload_PWM1(void);
void One_Shot_PWM1(void);
void Start_PWM1(void);
void Stop_PWM1(void);
//[86h]PWM0
void Enable_PWM0_Dead_Zone(void);
void Disable_PWM0_Dead_Zone(void);
void Enable_PWM0_Inverter(void);
void Disable_PWM0_Inverter(void);
void Auto_Reload_PWM0(void);
void One_Shot_PWM0(void);
void Start_PWM0(void);
void Stop_PWM0(void);
//**[87h]**//
void Set_Timer0_Dead_Zone_Length(unsigned char temp);
//**[88h][89h]**//
void Set_Timer0_Compare_Buffer(unsigned short WX);
//**[8Ah][8Bh]**//
void Set_Timer0_Count_Buffer(unsigned short WX);
//**[8Ch][8Dh]**//
void Set_Timer1_Compare_Buffer(unsigned short WX);
//**[8Eh][8Fh]**//
void Set_Timer1_Count_Buffer(unsigned short WX);

////////////////////////////////////////////////////////////////////////
////**** [ Function : BTE ] ****////

//[90h]=========================================================================
void BTE_Enable(void);
void BTE_Disable(void);

void Check_BTE_Busy(void);

void Pattern_Format_8X8(void);
void Pattern_Format_16X16(void);

//[91h]=========================================================================
void BTE_ROP_Code(unsigned char setx);
void BTE_Operation_Code(unsigned char setx);

//[92h]=========================================================================
void BTE_S0_Color_8bpp(void);
void BTE_S0_Color_16bpp(void);
void BTE_S0_Color_24bpp(void);

void BTE_S1_Color_8bpp(void);
void BTE_S1_Color_16bpp(void);
void BTE_S1_Color_24bpp(void);
void BTE_S1_Color_Constant(void);
void BTE_S1_Color_8bit_Alpha(void);
void BTE_S1_Color_16bit_Alpha(void);
void BTE_S1_Color_32bit_Alpha(void);

void BTE_Destination_Color_8bpp(void);
void BTE_Destination_Color_16bpp(void);
void BTE_Destination_Color_24bpp(void);

//[93h][94h][95h][96h]=========================================================================
void BTE_S0_Memory_Start_Address(unsigned long Addr);	

//[97h][98h]=========================================================================
void BTE_S0_Image_Width(unsigned short WX);	

//[99h][9Ah][9Bh][9Ch]=========================================================================
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[9Dh][9Eh][9Fh][A0h]=========================================================================
void BTE_S1_Memory_Start_Address(unsigned long Addr);
void S1_Constant_color_256(unsigned char temp);
void S1_Constant_color_65k(unsigned short temp);
void S1_Constant_color_16M(unsigned long temp);	

//[A1h][A2h]=========================================================================
void BTE_S1_Image_Width(unsigned short WX);	

//[A3h][A4h][A5h][A6h]=========================================================================
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[A7h][A8h][A9h][AAh]=========================================================================
void BTE_Destination_Memory_Start_Address(unsigned long Addr);	

//[ABh][ACh]=========================================================================
void BTE_Destination_Image_Width(unsigned short WX);	

//[ADh][AEh][AFh][B0h]=========================================================================
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY);	

//[B1h][B2h][B3h][B4h]=========================================================================
void BTE_Window_Size(unsigned short WX, unsigned short HY);

//[B5h]=========================================================================
void BTE_Alpha_Blending_Effect(unsigned char temp);


//**[B5h]**//

////////////////////////////////////////////////////////////////////////
////**** [ Function : Serial Flash ] ****////


//REG[B6h] Serial flash DMA Controller REG (DMA_CTRL) 
void Select_SFI_Quad_Mode_Dummy_8T_6Bh(void);// 
void Select_SFI_Quad_Mode_Dummy_4T_EBh(void);// 


void Start_SFI_DMA(void);
void Check_Busy_SFI_DMA(void);

//REG[B7h] Serial Flash/ROM Controller Register (SFL_CTRL) 

void Select_SFI_0(void);// 
void Select_SFI_1(void);// 
void Select_SFI_2(void);// 
void Select_SFI_3(void);// 

void Select_SFI_Font_Mode(void);
void Select_SFI_DMA_Mode(void);

void Select_SFI_24bit_Address(void);
void Select_SFI_32bit_Address(void);

void Select_standard_SPI_Mode0_or_Mode3(void);
void Select_RA8875_SPI_Mode0_and_Mode3(void);

void Select_SFI_Single_Mode_Dummy_0T_03h(void);// 
void Select_SFI_Single_Mode_Dummy_8T_0Bh(void);// 
void Select_SFI_Single_Mode_Dummy_16T_1Bh(void);// 
void Select_SFI_Dual_Mode_Dummy_8T_3Bh(void);// 
void Select_SFI_Dual_Mode_Dummy_4T_BBh(void);// 




//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR)
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data);
unsigned char SPI_Master_FIFO_Data_Get(void);

//REG[B9h] SPI master Control Register (SPIMCR2)
void Select_nSS_drive_on_xnsfcs2(void);// 
void Select_nSS_drive_on_xnsfcs3(void);// 

void Enable_SPI_Master_Interrupt(void);// 
void Disable_SPI_Master_Interrupt(void);// 

void Select_nSS_drive_on_xnsfcs0(void);// 
void Select_nSS_drive_on_xnsfcs1(void);// 


void nSS_Inactive(void);
void nSS_Active(void);

void Mask_FIFO_overflow_error_Interrupt(void) ;
void Unmask_FIFO_overflow_error_Interrupt(void);

void Mask_EMTIRQEN_Interrupt(void);
void Unmask_EMTIRQEN_Interrupt(void);

void Reset_CPOL(void);
void Set_CPOL(void);
void Reset_CPHA(void);
void Set_CPHA(void);

//REG[BAh] SPI master Status Register (SPIMSR)
unsigned char Tx_FIFO_Empty_Flag(void);
unsigned char Tx_FIFO_Full_Flag(void);
unsigned char Rx_FIFO_Empty_Flag(void);
unsigned char Rx_FIFO_full_flag(void);
unsigned char OVFI_Flag(void);
void Clear_OVFI_Flag(void);
unsigned char EMTI_Flag(void);
void Clear_EMTI_Flag(void);

//REG[BB] SPI Clock period (SPIDIV) 
void SPI_Clock_Period(unsigned char temp);


//**[BCh][BDh][BEh][BFh]**//
void SFI_DMA_Source_Start_Address(unsigned long Addr);
//**[C0h][C1h][C2h][C3h]**//
void SFI_DMA_Destination_Start_Address(unsigned long Addr);
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY);
//**[C6h][C7h][C8h][C9h]**//
void SFI_DMA_Transfer_Number(unsigned long Num);
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY);
//**[CAh][CBh]**//
void SFI_DMA_Source_Width(unsigned short WX);

////////////////////////////////////////////////////////////////////////
////**** [ Function : Font ] ****////
//**[CCh]**//
void Font_Select_UserDefine_Mode(void);
void CGROM_Select_Internal_CGROM(void);
void CGROM_Select_Genitop_FontROM(void);

void Font_Select_8x16_16x16(void);
void Font_Select_12x24_24x24(void);
void Font_Select_16x32_32x32(void);

void Internal_CGROM_Select_ISOIEC8859_1(void);
void Internal_CGROM_Select_ISOIEC8859_2(void);
void Internal_CGROM_Select_ISOIEC8859_4(void);
void Internal_CGROM_Select_ISOIEC8859_5(void);
//**[CDh]**//
void Enable_Font_Alignment(void);
void Disable_Font_Alignment(void);
void Font_Background_select_Original_Canvas(void);
void Font_Background_select_Color(void);
void Font_0_degree(void);
void Font_90_degree(void);
void Font_Width_X1(void);
void Font_Width_X2(void);
void Font_Width_X3(void);
void Font_Width_X4(void);
void Font_Height_X1(void);
void Font_Height_X2(void);
void Font_Height_X3(void);
void Font_Height_X4(void);
//**[CEh]**//
void GTFont_Select_GT21L16T1W(void);
void GTFont_Select_GT30L16U2W(void);
void GTFont_Select_GT30L24T3Y(void);
void GTFont_Select_GT30L24M1Z(void);
void GTFont_Select_GT30L32S4W(void);
void GTFont_Select_GT20L24F6Y(void);
void GTFont_Select_GT21L24S1W(void);

//**[CFh]**//
void Set_GTFont_Decoder(unsigned char temp);
//**[D0h]**//
void Font_Line_Distance(unsigned char temp);
//**[D1h]**//
void Set_Font_to_Font_Width(unsigned char temp);
//**[D2h]~[D4h]**//
void Foreground_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Foreground_color_256(unsigned char temp);
void Foreground_color_65k(unsigned short temp);
void Foreground_color_16M(unsigned long temp);
//**[D5h]~[D7h]**//
void Background_RGB(unsigned char RED,unsigned char GREEN,unsigned char BLUE);
void Background_color_256(unsigned char temp);
void Background_color_65k(unsigned short temp);
void Background_color_16M(unsigned long temp);
//**[DBh]~[DEh]**//
void CGRAM_Start_address(unsigned long Addr);
//**[DFh]**//
void Power_Normal_Mode(void);
void Power_Saving_Standby_Mode(void);
void Power_Saving_Suspend_Mode(void);
void Power_Saving_Sleep_Mode(void);

////////////////////////////////////////////////////////////////////////
////**** [ Function : SDRAM ] ****////
//**[E0h][E1h][E2h][E3h][E4h]**//
void RA8889_SDRAM_initial(void);//Set SDRAM 



////////////////////////////////////////////////////////////////////////
////**** [ Function : I2C ] ****////
//**[E5h]~[EAh]**//
void RA8889_I2CM_Clock_Prescale(unsigned short WX);
//**[E7h]**//
void RA8889_I2CM_Transmit_Data(unsigned char temp);
//**[E8h]**//
unsigned char RA8889_I2CM_Receiver_Data(void);
//**[E9h]**//
void RA8889_I2CM_Write_With_Start(void);
void RA8889_I2CM_Stop(void); 
void RA8889_I2CM_Read_With_Ack(void);
void RA8889_I2CM_Read_With_Nack(void);
void RA8889_I2CM_Write(void);

 

//**[EAh]**//
unsigned char RA8889_I2CM_Check_Slave_ACK(void);
unsigned char RA8889_I2CM_Bus_Busy(void);
unsigned char RA8889_I2CM_transmit_Progress(void);
unsigned char RA8889_I2CM_Arbitration(void);



////////////////////////////////////////////////////////////////////////
////**** [ Function : GPIO ] ****////
//[F0h][F1h]
void Set_GPIO_A_In_Out(unsigned char temp);
void Write_GPIO_A_7_0(unsigned char temp);
unsigned char Read_GPIO_A_7_0(void);
//[F2h]
void Write_GPIO_B_7_4(unsigned char temp);
unsigned char Read_GPIO_B_7_0(void);
//[F3h][F4h]
void Set_GPIO_C_In_Out(unsigned char temp);
void Write_GPIO_C_7_0(unsigned char temp);
unsigned char Read_GPIO_C_7_0(void);
//[F5h][F6h]
void Set_GPIO_D_In_Out(unsigned char temp);
void Write_GPIO_D_7_0(unsigned char temp);
unsigned char Read_GPIO_D_7_0(void);
//[F7h][F8h]
void Set_GPIO_E_In_Out(unsigned char temp);
void Write_GPIO_E_7_0(unsigned char temp);
unsigned char Read_GPIO_E_7_0(void);
//[F9h][FAh]
void Set_GPIO_F_In_Out(unsigned char temp);
void Write_GPIO_F_7_0(unsigned char temp);
unsigned char Read_GPIO_F_7_0(void);


////////////////////////////////////////////////////////////////////////
////**** [ Function : Key ] ****////
//**[FBh]~[FFh]**//
//[FBh]
void Long_Key_enable(void);
void Key_Scan_Freg(unsigned char temp);	//set bit2~0 

//[FCh]
void Key_Scan_Wakeup_Function_Enable(void);
void Long_Key_Timing_Adjustment(unsigned char setx);//set bit5~3
unsigned char Numbers_of_Key_Hit(void);

//[FDh][FEh][FFh]
unsigned char Read_Key_Strobe_Data_0(void);
unsigned char Read_Key_Strobe_Data_1(void);
unsigned char Read_Key_Strobe_Data_2(void);










//[REG PAGE 1]
//[0Bh]=========================================================================
void Disable_IDEC_Interrupt(void);// 
void Enable_IDEC_Interrupt(void);// 

//[REG PAGE 1]
//[0Ch]=========================================================================
void Clear_IDEC_Interrupt(void);// 
//[REG PAGE 1]
//[0Dh]=========================================================================
void Unmask_IDEC_Interrupt(void);// 
void Mask_IDEC_Interrupt(void);// 




//**[46h]**//
void SPIM_Clock_Divided_2(void);//(default)// 
void SPIM_Clock_Divided_1(void);// 



void REG_Switch_to_REG0(void);//(default)// 
void REG_Switch_to_REG1(void);// 


//[REG PAGE 1]
//[2Eh]~[31h]=========================================================================
void AVI_Shadow_PIP_Start_Address(unsigned long Addr);// 


//[REG PAGE 1]
//[A0h] – Video Control (VC)=========================================================================
unsigned char MediaFile_ErrorFlag(void);// 
void Check_Busy_Media_Decode(void);// 
unsigned char Read_Media_Decode_Busy(void);
unsigned char Check_Media_Fifo_Flag(void);// 


//[REG PAGE 1]
//[D3h] – Video Control (VC)=========================================================================
void Press_AVI_Pause(void);// 
//[D4h] – Video Control (VC)=========================================================================
void Stop_AVI(void);// 




//[REG PAGE 1]
//[A1h]~[A8h]=========================================================================
unsigned short Read_Media_Height(void);// 
unsigned short Read_Media_Width(void);// 
unsigned long Read_Media_Data_Amount(void);// 

//[REG PAGE 1]
//[A9h]=========================================================================
void Reset_IDEC(void);// 


//[REG PAGE 1]
//[B6h]=========================================================================

void IDEC_Select_SFI_0(void);// 
void IDEC_Select_SFI_1(void);// 
void IDEC_Select_SFI_2(void);// 
void IDEC_Select_SFI_3(void);// 


void FONT_DMA_Select_Bus_0(void);//(default)// 
void FONT_DMA_Select_Bus_1(void);// 
void IDEC_Select_Bus_0(void);//(default)// 
void IDEC_Select_Bus_1(void);// 

void IDEC_Destination_Color_8bpp(void);// 
void IDEC_Destination_Color_16bpp(void);// 
void IDEC_Destination_Color_24bpp(void);// 

void IDEC_Starts_Decoding(void);// 
void Check_Busy_IDEC(void);// 
unsigned char Read_IDEC_Busy(void);

//[REG PAGE 1]
//[B7h]=========================================================================
//void IDEC_Select_SFI_0(void);// 
//void IDEC_Select_SFI_1(void);// 
//void IDEC_Select_SFI_2(void);// 
//void IDEC_Select_SFI_3(void);// 

void IDEC_Select_SFI_24bit_Address(void);// 
void IDEC_Select_SFI_32bit_Address(void);// 

void IDEC_Select_standard_SPI_Mode0_or_Mode3(void);// 
void IDEC_Select_RA8875_SPI_Mode0_and_Mode3(void);// 

void IDEC_Select_SFI_Single_Mode_Dummy_0T_03h(void);// 
void IDEC_Select_SFI_Single_Mode_Dummy_8T_0Bh(void);// 
void IDEC_Select_SFI_Single_Mode_Dummy_16T_1Bh(void);// 
void IDEC_Select_SFI_Dual_Mode_Dummy_8T_3Bh(void);// 
void IDEC_Select_SFI_Dual_Mode_Dummy_4T_BBh(void);// 
void IDEC_Select_SFI_Quad_Mode_Dummy_8T_6Bh(void);// 
void IDEC_Select_SFI_Quad_Mode_Dummy_4T_EBh(void);// 

//[REG PAGE 1]
//[BBh]=========================================================================
void IDEC_SPI_Clock_Divide(unsigned char IDEC_SPIclock_divide);// 

//[REG PAGE 1]
//[BCh]~[BFh]=========================================================================
void IDEC_Source_Start_Address(unsigned long Addr);//AVI,JPG,BMP	// 

//[REG PAGE 1]
//[C0h][C1h][C2h][C3h]=========================================================================
void IDEC_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY);// ,JPG,BMP	// 


//[REG PAGE 1]
//[C5h]=========================================================================
void IDEC_AVI_Decoding_to_PIP1_and_Shadow(void);// 
void IDEC_AVI_Decoding_to_PIP2_and_Shadow(void);// 
void IDEC_AVI_Decoding_to_PIP1_no_Shadow(void);// 

//[REG PAGE 1]
//[C6h][C7h][C8h][C9h]=========================================================================
void IDEC_Transfer_Number(unsigned long Num);//AVI,JPG,BMP	// 


//[REG PAGE 1]
//[CAh][CBh][CCh][CDh]=========================================================================
void IDEC_Destination_Start_Address(unsigned long Addr);// ,JPG,BMP	// 


//[REG PAGE 1]
//[CEh][CFh]=========================================================================
void IDEC_Destination_Image_Width(unsigned short WX);// ,JPG,BMP	// 


//[REG PAGE 0]
//[C5h]=========================================================================
void SPIM_Select_Bus_0(void);
void SPIM_Select_Bus_1(void);
void SPI_Master_LatchEdge_Select_Rising(void);
void SPI_Master_LatchEdge_Select_Falling(void);


#endif 
