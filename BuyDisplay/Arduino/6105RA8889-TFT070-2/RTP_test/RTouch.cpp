 
#include "Arduino.h"
#include "RTouch.h"
#include "RTouchCD.h"
#include <SPI.h>

#define  T_IRQ    7
#define  T_CS     2




void UTouch::touch_WriteData(byte data)
{SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
    SPI.transfer(data);
	
}

word UTouch::touch_ReadData()
{SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
	word rec1=0;word rec2=0;
	rec1 = SPI.transfer(0);rec1<<=4;
	rec2 = SPI.transfer(0);rec2>>=4;
        rec1|=rec2 ;
	return(rec1);
}

UTouch::UTouch(byte t)
{

}


void UTouch::InitTouch(byte orientation)
{
	orient					= orientation;
	_default_orientation	= CAL_S>>31;
	touch_x_left			= (CAL_X>>14) & 0x3FFF;
	touch_x_right			= CAL_X & 0x3FFF;
	touch_y_top				= (CAL_Y>>14) & 0x3FFF;
	touch_y_bottom			= CAL_Y & 0x3FFF;
	disp_x_size				= (CAL_S>>12) & 0x0FFF;
	disp_y_size				= CAL_S & 0x0FFF;
	//prec					= 10;


    pinMode(T_CS,   OUTPUT);
    pinMode(T_IRQ,  OUTPUT);
    digitalWrite(T_CS, HIGH);
    digitalWrite(T_IRQ, HIGH);

	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

	SPI.begin();

}

void UTouch::read()
{       bool avail;
	unsigned long tx=0, temp_x=0;
	unsigned long ty=0, temp_y=0;
	unsigned long minx=99999, maxx=0;
	unsigned long miny=99999, maxy=0;
	int datacount=0;

	digitalWrite(T_CS, LOW);               
	pinMode(T_IRQ,  INPUT);
	for (int i=0; i<prec; i++)
	{avail = digitalRead(T_IRQ);
		if (avail ==0)
		{
			touch_WriteData(0x90);   

			temp_x=touch_ReadData();

			
			touch_WriteData(0xD0);    


			temp_y=touch_ReadData();


				if ((temp_x>0) and (temp_x<4096) and (temp_y>0) and (temp_y<4096))
				{
					tx+=temp_x;
					ty+=temp_y;
					if (prec>5)
					{
						if (temp_x<minx)
							minx=temp_x;
						if (temp_x>maxx)
							maxx=temp_x;
						if (temp_y<miny)
							miny=temp_y;
						if (temp_y>maxy)
							maxy=temp_y;
					}
					datacount++;
				}
			
		}
	}
	pinMode(T_IRQ,  OUTPUT);
	digitalWrite(T_IRQ, HIGH);
	if (prec>5)
	{
		tx = tx-(minx+maxx);
		ty = ty-(miny+maxy);
		datacount -= 2;
	}

	digitalWrite(T_CS, HIGH);                     
	if ((datacount==(prec-2)) or (datacount==PREC_LOW))
	{
		if (orient == _default_orientation)
		{
			TP_X=ty/datacount;
			TP_Y=tx/datacount;
		}
		else
		{
			TP_X=tx/datacount;
			TP_Y=ty/datacount;
		}
	}
	/*else
	{
		TP_X=-1;
		TP_Y=-1;
	}*/
}

bool UTouch::dataAvailable()
{ 
	bool avail;
	pinMode(T_IRQ,  INPUT);
        avail = digitalRead(T_IRQ);
	pinMode(T_IRQ,  OUTPUT);
	digitalWrite(T_IRQ, HIGH);
	return avail;
}

int16_t UTouch::getX()
{
	long c;

	if ((TP_X==-1) or (TP_Y==-1))
		return -1;
	if (orient == _default_orientation)
	{
		c = long(long(TP_X - touch_x_left) * (disp_x_size)) / long(touch_x_right - touch_x_left);
		if (c<0)
			c = 0;
		if (c>disp_x_size)
			c = disp_x_size;
	}
	else
	{
		if (_default_orientation == PORTRAIT)
			c = long(long(TP_X - touch_y_top) * (-disp_y_size)) / long(touch_y_bottom - touch_y_top) + long(disp_y_size);
		else
			c = long(long(TP_X - touch_y_top) * (disp_y_size)) / long(touch_y_bottom - touch_y_top);
		if (c<0)
			c = 0;
		if (c>disp_y_size)
			c = disp_y_size;
	}
	return c;
}

int16_t UTouch::getY()
{
	int c;

	if ((TP_X==-1) or (TP_Y==-1))
		return -1;
	if (orient == _default_orientation)
	{
		c = long(long(TP_Y - touch_y_top) * (disp_y_size)) / long(touch_y_bottom - touch_y_top);
		if (c<0)
			c = 0;
		if (c>disp_y_size)
			c = disp_y_size;
	}
	else
	{
		if (_default_orientation == PORTRAIT)
			c = long(long(TP_Y - touch_x_left) * (disp_x_size)) / long(touch_x_right - touch_x_left);
		else
			c = long(long(TP_Y - touch_x_left) * (-disp_x_size)) / long(touch_x_right - touch_x_left) + long(disp_x_size);
		if (c<0)
			c = 0;
		if (c>disp_x_size)
			c = disp_x_size;
	}
	return c;
}

void UTouch::setPrecision(byte precision)
{
	switch (precision)
	{
		case PREC_LOW:
			prec=1;		// DO NOT CHANGE!
			break;
		case PREC_MEDIUM:
			prec=12;	// Iterations + 2
			break;
		case PREC_HI:
			prec=27;	// Iterations + 2
			break;
		case PREC_EXTREME:
			prec=102;	// Iterations + 2
			break;
		default:
			prec=12;	// Iterations + 2
			break;
	}
}

void UTouch::calibrateRead()
{
	unsigned long tx=0;
	unsigned long ty=0;

	digitalWrite(T_CS, LOW);                 

	touch_WriteData(0x90);        

	tx=touch_ReadData();

	touch_WriteData(0xD0);      

	ty=touch_ReadData();

	digitalWrite(T_CS, HIGH);                 

	TP_X=ty;
	TP_Y=tx;
}

