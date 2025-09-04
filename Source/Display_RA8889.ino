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

#define  PIN_RESET  16
#define  PIN_CS     5

Panel_RA8889 gfx(PIN_CS, PIN_RESET);


void setup() {
  
  gfx.init();
  gfx.DisplayOn(true);  //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico

}

void loop() {
  //Fazer:
  //essa funcao nao deveria ser publica
  //verificar comof azer a selecao do tipo de memoria e pixel e cores padrao
  //numca funcao e em configuracao de display
  gfx.Select_MainWindow_16bpp();
  
  gfx.MainImage_StartAddress( LayerStartAddr(0) );
  gfx.MainImage_Width( gfx.Width() );
  
  gfx.MainWindow_StartXY(0,0);
  gfx.CanvasImage_StartAddr(0);
  gfx.CanvasImage_Width( gfx.Width() );
  
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );
  
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clRed), true);
  delay(1000);
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clGreen), true);
  delay(1000);
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clBlue), true);
  delay(1000);

  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clCyan), true);
  delay(1000);
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clYellow, true);
  delay(1000); 
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clPurple), true);
  delay(1000);   
 
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clBlack), true);
  delay(1000); 
  gfx.DrawSquare(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT, toValue(Color::clWhite), true);
  delay(1000);

  ////////BackLight Brightness control test  whit ER's PWM0
  unsigned char  brightness=10;
  gfx.ForegroundColor_65k(toValue(Color::clWhite));
  gfx.BackgroundColor_65k(toValue(Color::clRed));
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
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( LayerStartAddr(0) );				
  gfx.MainImage_Width( gfx.Width() );
  gfx.MainWindow_StartXY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );

  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
    gfx.DrawSquare(0+i, 0+i, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, toValue(Color::clRed));
	delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clRed));
    //ER_TFT.Line_Start_XY(0+i,0+i);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Start_Square();
    //delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
	gfx.DrawSquare(0+i, 0+i, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, toValue(Color::clBlack));
	delay(10);
    //gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Line_Start_XY(0+i,0+i);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Start_Square();
    //delay(10);
  }
 delay(100);
 
///////////////////////////Square Of Circle
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
    gfx.DrawCircleSquare(0+i, 0+i, CD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, 10, 10, toValue(Color::clGreen));
	delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clGreen));
    //ER_TFT.Line_Start_XY(0+i,0+i);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Circle_Square_Radius_RxRy(10,10);
    //ER_TFT.Start_Circle_Square();
    //delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
   	gfx.DrawCircleSquare(0+i, 0+i, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, 10, 10, toValue(Color::clBlack));
    delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Line_Start_XY(0+i,0+i);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Circle_Square_Radius_RxRy(10,10);
    //ER_TFT.Start_Circle_Square();
    //delay(10);
  }
  delay(100);

///////////////////////////Circle
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
	gfx.DrawCircle(LCD_XSIZE_TFT/2, LCD_YSIZE_TFT/2, i, toValue(Color::clBlue));
    delay(10);
    //gfx.ForegroundColor_65k(toValue(Color::clBlue));
    //ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
    //ER_TFT.Circle_Radius_R(i);
    //ER_TFT.Start_Circle_or_Ellipse();
    //delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
	gfx.DrawCircle(LCD_XSIZE_TFT/2, LCD_YSIZE_TFT/2, i, toValue(Color::clBlack));
    delay(10);
    //gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
    //ER_TFT.Circle_Radius_R(i);
    //ER_TFT.Start_Circle_or_Ellipse();
    //delay(10);
  }
  delay(100);

///////////////////////////Ellipse
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
	gfx.DrawEllipse(LCD_XSIZE_TFT/2, LCD_YSIZE_TFT/2, i+100, i, toValue(Color::clWhite));
    delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clWhite));
    //ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
    //ER_TFT.Ellipse_Radius_RxRy(i+100,i);
    //ER_TFT.Start_Circle_or_Ellipse();
    //delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
    gfx.DrawEllipse(LCD_XSIZE_TFT/2, LCD_YSIZE_TFT/2, i+100, i, toValue(Color::clBlack));
	delay(10);
    //gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Circle_Center_XY(LCD_XSIZE_TFT/2,LCD_YSIZE_TFT/2);
    //ER_TFT.Ellipse_Radius_RxRy(i+100,i);
    //ER_TFT.Start_Circle_or_Ellipse();
    //delay(10);
  }
  delay(100);

 ////////////////////////////Triangle
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
    gfx.DrawTriangle(LCD_XSIZE_TFT/2, i, i, LCD_YSIZE_TFT-1-i, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, toValue(Color::clYellow));
    delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clYellow));
    //ER_TFT.Triangle_Point1_XY(LCD_XSIZE_TFT/2, i);
    //ER_TFT.Triangle_Point2_XY(i, LCD_YSIZE_TFT-1-i);
    //ER_TFT.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i);
    //ER_TFT.Start_Triangle();
    //delay(10);
  }

  for(i=0;i<=LCD_YSIZE_TFT/2-10;i+=8)
  {
	gfx.DrawTriangle(LCD_XSIZE_TFT/2, i, i, LCD_YSIZE_TFT-1-i, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1-i, toValue(Color::clBlack));  
    //gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Triangle_Point1_XY(LCD_XSIZE_TFT/2,i);
    //ER_TFT.Triangle_Point2_XY(i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Triangle_Point3_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Start_Triangle();
    //delay(10);
  }
  delay(100);

 ////////////////////////////line
  gfx.DrawLine(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack))
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {
	gfx.DrawLine(i, 0, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1, toValue(Color::clRed))
	delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clRed));
    //ER_TFT.Line_Start_XY(i,0);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
    //ER_TFT.Start_Line();
    //delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {
	gfx.DrawLine(0, LCD_YSIZE_TFT-1-i, LCD_XSIZE_TFT-1, i, toValue(Color::clRed))
	delay(10);
    //gfx.ForegroundColor_65k();
    //Point1_XY();
    //Point2_XY(LCD_XSIZE_TFT-1,i);
    //gfx.LineMode_Start();
    //delay(10);
  }

  for(i=0;i<=LCD_XSIZE_TFT;i+=8)
  {
	gfx.DrawLine(i, 0, LCD_XSIZE_TFT-1-i, LCD_YSIZE_TFT-1, toValue(Color::clBlack))
	delay(10);
    //gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Line_Start_XY(i,0);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1-i,LCD_YSIZE_TFT-1);
    //ER_TFT.Start_Line();
    //delay(10);
  }
  for(i=0;i<=LCD_YSIZE_TFT;i+=8)
  {
    gfx.DrawLine(0, LCD_YSIZE_TFT-1-i, LCD_XSIZE_TFT-1, i, toValue(Color::clBlack))
	delay(10);
	//gfx.ForegroundColor_65k(toValue(Color::clBlack));
    //ER_TFT.Line_Start_XY(0,LCD_YSIZE_TFT-1-i);
    //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,i);
    //ER_TFT.Start_Line();
    //delay(10);
  }

  delay(100);  

