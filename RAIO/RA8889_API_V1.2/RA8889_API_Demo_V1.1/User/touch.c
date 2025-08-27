#include "stm32f10x.h"	   
#include "delay.h"
#include "touch.h"   
#include "platform_config.h"	
#include "stm32f10x_exti.h"	
#include "stdlib.h"
#include "math.h"
//EXTI_InitTypeDef EXTI_InitStructure;
////////////////////////////////////////////////////// 	



Pen_Holder Pen_Point;//定义笔实体

void Touch_Start(void);
void start_spi(void);
u8 Read_Ads7846(void);
void delay_ms(u16 nms);
void delay_us(u32 Nus);
void WriteByteADS(u8 num);
void Select_Menu(u8 M_Type,u8 Cnt,u8 En);


extern u16 SPI_Flash_SendByte(u8 byte);
extern u16 SPI_Flash_ReadByte(u8 a);
extern void TFT_ShowString(unsigned int x,unsigned int y,const unsigned char *p);
extern void TFT_CLEAR(unsigned int color);//TFT清屏函数
extern void drawbigpoint(unsigned int x,unsigned int y);
extern void drow_touch_point(unsigned int x,unsigned int y);
extern void TFT_Fill(unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend,unsigned int color);
extern void TFT_ShowNum(unsigned int x,unsigned int y,unsigned int num,unsigned int size,unsigned int mode);
extern void TFT_Show4Num(unsigned int x,unsigned int y,unsigned int num,unsigned int size,unsigned int mode);


//画笔颜色
u16  POINT_COLOR=RED;//默认红色    
u16  BACK_COLOR=WHITE;//背景颜色.默认为白色

unsigned char _it0=0,_it1=0,_it2=0,_it3=0,num=0;
unsigned char dw=0;
unsigned int Xs=0,Xe=0,Ys=0,Ye=0;
unsigned int X=0,Y=0;  
float X2=0,Y2=0;

void touch(void)
{
  unsigned char a=0,b=0;
   	Pen_Point.Key_Sta=Key_Up;//按键抬起
  //TFT_CLEAR(0,0,240,320);     //清屏		
	TFT_CLEAR(0xffff);
//	POINT_COLOR=RED;
	TFT_ShowString(40,40,"Touch First Point");
	for(b=0; b<10; b++){
		drawbigpoint(0,0+b);
		drawbigpoint(b,0);
	} 	
	while (1)                   //等待中断到来
  	{    						   
		if(_it1==1){
		//   	Pen_Point.Key_Sta=Key_Down;//按键按下
			delay_ms(340);
			a=PEN;
			if(a==0){
				while(a==0){
				    delay_us(10);
					Read_Ads7846();
					Xs=X; Ys=Y;
					a=PEN;
					_it1=0;	
		//			Pen_Point.Key_Sta=Key_Up;//按键抬起
					break;
				}
				 break;	 
			}
			else
			 {
			  _it1=0;
		//	  Pen_Point.Key_Sta=Key_Up;//按键抬起
			  }	
		}				
  	}

		TFT_CLEAR(0xffff);     //清屏   
	TFT_ShowString(80,40,"Touch Second Point"); 		
	for(b=0; b<20; b++){
		drawbigpoint(120,150+b);
		drawbigpoint(110+b,160);
	}
	_it1=0;	
	while (1)                   //等待中断到来
  	{    						   
		if(_it1==1){	 
			delay_ms(340);
			a=PEN;
			if(a==0){
				while(a==0){
				    delay_us(10);
					Read_Ads7846();
					Xe=X; Ye=Y;
					a=PEN;	
					_it1=0;	
					break;
			              }
				break;	 	
			       }
			else 
			    _it1=0;		
		}				
  	}
	//X2=Xs-Xe; X2=X2/120;  
	//Y2=Ye-Ys; Y2=Y2/160;  

	X2=Xs-Xe; X2=X2/120;  
	Y2=Ys-Ye; Y2=Y2/160;  
	dw=1;


	POINT_COLOR=RED;
	TFT_CLEAR(0xffff);     //清屏   	
	TFT_Fill(0,0,20,20,RED); 
	TFT_Fill(20,0,40,20,BLUE); 
	TFT_Fill(40,0,60,20,GREEN); 
	TFT_Fill(60,0,80,20,GRED); 
	TFT_Fill(80,0,100,20,BRED); 
	TFT_Fill(100,0,120,20,0); 	 
	TFT_ShowString(210,0,"CLR");
	//touch_init();               //触摸屏初始化  
	while (1)                   //等待中断到来
  	{    						   
		if(_it1==1){
			delay_ms(50);
			a=PEN;
			while(a==0){
			    //delay_us(10);
				Read_Ads7846();
				a=PEN;
			}
			_it1=0;	
		
		}
		//delay_ms(10);		  
  	}




}

