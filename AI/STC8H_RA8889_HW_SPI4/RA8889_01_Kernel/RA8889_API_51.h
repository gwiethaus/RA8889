 
 /************** RAIO Technology Inc. / Lcdvision Co., Ltd.***************
 * file			: RA8889_API_51.h
 * author		: BW.SU
 * version	: V1.2
 * date			: 2024/06/07
 * brief		:
************************* bbs.lcdvision.com.cn **************************/

#define nand_buff  6912000
#define shadow_buff 6912000


/* 1. 数据直接写显存 */
void MPU8_8bpp_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned char *data_array);
void MPU8_16bpp_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned char *data_array);
void MPU8_24bpp_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned char *data_array);
void MPU16_16bpp_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned short *data_array);
void MPU16_24bpp_Mode1_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned short *data_array);
void MPU16_24bpp_Mode2_Memory_Write (unsigned short x,unsigned short y,unsigned short w ,unsigned short h ,const unsigned short *data_array);


/* PIP画中画开窗口：可以开2个窗口 */	 
void PIP(unsigned char On_Off, unsigned char Select_PIP,unsigned long PAddr,unsigned short XP,unsigned short YP,unsigned long ImageWidth,unsigned short X_Dis,unsigned short Y_Dis,unsigned short X_W,unsigned short Y_H);


/* 显示文字：包含RA8889内建ASCII，外接集通字库的显示 */
void Print_Internal_Font_Hexvariable(unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,  unsigned int tmp2);
void Print_Internal_Font_Decimalvariable(unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,unsigned int tmp2);
void Print_Internal_Font_String(unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor, char tmp2[]);
void Print_BIG5String(unsigned char Clk ,unsigned char BUS,unsigned char SCS,unsigned short x ,unsigned short y ,unsigned short X_W ,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,char *tmp2);
void Print_GB2312String(unsigned char Clk,unsigned char BUS,unsigned char SCS,unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,char *tmp2);
void Print_GB12345String(unsigned char Clk,unsigned char BUS,unsigned char SCS,unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,char tmp2[]);
void Print_UnicodeString(unsigned char Clk,unsigned char BUS,unsigned char SCS,unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,unsigned short *tmp2);
void Select_Font_Height_WxN_HxN_ChromaKey_Alignment(unsigned char Font_Height,unsigned char XxN,unsigned char YxN,unsigned char ChromaKey,unsigned char Alignment);
void Show_String(char *str);


/* 4. 绘图：各种几何图形（硬件实现，高速） */
void Draw_Line(unsigned long LineColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2);
void Draw_Triangle(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2,unsigned short X3,unsigned short Y3);
void Draw_Triangle_Fill(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2,unsigned short X3,unsigned short Y3);
void Draw_Circle(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short R);
void Draw_Circle_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short R);
void Draw_Ellipse(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Ellipse_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Left_Up_Curve(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Left_Up_Curve_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Right_Down_Curve(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Right_Down_Curve_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Right_Up_Curve(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Right_Up_Curve_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Left_Down_Curve(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Left_Down_Curve_Fill(unsigned long ForegroundColor,unsigned short XCenter,unsigned short YCenter,unsigned short X_R,unsigned short Y_R);
void Draw_Square(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2);
void Draw_Square_Fill(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2);
void Draw_Circle_Square(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2,unsigned short R,unsigned short Y_R);
void Draw_Circle_Square_Fill(unsigned long ForegroundColor,unsigned short X1,unsigned short Y1,unsigned short X2,unsigned short Y2,unsigned short R,unsigned short Y_R);


/* 内存操作：复制、运算、剔除底色、单色延展彩色、淡入淡出（硬件实现，快速） */
void BTE_Memory_Copy(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned int ROP_Code,unsigned short X_W,unsigned short Y_H);
void BTE_Memory_Copy_Chroma_key(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned long Background_color,unsigned short X_W,unsigned short Y_H);
void BTE_MCU_Write_MCU_8bit(unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned int ROP_Code,unsigned short X_W,unsigned short Y_H,const unsigned char *data_array);
void BTE_MCU_Write_MCU_16bit(unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned int ROP_Code,unsigned short X_W,unsigned short Y_H,const unsigned short *data_array);
void BTE_MCU_Write_Chroma_key_MCU_8bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned long Background_color,unsigned short X_W,unsigned short Y_H,const unsigned char *data_array);
void BTE_MCU_Write_Chroma_key_MCU_16bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned long Background_color,unsigned short X_W,unsigned short Y_H,const unsigned short *data_array);
void BTE_Memory_Copy_ColorExpansion(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,unsigned long Background_color);
void BTE_Memory_Copy_ColorExpansion_Chroma_key(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color);
void BTE_MCU_Write_ColorExpansion_MCU_8bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,unsigned long Background_color,const unsigned char *data_array);
void BTE_MCU_Write_ColorExpansion_MCU_16bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,unsigned long Background_color,const unsigned short *data_array);
void BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,const unsigned char *data_array);
void BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,const unsigned short *data_array);
void BTE_Pattern_Fill(unsigned char P_8x8_or_16x16, unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned int ROP_Code,unsigned short X_W,unsigned short Y_H);
void BTE_Pattern_Fill_With_Chroma_key(unsigned char P_8x8_or_16x16 ,unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned int ROP_Code,unsigned long Background_color,unsigned short X_W,unsigned short Y_H);
void BTE_Solid_Fill(unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned long Foreground_color,unsigned short X_W,unsigned short Y_H);
void BTE_Alpha_Blending_Picture_Mode(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned char alpha);


