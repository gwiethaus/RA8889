/*
Solucao do problema
https://community.particle.io/t/cpp-attachinterrupt-to-class-function-help-solved/5147/2


----------------
As I don't fully understand the technicalities of programming nor their terms, I'll give more of a conceptual overview of what I'm aiming for.

I'm currently switch all my structured code to it's object equivalent - partly as a learning exercise in to an area I don't know so well.

Regarding the interrupts specifically, I want to have only 2 instances; i) to count pulses, ii) to count 1 second. The way I have been trying to instantiate these two is by name during setup in main.cpp. Things like this:

Pulse pulse0(2, pulseTime);  // interruptPin, interruptFunction
Pulse pulse0(2, void (*pulseTime)());
I'm not afraid to admit, I'm feeling pretty lost at the moment, I could go on to explain various things I've tried, but mostly it's been guesses so not worth posting.

... I want a very clean OO way to create an instance of an interrupt (of which there will only ever be 2) by passing the interrupt member to call as an argument, if this is even possible; something like this constructor:

pulse.cpp

Pulse::Pulse(int pulsePin,  void (*pulseFunc)()) {
and pulse.h

Pulse(int pulsePin, void (*)());
-------------------
  erro: uso inválido da função membro não estática 'void Button::buttonisr()'

O compilador está dizendo que buttonisr é uma função membro da classe. Você não pode chamá-la diretamente, você deve chamá-la com um ponteiro this para o objeto relevante. A função de interrupção precisa de um endereço de memória estático.

Em resumo: você não pode fazer isso da maneira que deseja.

Existe uma solução alternativa se você criar a classe como um modelo usando o número do pino e definir o ISR como uma função estática. Dessa forma, não há necessidade de um ponteiro this , e cada pino recebe sua própria função exclusiva. A desvantagem é que você não pode definir instâncias em tempo de execução.

Exemplo:

template<int Pin>
class Foo {
public:
    static void isr() {
        buttonPressed = true;
    }

    void setup() {
        ::attachInterrupt(digitalPinToInterrupt(Pin), reinterpret_cast<void(*)()>(isr), CHANGE);
    }

private:
    static volatile bool buttonPressed;
}
Aqui está uma biblioteca construída com essa técnica (https://github.com/dmadison/ServoInput/blob/master/src/ServoInput.h) se você quiser dar uma olhada. Lembre-se de que você não pode definir modelos no arquivo .ino devido à estranheza do Arduino, você precisa criar um cabeçalho (.h).
comnets:
Não deveria haver necessidade de conversão, pois o tipo de isr já é void(*)(). Uma desvantagem da conversão é que ela irá encobrir um bug como adicionar argumentos incorretamente para isr() ou dar a ele um valor de retorno não vazio.
O uso de um template poderia ser evitado, se alguém apenas codificasse (ou seja, #define) um pino específico para usar. Claro, se alguém quiser vários botões, então o template é melhor para que você não precise escrever várias cópias da mesma classe.
Mas também tenha em mente que, ao escrever um template e depois criar Button<1>, Button<2>, etc., você só escreveu o código uma vez, o compilador precisa emitir a saída compilada para cada instanciação. Haverá uma cópia de cada função em Button para cada instanciação diferente de Button, mesmo que a função membro não se importe com o valor que Pin tem e cada cópia seja, na verdade, exatamente o mesmo código compilado.
-------------------------

Nota: milli(). Serial.print() nao funcionam dentro das itnerrupções porque usam o clock para gerar a cotangem destas funcoes e isso apra porque mas itnerrupções são desativadas quando entra numa ISR.
o serial.print usa as interrupções para pegar o rpoximo caracter epla serial apor chegar no buffer, como as itenrrupçoes estao desabilitadas neste momento nunca chegará mais caracteres ate que a ISR termine.
Entao semrep que for usar alguma função de uma classe que dependa do clock, e necessita de itenrrupções n~çao será possivel ate´que a rotina ISR de itnerrupção atual termine.
se for usar uma variavel global de uma classe para dentro de uma isr, declare ela sempre como volatile ou static volatile

------------------------------

*/

#include <Arduino.h>
#include <Wire.h>
#include <FT5316.hpp>
#include <Debug.hpp>
  
/*
static struct _ts_event ts_event = {0};

static void FT5316_RdParFrPCTPFun(uchar *PCTP_Par,uchar ValFlag)
{  
  I2Cx_readDataBurst(I2C1, FT5316_ADDRESS, 0x00, ValFlag, PCTP_Par);
}


bool FT5316_isInterrupt(void)
{
 	if(FT5316_INT_READ)      //Detect the occurrence of an interrupt
 	{
		ts_event.Key_Sta = KEY_DOWN;
    return true;
 	}
  return false;
}


void FT5316::Begin(void)
{
  FT5316_reset();
}


void FT5316_Wr_Reg(uchar RegIndex, uchar RegValue1)
{  
  I2Cx_writeData(I2C1, FT5316_ADDRESS, RegIndex, RegValue1);
  
	delay_us(100);
}


uchar FT5316_Read_Reg(uchar RegIndex)
{
  uchar receive=0;
 	
  receive = I2Cx_readData(I2C1, FT5316_ADDRESS, RegIndex);
	return receive;
}


uchar FT5316_touchDataRead(void)
{
  uchar buf[32] = {0}; uchar ret = 0;

	#ifdef CONFIG_FT5X0X_MULTITOUCH
		FT5316_RdParFrPCTPFun(buf, 31);
	#else
  	FT5316_RdParFrPCTPFun(buf, 7);
	#endif

  ts_event.touch_point = buf[2] & 0xf;

  if (ts_event.touch_point == 0) 
	{
		return 0;
  }

	#ifdef CONFIG_FT5X0X_MULTITOUCH
	switch (ts_event.touch_point) 
	{
    case 5:
      ts_event.x5 = (uint)(buf[0x1b] & 0x0F)<<8 | (uint)buf[0x1c];
      ts_event.y5 = (uint)(buf[0x1d] & 0x0F)<<8 | (uint)buf[0x1e];
    
    case 4:
      ts_event.x4 = (uint)(buf[0x15] & 0x0F)<<8 | (uint)buf[0x16];
      ts_event.y4 = (uint)(buf[0x17] & 0x0F)<<8 | (uint)buf[0x18];
    
    case 3:
      ts_event.x3 = (uint)(buf[0x0f] & 0x0F)<<8 | (uint)buf[0x10];
      ts_event.y3 = (uint)(buf[0x11] & 0x0F)<<8 | (uint)buf[0x12];
    
    case 2:
      ts_event.x2 = (uint)(buf[9] & 0x0F)<<8 | (uint)buf[10];
      ts_event.y2 = (uint)(buf[11] & 0x0F)<<8 | (uint)buf[12];
    
    case 1:
      ts_event.x1 = (uint)(buf[3] & 0x0F)<<8 | (uint)buf[4];
      ts_event.y1 = (uint)(buf[5] & 0x0F)<<8 | (uint)buf[6];
      break;
    
    default:
      return 0;
	}
	#else
	if (ts_event.touch_point == 1)
	{
	 	ts_event.x1 = (uint)(buf[3] & 0x0F)<<8 | (uint)buf[4];
	 	ts_event.y1 = (uint)(buf[5] & 0x0F)<<8 | (uint)buf[6];
	 	ret = 1;
	}
	else
	{
    ts_event.x1 = 0xFFFF;
	 	ts_event.y1 = 0xFFFF;
	 	ret = 0;
	}
	#endif
  
	return ret;
}



bool FT:SampleTouch(uint16_t *x, uint16_t *y)
{
  *x = -1*(ts_event.x1-1023);
  *y = -1*(ts_event.y1-599);
  if(ts_event.Key_Sta == KEY_DOWN)
  {
    ts_event.Key_Sta = KEY_UP;
    return true;
  }
  return false;
}

*/