////////////////////////////////////////////////////////////////////////		    
//选择菜单
 /*
void Select_Menu(u8 M_Type,u8 Cnt,u8 En)
{
	u8 tx;
	u16 ty;	
	ty=10; 
	if(M_Type==0)//主菜单
	{	  
		tx=80*(Cnt%3)+10;//10,90,170			
		
		if(Cnt>5)ty=216;
		else if(Cnt>2)ty=113;	 

		if(En)//选中当前的菜单
		{	  
			POINT_COLOR=WHITE;//白色字体
			BACK_COLOR=BLUE;  //蓝色背景      
		}else
		{
			POINT_COLOR=BLACK;//黑色字体
			BACK_COLOR=WHITE; //白色背景   
		}
	//	if(Cnt<6)Show_Str(tx+13,ty+74,(u8*)Menu[Cnt],0);	
	//	else Show_Str(tx+5,ty+74,(u8*)Menu[Cnt],0);	
	}
}  */
   


void start_spi(void)   
{ 	  
	//TCLK_SET(0);   
	//TCS_SET(1); 	 
	//TDIN_SET(1);  
	//sTCLK_SET(1);
	//SPI_Flash_SendByte(1);  
	TCS_SET(0);   
}
//SPI写数据
//向7846写入1byte数据   
void WriteByteADS(u8 num)    
{  
	u8 count=0;  
	TCLK_SET(0);   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN_SET(1);  
		else TDIN_SET(0);   
		num<<=1;    
		TCLK_SET(0);//上升沿有效  	 
		TCLK_SET(1);      
	} 			    
} 
//SPI读数据 
//从7846读取adc值	   
u16 ReadWordADS(void)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 	 
	for(count=0;count<12;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK_SET(1);//下降沿有效 	   
		TCLK_SET(0);    
		if(DOUT)Num++; 		 
	} 		  
	return(Num);   
}
//#define CMD_RDX 0X90  //0B10010000即用差分方式读X坐标
//#define CMD_RDY	0XD0  //0B11010000即用差分方式读Y坐标 


#define CMD_RDX 0XD0  //0B10010000即用差分方式读X坐标
#define CMD_RDY	0X90  //0B11010000即用差分方式读Y坐标
 
