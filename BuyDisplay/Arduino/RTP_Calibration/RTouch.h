

#ifndef RTouch_h
#define RTouch_h



// *** Hardwarespecific defines ***
#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask
#define rbi(reg, bitmask) ((*reg) & bitmask)

#define pulse_high(reg, bitmask) sbi(reg, bitmask); cbi(reg, bitmask);
#define pulse_low(reg, bitmask) cbi(reg, bitmask); sbi(reg, bitmask);

#define swap(type, i, j) {type t = i; i = j; j = t;}

#define regtype volatile uint8_t
#define regsize uint8_t

#include "Arduino.h"
#include <SPI.h>

#define PORTRAIT			0
#define LANDSCAPE			1

#define PREC_LOW			1
#define PREC_MEDIUM			2
#define PREC_HI				3
#define PREC_EXTREME		      4

class UTouch
{ 
	public:
		int16_t	TP_X ,TP_Y;

				UTouch(byte t);

		void	InitTouch(byte orientation = LANDSCAPE);
		void	read();
		bool	dataAvailable();
		int16_t	getX();
		int16_t	getY();
		void	setPrecision(byte precision);

		void	calibrateRead();
    
    private:
	//	regtype *P_CLK, *P_CS, *P_DIN, *P_DOUT, *P_IRQ;
	//	regsize B_CLK, B_CS, B_DIN, B_DOUT, B_IRQ;
	//	byte	T_CLK, T_CS, T_DIN, T_DOUT, T_IRQ;
		long	_default_orientation;
		byte	orient;
		byte	prec;
		byte	display_model;
		long	disp_x_size, disp_y_size, default_orientation;
		long	touch_x_left, touch_x_right, touch_y_top, touch_y_bottom;

		void	touch_WriteData(byte data);
		word	touch_ReadData();
};

#endif