/////////////Text  
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( LayerStartAddr(0) );				
  gfx.MainImage_Width( gfx.Width() );
  gfx.MainWindow_StartXY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );	
 
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();

  gfx.ForegroundColor_65k(toValue(Color::clGreen));
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,26);
  ER_TFT.Show_String("buydisplay.com");

  gfx.BackgroundColor_65k(toValue(Color::clBlack)); 
  gfx.ForegroundColor_65k(toValue(Color::clRed));
  ER_TFT.Font_Width_X4(); 
  ER_TFT.Font_Height_X4();
  ER_TFT.Goto_Text_XY(0,90);  
  ER_TFT.Show_String("buydisplay.com");    
  delay(2000); 
  ER_TFT.Font_Width_X1(); 
  ER_TFT.Font_Height_X1();
  
  
  unsigned int temp;
  unsigned long im=1;
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( LayerStartAddr(0) );  
  gfx.MainImage_Width( gfx.Width() );							
  gfx.MainWindow_StartXY(0,0);

  ER_TFT.Canvas_Image_Start_address(0);//Layer 1
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);//
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );

  gfx.ForegroundColor_65k(toValue(Color::clBlack));
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-25);
  ER_TFT.Start_Square_Fill();

  gfx.DrawSquare(0, LCD_YSIZE_TFT-24, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlue), true);
  //gfx.ForegroundColor_65k(toValue(Color::clBlue));
  //ER_TFT.Line_Start_XY(0,LCD_YSIZE_TFT-24);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();
  
  gfx.ForegroundColor_65k(toValue(Color::clWhite));
  gfx.BackgroundColor_65k(toValue(Color::clBlue));
  ER_TFT.CGROM_Select_Internal_CGROM();
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,LCD_YSIZE_TFT-24);
  ER_TFT.Show_String("  Demo BTE Compare");
  gfx.ForegroundColor_65k(toValue(Color::clBlack));
  gfx.BackgroundColor_65k(toValue(Color::clWhite));
 
  ER_TFT.Goto_Text_XY(0,LCD_YSIZE_TFT-48);
  ER_TFT.Show_String("Execute Logic 'OR' 0xf000");

  gfx.ActiveWindow_XY(20,40);
  gfx.ActiveWindow_WidhtHeight(80, 80);
  ER_TFT.Goto_Pixel_XY(20,40);
  ER_TFT. Show_picture(80*80,pic_80x80); 
  gfx.ActiveWindow_XY(20+80+20,40);
  gfx.ActiveWindow_WidhtHeight(80,80);
  ER_TFT.Goto_Pixel_XY(120,40);
  ER_TFT.Show_picture(80*80,pic_80x80);
  gfx.ActiveWindow_XY(20+80+20+80+20,40);
  gfx.ActiveWindow_WidhtHeight(80,80);
  ER_TFT.Goto_Pixel_XY(220,40);
  ER_TFT.Show_picture(80*80,pic_80x80);		 
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );
  gfx.ForegroundColor_65k(toValue(Color::clBlack));
  gfx.BackgroundColor_65k(toValue(Color::clWhite));
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
  gfx.ActiveWindow_XY(20,40);
  gfx.ActiveWindow_WidhtHeight(80,80); 
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
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );
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
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );

  gfx.BackgroundColor_65k(toValue(Color::clBlack));
  gfx.DrawSquare(0, 40, 80, 120, 0xf000, true);
  //gfx.ForegroundColor_65k(0xf000);
  //ER_TFT.Line_Start_XY(0,40);
  //ER_TFT.Line_End_XY(80,120);
  //ER_TFT.Start_Square_Fill();  
  
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
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( LayerStartAddr(0) );				
  gfx.MainImage_Width( gfx.Width() );
  gfx.MainWindow_StartXY(0,0);
  ER_TFT.Canvas_Image_Start_address(layer1_start_addr);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );	
  
  gfx.DrawSquare(0, 0, LCD_XSIZE_TFT-1, LCD_YSIZE_TFT-1, toValue(Color::clBlack), true);  
  //gfx.ForegroundColor_65k(toValue(Color::clBlack));
  //ER_TFT.Line_Start_XY(0,0);
  //ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  //ER_TFT.Start_Square_Fill();
 
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