//Somente permite uma instancia de objeto

// Definição da instância (somente aqui!)
FT* FT::_instanceft = nullptr;

FT::FT(uint8_t sdapin, uint8_t sclpin, uint8_t intpin, uint8_t rstpin)
{
  _ctp_intpin = intpin;
  _ctp_rstpin = rstpin;
  _ctp_sdapin = sdapin;
  _ctp_sclpin = sclpin;
  _ctp_addr = FT_I2C_ADDRESS;
  _maxmultitouch = FT5316_MAX_TOUCHES;
  _newtouch = false;
  _touchcount = 0;
  _useinterrupt = false;
  _interrupt_enabled = false;
  _allowmultitouch = false;
  _numtouchesallow = 1;
  _UserCallback = nullptr;
  _callbackenable = false;
  _instanceft = this;                          //Salva esta instância
  _width = 0;                                  //Total de pontos na horizontal da tela de toque
  _height = 0;                                 //Total de pontos na vertical da tela de toque
  _settoucharea = false;                       //configurou a area de toque
  _started = false;
}


FT::~FT(void) 
{
  #ifdef DIGITALPINTOINTERRUPT
    detachInterrupt(digitalPinToInterrupt(_ctp_intpin));
  #else
    detachInterrupt(_ctp_intpin);
  #endif
};


/**
 * @brief Lê um registrador do dispositivo I2C de toque (FT5316).
 *
 * Esta função envia o endereço do registrador ao controlador de toque e
 * lê 1 byte de resposta via I2C.
 *
 * @param reg Endereço do registrador a ser lido.
 *
 * @return Valor do registrador lido. Retorna 0 se ocorrer erro de transmissão
 *         ou se nenhum byte foi recebido.
 *
 * @code
 * uint8_t val = ReadRegister(0x02); // Lê o registrador de status FT_TD_STATUS
 * Serial.print("Valor do registrador 0x02: ");
 * Serial.println(val);
 * @endcode
 */
uint8_t FT::ReadRegister(uint8_t reg)
{
  Wire.beginTransmission(_ctp_addr);
  Wire.write(reg);                                             //Register
  if (Wire.endTransmission() != 0) return 0;                   //Falha na transmissão
  uint8_t returned = Wire.requestFrom(_ctp_addr, (uint8_t)1 ); //Solicita 1 byte do registrador
  if (returned < 1) return 0;                                  //Nenhum byte recebido
  return (uint8_t)Wire.read();
}


/**
 * @brief Escreve um valor em um registrador do dispositivo I2C de toque.
 *
 * @param reg Endereço do registrador.
 * @param val Valor a ser escrito.
 * 
 * @return true se a escrita foi bem-sucedida, false caso contrário.
 */
bool FT::WriteRegister(uint8_t reg, uint8_t data)
{
  uint8_t retVal = 0xff;
  if (_ctp_addr != 0) {
    Wire.beginTransmission(_ctp_addr);
    Wire.write(reg);                  // register 0
    Wire.write(data);                 // value
    retVal = Wire.endTransmission();
  }
  return retVal == 0;                 // true se transmissão OK
}


/**
 * @brief Lê múltiplos registradores sequenciais do FT5316 a partir de um endereço inicial (registrador).
 *
 * Esta função envia o endereço inicial de um registrador do touch controller
 * e em seguida solicita a leitura de múltiplos bytes subsequentes, que serão
 * armazenados no buffer fornecido.
 *
 * @param reg Endereço do registrador inicial (por exemplo, 0x03 = TOUCH1_XH).
 * @param pBuf Ponteiro para o buffer de destino onde os bytes serão armazenados.
 * @param len Número de bytes a serem lidos (deve ser múltiplo de 6 se for capturar toques completos).
 *
 * @return Número de bytes efetivamente lidos.
 *
 * @note Cada toque ocupa 6 bytes (XH, XL, YH, YL, WEIGHT, MISC).
 *       Para o caso de outra aplicacao de leitura de multiplos bytes, len é um numero total de registrdores a serem lidos
 *       e armazenados em pBuf. pBuf deve ter tamanho suficiente apra armazenar a quantidade de bytes requisitados.
 *       Para uma saida efetiva de numero de multitoques basta fazer o calculo (num_multitouch = bytesRead / 6).
 *
 * @code
 * static uint8_t tbuf[30];
 * uint8_t hitPoints = readFT5316TouchRegister(0x02) & 0x0F;
 * uint8_t bytesRead = TouchAddress(0x03, tbuf, hitPoints * 6);
 * if (bytesRead >= 6) {
 *   // Processar coordenadas manualmente se necessário
 * }
 * @endcode
 *
 */
uint8_t FT::TouchAddress(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
  uint8_t i = 0;
  Wire.beginTransmission(_ctp_addr);
  Wire.write(reg);                                             //Pega o primeira registrador
  uint8_t retVal = Wire.endTransmission();
  uint8_t returned = Wire.requestFrom(_ctp_addr, len);         //request 1 bytes from slave device #2 dos proximos registradores
  while (Wire.available()) {                                   //Verifica bytes disponiveis
    if (i < len) {                                                
      pBuf[i] = Wire.read();                                   //Leia todos os bytes de toques para o buffer
    } else break; 
    i++;                                                        //proximo byte
  }
  return returned;                                              //número de bytes lidos
}


//Realiza a leitura de todos os registradores de toque
//a leitura alimenta a matriz de dados interno 
//Alimenta todos os registradores de toques touchPoints[]
//void FT::ReadDataTouchRaw()
//{
//  _touchcount = ReadRegister(FT_TD_STATUS) & 0x0f;                       // 0x02, Frame remaining or number of events (Win7 protocol)[7:4], Number of touch points[3:0]
//  static uint8_t tbuf[FT5316_TOUCH_DATA_SIZE];                           // buffer temporário para leitura dos registros (6 bytes por toque) 
//  uint8_t bytesRead = TouchAddress(0x03, tbuf, FT5316_TOUCH_DATA_SIZE);  // 0x03 = TOUCH1_XH
//
//  // Atualiza array interno
//  for (uint8_t i = 0; (i < FT5316_MAX_TOUCHES); i++) {
//    touchPoints[i].x      = ((tbuf[i+0] & 0x0F) << 8) | tbuf[(i+1)*FT5316_TOUCH_ENTRY];       //Coordenada X 
//    touchPoints[i].y      = ((tbuf[i+2] & 0x0F) << 8) | tbuf[(i+3)*FT5316_TOUCH_ENTRY];       //Coordenada Y 
//    touchPoints[i].event  = ToPointEvent((tbuf[(i+0)*FT5316_TOUCH_ENTRY] & 0xC0) >> 6);       //Event Flag
//    touchPoints[i].id     = (tbuf[(i+2)*FT5316_TOUCH_ENTRY] & 0xF0) >> 4;                     //ID hardware fornece
//    touchPoints[i].weight = tbuf[(i+4)*FT5316_TOUCH_ENTRY];                                   //Pressao na tela
//  }
//}