//读取一次X,Y值
//读到的X,Y坐标值必须都大于100
//成功返回1,不成功返回0
//读数限制在100~3800之间.
u8 read_once(void)
{	unsigned int a,b;
	start_spi();//启动SPI  	 
	SPI_Flash_SendByte(CMD_RDY); 
	delay_us(10);	
	a=SPI_Flash_ReadByte(0);
	a=a<<8;
	a|=SPI_Flash_ReadByte(0);
	a>>=3; 
	Y=a;	  
    Pen_Point.Y=a;//读Y轴坐标
	SPI_Flash_SendByte(CMD_RDX);
	delay_us(10);	
	b=SPI_Flash_ReadByte(0);
	b=b<<8;
	b|=SPI_Flash_ReadByte(0);
	b>>=3; 
	Pen_Point.X=b;
	X=b;   
	/*WriteByteADS(CMD_RDY); 
	//ADS7846的转换时间最长为6us
	TCLK_SET(1);delay_us(13);   	    
	TCLK_SET(0);delay_us(13);  
	Y=ReadWordADS();  
	WriteByteADS(CMD_RDX); 
	//ADS7846的转换时间最长为6us
	TCLK_SET(1);delay_us(13);   	    
	TCLK_SET(0);delay_us(13);   
	X=ReadWordADS();//读Y轴坐标  */ 
	TCS_SET(1); 
//	return 1;
	if(Pen_Point.X>100&&Pen_Point.Y>100&&Pen_Point.X<4090&&Pen_Point.Y<4090)return 1;//读数成功(范围限制)
//	if(X>100&&Y>100&&X<4000&&Y<4000)return 1;//读数成功(范围限制)
	else return 0;
				                 //读数失败
}
/*
u8 Read_Ads7846(void)
{	float X1,Y1;
    u16 x1,x2,y1,y2;
	u8 t,t1,count=0;
	//u16 databuffer[2][10]={{5,7,9,3,2,6,4,0,3,1},{5,7,9,3,2,6,4,0,3,1}};//数据组
	u16 databuffer[2][10];//数据组
	u16 temp=0;	 
    do					  //循环读数10次
	{
		//t=PEN;		   
		if(read_once())//读数成功
		{	  
			databuffer[0][count]=X;
			databuffer[1][count]=Y;
			count++;  
		}
		t=PEN;
	}while(!t&&count<10); 
	//}while(count<10); 
	if(count==10)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[0][t]>databuffer[0][t+1])//升序排列
				{
					temp=databuffer[0][t+1];
					databuffer[0][t+1]=databuffer[0][t];
					databuffer[0][t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	  
		do//将数据Y升序排列
		{	
			t1=0;		 
			for(t=0;t<count-1;t++)
			{
				if(databuffer[1][t]>databuffer[1][t+1])//升序排列
				{
					temp=databuffer[1][t+1];
					databuffer[1][t+1]=databuffer[1][t];
					databuffer[1][t]=temp;
					t1=1;	 
				}  
			}
		}while(t1);
		x1=databuffer[0][3]; x2=databuffer[0][4]; //x3=databuffer[0][8];		
		y1=databuffer[1][3]; y2=databuffer[1][4]; //y3=databuffer[1][8];	   
		if(((x1>x2)&&(x1>x2+30))||((x2>x1)&&(x2>x1+30))||((y1>y2)&&(y1>y2+30))||((y2>y1)&&(y2>y1+30)));	 		  
		else{
        
		//X1=(databuffer[0][3]+databuffer[0][4]+databuffer[0][5])/3;
		//Y1=(databuffer[1][3]+databuffer[1][4]+databuffer[1][5])/3;	
		X1=(databuffer[0][3]+databuffer[0][4])/2;
		Y1=(databuffer[1][3]+databuffer[1][4])/2;	
			//X1=(databuffer[0][3]+databuffer[0][4])/2;//+databuffer[0][8])/3;
			//Y1=(databuffer[1][3]+databuffer[1][4])/2;//+databuffer[1][8])/3;	  
			    
		
			if(X1<=4096&&Y1<=4096) //个人的屏根据初始参数修改.		  正常
			{	X=X1;
			    Y=Y1;
			    if(dw==1){
			 
					if(X1>=Xs)X1=0;
					else X1=Xs-X1;
					//if(Y1>=Ys)Y1-=Ys;
					//else Y1=0; 
					if(Y1<=Ys)Y1=Ys-Y1;
					else Y1=0; 
					//X2=Xs-Xe; X2=X2/240;  
					//Y2=Ye-Ys; Y2=Y2/320;  
					X=X1/X2; 
					Y=Y1/Y2;
			   		
					drawbigpoint(X,Y); 
				}
			}



		} 
		return 1;  
	}

   return 0;


}  */
 
