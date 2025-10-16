#ifndef FT5316_HPP
#define FT5316_HPP

#include <stdbool.h>
#include <Wire.h>
#include <FT5316_reg.hpp>

//Controle de depuração, descom ente apra mostrar os dados na porta serial
#define DEBUG_TOUCH


/*
// порты
#define	FT5316_RSTN_PORT	GPIOD
#define	FT5316_RSTN_PIN		GPIO_Pin_4

#define	FT5316_INT_PORT		GPIOD
#define	FT5316_INT_PIN		GPIO_Pin_5

#define	FT5316_RSTN_RESET	GPIO_ResetBits(FT5316_RSTN_PORT,	FT5316_RSTN_PIN)
#define	FT5316_RSTN_SET		GPIO_SetBits(FT5316_RSTN_PORT,		FT5316_RSTN_PIN)

#define	FT5316_INT_READ		!GPIO_ReadInputDataBit(FT5316_INT_PORT,	FT5316_INT_PIN)


//#define CONFIG_FT5X0X_MULTITOUCH    //Define the multi-touch
//Touch Status Event 
#define PUT_DOWN    0x00
#define PUT_UP      0x01
#define PUT_CONTACT 0x02


struct _ts_event
{
    uint    x1;
    uint    y1;
    uint    x2;
    uint    y2;
    uint    x3;
    uint    y3;
    uint    x4;
    uint    y4;
    uint    x5;
    uint    y5;
    uchar   touch_Point;
	  uchar   touch_Status;
};


#define FT5316_ADDRESS 0x38

#define WRITE_ADD	0x70
#define READ_ADD	0x71


void Begin(void);
void FT5316_Wr_Reg(uchar RegIndex, uchar RegValue1);
uchar FT5316_Read_Reg(uchar RegIndex);
bool FT5316_isInterrupt(void);
uchar FT5316_touchDataRead(void);
bool FT5316_sampleTouch(unsigned short* x, unsigned short* y);

*/




// Enum fortemente tipado para os eventos de toque
enum class TouchEvent : uint8_t {
    Down  = 0,
    Up    = 1,
    Move  = 2,
    Unknown = 3
};


struct TouchPoint {
    uint16_t x;         //Touch pos x
    uint16_t y;         //Touch pos y
    uint16_t id;        //Touch ID of Touch Point bit [3:0]
    TouchEvent status;  // agora só aceita valores do enum
};


// Definição do tipo de callback
using TouchCallback = void (*)(const TouchPoint&);

class FT {
  private:
     bool WriteTouchRegister(uint8_t reg, uint8_t data);
     uint8_t ReadTouchRegister(uint8_t reg);

  public:
      FT(uint8_t intpin, uint8_t rstpin);
      void Begin(void);
      void setTouchArea(uint16_t width, uint16_t height);
      void AttachInterruptService(void);
      void DetachInterruptService(void);
      void setUserCallback(TouchCallback cb);
      void UserCallbackEnable(bool b);
      bool Touched();
      uint8_t getTouches(TouchPoint *pPoint, uint8_t num);
      uint8_t getTouches(uint8_t num);
      const TouchPoint& getTouch(uint8_t index) const;
      uint8_t TouchAddress(uint8_t reg, uint8_t *pBuf, uint8_t len);
      void Reset(void);
	  void Poll(void);
	  bool SampleTouch(uint8_t index, uint16_t *x, uint16_t *y);
	  void setMultitouch(bool enable) { _allowMultitouch = enable; }
      bool getMultitouch() const { return _allowMultitouch; }
  protected:
      uint16_t _width;         //Total de pontos na horizontal da tela de toque
      uint16_t _height;        //Total de pontos na vertical da tela de toque
      bool _settoucharea;
      uint8_t _maxmultitouch;  //Mximum multitouch allowed for this microcontroler
      uint8_t _ctpintpin;      //Touch interrupt pin
      uint8_t _ctprstpin;      //Reset pin
      uint8_t _addr;           //Touch I2C Adrress
      TouchPoint touchPoints[FT_MAX_TOUCHES];
      uint8_t _touchcount;     
      bool _allowMultitouch = false;                 // true = multitouch, false = apenas single
      volatile bool _newtouch;
      volatile bool _usercallbackenable;             //enable/disable callback interrupt user, sem perder o ponteiro do callback
      TouchCallback _UserCallback = nullptr;         //Interrupt Service Request user (function pointer)
      
      static FT* _instance;  // ponteiro estático para roteamento

      uint32_t Dist(const TouchPoint &loc);
      uint32_t Dist(const TouchPoint &loc1, const TouchPoint &loc2);
      bool SameLoc(const TouchPoint &loc, const TouchPoint &loc2, uint32_t aproach);
      TouchEvent ToPointEvent(uint8_t status);
	  void InternalInterrupt(void);   // Função interna chamada pela ISR
      static void isrRouter(void);     //
};

#endif //fim do include guard FT5316_HPP