//Realiza a leitura somente de numero de toques permitidos dos registradores de toque
//a leitura alimenta a matriz de dados interno 
//Alimenta todos os registradores de toques touchPoints[]
//void FT::ReadDataTouch(uint8_t num_touch)
//{
//	
//}





/**
 * @brief Inicia a tela de toque Serie FT5x06 e FT5x16
 *
 * @param uint8_t addr: Endereço do dispositivo I2C conectado ao MCU
 * 
 * @return true, sucesso
 * 
 * @note None
 *
 * @code
 * None
 * @endcode
 * 
 */
bool FT::Begin(uint8_t addr)
{
  _ctp_addr = addr;
  
  //Faz as devidas verificacoes antes de habilitar as interrupções
  
  if (_ctp_sdapin == 0 || _ctp_sclpin == 0)    //usa o pinos padrao 
    Wire.begin();
  else {                                        //pinos definidos pelo usuario
    Wire.begin(_ctp_sdapin, _ctp_sclpin);      
  }
  Wire.setClock(400000UL);                       //Set I2C frequency to 400kHz
  delay(10);
  
  // Verifica se o pino de interrupção é válido, mas NÃO ativa ainda
  if (_ctp_intpin != 0xff) {
    pinMode(_ctp_intpin, INPUT_PULLUP);
  }
  
  if (_ctp_rstpin != 0xff) {
    pinMode(_ctp_rstpin, OUTPUT);
    Reset();
  }

//  if (readRegister8(FT6X36_REG_PANEL_ID) != FT6X36_VENDID)
//		return false;	

  //Verifica se esta tudo Ok
  Wire.beginTransmission(_ctp_addr);
  bool sucess = (Wire.endTransmission() == 0);
  DEBUG_PRINTD("Deu bom FT::Begin(void) (0 ruim, 1 bom)? ", sucess, true, 0, true);         //Debug

  WriteRegister(FT_ID_G_THGROUP, 0x16);             //0x80               
  WriteRegister(FT_ID_G_THPEAK, 0x3c);              //0x81
  WriteRegister(FT_ID_G_THCAL, 0xe9);               //0x82
  WriteRegister(FT_ID_G_COMPENSATE_STATUS, 0x01);   //0x83
  WriteRegister(FT_D_G_COMPENSATE_FLAG, 0x01);      //0x84
  WriteRegister(FT_ID_G_THDIFF, 0xa0);              //0x85
  WriteRegister(FT_ID_G_CTRL, 0x0a);                //0x86
  WriteRegister(FT_ID_G_TIME_ENTER_MONITOR, 0x06);  //0x87
  WriteRegister(FT_ID_G_PERIODACTIVE, 0x28);        //0x88
  WriteRegister(FT_DEVIDE_MODE, 0x00);              //0x00
  
  DEBUG_PRINTD("Entrou FT::Begin(void)", 0, false, 0, true);         //Debug

  return sucess;
}


/**
 * @brief Hardware Reset
 *
 * @verbatim
 * No Modulo de display do RA8889 o datasheed diz o seguinte:
 * Pin 36: External low signal reset the chip. RC reset circuit on Board, this pin can be left unconnected.
 * 
 * @param None
 * 
 * @return None
 *
 */
void FT::Reset(void)
{
  if (_ctp_rstpin == 0) return;
  digitalWrite(_ctp_rstpin, HIGH);//on
  delay(5);
  digitalWrite(_ctp_rstpin, LOW);//on
  delay(750);
  digitalWrite(_ctp_rstpin, HIGH);//on
}


/**
 * @brief Configura a area de toque
 *
 * @param width numero pontos na horizontal
 * @return height numero pontos na vertical
 *
 * @code
 * setTouchArea(800, 480);
 * 
 * @endcode
 */
void FT::setTouchArea(uint16_t width, uint16_t height)
{
  _width = width;
  _height = height;
  _settoucharea = (_width > 0) && (_height > 0);
}


/**
 * @brief Converte valor de evento cru
 *
 * @param status
 * 
 * @return TouchEvent
 */
TouchEvent FT::ToPointEvent(uint8_t event)
{
  switch (event) {
    case 0x00:  return TouchEvent::Press;
    case 0x01:  return TouchEvent::Release;
    case 0x02:  return TouchEvent::Change;
    default:    return TouchEvent::Unknown;
  }	
}


/**
 * @brief Lê múltiplos pontos de toque do FT5316 usando um array externo
 *
 * Esta função utiliza o registrador de status para determinar quantos pontos de toque
 * estão ativos e em seguida lê as coordenadas (x,y) de até @p num pontos simultâneos.
 * O resultado é armazenado em uma matriz de estruturas TouchPoint fornecida pelo usuário.
 *
 * @param pPoint Ponteiro para uma matriz de estruturas TouchPoint onde as coordenadas serão gravadas.
 * @param num Número máximo de toques que serão tratados (máximo suportado pelo FT5316 é 5).
 *
 * @return Número de toques efetivamente lidos e armazenados em pLoc.
 *
 * @note Cada ponto de toque ocupa 6 bytes no registrador do FT5316.
 *       O retorno pode ser menor que num se menos toques foram detectados.
 *
 * @code
 * struct TouchPoint {
 *   uint16_t x;         //Touch pos x
 *   uint16_t y;         //Touch pos y
 *   uint16_t id;        //Touch ID of Touch Point bit [3:0]
 *   TouchEvent status;  // agora só aceita valores do enum
 * };
 *
 * TouchPoint touchPoints[5];
 * uint8_t touches = getTouches(touchPoints, 5);
 *
 * if (touches > 0) {
 *   for (uint8_t i = 0; i < touches; i++) {
 *     Serial.print("Toque "); Serial.print(i);
 *     Serial.print(": X="); Serial.print(touchPoints[i].x);
 *     Serial.print(" Y="); Serial.println(touchPoints[i].y);
 *   }
 * }
 * @endcode
 * 
 * @note é removido _touchcount da função pois deve ser avaliado que houve um pressionamento inctementando touchcount e se houve uma liberação, decrementando touchcount.
 */ 
