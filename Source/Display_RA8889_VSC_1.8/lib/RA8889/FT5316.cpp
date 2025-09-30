#include <Arduino.h>
#include <Wire.h>
#include <FT5316.hpp>

  
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

// Definição da instância estática (somente aqui!)
FT* FT::_instance = nullptr;

FT::FT(uint8_t intpin, uint8_t rstpin)
{
  _ctpintpin = intpin;
  _ctprstpin = rstpin;
  _addr = FT_I2C_ADDRESS;
  _maxmultitouch = FT_MAX_TOUCHES;
  _newtouch = false;
  _UserCallback = nullptr;
  _usercallbackenable = false;
  _instance = this;        //Salva esta instância
  _width = 0;              //Total de pontos na horizontal da tela de toque
  _height = 0;             //Total de pontos na vertical da tela de toque
  _settoucharea = false;   //configurou a area de toque
  pinMode(_ctpintpin, INPUT);
  pinMode(_ctprstpin, OUTPUT);
  
  Reset();
}


//Hardware Reset
void FT::Reset(void)
{
  if (_ctprstpin == 0) return;
  digitalWrite(_ctprstpin, HIGH);//on
  delay(5);
  digitalWrite(_ctprstpin, LOW);//on
  delay(350);
  digitalWrite(_ctprstpin, HIGH);//on
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
 * @brief Lê um registrador do dispositivo I2C de toque (FT5316).
 *
 * Esta função envia o endereço do registrador ao controlador de toque e
 * lê 1 byte de resposta via I2C.
 *
 * @param reg Endereço do registrador a ser lido.
 * @return Valor do registrador lido. Retorna 0 se ocorrer erro de transmissão
 *         ou se nenhum byte foi recebido.
 *
 * @code
 * uint8_t val = ReadTouchRegister(0x02); // Lê o registrador de status FT_TD_STATUS
 * Serial.print("Valor do registrador 0x02: ");
 * Serial.println(val);
 * @endcode
 */
uint8_t FT::ReadTouchRegister(uint8_t reg)
{
  if (_addr == 0) return 0;
  
  Wire.beginTransmission(_addr);
  Wire.write(reg);                             //Register 0
  if (Wire.endTransmission() != 0) return 0; // falha na transmissão
  
  uint8_t returned = Wire.requestFrom(_addr, (uint8_t)1 );     // Solicita 1 byte do registrador
  if (returned < 1) return 0; // nenhum byte recebido
  
  uint8_t retVal = 0;
  if (Wire.available()) {retVal = Wire.read(); }
  return retVal;
}


/**
 * @brief Escreve um valor em um registrador do dispositivo I2C de toque.
 *
 * @param reg Endereço do registrador.
 * @param val Valor a ser escrito.
 * @return true se a escrita foi bem-sucedida, false caso contrário.
 */
bool FT::WriteTouchRegister(uint8_t reg, uint8_t data)
{
  if (_addr != 0) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);                  // register 0
    Wire.write(data);                 // value
    uint8_t retVal = Wire.endTransmission();
	  return retVal == 0;              // true se transmissão OK
  }
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
  Wire.beginTransmission(_addr);
  Wire.write(reg);                                    // Pega o primeira registrador
  uint8_t retVal = Wire.endTransmission();
  uint8_t returned = Wire.requestFrom(_addr, len);    // request 1 bytes from slave device #2 dos proximos registradores
  
  uint8_t i = 0;
  while (i < len && Wire.available()) {
    pBuf[i++] = Wire.read();
  }
  
  return returned;  //número de bytes lidos
}