/* PWM输出：RA8889内建2个PWM发生器，可用于调整背光亮度等 */
void PWM0(unsigned char on_off, unsigned char Clock_Divided, unsigned char Prescalar, unsigned short Count_Buffer,unsigned short Compare_Buffer);
void PWM1(unsigned char on_off, unsigned char Clock_Divided, unsigned char Prescalar, unsigned short Count_Buffer,unsigned short Compare_Buffer);


/* 将数据按16进制打印出来 */
void Print_Hex(unsigned char buf);


/* MCU直接写点，基础函数 */
void putPixel(unsigned short x,unsigned short y,unsigned long color);


/* NAND FLASH功能相对复杂，暂不使用 */
//void SPI_NAND_initial_DMA (unsigned char SCS , unsigned char BUS);
//void SPI_NAND_initial_JPG_AVI (unsigned char BUS,unsigned char SCS,unsigned char SCK_Divide);
//void AVI_NAND(unsigned long addr,unsigned long videosize,unsigned long shadow_buffer_addr, unsigned long PIP_buffer_addr,unsigned long x,unsigned long y,unsigned long height,unsigned long width,unsigned long PIP_canvas_Width);
//void JPG_NAND (unsigned long addr,unsigned long JPGsize,unsigned long IDEC_canvas_width, unsigned short x,unsigned short y);
//void SPI_NAND_DMA_png (unsigned long dma_page_addr,unsigned long pic_buffer_Layer,unsigned long Show_pic_Layer,unsigned int picture_Width,unsigned int picture_Height);
//void SPI_NAND_DMA (unsigned long dma_page_addr,unsigned long X_coordinate,unsigned long Y_coordinate,unsigned long des_canvas_width,unsigned int picture_Width,unsigned int picture_Height,unsigned long pic_buffer_Layer,unsigned long Show_pic_Layer,unsigned char chorma,unsigned long Background_color);


/* DMA功能，快速显示图片（JPG，BMP），播放AVI影片 */
void DMA_24bit(unsigned char Clk ,unsigned short X1,unsigned short Y1,unsigned short X_W,unsigned short Y_H ,unsigned short P_W ,unsigned long Addr);
void DMA_32bit(unsigned char Clk ,unsigned short X1,unsigned short Y1,unsigned short X_W,unsigned short Y_H ,unsigned short P_W,unsigned long Addr);
void switch_24bits_to_32bits(unsigned char BUS,unsigned char SCS);

void SPI_NOR_Flash_switch_QUAD_MODE(unsigned char BUS,unsigned char SCS,unsigned char flash);
void SPI_NOR_initial_DMA (unsigned char mode,unsigned char BUS,unsigned char SCS,unsigned char flash,unsigned char addr_24b_32b);
void SPI_NOR_initial_JPG_AVI (unsigned char flash,unsigned char addr_24b_32b,unsigned char BUS,unsigned char SCS,unsigned char SCK_Divide);
void SPI_NOR_DMA_png (unsigned long dma_page_addr,unsigned long pic_buffer_Layer,unsigned long Show_pic_Layer,unsigned long picture_Width,unsigned long picture_Height);

void JPG_NOR (unsigned long addr,unsigned long JPGsize,unsigned long IDEC_canvas_width,unsigned short x,unsigned short y);
void AVI_NOR(unsigned long addr,unsigned long videosize,unsigned long shadow_buffer_addr,unsigned long PIP_buffer_addr,unsigned long x,unsigned long y,unsigned long height,unsigned long width,unsigned long PIP_width);

void BTE_alpha_blending_32bit_Pixel_mode(unsigned int picture_Width,unsigned int BTE_X,unsigned int BTE_Y,unsigned long S0X,unsigned long S0Y,unsigned long S0_Start_Addr,unsigned long S0_canvas_width,unsigned long desX,unsigned long desY,unsigned long DES_Start_Addr,unsigned long DES_canvas_width,unsigned long pic_buffer_Layer);
void BTE_alpha_blending_24bit_Pixel_mode(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H);

void AVI_window(unsigned char ON_OFF);