uint8_t FT::getTouches(TouchPoint *tpoint)
{
  uint8_t i = 0;
  uint8_t k = 0;

  if (!tpoint) {_touchcount = 0; return 0;}                             //must have a buffer and be able to take at least one

  uint8_t hitPoints = ReadRegister(FT_TD_STATUS) & 0x0F;                 //0x02, bits [3:0], lê número de toques ativos
  if (!_allowmultitouch && hitPoints > 1) hitPoints = 1;                 //Se multitouch estiver desabilitado, força apenas 1 toque

  static uint8_t tbuf[FT5316_TOUCH_DATA_SIZE];                           //buffer temporário para leitura dos registros (6 bytes por toque) 
  uint8_t bytesRead = TouchAddress(0x03, tbuf, FT5316_TOUCH_DATA_SIZE);  //0x03 = TOUCH1_XH
  if (bytesRead < FT5316_TOUCH_DATA_SIZE) {                              // Evita deixar o sistema com estado inconsistente, “liberações rápidas” (flicks ou toques muito curtos)
    _touchcount = 0;
    _newtouch = false;
    return 0;
  }

  uint8_t validTouches = 0;                                              //Contador toques válidos
  for (k = 0, i = 0; (k < _numtouchesallow); k++, i += FT5316_TOUCH_ENTRY) { //Pega a coordenada x,y de cada toque de tela simultâneo
    uint8_t ev            = (tbuf[i+0] & 0xC0) >> 6;                     //Eventos de Touch
	tpoint[k].x           = ((tbuf[i+0] & 0x0f) << 8) | tbuf[i+1];       //Coordenada X
    tpoint[k].y           = ((tbuf[i+2] & 0x0f) << 8) | tbuf[i+3];       //Coordenada Y
    tpoint[k].id          = (tbuf[i+2] & 0xF0) >> 4;                     //ID hardware fornece
    tpoint[k].weight      = tbuf[i+4];                                   //Pressao na tela
    tpoint[k].event       = ToPointEvent(ev);                            //Event Flag
    if (tpoint[k].event != TouchEvent::Unknown) validTouches++;          //Conta apenas eventos válidos (Press, Release, Move/Change)
  }

  //_touchcount = validTouches;                                            //Atualiza quantidade de toques ativos
  return validTouches;
}


/**
 * @brief Atualiza e retorna todos os toques ativos usando o array interno protegido.
 *
 * @verbatim
 * Limita os numero de toques através das veriaveis:
 *   
 *   > _touchcount → conta quantos toques ativos ainda não “consumidos” no 
 *     sistema.
 *   > _newtouch → indica se há algum toque novo detectado desde a 
 *     última leitura ou processamento.
 * 
 * Eles sempre refletem apenas os toques que estão sendo efetivamente 
 * tratados, ou seja, limitados pelo _numtouchesallow e _allowMultitouch.
 *
 * for (k = 0, i = 0; k < _numtouchesallow; k++, i += FT5316_TOUCH_ENTRY) { 
 *   ...
 * }
 * Ou seja:
 *  _numtouchesallow controla quantos índices do array touchPoints[] serão processados.
 * Se _numtouchesallow = 1 (multitouch desativado), apenas o primeiro toque será avaliado.
 * Mesmo que o hardware detecte 5 dedos, _touchcount só será incrementado para os toques que realmente passaram pelo loop.
 * 
 * @endverbatim
 * 
 * @param None
 * 
 * @return Número de toques detectados
 *
 * @code
 * touch.
 * touch.setNumTouches(5);
 * ...
 * uint8_t n = ft.getTouches();
 *
 * for(uint8_t i = 0; i < n; i++) {
 *     const TouchPoint& t = ft.getTouch(i);
 *     Serial.printf("Touch %u: x=%u, y=%u, id=%u\n", i, t.x, t.y, t.ID);
 * }
 * @endcode
 * 
 * @note é removido _touchcount da função pois deve ser avaliado que houve um pressionamento inctementando touchcount e se houve uma liberação, decrementando touchcount.
 */
uint8_t FT::getTouches()
{
  uint8_t i = 0;
  uint8_t k = 0;

  uint8_t hitPoints = ReadRegister(FT_TD_STATUS) & 0x0F;                 //0x02, bits [3:0], lê número de toques ativos

  if (!_allowmultitouch && hitPoints > 1) hitPoints = 1;                 //Se multitouch estiver desabilitado, força apenas 1 toque

  static uint8_t tbuf[FT5316_TOUCH_DATA_SIZE];                           //buffer temporário para leitura dos registros (6 bytes por toque) 
  uint8_t bytesRead = TouchAddress(0x03, tbuf, FT5316_TOUCH_DATA_SIZE);  //0x03 = TOUCH1_XH
  if (bytesRead < FT5316_TOUCH_DATA_SIZE) {                              // Evita deixar o sistema com estado inconsistente, “liberações rápidas” (flicks ou toques muito curtos)
    _touchcount = 0;
    _newtouch = false;
    return 0;
  }

  uint8_t validTouches = 0;                                              //Contador toques válidos
  for (k = 0, i = 0; (k < _numtouchesallow); k++, i += FT5316_TOUCH_ENTRY) {          //Atualiza array interno
    uint8_t ev            = (tbuf[i+0] & 0xC0) >> 6;                     //Eventos de Touch
	touchPoints[k].x      = ((tbuf[i+0] & 0x0F) << 8) | tbuf[i+1];       //Coordenada X
    touchPoints[k].y      = ((tbuf[i+2] & 0x0F) << 8) | tbuf[i+3];       //Coordenada Y
    touchPoints[k].id     = (tbuf[i+2] & 0xF0) >> 4;                     //ID hardware fornece
    touchPoints[k].weight = tbuf[i+4];                                   //Pressao na tela
    touchPoints[k].event  = ToPointEvent(ev);                            //Event Flag
    if (touchPoints[k].event != TouchEvent::Unknown) validTouches++;     //Conta apenas eventos válidos (Press, Release, Move/Change)
  }
  //_touchcount = validTouches;                                            //Atualiza quantidade de toques ativos
  return validTouches;
}


/**
 * @brief Verifica e consome um toque específico da tela de toque.
 * 
 * Esta função retorna as coordenadas de um toque específico (índice `num`) e
 * consome o evento, marcando o status do toque como `Up`. Se o toque for consumido,
 * o flag interno `_newtouch` também é resetado, sinalizando que não há toques novos
 * pendentes.
 * 
 * O índice do toque (`num`) é 1-based; ou seja, o primeiro toque é `num = 1`.
 * A função automaticamente converte para 0-based internamente.
 * 
 * @param index Número do toque a ser consultado (1 a _maxmultitouch).
 * @param x Ponteiro para armazenar a coordenada X do toque.
 * @param y Ponteiro para armazenar a coordenada Y do toque.
 *
 * @return true se houve toque Down consumido; false se nao tiver toques Down
 * 
 * @note As coordenadas X e Y são invertidas e ajustadas conforme a largura (_width)
 *       e altura (_height) da tela.
 * 
 * @code
 * uint16_t x, y;
 * if (ft.SampleTouch(1, &x, &y)) {
 *     Serial.printf("Toque detectado: x=%u, y=%u\n", x, y);
 * }
 * @endcode
 */