//读取ADS7846	  
//连续读取10次数据,对十次数据排序,然后对
//中间三次取平均值,得到最终的X,Y值  	   
u8 Read_Ads7846(void)
{
	u8 t,t1,count=0;
	u16 databuffer[2][20];//数据组
	u16 temp=0;	 
	do					  //循环读数10次
	{		    		   
		if(read_once())//读数成功
		{	  
			databuffer[0][count]=Pen_Point.X;
			databuffer[1][count]=Pen_Point.Y;
			count++;  
		}
	}while(NPEN&&count<20);   
	if(count==20)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=10;t<count-1;t++)
			{
				if(databuffer[0][t]>databuffer[0][t+1])//升序排列
				{
					temp=databuffer[0][t+1];
					databuffer[0][t+1]=databuffer[0][t];
					databuffer[0][t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	  
		do//将数据Y升序排列
		{	
			t1=0;		 
			for(t=10;t<count-1;t++)
			{
				if(databuffer[1][t]>databuffer[1][t+1])//升序排列
				{
					temp=databuffer[1][t+1];
					databuffer[1][t+1]=databuffer[1][t];
					databuffer[1][t]=temp;
					t1=1;	 
				}  
			}
		}while(t1);		   
		 	 		  
		Pen_Point.X=(databuffer[0][13]+databuffer[0][14]+databuffer[0][15])/3;
		Pen_Point.Y=(databuffer[1][13]+databuffer[1][14]+databuffer[1][15])/3;	  
		
		
	
		
																		  
	//	TFT_ShowString(100,140,"X read:");TFT_Show4Num(162,140,Pen_Point.X,16,0);	   //
	//	TFT_ShowString(100,172,"Y read:");TFT_Show4Num(162,172,Pen_Point.Y,16,0);
	 //  if(abs(databuffer[0][2]-Pen_Point.X)<=100&&abs(databuffer[0][17]-Pen_Point.X)<=100)
		return 1;
	//	else
	//	return 0;									   
	
	}
	return 0;//读数失败
}
	 

 void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//选择内部时钟 HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//延时Nms
//注意Nms的范围
//Nms<=0xffffff*8/SYSCLK
//对72M条件下,Nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}   
//延时us								   
void delay_us(u32 Nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=Nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	    
}

//按键处理程序
//type:按键响应类型
//0,单点,定点,不扩展.一定要按键松开才返回
//1,单点,滑动,不扩展.滚动条操作/连加操作
//2,扩展按键支持:
//即:MOVE_LEFT,MOVE_RIGHT,MOVE_UP,MOVE_DOWN使能
u8 Touch_Key_Pro(u8 type)
{
	u16 tempx=0,tempy=0;//暂时保存X,Y坐标 	 	 
	u8 ml=0,mr=0,mu=0,md=0;//四个方向上移动次数  
	u8 first=1;	  		 	    					   		 
	//按键还是按下的
	//手动按触摸屏的时候,至少需要15ms才能退出这个循环	   	
	do
	{	    
		Pen_Point.Key_Sta=Key_Up;//按键状态变为松开    
		if(Read_Ads7846())//成功读数
		{	 
			if(first)
			{
				Pen_Point.X0=Pen_Point.X;Pen_Point.Y0=Pen_Point.Y;//得到初始按下的坐标
				tempx=Pen_Point.X;tempy=Pen_Point.Y;  
				//printf("X0:%d Y0:%d\n",Pen_Point.X,Pen_Point.Y);
				first=0;  //标记清空
			}else if(type==2) //扩展按键														  
			{	  
				if(tempx>Pen_Point.X)ml++;
				else mr++;	   	
				if(tempy>Pen_Point.Y)mu++;
				else md++;
				//设定一个门限值,不能让一次移动大于这个值,如果一次大于这个值
				//认为触摸屏误动作了.400的时候,反映比较慢
				if(abs(tempx-Pen_Point.X)>700||abs(tempy-Pen_Point.Y)>700)//有抖动
				{
					ml=mr=mu=md=0;//全部清掉
					tempx=Pen_Point.X=Pen_Point.X0;//坐标复位
					tempy=Pen_Point.Y=Pen_Point.Y0;
					break;//退出数据采集,结果为点采集
				}   
				tempx=Pen_Point.X;tempy=Pen_Point.Y;//转移临时坐标 
			}
			else if(type==1)break; 
			//printf("X:%d Y:%d\n",Pen_Point.X,Pen_Point.Y); 
		}
		delay_ms(10);//10ms消抖 						   		     								 	
	}while(NPEN||Pen_Point.Key_Sta==Key_Down);//PEN=0或者按键状态为按下状态;
	delay_ms(50);
	Pen_Point.Key_Sta=Key_Up;//按键状态变为松开	 
 	//单次/不扩展 键值处理  
	if(abs(tempx-Pen_Point.X0)<=200&&abs(tempy-Pen_Point.Y0)<=200||type<2)//单次按键/不扩展按键功能
//	if(abs(tempx-Pen_Point.X0)<=100&&abs(tempy-Pen_Point.Y0)<=100||type<2)//单次按键/不扩展按键功能
  // if(abs(tempx-Pen_Point.X0)<=100&&abs(tempy-Pen_Point.Y0)<=100)
	
	{	 
		if(Pen_Point.xfac!=0)//已经校准过了
		{
			
			TFT_Fill(410,220,479,283,0xffff);
			TFT_Show4Num(410,220,Pen_Point.X0,16,1);
			TFT_Show4Num(410,250,Pen_Point.Y0,16,1);
			
			Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
			Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;
			

		}
		Pen_Point.X=Pen_Point.X0;Pen_Point.Y=Pen_Point.Y0;
		return CLICK_POINT;//没有移动  
	}	 
	//扩展键值处理
	if(abs(tempx-Pen_Point.X0)<=500&&abs(tempy-Pen_Point.Y0)<=500)return 0;//滑动距离最少要大于500
	else if(Pen_Point.xfac!=0)//已经校准过了
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;

		tempx=Pen_Point.xfac*tempx+Pen_Point.xoff;
		tempy=Pen_Point.yfac*tempy+Pen_Point.yoff;
	}
		
	if(abs(ml-mr)>abs(mu-md))//数量 满足
	{
		if(abs(tempx-Pen_Point.X0)>abs(tempy-Pen_Point.Y0))//质量满足
		{
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT; 
		}else						//质量不满足
		{
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;
		}
	}else
	{
		if(abs(tempy-Pen_Point.Y0)>abs(tempx-Pen_Point.X0))//质量满足
		{	    
			if(tempy>Pen_Point.Y0)return MOVE_DOWN;
			else return MOVE_UP;			 
		}else						//质量不满足
		{	  
			if(tempx>Pen_Point.X0)return MOVE_RIGHT;
			else return MOVE_LEFT;
		}
	}   	  
}




