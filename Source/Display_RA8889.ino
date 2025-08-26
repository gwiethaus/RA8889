/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2 Display test
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply
Tested and worked with:
Works with Arduino 2.3.4 IDE  
NOTE: test OK:ESP32-WROOM-32 
****************************************************/

/*
  ==TFT Hardware SPI to ESP32  ==
    TFT   =>    ESP32
  *1,2. GND       ->    GND
  *3,4. VCC       ->    5V    3.3V OR 5V is optional, depending on the voltage of the module purchased 
  *5. CS        ->       5
  *6. MISO        ->    19  
  *7. MOSI       ->    23  
  *8. SCK       ->    18  
  *11. RES       ->    16   
*/

#include <SPI.h>
#include "Panel_RA8889.h"

#define  LCD_RESET  16
#define  LCD_CS     5

Panel_RA8889 gfx(LCD_CS, LCD_RESET);


void setup() {
  
  gfx.init();
  
  ER_TFT.System_Check_Temp();
  delay(100);
  while(ER_TFT.LCD_StatusRead()&0x02);
  ER_TFT.initial();     //aqui configura o PLL, frequencia, modo do display, cor... etc... todos os registardores
  
  gfx.DisplayOn(true);

}
void loop() {
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(layer1_start_addr);        
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.Main_Window_Start_XY(0,0);
  ER_TFT.Canvas_Image_Start_address(0);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
  
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Red);
  delay(1000);
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Green);
  delay(1000);
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Blue);
  delay(1000);

  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Cyan);
  delay(1000);
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Yellow);
  delay(1000); 
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Purple);
  delay(1000);   
 
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
  delay(1000); 
  ER_TFT.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
  delay(1000);

  ////////BackLight Brightness control test  whit ER's PWM0
  unsigned char  brightness=10;
  ER_TFT.Foreground_color_65k(White);
  ER_TFT.Background_color_65k(Red);
  ER_TFT.CGROM_Select_Internal_CGROM();  
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,10); 
  ER_TFT.Show_String( "BackLight Brightness control");
  while(brightness<=100)
 {
  ER_TFT.Select_PWM1();
  ER_TFT.Set_PWM_Prescaler_1_to_256(20);
  ER_TFT.Select_PWM1_Clock_Divided_By_1();
  ER_TFT.Set_Timer1_Count_Buffer(100); 
  ER_TFT.Set_Timer1_Compare_Buffer(brightness); 
  ER_TFT.Start_PWM1(); 
  delay(50);
  brightness+=10;
  } 
   delay(1000); 
   
  ////////Drawing
  unsigned int i;
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(layer1_start_addr);				
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.Main_Window_Start_XY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	

  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Red);
  ER_TFT.Line_Start_XY(0+i,0+i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Start_Square();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0+i,0+i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Start_Square();
  delay(10);
  }
 delay(100);
///////////////////////////Square Of Circle
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Green);
  ER_TFT.Line_Start_XY(0+i,0+i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Circle_Square_Radius_RxRy(10,10);
  ER_TFT.Start_Circle_Square();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0+i,0+i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Circle_Square_Radius_RxRy(10,10);
  ER_TFT.Start_Circle_Square();
  delay(10);
  }
  delay(100);

///////////////////////////Circle
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Blue);
  ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER_TFT.Circle_Radius_R(i);
  ER_TFT.Start_Circle_or_Ellipse();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER_TFT.Circle_Radius_R(i);
  ER_TFT.Start_Circle_or_Ellipse();
  delay(10);
  }
  delay(100);

