/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2 Display test
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test ok:  Arduino Due,Arduino UNO,Arduino MEGA2560
****************************************************/

#include <SPI.h>
#include <Wire.h>
#include "RA8889.h"

void setup() {
  
  pinMode(5,   OUTPUT);
  digitalWrite(5, HIGH);//Disable  SD 
  pinMode(2,   OUTPUT);
  digitalWrite(2, HIGH);//Disable  RTP    
  
  ER5517.Parallel_Init();
  ER5517.HW_Reset();
  ER5517.System_Check_Temp();
  delay(100);
  while(ER5517.LCD_StatusRead()&0x02);
  ER5517.initial();
  ER5517.Display_ON();

}
void loop() {
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
  
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Red);
  delay(1000);
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Green);
  delay(1000);
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Blue);
  delay(1000);

  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Cyan);
  delay(1000);
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Yellow);
  delay(1000); 
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Purple);
  delay(1000);   
 
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  delay(1000); 
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
  delay(1000);

  ////////BackLight Brightness control test  whit ER's PWM0
  unsigned char  brightness=10;
  ER5517.Foreground_color_65k(White);
  ER5517.Background_color_65k(Red);
  ER5517.CGROM_Select_Internal_CGROM();  
  ER5517.Font_Select_12x24_24x24();
  ER5517.Goto_Text_XY(0,10); 
  ER5517.Show_String( "BackLight Brightness control");
  while(brightness<=100)
 {
  ER5517.Select_PWM1();
  ER5517.Set_PWM_Prescaler_1_to_256(20);
  ER5517.Select_PWM1_Clock_Divided_By_1();
  ER5517.Set_Timer1_Count_Buffer(100); 
  ER5517.Set_Timer1_Compare_Buffer(brightness); 
  ER5517.Start_PWM1(); 
  delay(50);
  brightness+=10;
  } 
   delay(1000); 
   
  ////////Drawing
  unsigned int i;
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);				
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(layer1_start_addr);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	

  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Red);
  ER5517.Line_Start_XY(0+i,0+i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Start_Square();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0+i,0+i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Start_Square();
  delay(10);
  }
 delay(100);
///////////////////////////Square Of Circle
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Green);
  ER5517.Line_Start_XY(0+i,0+i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Circle_Square_Radius_RxRy(10,10);
  ER5517.Start_Circle_Square();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0+i,0+i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Circle_Square_Radius_RxRy(10,10);
  ER5517.Start_Circle_Square();
  delay(10);
  }
  delay(100);

///////////////////////////Circle
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Blue);
  ER5517.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER5517.Circle_Radius_R(i);
  ER5517.Start_Circle_or_Ellipse();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER5517.Circle_Radius_R(i);
  ER5517.Start_Circle_or_Ellipse();
  delay(10);
  }
  delay(100);

///////////////////////////Ellipse
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(White);
  ER5517.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER5517.Ellipse_Radius_RxRy(i+100,i);
  ER5517.Start_Circle_or_Ellipse();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER5517.Ellipse_Radius_RxRy(i+100,i);
  ER5517.Start_Circle_or_Ellipse();
  delay(10);
  }
  delay(100);

 ////////////////////////////Triangle
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Yellow);
  ER5517.Triangle_Point1_XY(LCD_XSIZE_TFT/2,i);
  ER5517.Triangle_Point2_XY(i,LCD_YSIZE_TFT-1-i);
  ER5517.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Start_Triangle();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Triangle_Point1_XY(LCD_XSIZE_TFT/2,i);
  ER5517.Triangle_Point2_XY(i,LCD_YSIZE_TFT-1-i);
  ER5517.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER5517.Start_Triangle();
  delay(10);
  }
  delay(100);


 ////////////////////////////line
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();

  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {ER5517.Foreground_color_65k(Red);
  ER5517.Line_Start_XY(i,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
  ER5517.Start_Line();
  delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {ER5517.Foreground_color_65k(Red);
  ER5517.Line_Start_XY(0,LCD_YSIZE_TFT-1-i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,i);
  ER5517.Start_Line();
  delay(10);
  }


  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(i,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
  ER5517.Start_Line();
  delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,LCD_YSIZE_TFT-1-i);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,i);
  ER5517.Start_Line();
  delay(10);
  }


  delay(100);  