bool FT::SampleTouch(uint8_t index, uint16_t *x, uint16_t *y)
{
  if (index == 0 || index > _maxmultitouch) return false; //limita conforme configuração
  index--; // converte para 0-based

  const auto &tp = touchPoints[index];

  if (tp.event == TouchEvent::Press || tp.event == TouchEvent::Change) {
    // Ajusta coordenadas para o sistema de tela
    *x = tp.x;
    *y = tp.y;
	
    // Marca toque como consumido
    touchPoints[index].event = TouchEvent::Release;  
  
    // Só zera _newtouch se não houver mais toques ativos
    bool anyActive = false;
    for (uint8_t i = 0; i < _maxmultitouch; i++) {
        if (touchPoints[i].event == TouchEvent::Press || touchPoints[i].event == TouchEvent::Change) {
          anyActive = true;
          break;
        }
    }
    _newtouch = anyActive;
    return true;
  }

  return false;

}


/**
 * @brief Habilita ou desabilita o modo multitouch do controlador FT.
 *
 * Essa função define se o driver deve interpretar múltiplos pontos de toque
 * simultaneamente (multitouch) ou restringir o reconhecimento apenas a um toque
 * por vez (modo single-touch).
 *
 * @details
 * Quando o multitouch é desabilitado (`enable = false`), o driver ignora
 * os toques adicionais e processa apenas o primeiro ponto detectado.
 * 
 * Em contrapartida, quando habilitado (`enable = true`), o driver passa a
 * armazenar e reportar múltiplos toques através das estruturas internas
 * `touchPoints[]` e `_history[]`.
 *
 * Essa configuração é útil em casos onde o hardware físico suporta multitouch,
 * mas o firmware da aplicação deseja restringir o comportamento (por exemplo,
 * para simplificar interfaces de usuário baseadas em botões).
 *
 * @param enable Define o estado do multitouch:
 * - `true`  → habilita multitouch (até FT5316_MAX_TOUCHES simultâneos)
 * - `false` → desabilita multitouch (apenas um toque ativo)
 *
 * @return Nenhum valor de retorno.
 *
 * @note
 * - A função afeta diretamente a interpretação de eventos em @ref getTouches().
 * - É recomendado chamá-la durante a inicialização do driver (ex: `begin()`).
 * - O número máximo de toques simultâneos depende do modelo FT utilizado.
 */
void FT::AllowMultitouch(bool enable) { _allowmultitouch = enable; }


/**
 * @brief Verifica se o modo multitouch está habilitado no driver.
 *
 * Retorna o estado atual da configuração de multitouch definida
 * anteriormente por @ref AllowMultitouch().
 *
 * @details
 * Essa função é útil para verificar dinamicamente, durante a execução,
 * se o sistema está operando em modo multitouch ou single-touch.
 *
 * @return `true` se o multitouch está habilitado; `false` caso contrário.
 *
 * @note Nenhum efeito colateral — apenas leitura do estado interno.
 */
bool FT::IsAllowMultitouch(void) { return _allowmultitouch; }


/**
 * @brief Define o número máximo de toques simultâneos permitidos.
 *
 * Essa função limita a quantidade de toques que o driver pode registrar
 * e processar simultaneamente, dentro do limite físico suportado pelo chip FT.
 *
 * @details
 * O valor informado por `num` nunca excede `FT5316_MAX_TOUCHES` (limite do hardware).  
 * Caso o multitouch esteja desabilitado, o valor é automaticamente forçado para 1.
 *
 * Isso garante que o sistema não entre em estado inconsistente,
 * mesmo que o usuário tente configurar mais toques do que o permitido.
 *
 * @param num Quantidade máxima de toques simultâneos desejada (1 a FT5316_MAX_TOUCHES).
 *
 * @return Nenhum valor de retorno.
 *
 * @note
 * - Essa configuração é usada internamente por @ref getTouches().
 * - O número de toques reais detectados depende também da configuração física do painel.
 * - Recomenda-se ajustar essa configuração apenas após definir o modo multitouch.
 */
void FT::setNumTouches(uint8_t num)
{
  if (num > FT5316_MAX_TOUCHES) num = FT5316_MAX_TOUCHES;
  _allowmultitouch ? _numtouchesallow = num : _numtouchesallow = 1;
}


/**
 * @brief Obtém o número máximo de toques simultâneos configurado.
 *
 * Retorna o valor atualmente definido como limite de toques processáveis
 * pelo driver FT, conforme configurado por @ref setNumTouches().
 *
 * @details
 * Esse valor indica a quantidade máxima de pontos de toque que podem ser
 * reconhecidos simultaneamente, mas não necessariamente a quantidade de toques
 * ativos no momento (essa informação está em `_touchcount`).
 *
 * @return Número máximo de toques simultâneos configurado.
 *
 * @note
 * - Essa função retorna o limite configurado, não o número de toques ativos.
 * - Para verificar toques em tempo real, utilize @ref getTouches().
 */
uint8_t FT::getNumTouches()
{
  return _numtouchesallow;
}


/**
 * @brief Retorna referência para um único ponto de toque.
 * 
 * @param index Índice do toque desejado (0 a _touchcount-1)
 * 
 * @return Referência constante para o TouchPoint. Retorna um ponto "vazio"
 *         se o índice for inválido.
 *
 * @code
 * const TouchPoint &touch = ft.getTouch(0);
 * if (touch.Status) {
 *     Serial.printf("Primeiro toque: x=%u, y=%u, id=%u\n", 
 *                   touch.x, touch.y, touch.ID);
 * }
 * @endcode
 */
const TouchPoint& FT::getTouch(uint8_t index) const 
{
  if (index >= _touchcount) {
      static TouchPoint empty = {0, 0, 0, 0, TouchEvent::Unknown};
      return empty; // ponto vazio se índice inválido
  }
  return touchPoints[index];
}


/**
 * @brief 
 * 
 * @verbatim
 * A distância origem 0,0 até o ponto na tela de toque
 * regido pela equção d^2 = x^2 + y^2
 * 
 * @param 
 * 
 * @return 
 *
 * @code
 * 
 * @endcode
 */
uint32_t FT::Dist(const TouchPoint & loc)
{
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  return x*x + y*y;
}


/**
 * @brief 
 * 
 * @verbatim
 * A distância entre dois pontos a partir de um ponto inicial (x1,y1) até o ponto final (x2,y2)
 * regido pela equção d = raiz_quadra(x^2 + y^2)
 * 
 * @param 
 * 
 * @return 
 *
 * @code
 * 
 * @endcode
 */
uint32_t FT::Dist(const TouchPoint & loc1, const TouchPoint & loc2)
{
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  return sqrt(x*x + y*y);         //calcula a distância entre dois pontos
}


/**
 * @brief Verifica se é mesma localização
 * 
 * @verbatim
 * Baseia-se na comapração de distância entre dois pontos determinado pelo valor minimo de proximidade
 * @endverbatim
 * 
 * @param const TouchPoint &loc1
 * @param const TouchPoint &loc2
 * @param uint32_t aproach
 * 
 * @return Bool
 *
 */
bool FT::SameLoc(const TouchPoint &loc1, const TouchPoint &loc2, uint32_t aproach)
{
  return Dist(loc1, loc2) < aproach;
}


