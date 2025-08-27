/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2  Show AVI, JPEG ,PNG   
Direct Data Transfer from External Serial Flash to Frame Buffer
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test ok:  Arduino Due,Arduino UNO,Arduino MEGA2560
****************************************************/

#include <SPI.h>
#include <Wire.h>
#include "RA8889.h"

#define BUS0  0
#define BUS1  1
#define CS0  0
#define CS1  1
#define CS2  2
#define CS3  3


typedef struct _info
{
	unsigned short number;
	unsigned short img_width;
	unsigned short img_height;
	unsigned long img_size;
	unsigned long start_addr;
}INFO;





const INFO BINARY_INFO[11] =
{


  /*  No. , Width , Height , Size , Start Address  */ 
  {1,800,480,768000,0},          /*     BMP_1 , element 0     */
  {2,800,480,768000,768000},          /*     BMP_2 , element 1     */
  {3,800,480,768000,1536000},          /*     BMP_3 , element 2     */
  {4,800,480,113280,2304000},          /*     BMP_5_YUV420_q80 , element 3     */
  {5,800,480,62483,2417280},          /*     BMP_6_YUV420_q80 , element 4     */
  {6,800,480,103689,2479763},          /*     BMP_7_YUV420_q80 , element 5     */
  {7,800,480,34784,2583452},          /*     BMP_8_YUV420_q80 , element 6     */
  {8,800,480,74553,2618236},          /*     BMP_9_YUV420_q80 , element 7     */
  {9,400,100,160000,2692789},          /*     EastrisingPNG , element 8     */
  {10,600,250,5253784,2852789},          /*     AVI600250mjpeg , element 9     */



};