///////////////////////////Ellipse
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(White);
  ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER_TFT.Ellipse_Radius_RxRy(i+100,i);
  ER_TFT.Start_Circle_or_Ellipse();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
  ER_TFT.Ellipse_Radius_RxRy(i+100,i);
  ER_TFT.Start_Circle_or_Ellipse();
  delay(10);
  }
  delay(100);

 ////////////////////////////Triangle
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Yellow);
  ER_TFT.Triangle_Point1_XY(LCD_XSIZE_TFT/2,i);
  ER_TFT.Triangle_Point2_XY(i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Start_Triangle();
  delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Triangle_Point1_XY(LCD_XSIZE_TFT/2,i);
  ER_TFT.Triangle_Point2_XY(i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
  ER_TFT.Start_Triangle();
  delay(10);
  }
  delay(100);


 ////////////////////////////line
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {ER_TFT.Foreground_color_65k(Red);
  ER_TFT.Line_Start_XY(i,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Line();
  delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {ER_TFT.Foreground_color_65k(Red);
  ER_TFT.Line_Start_XY(0,LCD_YSIZE_TFT-1-i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,i);
  ER_TFT.Start_Line();
  delay(10);
  }


  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(i,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Line();
  delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,LCD_YSIZE_TFT-1-i);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,i);
  ER_TFT.Start_Line();
  delay(10);
  }


  delay(100);  

/////////////Text  
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(layer1_start_addr);				
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.Main_Window_Start_XY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	
 
  
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();



  ER_TFT.Foreground_color_65k(Green);
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,26);
  ER_TFT.Show_String("buydisplay.com");

 ER_TFT.Background_color_65k(Black); 
  ER_TFT.Foreground_color_65k(Red);
  ER_TFT.Font_Width_X4(); 
  ER_TFT.Font_Height_X4();
  ER_TFT.Goto_Text_XY(0,90);  
  ER_TFT.Show_String("buydisplay.com");    
  delay(2000); 
  ER_TFT.Font_Width_X1(); 
  ER_TFT.Font_Height_X1();
  
  
  unsigned int temp;
  unsigned long im=1;
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(0);				
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);							
  ER_TFT.Main_Window_Start_XY(0,0);

  ER_TFT.Canvas_Image_Start_address(0);//Layer 1
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);//
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);

  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-25);
  ER_TFT.Start_Square_Fill();

  ER_TFT.Foreground_color_65k(Blue);
  ER_TFT.Line_Start_XY(0,LCD_YSIZE_TFT-24);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();
  ER_TFT.Foreground_color_65k(White);
  ER_TFT.Background_color_65k(Blue);
  ER_TFT.CGROM_Select_Internal_CGROM();
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,LCD_YSIZE_TFT-24);
  ER_TFT.Show_String("  Demo BTE Compare");
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Background_color_65k(White);
 
  ER_TFT.Goto_Text_XY(0,LCD_YSIZE_TFT-48);
  ER_TFT.Show_String("Execute Logic 'OR' 0xf000");

  ER_TFT.Active_Window_XY(20,40);
  ER_TFT.Active_Window_WH(80,80);
  ER_TFT.Goto_Pixel_XY(20,40);
  ER_TFT. Show_picture(80*80,pic_80x80); 
  ER_TFT.Active_Window_XY(20+80+20,40);
  ER_TFT.Active_Window_WH(80,80);
  ER_TFT.Goto_Pixel_XY(120,40);
  ER_TFT.Show_picture(80*80,pic_80x80);
  ER_TFT.Active_Window_XY(20+80+20+80+20,40);
  ER_TFT.Active_Window_WH(80,80);
  ER_TFT.Goto_Pixel_XY(220,40);
  ER_TFT.Show_picture(80*80,pic_80x80);		 
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Background_color_65k(White);
  ER_TFT.CGROM_Select_Internal_CGROM();
 
  ER_TFT.Goto_Text_XY(20,130 );
  ER_TFT.Show_String("NO BTE");
  ER_TFT.Goto_Text_XY(120,130 );
  ER_TFT.Show_String("BTE ");
  ER_TFT.Goto_Text_XY(120,154 );
  ER_TFT.Show_String("ROP");
  ER_TFT.Goto_Text_XY(220,130 );
  ER_TFT.Show_String("BTE Move");
  ER_TFT.Goto_Text_XY(220,154 );
  ER_TFT.Show_String("ROP");
  delay(1000);
  ER_TFT.Active_Window_XY(20,40);
  ER_TFT.Active_Window_WH(80,80); 
  ER_TFT. Goto_Pixel_XY(20,40);
  ER_TFT.LCD_CmdWrite(0x04);
  temp =   ER_TFT.LCD_DataRead();
  ER_TFT.Check_Mem_RD_FIFO_not_Empty();  //dummy
  for(i=0; i<80*80;i++)
  {				
  temp =   ER_TFT.LCD_DataRead();		   
  temp=temp|(  ER_TFT.LCD_DataRead()<<8);
  ER_TFT.Check_Mem_RD_FIFO_not_Empty();
  temp |= 0xf000; 
  ER_TFT.LCD_DataWrite(temp);
  ER_TFT.LCD_DataWrite(temp>>8);
  ER_TFT.Check_Mem_WR_FIFO_not_Full();
  }
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
  delay(1000);
   //second block, MCU write with BTE ROP 
  ER_TFT.BTE_S0_Color_16bpp();
  ER_TFT.BTE_S1_Color_16bpp();
  ER_TFT.BTE_S1_Memory_Start_Address(0);
  ER_TFT.BTE_S1_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.BTE_S1_Window_Start_XY(120,40);

  ER_TFT.BTE_Destination_Color_16bpp();  
  ER_TFT.BTE_Destination_Memory_Start_Address(0);
  ER_TFT.BTE_Destination_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.BTE_Destination_Window_Start_XY(120,40);  
  ER_TFT.BTE_Window_Size(80,80);

  ER_TFT.BTE_ROP_Code(14);
  ER_TFT.BTE_Operation_Code(0); //BTE write
  ER_TFT.BTE_Enable();

  ER_TFT.LCD_CmdWrite(0x04);
  for(i=0; i<80*80;i++)
  {				
  ER_TFT.LCD_DataWrite(0xf000);
  ER_TFT.LCD_DataWrite(0xf000>>8);
  ER_TFT.Check_Mem_WR_FIFO_not_Full();
  }
  ER_TFT.Check_Mem_WR_FIFO_Empty();//糶Ч浪琩
  ER_TFT.Check_BTE_Busy();

  delay(1000);   
	  //third block, BTE MOVE with ROP
  ER_TFT.Canvas_Image_Start_address(layer2_start_addr);//
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);//
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);

 
  ER_TFT.Foreground_color_65k(0xf000);
  ER_TFT.Background_color_65k(Black);
  ER_TFT.Line_Start_XY(0,40);
  ER_TFT.Line_End_XY(80,120);
  ER_TFT.Start_Square_Fill();  
   
   
  ER_TFT.BTE_S0_Color_16bpp();
  ER_TFT.BTE_S0_Memory_Start_Address(layer2_start_addr);
  ER_TFT.BTE_S0_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.BTE_S0_Window_Start_XY(0,40);

  ER_TFT.BTE_S1_Color_16bpp();
  ER_TFT.BTE_S1_Memory_Start_Address(layer1_start_addr);
  ER_TFT.BTE_S1_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.BTE_S1_Window_Start_XY(220,40);

  ER_TFT.BTE_Destination_Color_16bpp();  
  ER_TFT.BTE_Destination_Memory_Start_Address(layer1_start_addr);
  ER_TFT.BTE_Destination_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.BTE_Destination_Window_Start_XY(220,40);  
  ER_TFT.BTE_Window_Size(80,80);

  ER_TFT.BTE_ROP_Code(14);
  ER_TFT.BTE_Operation_Code(2); //BTE write
  ER_TFT.BTE_Enable();
  ER_TFT.Check_BTE_Busy();
  delay(1000);  
  ER_TFT.BTE_Disable();

  
 ///////////////////////// DMA
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(layer1_start_addr);				
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.Main_Window_Start_XY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);	
 
  
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill();
 
 
  ER_TFT.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,0);
// Select SPI : SCS��0       SCS��1
// SPI Clock = System Clock /{(Clk+1)*2}
// Transfer to SDRAM address:X1
 // Transfer to SDRAM address:Y1
// DMA data width
// DMA data height
// Picture's width
// Flash address
  delay(2000);
  ER_TFT.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,768000);
  delay(2000);  
  ER_TFT.DMA_24bit_Block(1,1,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,1536000);
  delay(2000); 
  
    
}