/**
 * @brief Define a função de callback de usuário chamada pelo método poll().
 *
 * Diferente do callback ISR, este callback é chamado no contexto normal
 * da aplicação (tipicamente dentro de `loop()`). Isso permite utilizar
 * funções seguras como `Serial.print()`, `delay()`, ou até mesmo
 * rotinas mais pesadas.
 *
 * Para que este callback seja disparado, é necessário chamar periodicamente
 * o método `poll()`.
 *
 * @param cb Ponteiro para função do usuário: "void minha_callback(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch)"
 *        
 *
 * @code
 * void minha_callback(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch) {
 *     Serial.printf("Toque em x=%u, y=%u, evento=%d\n", tevent.x, tevent.y, tevent.event);
 * }
 *
 * FT touch(SDA, SCL, TOUCH_INT, TOUCH_RST);
 * touch.UserCallback(minha_callback);
 *
 * void loop() {
 *     touch.Poll(); // processa toques e dispara callback
 * }
 * @endcode
 */
void FT::OnCallback(UserISR_t cb)
{
  _UserCallback = cb;
}


/**
 * @brief Liga/Desliga o ISR de usuario (callback)
 * 
 * @verbatim
 * Esta função não destroi o ponteiro do callback para a funcao do usuario, mantendo as interrupções de sistema ativo, mas pausa/continua a exeução do ISR de usuário
 * Semelhante a execucaçõ da funcao Continue()/Pause()
 * @endverbatim
 * 
 * @param bool b
 * 
 * @return Bool
 *
 */
void FT::CallbackEnable(bool b)
{
  _callbackenable = b;
}


/**
 * @brief Rotina ISR interna da classe.
 *
 * Executada quando ocorre um evento de toque no pino de interrupção.
 * A rotina apenas marca a flag `_newtouch` como verdadeira.
 *
 * @note Esta função deve ser extremamente curta e não realizar chamadas
 *       que possam travar o sistema (como Serial, delay, malloc, etc.).
 */
void ISR_ATTR FT::HandleInterrupt(void)
{
  _newtouch = true;
}


/**
 * @brief ISR estática para attachInterrupt()
 * 
 * Redireciona para a instância do driver.
 */
static void IRAM_ATTR FT::HandleInterruptStatic() {
    if (_instanceft) _instanceft->HandleInterrupt();
}


/**
 * @brief Verifica se houve um novo toque desde a última leitura.
 *
 * Caso as interrupções estejam habilitadas (`_use_interrupt == true`),
 * a flag `_newtouch` será usada como indicador. Caso contrário,
 * a função faz leitura direta do registrador de status via I2C.
 *
 * @return true  Se houve toque novo.
 * @return false Caso contrário.
 *
 * @note A flag `_newtouch` é zerada dentro de `poll()` após o toque ser processado.
 */
bool FT::Touched()
{
  // Usa o estado interno definido pelo ISR
  if (_use_interrupt) return _newtouch; 

  // Caso contrário, faz leitura direta via I2C
  uint8_t touches = 0;
  if (TouchAddress(FT_TD_STATUS, &touches, 1) == 0) return (touches > 0);

  return false;
}


/**
 * @brief Retorna a quantidade de toques pendentes para callback.
 *
 * Esta função reflete quantos toques ainda não foram processados pelo usuário.
 *
 * @return Número de toques pendentes (_touchcount)
 */
uint8_t FT::AvailableTouch(void)
{
  return (_touchcount);
}


/**
 * @brief Define se o sistema deve aplicar o filtro de debounce aos movimentos de toque.
 *
 * O debounce de toque é um filtro opcional que serve para reduzir a sensibilidade
 * a pequenas variações de coordenadas (ruídos ou oscilações do sensor capacitivo).
 *
 * Quando ativado (`true`):
 *  - Movimentos inferiores a `FT_TOUCH_DEBOUNCE_PX` pixels são ignorados.
 *  - Evita falsos positivos em telas sensíveis ou instáveis.
 *
 * Quando desativado (`false`):
 *  - Qualquer variação mínima de coordenada é interpretada como movimento (`TOUCH_MOVE`).
 *  - Indicado para aplicações que exigem resposta fluida e imediata, como desenho livre
 *    ou manipulação contínua de objetos na tela.
 *
 * ---
 *
 * @param enable Valor booleano:
 *  - `true`  → ativa o filtro de debounce.
 *  - `false` → desativa o filtro (modo direto).
 *
 * @see getDebounceTouch()
 * @see ProcessTouchEvents()
 * @see FT_TOUCH_DEBOUNCE_PX
 */
void FT::setDebounceTouch(bool enable)
{
  _debouncetouch = enable;
}


/**
 * @brief Retorna o estado atual do filtro de debounce de toque.
 *
 * Esta função permite consultar se o sistema está aplicando o controle
 * de debounce para suavizar microvariações de coordenadas durante o toque.
 *
 * ---
 *
 * @return `true` se o debounce estiver ativo, ou `false` se estiver desativado.
 *
 * @see setDebounceTouch()
 * @see ProcessTouchEvents()
 */
bool FT::getDebounceTouch() const
{
  return _debouncetouch;
}


/**
 * @brief Processa e traduz os eventos de toque detectados pelo controlador FT5x16.
 *
 * Esta função é responsável por analisar o estado atual dos toques reportados pelo
 * controlador (via `touchPoints[]`) e determinar as transições de estado entre
 * "TOUCH_DOWN", "TOUCH_MOVE" e "TOUCH_UP", gerando uma lista de eventos limpos
 * e prontos para consumo pela aplicação (por exemplo, uma GUI ou sistema de input).
 *
 * O algoritmo utiliza um sistema interno de **histórico por dedo (_history[])**
 * para rastrear o estado anterior de cada ponto de toque. Esse histórico permite:
 *   - Saber quando um novo dedo entrou em contato (TOUCH_DOWN).
 *   - Detectar movimentos reais (TOUCH_MOVE).
 *   - Reconhecer quando o dedo foi removido (TOUCH_UP).
 *   - Aplicar timeout automático quando o toque "desaparece" fisicamente (sem evento de release).
 *
 * ---
 *
 * @section debounce_sec Controle de Debounce
 *
 * O **debounce de toque** é um recurso opcional (ativado por `setDebounceTouch(true)`)
 * que serve para filtrar microvariações de coordenadas geradas por ruído ou instabilidade
 * no sensor capacitivo.
 *
 * Quando ativo (`_debouncetouch = true`):
 *   - Movimentos inferiores a `FT_TOUCH_DEBOUNCE_PX` pixels são ignorados.
 *   - Isso evita que pequenos tremores do dedo sejam interpretados como movimentos reais.
 *
 * Quando desativado (`_debouncetouch = false`):
 *   - Qualquer variação de coordenada (mesmo mínima) é considerada um `TOUCH_MOVE`.
 *   - Indicado para sistemas que precisam de resposta instantânea e suave (ex.: desenhos livres).
 *
 * ---
 *
 * @section history_sec Histórico (_history)
 *
 * Cada posição de toque (até `_maxmultitouch`) mantém um registro independente com:
 *   - `active` → indica se aquele dedo estava anteriormente ativo.
 *   - `prev` → guarda as últimas coordenadas conhecidas do toque.
 *   - `lastSeen` → registra o momento (em ms) da última atualização.
 *
 * O histórico garante estabilidade na detecção de transições, permitindo que o
 * sistema reconheça corretamente eventos mesmo quando há pequenas falhas momentâneas
 * na leitura do controlador.
 *
 * ---
 *
 * @param[out] events     Ponteiro para o array de destino dos eventos gerados.
 * @param[in]  maxEvents  Quantidade máxima de eventos que podem ser armazenados em `events`.
 *
 * @return O número de eventos válidos processados e preenchidos em `events`.
 *
 * @note Esta função deve ser chamada periodicamente (ex.: dentro de um `Poll()` ou `loop()`).
 * @note O método `setDebounceTouch(bool)` permite ativar ou desativar o filtro de debounce.
 *
 * ---
 *
 * @see FT::setDebounceTouch()
 * @see FT::getDebounceTouch()
 * @see TouchEventInfo
 * @see TouchTransition
 */