void setup() {
  
  pinMode(5,   OUTPUT);
  digitalWrite(5, HIGH);//Disable  SD 
  pinMode(2,   OUTPUT);
  digitalWrite(2, HIGH);//Disable  RTP    
  
  ER5517.Parallel_Init();
  ER5517.HW_Reset();
  ER5517.Check_IC_ready();
  delay(100);
  ER5517.initial();
  ER5517.Display_ON();

}
void loop() {
   unsigned int n,a; 
  
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 

  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  ER5517.Background_color_65k(White);
  ER5517.Foreground_color_65k(Red);
 ER5517.Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,0,0);			//设定文字格式，放大两倍
  ER5517.Goto_Text_XY(10,10);
  ER5517.Show_String("Showing picture of BMP format.");
  
  ER5517.Background_color_65k(Black);
  ER5517.Foreground_color_65k(Green);
  ER5517.Goto_Text_XY(10,60);
  ER5517.Show_String("DMA BMP: FLASH->DISPLAY RAM");
	delay(1000);

	ER5517.SPI_NOR_initial_DMA (3,BUS0,CS1,1,0);				//片选BUS0,CS1 （注意：初始化函数BMP和JPG的不同）

	//第1~3张是BMP图片
	for(n=0;n<3;n++)
	{		
		ER5517.DMA_24bit(1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,BINARY_INFO[n].img_width,BINARY_INFO[n].start_addr);
		delay(2000);  
	}


  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
	
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  ER5517.Background_color_65k(Black);
  ER5517.Foreground_color_65k(Green);
 ER5517.Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,0,0);			//设定文字格式，放大两倍
  ER5517.Goto_Text_XY(10,10);
  ER5517.Show_String("Showing picture of JPG format.");
  ER5517.Goto_Text_XY(10,60);
  ER5517.Show_String("DMA JPG: FLASH->DISPLAY RAM");
	delay(1000);
	
  ER5517.SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//片选BUS0,CS1
	
	//第4~8张是JPG图片
	for(n=3;n<8;n++)
	{
	  ER5517.JPG_NOR(BINARY_INFO[n].start_addr,BINARY_INFO[n].img_size,LCD_XSIZE_TFT,0,0);
		delay(2000);  
	}

  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT*2);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT*2);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
	
	 	
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  ER5517.Foreground_color_65k(Blue);
  ER5517.Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//设定文字格式，放大两倍
  ER5517.Goto_Text_XY(10,10);
  ER5517.Show_String("Fade in & fade out.");
	delay(1000);
	
	 ER5517.SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//片选BUS0,CS1
        
	//第4~8张是JPG图片，循环调出2张并进行过渡切换演示
	for(n=3;n<7;n++)
	{
		  ER5517.JPG_NOR(BINARY_INFO[n].start_addr,BINARY_INFO[n].img_size,LCD_XSIZE_TFT*2,0,LCD_YSIZE_TFT);
		  ER5517.JPG_NOR(BINARY_INFO[n+1].start_addr,BINARY_INFO[n+1].img_size,LCD_XSIZE_TFT*2,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		
		for(a=0;a<=32;a=a+2)
		{
			  ER5517.BTE_Alpha_Blending_Picture_Mode(0,LCD_XSIZE_TFT*2,0,LCD_YSIZE_TFT,0,LCD_XSIZE_TFT*2,LCD_XSIZE_TFT,LCD_YSIZE_TFT,0,LCD_XSIZE_TFT*2, 0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,a);
				delay(10);
		}
	}
	delay(1000);	

  unsigned char temp;
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
	
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  ER5517.Background_color_65k(Red);
  ER5517.Foreground_color_65k(Blue);
  ER5517.Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//设定文字格式，放大两倍
  ER5517.Goto_Text_XY(10,10);
  ER5517.Show_String("Playing AVI video.");

	  ER5517.SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//片选BUS0,CS1
	
 	  ER5517.AVI_NOR(BINARY_INFO[9].start_addr,BINARY_INFO[9].img_size,layer3_start_addr,layer4_start_addr,(LCD_XSIZE_TFT-600)/2,(LCD_YSIZE_TFT-250)/2,250,600,LCD_XSIZE_TFT);

	  ER5517.AVI_window(1);
	
	do{	
			temp =   ER5517.Read_Media_Decode_Busy();	//read busy flag to know media decode busy or idle
	}while(temp&0x40);
	  ER5517.AVI_window(0);	



    unsigned short x,y,dir_x,dir_y; 
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT*2);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT*2);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
	
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  ER5517.Background_color_65k(Red);
  ER5517.Foreground_color_65k(Yellow);
  ER5517.Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//设定文字格式，放大两倍
  ER5517.Goto_Text_XY(10,10);
	  ER5517.Show_String("Showing picture of PNG format.");
	delay(1000);

	  ER5517.SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//片选BUS0,CS1
	  ER5517.JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,LCD_XSIZE_TFT*2,0,0);			//Layer0 主画面
	  ER5517.JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,LCD_XSIZE_TFT*2,0,LCD_YSIZE_TFT);		//Layer1 主画面放在左侧
	  ER5517.JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,LCD_XSIZE_TFT*2,LCD_XSIZE_TFT,LCD_YSIZE_TFT);	//Layer1 主画面放在右侧

	  ER5517.SPI_NOR_initial_DMA (4,BUS0,CS1,1,0);				//片选BUS0,CS1 （注意：初始化函数BMP和JPG的不同）
	  ER5517.SPI_NOR_DMA_png (BINARY_INFO[8].start_addr,layer5_start_addr,0,BINARY_INFO[8].img_width,BINARY_INFO[8].img_height);		//将第8张，PNG图片数据先DMA放到Layer4缓存
	
	x=123;
	y=218;			
	dir_x=1;	//PNG移动方向：0:递减，1：递进
	dir_y=1;	//PNG移动方向：0:递减，1：递进
	
	for(n=0;n<300;n++)
	{
		//来源0=JPG背景图片   来源1=PNG数据
		  ER5517.BTE_alpha_blending_32bit_Pixel_mode(BINARY_INFO[8].img_width,BINARY_INFO[8].img_width,BINARY_INFO[8].img_height,x,y,layer3_start_addr,LCD_XSIZE_TFT*2,x,y,layer3_start_addr,LCD_XSIZE_TFT*2,layer5_start_addr);	//(S0X,S0Y)<--相同坐标-->(desX,desY)，PNG展开放在Layer1(x,y)
		  ER5517.BTE_Memory_Copy(layer3_start_addr,LCD_XSIZE_TFT*2,0,0,0,LCD_XSIZE_TFT*2,0,0,layer1_start_addr,LCD_XSIZE_TFT*2,0,0,0x0C,LCD_XSIZE_TFT,LCD_YSIZE_TFT);				//将Layer1(0,0) -> Layer0(0,0) 主画面叠加了PNG图片后copy
		  ER5517.BTE_Memory_Copy(layer3_start_addr,LCD_XSIZE_TFT*2,LCD_XSIZE_TFT,0,0,LCD_XSIZE_TFT*2,0,0,layer3_start_addr,LCD_XSIZE_TFT*2,0,0,0x0C,LCD_XSIZE_TFT,LCD_YSIZE_TFT);			//将Layer1(800,0) -> Layer1(0,0) 恢复底图

		//x向递增和递减
		if(dir_x==1 && x<LCD_XSIZE_TFT-400)
		{
			x++;
			if(x==LCD_XSIZE_TFT-400-1)	dir_x=0;
		}
		else if(dir_x==0 && x>0)
		{
			x--;
			if(x==0)	dir_x=1;
		}		
		
		//y向递增和递减
		if(dir_y==1 && y<LCD_YSIZE_TFT-100)
		{
			y++;
			if(y==LCD_YSIZE_TFT-100-1)	dir_y=0;
		}
		else if(dir_y==0 && y>0)
		{
			y--;
			if(y==0)	dir_y=1;
		}
	}	    
    
}