//converte valor de status em evento
TouchEvent FT::ToPointEvent(uint8_t status)
{
  switch (status) {
    case 0x00:  return TouchEvent::Down;
    case 0x01:  return TouchEvent::Up;
    case 0x02:  return TouchEvent::Move;
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
 */ 
uint8_t FT::getTouches(TouchPoint *pPoint, uint8_t num)
{
  uint8_t retVal = 0;
  uint8_t i = 0;
  uint8_t k = 0;
  
  if (!pPoint || !num) return 0;                           // must have a buffer and be able to take at least one
  
  //Lê número de toques ativos
  uint8_t status = ReadTouchRegister(FT_TD_STATUS);      //0x02, bits [3:0], Number of touch points
  uint8_t hitPoints = status & 0x0f;                     //
  if (hitPoints == 0) return 0;                          // no points detected

  // se multitouch estiver desabilitado, força apenas 1 toque
  if (!_allowMultitouch && hitPoints > 1) hitPoints = 1;
  
  if (hitPoints <= num) num = hitPoints;                 //Se for menor que o desejado entao o desejado é o numero de toques simultaneo na tela que foi realziado
  if (num > _maxmultitouch) num = _maxmultitouch;        //Limita conforme configuração

  static uint8_t tbuf[FT_MAX_TOUCHES * 6];               //maximo de toques * 6 bytes
  uint8_t bytesRead = TouchAddress( 0x03, tbuf, num*6);  //0x03, coletar a partir do registador TOUCH1_XH
  if (bytesRead < num * 6) {                             //Erro de leitura
    _touchcount = 0;
    return 0;
  }     
  
#ifdef DEBUG_TOUCH
  Serial.print("number of hit points = ");
  Serial.println( hitPoints );
#endif
    
  //Pega a coordenada x,y de cada toque de tela simultâneo
  for (k = 0, i = 0; (k < num); k++, i += 6) {
    pPoint[k].x      = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
    pPoint[k].y      = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    pPoint[k].status = ToPointEvent((tbuf[i + 0] & 0xC0) >> 6);        //Event Flag
    pPoint[k].id     = (tbuf[i + 2] & 0xF0) >> 4;              //ID hardware fornece
  }

  retVal = k; 
  
  return retVal;
}


/**
 * @brief Atualiza e retorna todos os toques ativos usando o array interno protegido.
 * @param num Número máximo de toques a considerar (até FT_MAX_TOUCHES)
 * @return Número de toques detectados
 *
 * @code
 * uint8_t n = ft.getTouches(FT_MAX_TOUCHES);  // atualiza a memória interna
 *
 * for(uint8_t i = 0; i < n; i++) {
 *     const TouchPoint& t = ft.getTouch(i);
 *     Serial.printf("Touch %u: x=%u, y=%u, id=%u\n", i, t.x, t.y, t.ID);
 * }
 * @endcode
 */
uint8_t FT::getTouches(uint8_t num)
{
  uint8_t i = 0;
  uint8_t k = 0;
  if (num == 0) return 0;

  // lê número de toques ativos
  uint8_t status = ReadTouchRegister(FT_TD_STATUS);    // 0x02, bits [3:0]
  uint8_t hitPoints = status & 0x0F;
  if (hitPoints == 0) return 0;

  // se multitouch estiver desabilitado, força apenas 1 toque
  if (!_allowMultitouch && hitPoints > 1) hitPoints = 1;
  
  if (hitPoints <= num) num = hitPoints;              //Se for menor que o desejado entao o desejado é o numero de toques simultaneo na tela que foi realziado
  if (num > _maxmultitouch) num = _maxmultitouch;     //limita conforme configuração

  // buffer temporário para leitura dos registros (6 bytes por toque)
  static uint8_t tbuf[FT_MAX_TOUCHES * 6];
  uint8_t bytesRead = TouchAddress(0x03, tbuf, num * 6);  // 0x03 = TOUCH1_XH
  if (bytesRead < num * 6) {
    _touchcount = 0;
    return 0;
  } 

  // Atualiza array interno
  for (k = 0, i = 0; (k < num); k++, i += 6) {
      touchPoints[k].x      = (tbuf[i + 0] & 0x0F) << 8 | tbuf[i + 1];
      touchPoints[k].y      = (tbuf[i + 2] & 0x0F) << 8 | tbuf[i + 3];
      touchPoints[k].status = ToPointEvent((tbuf[i + 0] & 0xC0) >> 6);              //Event Flag
      touchPoints[k].id     = (tbuf[i + 2] & 0xF0) >> 4;              //ID hardware fornece
  }

  _touchcount = num;             // atualiza quantidade de toques ativos

  return num;
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
 * @return true se houve toque Down consumido; false caso contrário
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
  if (index == 0) return false;
  if (index > _maxmultitouch) index = _maxmultitouch;     //limita conforme configuração
  
  index--;
  *x = -1*(touchPoints[index].x - (_width-1));
  *y = -1*(touchPoints[index].y - (_height-1));
  
  if(touchPoints[index].status == TouchEvent::Down)
  {
    touchPoints[index].status = TouchEvent::Up;
	_newtouch = false;    // ✅ Marca que o toque foi consumido
    return true;
  }
  
  return false;
}


/**
 * @brief Retorna referência para um único ponto de toque.
 * @param index Índice do toque desejado (0 a _touchcount-1)
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
      static TouchPoint empty = {0, 0, 0, TouchEvent::Unknown};
      return empty; // ponto vazio se índice inválido
  }
  return touchPoints[index];
}


//A distância origem 0,0 até o ponto na tela de toque
//regido pela equção d^2 = x^2 + y^2
uint32_t FT::Dist(const TouchPoint & loc)
{
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  return x*x + y*y;
}


//A distância entre dois pontos a partir de um ponto inicial (x1,y1) até o ponto final (x2,y2)
//regido pela equção d = raiz_quadra(x^2 + y^2)
uint32_t FT::Dist(const TouchPoint & loc1, const TouchPoint & loc2)
{
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  return sqrt(x*x + y*y);         //calcula a distância entre dois pontos
}


//Verifica se é mesma localização
//Baseia-se na comapração de distância entre dois pontos determinado pelo valor minimo de proximidade
bool FT::SameLoc(const TouchPoint &loc1, const TouchPoint &loc2, uint32_t aproach)
{
  return Dist(loc1, loc2) < aproach;
}


//deliga o callback sem destruir o potneiro para a funcao
void FT::UserCallbackEnable(bool b)
{
  _usercallbackenable = b;
}


/**
 * @brief Define a função de callback de usuário chamada no método poll().
 *
 * Diferente do callback ISR, este callback é chamado no contexto normal
 * da aplicação (tipicamente dentro de `loop()`). Isso permite utilizar
 * funções seguras como `Serial.print()`, `delay()`, ou até mesmo
 * rotinas mais pesadas.
 *
 * Para que este callback seja disparado, é necessário chamar periodicamente
 * o método `poll()`.
 *
 * @param cb Ponteiro para a função do usuário no formato:
 *        `void userCallback(const TouchPoint& tp);`
 *
 * @code
 * void myTouch(const TouchPoint& tp) {
 *     Serial.printf("Toque em x=%u, y=%u, evento=%d\n", tp.x, tp.y, tp.Status);
 * }
 *
 * FT touch(TOUCH_INT, TOUCH_RST);
 * touch.setUserCallback(myTouch);
 *
 * void loop() {
 *     touch.poll(); // processa toques e dispara callback
 * }
 * @endcode
 */
void FT::setUserCallback(TouchCallback cb)
{
  _UserCallback = cb;
}


//funcao indispensavel para o AttachInterrupt()
void FT::isrRouter()
{
  if (_instance) _instance->InternalInterrupt();
}


/** ISR interna */
void FT::InternalInterrupt()
{
  _newtouch = true;
  uint8_t n = getTouches(_maxmultitouch);
  if (_UserCallback && _usercallbackenable) {
    for (uint8_t i = 0; i < n; i++) {
      const TouchPoint& tp = touchPoints[i];
      _UserCallback(tp);
    }
  }
}


/**
 * @brief Verifica se houve um novo toque na tela desde a última chamada.
 * 
 * Esta função consulta a flag interna `_newtouch` que é marcada pela ISR
 * quando um toque é detectado. Se houver um toque novo, a flag é zerada,
 * garantindo que o mesmo evento não seja processado novamente.
 * 
 * @return true Se houve um novo toque desde a última verificação.
 * @return false Se não houve novos toques.
 *
 * @code
 * if (ft.Touched()) {
 *     // Um novo toque ocorreu, processar o evento
 *     uint8_t n = ft.getTouches(FT_MAX_TOUCHES);
 *     for (uint8_t i = 0; i < n; i++) {
 *         const TouchPoint& tp = ft.getTouch(i);
 *         Serial.printf("Touch %u: x=%u, y=%u, id=%u\n", i, tp.x, tp.y, tp.ID);
 *     }
 * }
 * @endcode
 */
bool FT::Touched(){
  if (_newtouch){
    _newtouch = false;
    return true;
  } else {
    return false;
  }
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
  if (_newtouch && _UserCallback) {
    _newtouch = false;
    for (uint8_t i = 0; i < _touchcount; i++) _UserCallback(touchPoints[i]);
  }
}


/**
 * @brief Anexa ou remove uma rotina de interrupção para o pino da tela de toque.
 *
 * @param pin Pino digital usado para interrupção da tela de toque.
 * @param isrfunction Ponteiro para função ISR (`void myISR()`). 
 *        Passe `nullptr` para remover a interrupção.
 *
 * @note A função ISR deve ser declarada com `IRAM_ATTR` no ESP32, 
 *       ex: `void IRAM_ATTR myISR() { ... }`.
 *
 * @code
 * void IRAM_ATTR myHandleInterrupt() {
 *   // código da ISR
 * }
 *
 * // Para anexar
 * ft.AttachInterruptService(10, myHandleInterrupt);
 *
 * // Para remover
 * ft.AttachInterruptService(10, nullptr);
 * @endcode
 */
void FT::AttachInterruptService(void)
{
  pinMode(_ctpintpin, INPUT);
  #ifdef DIGITALPINTOINTERRUPT
    attachInterrupt(digitalPinToInterrupt(_ctpintpin), isrRouter, FALLING);
  #else
    attachInterrupt(_ctpintpin, isrRouter, FALLING);
  #endif   
}


/**
 * @brief Remove a rotina de interrupção atualmente anexada ao pino da tela de toque.
 *
 * @note Após chamar esta função, a ISR anterior não será mais chamada.
 *
 * @code
 * // Remove ISR
 * ft.DetachInterruptService();
 * @endcode
 */
void FT::DetachInterruptService(void)
{
  #ifdef DIGITALPINTOINTERRUPT
    detachInterrupt(digitalPinToInterrupt(_ctpintpin));
  #else
    detachInterrupt(_ctpintpin);
  #endif
}


void FT::Begin(void)
{
  Wire.begin();
  Wire.beginTransmission(FT_I2C_ADDRESS);    //0x38
  Wire.write(FT_DEVIDE_MODE);                //0x00
  Wire.write(0);
  Wire.endTransmission(FT_I2C_ADDRESS);
}