uint8_t FT::ProcessTouchEvents(TouchEventInfo *events, uint8_t maxEvents)
{
  uint8_t count = 0;
  uint32_t now = millis();                                               //atial tempo antes de processar os toques
  
  for (uint8_t i = 0; i < _maxmultitouch && count < maxEvents; i++) {    //processa todos os eventos

      auto &curr = touchPoints[i];                                       //pega o atual evento de toque
      auto &hist = _history[i];                                          //pega o atual history dos eventos
      TouchTransition trans = TOUCH_NONE;
  
      bool isDownOrContact = (curr.event == TouchEvent::Press || curr.event == TouchEvent::Change); //pressionamento ou mudança
  
      if (isDownOrContact) {                                             //caso pressionamento/mudança
      
          if (!hist.active) {                                            //atual history nao ativo
              trans = TOUCH_DOWN;                                        //transicao é press
              hist.active = true;                                        //ativo history
              _touchcount++;                                             //cotnagem de toques 
              _newtouch = true;                                          //novo toque 
          } else if (curr.x != hist.prev.x || curr.y != hist.prev.y) {   //se a posicao x,y do anterior e atual sao diferentes
            if (_debouncetouch) {                                        // 👈 controle condicional de debounce
               int16_t dx = abs((int16_t)curr.x - (int16_t)hist.prev.x);
               int16_t dy = abs((int16_t)curr.y - (int16_t)hist.prev.y);
               if (dx > FT_TOUCH_DEBOUNCE_PX || dy > FT_TOUCH_DEBOUNCE_PX)
                 trans = TOUCH_MOVE;
            } else {
              trans = TOUCH_MOVE;                                         // 👈 sem debounce: qualquer variação já é movimento
            }
          }
          // Atualiza registro de presença
          hist.lastSeen = now;                                           //guarda o tempo atual antes dos toques 
          hist.prev = curr;                                              //guarda o atual toque como atnerior
      
      } else if (curr.event == TouchEvent::Release) {                    //caso liberacao
          
          if (hist.active) {                                             //o atual history esta ativo
              trans = TOUCH_UP;                                          //transicao é release
              hist.active = false;                                       //libera o history ativo
              if (_touchcount > 0) _touchcount--;                        //libera um toque a menos  
          }
          // Atualiza registro de presença
          hist.lastSeen = now;                                           //guarda o timer atual
          hist.prev = curr;                                              //o atual liberacao de tecla sera o anterior
      
      } else {                                                           //Não é nenhum caso
          
          // Timeout: assume que o toque sumiu
          if (hist.active && (now - hist.lastSeen) > FT_TOUCH_TIMEOUT_MS) { //history event ativo 
              trans = TOUCH_UP;
              hist.active = false;
              if (_touchcount > 0) _touchcount--;
          }
      }
  
      if (trans != TOUCH_NONE) {                                          //se transicao for diferente de nada, atualiza
          events[count].id         = curr.id;                             //ID
          events[count].x          = curr.x;                              //Coordenada X
          events[count].y          = curr.y;                              //Coordenada Y
          events[count].transition = trans;                               //transicao 
          count++;
      }
  }
  
  return count;
}


/**
 * @brief Processa toques capturados pela interrupção e dispara callback normal.
 *
 * Este método deve ser chamado periodicamente no "loop()" da aplicação.
 * Ele verifica se novos toques foram detectados pela ISR e, caso haja,
 * dispara o callback definido por "setUserCallback()".
 *
 * @code
 * void loop() {
 *     // outras tarefas...
 *     touch.Poll();  // garante que o callback seja chamado
 * }
 * @endcode
 */
void FT::Poll(void)
{
    // 1️⃣ Lê os toques atuais do hardware
    getTouches();

    // 2️⃣ Processa eventos lógicos (DOWN, MOVE, UP) usando histórico
    TouchEventInfo events[FT5316_MAX_TOUCHES];
    uint8_t n = ProcessTouchEvents(events, FT5316_MAX_TOUCHES);

    // 3️⃣ Dispara callback para cada evento processado
    if (_UserCallback && _usercallbackenable) {
        for (uint8_t i = 0; i < n; i++) {
          _UserCallback(events[i], i, n); // envia evento processado
        }
    }

   // 4️⃣ Atualiza flag _newtouch
    _newtouch = (_touchcount > 0); // permanece true se houver toques ativos
}


//apenas um sinal para indicar se a ISR de usuario sera mantido em pausa (sem execucao)
//no entando as itenrrupções manipulaveis pelo sistema continuam ativas
void FT::Pause(void) { _callbackenable = false; }


//apenas um sinal para indicar se a ISR de usuario será ativado arpa continuar (continua a execucação)
//no entando as itenrrupções manipulaveis pelo sistema continuam ativas
void FT::Continue(void) { _callbackenable = true; }


/**
 * @brief Habilita o mecanismo de interrupção por toque (ISR) do controlador FT.
 *
 * Esta função configura o pino de interrupção (_ctp_intpin) e associa a função
 * interna `HandleInterruptStatic` como rotina ISR. A ISR apenas marca a flag
 * `_newtouch` para indicar um novo toque, sem realizar leituras I2C.
 *
 * @note O método deve ser chamado após `Begin()`.
 *       A função ISR deve ser declarada com o atributo `IRAM_ATTR` no ESP32.
 *
 * @code
 * void IRAM_ATTR myISR(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch); {
 *   // código callback do usuario
 * }
 *
 * FT touch(SDA, SCL, TOUCH_INT, TOUCH_RST);
 * if (!touch.EnableInterrupt(true)) {
 *   Serial.println("Falha ao habilitar interrupção de toque!");
 * }
 * @endcode
 *
 * @retval true  Interrupção habilitada com sucesso.
 * @retval false Falha ao habilitar (pino inválido ou não suporta interrupção).
 */