//判断触点是不是在指定区域之内
//(x1,y1):起始坐标
//(x2,y2):结束坐标
//返回值 :1,在该区域内.0,不在该区域内.
u8 Is_In_Area(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(Pen_Point.X<=x2&&Pen_Point.X>=x1&&Pen_Point.Y<=y2&&Pen_Point.Y>=y1)return 1;
	else return 0;
}  

//把触摸屏得到的数据转换为数字(按钮值)
//M_Type:按钮模式;
//0,系统菜单	
//1,文件浏览模式 	
//2,时间模式下,按键的获取,连按支持! 
//3,音乐播放模式下
//4,功能选择模式下
//5,收音机模式下
//6,闹钟中断程序按键			  
//返回值:对应的按键值(1~n)
//返回0,则表示此次按键无效  
u8 Touch_To_Num(u8 M_Type)
{
	u16 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://系统菜单下
			xtemp=Touch_Key_Pro(1);//不使用扩展按钮
			if(!(xtemp&CLICK_POINT))return 0;//不是点触直接退出
			ytemp=30;
			for(t=0;t<12;t++)
			{
				xtemp=135*(t%6);
				//if(t>5)ytemp=216;
				if((t==6)||(t>6))ytemp=283;
				if(Is_In_Area(xtemp,ytemp,xtemp+110,ytemp+140))
				{
				   Pen_Point.X=0;
	               Pen_Point.Y=0;
				//   delay_ms(100);
				   return t+1;//返回按键值 

				}
			}
			break;
		case 1://1,文件浏览模式
	
			xtemp=Touch_Key_Pro(1);//不使用扩展按钮	 
			if(!(xtemp&CLICK_POINT))return 0;
			if(Is_In_Area(0,0,799,479))return 22;
			break; 
		case 2://2,时间模式下,按键的获取,连按支持!
		

			xtemp=Touch_Key_Pro(1);//不扩展
			if(!(xtemp&CLICK_POINT))return 0;
			if(Is_In_Area(654,40,728,117))return 13;
			
			if(Is_In_Area(132,202,192,262))return 20;
			if(Is_In_Area(132,362,192,422))return 21;
		    if(Is_In_Area(232,202,292,262))return 22;
			if(Is_In_Area(232,362,292,422))return 23;
			if(Is_In_Area(332,202,392,262))return 24;
			if(Is_In_Area(332,362,392,422))return 25;
			if(Is_In_Area(432,202,492,262))return 26;
			if(Is_In_Area(432,362,492,422))return 27;
			if(Is_In_Area(532,202,592,262))return 28;
			if(Is_In_Area(532,362,592,422))return 29;
			if(Is_In_Area(665,362,728,425))return 30;


			break;
		case 3://MP3界面下面的按钮值
			xtemp=Touch_Key_Pro(1);//不扩展
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(214,72,240,96))return 1;//循环模式更改	    
				else if(Is_In_Area(87,103,153,119))return 2; //supper bass
				else if(Is_In_Area(8,223,56,247))return 3;   //音效存/取
				else if(Is_In_Area(64,223,112,247))return 4; //音量
				else if(Is_In_Area(120,223,168,247))return 5;//其他
				else if(Is_In_Area(178,224,204,276))return 6;//回车	 
				else if(Is_In_Area(214,224,238,248))return 7;//增加	    
				else if(Is_In_Area(214,252,238,276))return 8; //减少
					
				else if(Is_In_Area(28,288,52,312))return 9;	  //后一取
				else if(Is_In_Area(108,288,132,312))return 10;//播放
				else if(Is_In_Area(188,288,212,312))return 11;//上一曲

				else if(Is_In_Area(30,83,210,98))return 12;	  //进度条按下,y方向扩大了一点范围
															   
				//else if(Is_In_Area(0,300,40,319))return 13;//选择按钮被按下
				//else if(Is_In_Area(200,300,239,319))return 14;//返回按钮被按下
				return 0;//按键无效
			}
		case 4://4,功能选择模式下
			xtemp=Touch_Key_Pro(1);//不支持扩展	  
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				for(t=0;t<8;t++)
				{	  
					if(Is_In_Area(0,42+t*32,239,42+(t+1)*32))return t+1;
				}
				if(Is_In_Area(0,300,40,319))return 9;//选择按钮被按下
				if(Is_In_Area(200,300,239,319))return 10;//返回按钮被按下
				return 0;//按键无效
			}										    
			break;
		case 5://5,收音机模式下
			xtemp=Touch_Key_Pro(1);//不支持扩展	
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(37,215,61,239))return 1;//循环模式更改	    
				else if(Is_In_Area(93,215,117,239))return 2;//循环模式更改
				else if(Is_In_Area(152,218,203,236))return 3;   //音效存/取		 
					
				else if(Is_In_Area(28,261,52,285))return 4;	 //后一取
				else if(Is_In_Area(108,261,132,285))return 5;//播放
				else if(Is_In_Area(188,261,212,285))return 6;//上一曲
																		   
				else if(Is_In_Area(0,300,40,319))return 7;//选项按钮被按下
				else if(Is_In_Area(200,300,239,319))return 8;//返回按钮被按下
				return 0;//按键无效
			}  
			break; 
		case 6://6,闹钟中断程序按键
			xtemp=Touch_Key_Pro(1);//不支持扩展	
			if(xtemp&CLICK_POINT)  //点触摸	 得到1~8的值
			{
				if(Is_In_Area(111,151,153,169))return 1;//循环模式更改	       
				return 0;//按键无效
			}  
			break; 	  	    		 
	}
	return 0;
}			