/////////////Text  
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);				
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(layer1_start_addr);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	
 
  
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();



  ER5517.Foreground_color_65k(Green);
  ER5517.Font_Select_12x24_24x24();
  ER5517.Goto_Text_XY(0,26);
  ER5517.Show_String("buydisplay.com");

 ER5517.Background_color_65k(Black); 
  ER5517.Foreground_color_65k(Red);
  ER5517.Font_Width_X4(); 
  ER5517.Font_Height_X4();
  ER5517.Goto_Text_XY(0,90);  
  ER5517.Show_String("buydisplay.com");    
  delay(2000); 
  ER5517.Font_Width_X1(); 
  ER5517.Font_Height_X1();
  
  
  unsigned int temp;
  unsigned long im=1;
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(0);				
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);							
  ER5517.Main_Window_Start_XY(0,0);

  ER5517.Canvas_Image_Start_address(0);//Layer 1
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);//
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);

  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-25);
  ER5517.Start_Square_Fill();

  ER5517.Foreground_color_65k(Blue);
  ER5517.Line_Start_XY(0,LCD_YSIZE_TFT-24);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();
  ER5517.Foreground_color_65k(White);
  ER5517.Background_color_65k(Blue);
  ER5517.CGROM_Select_Internal_CGROM();
  ER5517.Font_Select_12x24_24x24();
  ER5517.Goto_Text_XY(0,LCD_YSIZE_TFT-24);
  ER5517.Show_String("  Demo BTE Compare");
  ER5517.Foreground_color_65k(Black);
  ER5517.Background_color_65k(White);
 
  ER5517.Goto_Text_XY(0,LCD_YSIZE_TFT-48);
  ER5517.Show_String("Execute Logic 'OR' 0xf000");

  ER5517.Active_Window_XY(20,40);
  ER5517.Active_Window_WH(80,80);
  ER5517.Goto_Pixel_XY(20,40);
  ER5517. Show_picture(80*80,pic_80x80); 
  ER5517.Active_Window_XY(20+80+20,40);
  ER5517.Active_Window_WH(80,80);
  ER5517.Goto_Pixel_XY(120,40);
  ER5517.Show_picture(80*80,pic_80x80);
  ER5517.Active_Window_XY(20+80+20+80+20,40);
  ER5517.Active_Window_WH(80,80);
  ER5517.Goto_Pixel_XY(220,40);
  ER5517.Show_picture(80*80,pic_80x80);		 
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
  ER5517.Foreground_color_65k(Black);
  ER5517.Background_color_65k(White);
  ER5517.CGROM_Select_Internal_CGROM();
 
  ER5517.Goto_Text_XY(20,130 );
  ER5517.Show_String("NO BTE");
  ER5517.Goto_Text_XY(120,130 );
  ER5517.Show_String("BTE ");
  ER5517.Goto_Text_XY(120,154 );
  ER5517.Show_String("ROP");
  ER5517.Goto_Text_XY(220,130 );
  ER5517.Show_String("BTE Move");
  ER5517.Goto_Text_XY(220,154 );
  ER5517.Show_String("ROP");
  delay(1000);
  ER5517.Active_Window_XY(20,40);
  ER5517.Active_Window_WH(80,80); 
  ER5517. Goto_Pixel_XY(20,40);
  ER5517.LCD_CmdWrite(0x04);
  temp =   ER5517.LCD_DataRead();
  ER5517.Check_Mem_RD_FIFO_not_Empty();  //dummy
  for(i=0; i<80*80;i++)
  {				
  temp =   ER5517.LCD_DataRead();		   
  temp=temp|(  ER5517.LCD_DataRead()<<8);
  ER5517.Check_Mem_RD_FIFO_not_Empty();
  temp |= 0xf000; 
  ER5517.LCD_DataWrite(temp);
  ER5517.LCD_DataWrite(temp>>8);
  ER5517.Check_Mem_WR_FIFO_not_Full();
  }
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
  delay(1000);
   //second block, MCU write with BTE ROP 
  ER5517.BTE_S0_Color_16bpp();
  ER5517.BTE_S1_Color_16bpp();
  ER5517.BTE_S1_Memory_Start_Address(0);
  ER5517.BTE_S1_Image_Width(LCD_XSIZE_TFT);
  ER5517.BTE_S1_Window_Start_XY(120,40);

  ER5517.BTE_Destination_Color_16bpp();  
  ER5517.BTE_Destination_Memory_Start_Address(0);
  ER5517.BTE_Destination_Image_Width(LCD_XSIZE_TFT);
  ER5517.BTE_Destination_Window_Start_XY(120,40);  
  ER5517.BTE_Window_Size(80,80);

  ER5517.BTE_ROP_Code(14);
  ER5517.BTE_Operation_Code(0); //BTE write
  ER5517.BTE_Enable();

  ER5517.LCD_CmdWrite(0x04);
  for(i=0; i<80*80;i++)
  {				
  ER5517.LCD_DataWrite(0xf000);
  ER5517.LCD_DataWrite(0xf000>>8);
  ER5517.Check_Mem_WR_FIFO_not_Full();
  }
  ER5517.Check_Mem_WR_FIFO_Empty();//糶Ч浪琩
  ER5517.Check_BTE_Busy();

  delay(1000);   
	  //third block, BTE MOVE with ROP
  ER5517.Canvas_Image_Start_address(layer2_start_addr);//
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);//
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);

 
  ER5517.Foreground_color_65k(0xf000);
  ER5517.Background_color_65k(Black);
  ER5517.Line_Start_XY(0,40);
  ER5517.Line_End_XY(80,120);
  ER5517.Start_Square_Fill();  
   
   
  ER5517.BTE_S0_Color_16bpp();
  ER5517.BTE_S0_Memory_Start_Address(layer2_start_addr);
  ER5517.BTE_S0_Image_Width(LCD_XSIZE_TFT);
  ER5517.BTE_S0_Window_Start_XY(0,40);

  ER5517.BTE_S1_Color_16bpp();
  ER5517.BTE_S1_Memory_Start_Address(layer1_start_addr);
  ER5517.BTE_S1_Image_Width(LCD_XSIZE_TFT);
  ER5517.BTE_S1_Window_Start_XY(220,40);

  ER5517.BTE_Destination_Color_16bpp();  
  ER5517.BTE_Destination_Memory_Start_Address(layer1_start_addr);
  ER5517.BTE_Destination_Image_Width(LCD_XSIZE_TFT);
  ER5517.BTE_Destination_Window_Start_XY(220,40);  
  ER5517.BTE_Window_Size(80,80);

  ER5517.BTE_ROP_Code(14);
  ER5517.BTE_Operation_Code(2); //BTE write
  ER5517.BTE_Enable();
  ER5517.Check_BTE_Busy();
  delay(1000);  
  ER5517.BTE_Disable();

  
 ///////////////////////// DMA
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);				
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(layer1_start_addr);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	
 
  
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill();
 
 
  ER5517.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,0);
// Select SPI : SCS��0       SCS��1
// SPI Clock = System Clock /{(Clk+1)*2}
// Transfer to SDRAM address:X1
 // Transfer to SDRAM address:Y1
// DMA data width
// DMA data height
// Picture's width
// Flash address
  delay(2000);
  ER5517.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,768000);
  delay(2000);  
  ER5517.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,1536000);
  delay(2000); 
  
    
}