bool FT::Enable(void)
{
  if (_ctp_intpin == 0xFF) return false;           // Nenhum pino configurado

  if (!_interrupt_enabled) return true;            //Interrupções da controladora já está habilitado
  
  #ifdef DIGITALPINTOINTERRUPT
    int8_t irq = digitalPinToInterrupt(_ctp_intpin);
	if (irq == NOT_AN_INTERRUPT) {          // Não é possível usar esse pino como interrupção
	  _interrupt_enabled = false;
      _use_interrupt = false;
	  return false;
	}
    attachInterrupt(irq, HandleInterruptStatic, FALLING);
  #else
    //fallback: cores antigos/alternativos aceitam o pino direto
    attachInterrupt(_ctp_intpin, HandleInterruptStatic, FALLING);
  #endif
  
  _interrupt_enabled = true;
  _use_interrupt = true;
  _newtouch = false;
  _touchcount = 0;
  return true;
}


/**
 * @brief Desabilita o mecanismo de interrupção por toque (ISR).
 *
 * Após chamar esta função, a rotina ISR anterior não será mais executada
 * quando ocorrerem toques no display.
 *
 * @code
 * FT touch(SDA, SCL, TOUCH_INT, TOUCH_RST);
 * touch.Disable(); // desabilita a interrupção de toque
 * @endcode
 *
 * @retval true  Interrupção desabilitada com sucesso.
 * @retval false Falha ao desabilitar (pino inválido).
 */
bool FT::Disable(void)
{

  if (_ctp_intpin == 0xFF) return false;           // Nenhum pino configurado
  if (!_interrupt_enabled) return true;

  #ifdef DIGITALPINTOINTERRUPT
    int8_t irq = digitalPinToInterrupt(_ctp_intpin);
    if (irq != NOT_AN_INTERRUPT) {
       detachInterrupt(irq);
	} else { 
	   detachInterrupt(_ctp_intpin);
	}
  #else
    detachInterrupt(_ctp_intpin);
  #endif
  
  _interrupt_enabled = false;
  _use_interrupt = false;
  _newtouch = false;
  _touchcount = 0;		
  return true;
}


/**
 * @brief Liga ou desliga dinamicamente as interrupções de toque.
 *
 * Permite ao usuário ativar ou desativar o uso da interrupção sem
 * reconfigurar todo o driver. Caso `enable` seja falso, o toque será
 * detectado apenas via leitura direta I2C (`poll()` / `Touched()`).
 *
 * @param enable true para habilitar, false para desabilitar.
 * @retval true  Operação bem-sucedida.
 * @retval false Falha (pino inválido ou não suporta interrupção).
 */
bool EnableInterrupt(bool enable)
{

  if (_ctp_intpin == 0xFF) return false;           // Nenhum pino configurado

  if (enable && !_interrupt_enabled) {
    
  #ifdef DIGITALPINTOINTERRUPT
    int8_t irq = digitalPinToInterrupt(_ctp_intpin);
	if (irq == NOT_AN_INTERRUPT) {          // Não é possível usar esse pino como interrupção
	  _interrupt_enabled = false;
      _use_interrupt = false;
	  return false;
	}
    attachInterrupt(irq, HandleInterruptStatic, FALLING);
  #else
    //fallback: cores antigos/alternativos aceitam o pino direto
    attachInterrupt(_ctp_intpin, HandleInterruptStatic, FALLING);
  #endif
    _interrupt_enabled = true;
    _use_interrupt = true;
	
  } else if (!enable && _interrupt_enabled) {
	  
  #ifdef DIGITALPINTOINTERRUPT
    int8_t irq = digitalPinToInterrupt(_ctp_intpin);
    if (irq != NOT_AN_INTERRUPT) {
       detachInterrupt(irq);
	} else { 
	   detachInterrupt(_ctp_intpin);
	}
  #else
    detachInterrupt(_ctp_intpin);
  #endif
    _interrupt_enabled = false;
    _use_interrupt = false;
  
  }

  _newtouch = false;
  _touchcount = 0;	
  return true;

}







//void FT::ProcessTouch()
//{
//	
//	//construindo
//  getTouches();                               //Pega os toques
//  
//  for (i = 0; i<_counttouch;i++){
//    switch (tpoint[i].event) {                //Pega o evento RAW
//      case TouchEvent::Press:
//        _dragMode = false;
//        _touchStartTime = millis();
//		_UserCallback(touchPoints[i], i, n);
//  	  break;
//	  
//      case TouchEvent::Release:
//	  
//  	  break;
//	  
//      case TouchEvent::Change:
//  	  break;
//	  
//      case TouchEvent::Unknown:
//  	  break;
//  	
//    }	
//    _UserCallback(touchPoints[i], i, n);
//  }
//
//
/////orignal abaixo
//
//  getTouches();                               //Pega os toques
//  uint8_t n = 0;
//  TouchPoint tpoint = touchPoints[n];
//  TPoint point{_touchX[n], _touchY[n]};
//
//
//	if (tpoint.event == TRawEvent::Press)
//	{
//		_points[0] = point;
//		_pointIdx = 1;
//		_dragMode = false;
//		_touchStartTime = millis();
//		fireEvent(point, TEvent::TouchStart);	
//	}
//	else if (tpoint.event == TRawEvent::Change)
//	{
//		if (_pointIdx < 10)
//		{
//			_points[_pointIdx] = point;
//			_pointIdx += 1;
//		}
//		if (!_dragMode && _points[0].aboutEqual(point) && millis() - _touchStartTime > 300)
//		{
//			_dragMode = true;
//			fireEvent(point, TEvent::DragStart);
//		}
//		else if (_dragMode)
//			fireEvent(point, TEvent::DragMove);
//
//		fireEvent(point, TEvent::TouchMove);
//	}
//	else if (tpoint.event == TRawEvent::Release)
//	{
//		_points[9] = point;
//		_touchEndTime = millis();
//		fireEvent(point, TEvent::TouchEnd);
//		if (_dragMode)
//		{
//			fireEvent(point, TEvent::DragEnd);
//			_dragMode = false;
//		}
//		if (_points[0].aboutEqual(point) && _touchEndTime - _touchStartTime <= 300)
//		{
//			fireEvent(point, TEvent::Tap);
//			_points[0] = {0, 0};
//			_touchStartTime = 0;
//		}
//	}
//	else
//	{
//	}
//}



//void FT::Poll(void)
//{
//  if (!_newtouch) return;                      //nenhum toque para processar
//
//  if ((_UserCallback != nullptr) && _usercallbackenable) {
//    uint8_t n = getTouches();
//    //Para cada toque na tela esta funcao é disparada
//    DEBUG_PRINTD("Poll(void), toques: ", n, true, 0, true);         //Debug
//    DEBUG_PRINTD("Poll(void), _maxmultitouch: ", _maxmultitouch, true, 0, true);         //Debug
//    for (uint8_t i = 0; i < n; i++) {
//	  _UserCallback(touchPoints[i], i, n);
//      Serial.print("touchPoints[");Serial.print(i);Serial.print("] ");Serial.println(static_cast<uint8_t>(touchPoints[i].event));
//      if (_touchcount > 0) _touchcount--;                             //decremetna numero de toques ate zerar
//    }
//  }
//  if (_touchcount == 0) _newtouch = false;     //libera apos o ultimo toque sair
//
//}