//触摸屏校准代码
//得到四个校准参数
/*
void touch_adjust(void)
{								 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 
	 
	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	TFT_CLEAR(WHITE);//清屏 
	TFT_ShowString(24,110,"Touch Screen Adjusting..."); 
	delay_ms(1000);	    
	POINT_COLOR=RED;//红色 
	TFT_CLEAR(WHITE);//清屏 
	drow_touch_point(20,20);//画点1 
	Pen_Point.Key_Sta=Key_Up;//消除触发信号 
	Pen_Point.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//按键按下了
		{
			if(Touch_Key_Pro(1)&CLICK_POINT)//得到单次按键值
		//	read_once();
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;


			}			 
			switch(cnt)
			{			   
				case 1:
					TFT_CLEAR(WHITE);//清屏 
					TFT_Show4Num(100,100,pos_temp[cnt-1][0],16,1);
					TFT_Show4Num(100,150,pos_temp[cnt-1][1],16,1);
					
					drow_touch_point(460,20);//画点2
					break;
				case 2:
					TFT_CLEAR(WHITE);//清屏 
					TFT_Show4Num(100,100,pos_temp[cnt-1][0],16,1);
					TFT_Show4Num(100,150,pos_temp[cnt-1][1],16,1);
					drow_touch_point(20,252);//画点3
					break;
				case 3:
					TFT_CLEAR(WHITE);//清屏 
					TFT_Show4Num(100,100,pos_temp[cnt-1][0],16,1);
					TFT_Show4Num(100,150,pos_temp[cnt-1][1],16,1);
					drow_touch_point(460,252);//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					fac=(float)d1/d2;
				   	
					if(fac<0.80||fac>1.2)//不合格
					{
						cnt=0;
						TFT_CLEAR(WHITE);//清屏 
						TFT_Show4Num(100,100,pos_temp[3][0],16,1);
					TFT_Show4Num(100,150,pos_temp[3][1],16,1);
					
					   	TFT_Show4Num(100,10,d1,16,1);
					TFT_Show4Num(100,30,d2,16,1);
						TFT_Show4Num(100,50,1,16,1);

						drow_touch_point(20,20);
						continue;
					} 
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					fac=(float)d1/d2;
				   
					if(fac<0.80||fac>1.2)//不合格
					{
						cnt=0;
						TFT_CLEAR(WHITE);//清屏 
						TFT_Show4Num(100,100,pos_temp[3][0],16,1);
					TFT_Show4Num(100,150,pos_temp[3][1],16,1);
					
							TFT_Show4Num(100,10,d1,16,1);
					TFT_Show4Num(100,30,d2,16,1);
					   	TFT_Show4Num(100,50,2,16,1);
						drow_touch_point(20,20);
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					
				   

					if(fac<0.8||fac>1.2)//不合格
					{
						cnt=0;
						TFT_CLEAR(WHITE);//清屏 
						TFT_Show4Num(100,100,pos_temp[3][0],16,1);
					TFT_Show4Num(100,150,pos_temp[3][1],16,1);
						
						TFT_Show4Num(100,10,d1,16,1);
					TFT_Show4Num(100,30,d2,16,1);
						TFT_Show4Num(100,50,3,16,1);
						drow_touch_point(20,20);
						continue;
					}
					
				  	 

					//	delay_ms(200);

					//正确了
					//计算结果
					Pen_Point.xfac=(float)440/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					Pen_Point.xoff=(480-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					Pen_Point.yfac=(float)232/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					Pen_Point.yoff=(272-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					
					POINT_COLOR=BLUE;
					TFT_CLEAR(WHITE);//清屏
					TFT_ShowString(35,10,"Touch Screen Adjust OK!");//校正完成
				
					TFT_Show4Num(100,100,pos_temp[3][0],16,1);
					TFT_Show4Num(100,150,pos_temp[3][1],16,1);
					delay_ms(1800);
					delay_ms(1800);
					TFT_CLEAR(WHITE);//清屏   
				//	save_adjdata();
					return;//校正完成				 
			}
		}		 
	} 
}
*/ 

void touch_adjust(void)
{

  //Pen_Point.xfac= -0.20793951;
  u16 pos_temp[4][2];//坐标缓存值

  //pos_temp[0][0]=602;	//x1
  //pos_temp[1][0]=3854;	//x2

  //pos_temp[0][1]=3019;   //y1
  //pos_temp[2][1]=452;	//y3

  pos_temp[0][0]=3854;	//x1
  pos_temp[1][0]=225;	//x2

  pos_temp[0][1]=356;   //y1
  pos_temp[2][1]=3769;	//y3


 // pos_temp[0][0]=3619;	//x1
 // pos_temp[1][0]=252;	//x2

 // pos_temp[0][1]=602;   //y1
 // pos_temp[2][1]=3854;	//y3

  	                Pen_Point.xfac=(float)760/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					Pen_Point.xoff=(800-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					Pen_Point.yfac=(float)440/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					Pen_Point.yoff=(480-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff
 
 // Pen_Point.xfac= -0.225720;

 // Pen_Point.xoff= 836;
 // Pen_Point.yfac= 0.13530;
 // Pen_Point.yoff= -61;

//Pen_Point.X0=Pen_Point.xfac*Pen_Point.X0+Pen_Point.xoff;
//			Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y0+Pen_Point.yoff;

}
 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


