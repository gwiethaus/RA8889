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


*/

#include <Arduino.h>
#include <Wire.h>
#include <FT5316.hpp>
#include <Debug.hpp>
  
/*
static struct _ts_event ts_event = {0};


bool FT5316_isInterrupt(void)
{
 	if(FT5316_INT_READ)      //Detect the occurrence of an interrupt
 	{
		ts_event.Key_Sta = KEY_DOWN;
    return true;
 	}
  return false;
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

*/


//Somente permite uma instancia de objeto

// Definição da instância (somente aqui!)
FT* FT::_instanceft = nullptr;


/**
 * @brief Construtor da classe FT (touch controller).
 *
 * Inicializa os pinos de comunicação I2C, pino de interrupção e pino de reset,
 * configura os parâmetros internos da classe e aloca memória para os buffers de eventos.
 *
 * @param sdapin Pino SDA da interface I2C.
 * @param sclpin Pino SCL da interface I2C.
 * @param intpin Pino de interrupção do touch controller.
 * @param rstpin Pino de reset do touch controller.
 *
 * @note Os trechos de código comentados dentro do construtor servem como referência
 *       de alternativas de inicialização ou reset de buffers:
 *       - `touchPoints[i] = {0,0,false};` pode ser usado como alternativa ao `touchPoints[i].reset()`.
 *       - `_history[i] = {false,0,{0,0,0,0,TouchEvent::Unknown}};` pode substituir `_history[i].reset()`.
 *       - `_eventBuffer.reset();` poderia ser usado para limpar todo o buffer de eventos.
 *
 * @note Valores padrão configurados:
 *       - `_ctp_addr` = FT_I2C_ADDRESS
 *       - `_maxmultitouch` = FT5316_MAX_TOUCHES
 *       - `_newtouch` = false
 *       - `_touchcount` = 0
 *       - `_useinterrupt` = false
 *       - `_interrupt_enabled` = false
 *       - `_allowmultitouch` = false
 *       - `_numtouchesallow` = 1
 *       - `_callbackenable` = false
 *       - `_width` = 0
 *       - `_height` = 0
 *       - `_inverted_mount` = false
 *       - `_settoucharea` = false
 *       - `_transition_time_ms` = 50
 */
FT::FT(uint8_t sdapin, uint8_t sclpin, uint8_t intpin, uint8_t rstpin)
{
  _ctp_intpin = intpin;
  _ctp_rstpin = rstpin;
  _ctp_sdapin = sdapin;
  _ctp_sclpin = sclpin;
  _ctp_addr = FT_I2C_ADDRESS;
  _maxmultitouch = FT5316_MAX_TOUCHES;
  _useinterrupt = false;
  _interrupt_enabled = false;
  _allowmultitouch = false;
  _numtouchesallow = 1;
  _UserCallback = nullptr;
  _callbackenable = false;
  _instanceft = this;                          //Salva esta instância
  _width = 0;                                  //Total de pontos na horizontal da tela de toque
  _height = 0;                                 //Total de pontos na vertical da tela de toque
  _dispwidth = 0;
  _dispheight = 0;
  _inverted_mount = false;                     //Montagem normal do painel de tela de toque
  _settoucharea = false;                       //configurou a area de toque
  _transition_time_ms = 50;                    //tempo minimo de transição entre evento DOWN e MOVE
}


/**
 * @brief Destruidor da classe FT.
 *
 * Limpa os recursos associados à instância da classe, garantindo que a 
 * interrupção do pino do touch controller seja desanexada e que o callback
 * de usuário não seja mais chamado.
 *
 * @note Se futuramente a classe alocar memória dinâmica ou registrar
 *       outros recursos externos, este é o local apropriado para liberá-los.
 */
FT::~FT(void) 
{
  #ifdef DIGITALPINTOINTERRUPT
    detachInterrupt(digitalPinToInterrupt(_ctp_intpin));
  #else
    detachInterrupt(_ctp_intpin);
  #endif
  
  // Desassocia o callback de usuário para evitar chamadas após destruição
  _UserCallback = nullptr;
  _callbackenable = false;  
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
 * uint8_t val = ReadRegister(0x02);
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


/**
 * @brief Lê o ID do chip FT5x06 / FT5x16 / FT5x36 / FT6x06 / FT6x36
 *
 * Este método acessa o registrador de Vendor/Chip ID (FT_ID_G_CIPHER) via I2C
 * e retorna o valor lido. Pode ser usado para identificar o modelo do
 * controlador de touch conectado.
 *
 * @return uint8_t ID do chip lido.
 *
 * @note As constantes correspondentes aos IDs conhecidos são:
 * - FT5X06_ID  : 0x55
 * - FT5X16_ID  : 0x0a
 * - FT5X36_ID  : 0x14
 * - FT6X06_ID  : 0x06
 * - FT6X36_ID  : 0x36
 *
 * @code
 * uint8_t id = ft.ReadChipID();
 * Serial.print("Chip ID lido: 0x");
 * Serial.println(id, HEX);
 *
 * switch(id) {
 *     case FT5X06_ID: Serial.println("FT5x06 detectado"); break;
 *     case FT5X16_ID: Serial.println("FT5x16 detectado"); break;
 *     case FT5X36_ID: Serial.println("FT5x36 detectado"); break;
 *     case FT6X06_ID: Serial.println("FT6x06 detectado"); break;
 *     case FT6X36_ID: Serial.println("FT6x36 detectado"); break;
 *     default: Serial.println("Chip desconhecido"); break;
 * }
 * @endcode
 */
uint8_t FT::ReadChipID(void)
{
    // Lê o registrador do Vendor/Chip ID via I2C
    uint8_t chip_id = ReadRegister(FT_ID_G_CIPHER);
    return chip_id;
}


/**
 * @brief Inicializa a tela de toque das séries FT5x06 e FT5x16.
 *
 * Configura os pinos I2C (SDA/SCL), pinos de interrupção e reset, realiza 
 * verificação de comunicação com o dispositivo e aplica configurações 
 * iniciais nos registradores do controlador.
 *
 * @param addr Endereço I2C do touch controller conectado ao MCU.
 *
 * @return true se a comunicação I2C foi estabelecida corretamente; false caso contrário.
 *
 * @note 
 * - Se os pinos SDA ou SCL forem 0, os pinos padrão do Wire serão usados.
 * - O pino de interrupção (_ctp_intpin) é configurado, mas a ISR não é ativada aqui.
 * - O pino de reset (_ctp_rstpin) é configurado e o hardware é resetado, se necessário.
 * - O método **não limpa os buffers de toque automaticamente**, por isso é recomendado 
 *   chamar `reset()` no início do `Begin()` para garantir estado limpo.
 *
 * @see FT::Reset()
 * @see FT::HardwareReset()
 */
bool FT::Begin(uint8_t addr)
{
  _ctp_addr = addr;
  
  //Resetar os buffers antes de qualquer operação de hardware
  Reset();
  
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
    HardwareReset();
  }

  //Verifica comunicação I2C
  Wire.beginTransmission(_ctp_addr);
  bool sucess = (Wire.endTransmission() == 0);
  
  if (sucess) {

    if (ReadChipID != FT5X16_ID) {
      if (Serial) Serial.println("Chip ID FT5x16 don't found ");
      return false;
    }

    // Threshold de detecção de toque válido.
    // 0x16 = 22 decimal, sensibilidade média. Normalmente 0x10-0x30 é aceitável.
    // Valor apropriado para uso geral.
    WriteRegister(FT_ID_G_THGROUP, 0x16);             //0x80
    
    // Threshold do pico do toque, usado para filtrar picos rápidos.
    // 0x3C = 60 decimal, valor razoável. 
    // Valores típicos: 0x20~0x50 dependendo da sensibilidade desejada.	
	WriteRegister(FT_ID_G_THPEAK, 0x3c);              //0x81
	
    // Valor de calibração de threshold.
    // 0xE9 = 233 decimal, bastante alto, aumenta a tolerância.
    // Depende do ambiente e display. Geralmente entre 0x80~0xE9.
    // Se desejar menos falsos positivos.
    WriteRegister(FT_ID_G_THCAL, 0xe9);               //0x82
	
    // Habilita compensação automática do sensor (flag de status)
    // 0x01 = ativado
    // Normalmente se quer ligado.	
    WriteRegister(FT_ID_G_COMPENSATE_STATUS, 0x01);   //0x83
    
	// Flag de compensação de temperatura e variações do display
    // 0x01 = ativo
    // Normalmente ligado para estabilidade.
	WriteRegister(FT_D_G_COMPENSATE_FLAG, 0x01);      //0x84
    
	// Diferença de threshold para movimento de toque
    // 0xA0 = 160 decimal
    // Valores típicos: 0x50~0xA0 dependendo da aplicação
    // Ajustado para toque médio/pesado
	WriteRegister(FT_ID_G_THDIFF, 0xa0);              //0x85
	
    // Registro de controle geral
    // Bits do datasheet: 
    // 0x0A = 00001010b
    // Geralmente: 
    // - Bit 3 = 1 → habilita modo de monitoramento
    // - Bit 1 = 1 → habilita interrupção
    // Depende de quais recursos se deseja habilitar.
    WriteRegister(FT_ID_G_CTRL, 0x0a);                //0x86
	
	// Tempo para entrar no modo de monitoramento
    // 0x06 = 6ms ou unidade definida no datasheet
    // Valor baixo → sensor rápido para entrar em monitor
    WriteRegister(FT_ID_G_TIME_ENTER_MONITOR, 0x06);  //0x87
	
	// Período de varredura no modo ativo
    // 0x28 = 40 decimal, unidade do datasheet geralmente em ms
    // Taxa de atualização razoável, não muito lenta
    WriteRegister(FT_ID_G_PERIODACTIVE, 0x28);        //0x88
	
	// Define modo do dispositivo
    // 0x00 = normal mode (work mode)
    WriteRegister(FT_DEVIDE_MODE, 0x00);              //0x00

  }
  
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
void FT::HardwareReset(void)
{
  if (_ctp_rstpin == 0) return;
  digitalWrite(_ctp_rstpin, HIGH);//on
  delay(5);
  digitalWrite(_ctp_rstpin, LOW);//on
  delay(750);
  digitalWrite(_ctp_rstpin, HIGH);//on
}


/**
 * @brief Troca os valores entre duas variáveis, garantindo que 'maior' contenha o valor mais alto.
 * 
 * Esta função utiliza **referências (&)**, o que permite a troca direta de valores entre as variáveis,
 * sem necessidade de ponteiros ou retorno.  
 * Se o valor de @p maior já for maior ou igual a @p minor, nenhuma troca é realizada.
 * 
 * @param maior Referência para o valor que deve conter o maior número após a troca.
 * @param minor Referência para o valor que deve conter o menor número após a troca.
 * 
 * @note Aqui uso referência (&), que é o modo mais direto de trocar valores em C++.
 */
void FT::Exchange(uint16_t &maior, uint16_t &minor)
{
  if (maior >= minor) return;
  uint16_t temp = maior;                       //Guada o menor no temp
  maior = minor;                               //coloca o maior no lugar correto 
  minor = temp;                                //coloca o menor no lugar correto
}


/**
 * @brief Calcula os fatores de escala (compressão ou expansão) entre a área de toque e a área do display.
 *
 * Esta função ajusta os fatores de escala `scaleX` e `scaleY` de modo que as coordenadas do painel de toque
 * sejam proporcionalmente convertidas para as dimensões do display.
 *
 * A fórmula aplicada é:
 * @code
 * scaleX = (float)_dispwidth / (float)_width;
 * scaleY = (float)_dispheight / (float)_height;
 * @endcode
 *
 * @note A função evita divisão por zero verificando se `_width` e `_height` são válidos.
 */
void FT::ScaleFactor(void)
{
  //evitar divisão por zero
  if (_width == 0 || _height == 0) return;   
  
  //fator de escala de comrepssao ou expansão
  scaleX = (float)_dispwidth / (float)_width;
  scaleY = (float)_dispheight / (float)_height;
}


/**
 * @brief Configura a área de toque do painel.
 *
 * Define as dimensões brutas de leitura da tela de toque e ajusta automaticamente
 * o fator de escala em relação à área do display.
 *
 * @param width Número total de pontos (resolução) na horizontal da tela de toque.
 * @param height Número total de pontos (resolução) na vertical da tela de toque.
 * @param inverted_mount Indica se a tela de toque está fisicamente montada invertida sobre o display.
 *
 * @code
 * setTouchArea(800, 480, false);
 * @endcode
 *
 * @note Caso a tela de toque não esteja montada exatamente alinhada com o display,
 *       esse parâmetro permite compensar a inversão na leitura dos eixos.
 */
void FT::setTouchArea(uint16_t width, uint16_t height, bool inverted_mount)
{
  _width = width;
  _height = height;
  if (_inverted_mount != inverted_mount) _inverted_mount = inverted_mount;
  _settoucharea = (_width > 0) && (_height > 0);
  ScaleFactor();
}


/**
 * @brief Configura a área do display relacionada ao toque.
 *
 * Define as dimensões físicas do display que correspondem à projeção do toque
 * e calcula automaticamente os fatores de escala entre o painel e o display.
 *
 * @param width Número total de pontos (resolução) na horizontal do display.
 * @param height Número total de pontos (resolução) na vertical do display.
 *
 * @code
 * setDisplayArea(800, 480);
 * @endcode
 *
 * @note Se a área de toque ainda não foi configurada, assume-se que ela tem as mesmas dimensões do display.
 */
void FT::setDisplayArea(uint16_t width, uint16_t height)
{
  _dispwidth = width;
  _dispheight = height;
  
  // Se ainda não configurado, assume toque igual ao tamanho do display
  if (!_settoucharea) {
    _width = _dispwidth;
    _height = _dispheight;
  }
  
  ScaleFactor();
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


/** PRECISA AJUSTAR....
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
  // Verifica se há eventos a processar
  if (_eventBuffer.count == 0) return false;
  
  if (index == 0 || index > _eventBuffer.count) return false; // índice inválido
  
  index--; // converte para 0-based
  auto &ev = _eventBuffer.events[index];

  // Verifica se o evento é toque válido (toque novo ou em movimento)
  if (ev.transition == TOUCH_DOWN ||  ev.transition == TOUCH_MOVE) {
    
    // Ajusta coordenadas para o sistema de tela
    if (!_inverted_mount) {
      *x = static_cast<uint16_t>(ev.x * scaleX);
      *y = static_cast<uint16_t>(ev.y * scaleY);
    } else {
      // Inverte eixo X e/ou Y conforme montagem invertida (espelhamento)
      *x = static_cast<uint16_t>((_width - ev.x) * scaleX);
      *y = static_cast<uint16_t>((_height - ev.y) * scaleY);
    }
	
     // Marca o evento como consumido (transição para "up")
    ev.transition = TOUCH_UP;
  
	  // Atualiza o estado _newtouch para indicar se ainda há toques ativos
    bool anyActive = false;
    for (uint8_t i = 0; i <  _eventBuffer.count; i++) {
      if (_eventBuffer.events[i].transition == TOUCH_DOWN || 
          _eventBuffer.events[i].transition == TOUCH_MOVE) {
        anyActive = true;
        break;
      }
      _newtouch = anyActive;
      return true;
    }

  }
  
  return false;    //nao houve DOWN consumido

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
  if (num > _maxmultitouch) num = _maxmultitouch;
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
uint8_t FT::getNumTouches(void)
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
const TouchEventInfo& FT::getTouch(uint8_t index) const 
{
  static TouchEventInfo empty = {0, 0, 0, TouchEvent::Unknown, TouchTransition::TOUCH_NONE};	
  if (index >= _eventBuffer.count || index >= _maxmultitouch) return empty;  // retorna um elemento vazio se índice inválido
  return _eventBuffer.events[index];  // acessa corretamente o array
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
static void IRAM_ATTR FT::HandleInterruptStatic(void) {
    if (_instanceft) _instanceft->HandleInterrupt();
}


/**
 * @brief Reseta o estado interno do touch controller.
 *
 * Esta função limpa os buffers de eventos e reseta os pontos de toque,
 * garantindo que não haja toques pendentes ou históricos antigos.
 *
 * @note Os trechos de código comentados dentro do método servem como
 *       referência para alternativas de inicialização:
 *       - `touchPoints[i] = {0,0,false};` pode substituir `touchPoints[i].reset()`.
 *       - `_history[i] = {false,0,{0,0,0,0,TouchEvent::Unknown}};` pode substituir `_history[i].reset()`.
 *       - `_eventBuffer.reset();` poderia ser usado para limpar todo o buffer de eventos.
 *
 * @see FT::FT() Para referência de inicialização completa do objeto.
 */
void FT::Reset(void)
{
  for (uint8_t i = 0; i < _maxmultitouch; i++) {
    //touchPoints[i] = {0,0,false};
    touchPoints[i].reset();
    //_history[i] = {false,0,{0,0,0,0,TouchEvent::Unknown}};
    _history[i].reset();
    //se desejar pode usar tambem: _eventBuffer.reset();
    _eventBuffer.events[i] = {0,0,0, TouchEvent::Unknown, TouchTransition::TOUCH_NONE};
  }
  _eventBuffer.count = 0;
  _touchcount = 0;
  _newtouch = false;
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
  if (_useinterrupt) return _newtouch; 

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
 * @brief Define o tempo mínimo entre eventos de toque DOWN e MOVE.
 * 
 * Esse tempo é usado para controlar a sensibilidade entre o toque inicial
 * e a detecção de movimento. Valores muito baixos (< 5 ms) podem gerar
 * falsos movimentos, enquanto valores muito altos (> 1000 ms) tornam
 * a resposta lenta.
 * 
 * @verbatim
 * _transition_time_ms = 50
 *   Representa o tempo mínimo (em milissegundos) entre os eventos de toque DOWN e MOVE.
 *   Esse intervalo controla a sensibilidade da transição de um toque inicial (TOUCH_DOWN)
 *   para um movimento (TOUCH_MOVE). Valores menores tornam a resposta mais rápida, enquanto
 *   valores maiores reduzem a sensibilidade a pequenos movimentos acidentais.
 * @endverbatim 
 *
 * @param ms Tempo de transição em milissegundos (intervalo de 5–1000 ms recomendados).
 */
void setTransitionTime(uint16_t ms) {
 if (ms < 5) ms = 5;             //Evita zero ou valores extremamente baixos
 else if (ms > 1000) ms = 1000;  //Evita tempos excessivos
 _transition_time_ms = ms;       //Seta o tempo de transição
}


/**
 * @brief Retorna o tempo configurado para a transição entre DOWN e MOVE.
 * 
 * Esse valor pode ser usado para depuração ou ajuste dinâmico em tempo de execução.
 * 
 * @verbatim
 * _transition_time_ms = 50
 *   Representa o tempo mínimo (em milissegundos) entre os eventos de toque DOWN e MOVE.
 *   Esse intervalo controla a sensibilidade da transição de um toque inicial (TOUCH_DOWN)
 *   para um movimento (TOUCH_MOVE). Valores menores tornam a resposta mais rápida, enquanto
 *   valores maiores reduzem a sensibilidade a pequenos movimentos acidentais.
 * @endverbatim 
 *
 * @return Tempo atual de transição em milissegundos.
 */
 uint16_t getTransitionTime() const
{
  return _transition_time_ms;
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
uint8_t FT::ProcessTouchEvents(void)
{
  uint8_t count = 0;
  uint32_t now = millis();                                               //atial tempo antes de processar os toques
  
  for (uint8_t i = 0; i < _maxmultitouch && count < _maxmultitouch; i++) {    //processa todos os eventos

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
			   uint32_t delta = now - hist.lastSeen;
               // 🔸 Considera movimento somente se:
               // - houve deslocamento maior que o limite de debounce, e
               // - passou o tempo mínimo configurado pelo usuário
			   if (dx > FT_TOUCH_DEBOUNCE_PX || dy > FT_TOUCH_DEBOUNCE_PX || delta > _transition_time_ms)
                 trans = TOUCH_MOVE;
            } else {
              if ((now - hist.lastSeen) > _transition_time_ms) {
				  trans = TOUCH_MOVE;                                         // 👈 sem debounce: qualquer variação já é movimento
			  }
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
          
          // ⏱️ Timeout: assume que o toque desapareceu
          if (hist.active && (now - hist.lastSeen) > FT_TOUCH_TIMEOUT_MS) { //history event ativo 
              trans = TOUCH_UP;
              hist.active = false;
              if (_touchcount > 0) _touchcount--;
          }
      }
  
      // 🔹 Preenche o buffer interno apenas se houve transição
      if (trans != TOUCH_NONE) {
        auto &evt = _eventBuffer.events[_eventBuffer.count];
        evt.id         = curr.id;                                        //ID
        evt.x          = curr.x;                                         //Coordenada X
        evt.y          = curr.y;                                         //Coordenada Y
        evt.event      = curr.event;                                     //Atual evento
        evt.transition = trans;                                          //transicao
        _eventBuffer.count++;                                            //proximo buffer para tratar
      }

  }
  
  return _eventBuffer.count;
}


/**
 * @brief Retorna o número de pontos de toque atualmente detectados.
 *
 * Esta função acessa o buffer interno de eventos de toque e retorna
 * a contagem de pontos de toque válidos no momento da leitura.
 * O valor retornado indica quantos toques simultâneos estão sendo
 * processados pelo controlador.
 *
 * @return uint8_t Número de pontos de toque detectados (0 se nenhum toque ativo).
 */
uint8_t FT::getTouchCount(void) const 
{
  return _eventBuffer.count;
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
   _eventBuffer.reset();     // 🔹 Função struct para limpar buffer de eventos

    getTouches();            // 1️⃣ Atualiza os pontos crus do hardware
    ProcessTouchEvents();    // 2️⃣ Preenche _eventBuffer e atualiza transições

    // 3️⃣ Dispara callback para cada evento processado
    if (_UserCallback && _callbackenable) {
        for (uint8_t i = 0; i < _eventBuffer.count; i++) {
          _UserCallback(_eventBuffer.events[i], i, _eventBuffer.count); // envia evento processado
        }
    }

   // 4️⃣ Atualiza flag _newtouch
    _newtouch = (_touchcount > 0); // permanece true se houver toques ativos, modificado por ProcessTouchEvents()
}


/**
 * @brief Pausa temporariamente a execução do callback de usuário associado à ISR.
 *
 * Esta função desabilita a execução do callback definido pelo usuário sem 
 * interferir nas interrupções internas ou no funcionamento do sistema.
 * É útil quando se deseja suspender o tratamento de eventos de toque
 * sem desativar completamente a detecção de interrupções.
 *
 * @note As interrupções controladas internamente pelo sistema permanecem ativas.
 * @see Continue()
 */
void FT::Pause(void) { _callbackenable = false; }


/**
 * @brief Retoma a execução do callback de usuário previamente pausado.
 *
 * Esta função reativa a chamada do callback de usuário associado à ISR,
 * permitindo que os eventos de toque voltem a ser processados normalmente.
 * Não altera o estado das interrupções internas do sistema.
 *
 * @note As interrupções do sistema permanecem ativas independentemente deste estado.
 *
 * @see Pause()
 */
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
 *       Ao habilitar, os flags de toque pendentes (_newtouch, _touchcount) são
 *       zerados, mas o histórico de toques (_touchPoints[]) é preservado.
 *
 * @code
 * FT touch(SDA, SCL, TOUCH_INT, TOUCH_RST);
 * if (!touch.Enable()) {
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
  _useinterrupt = true;
  
  // Reset de flags de toque pendentes, preservando histórico
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
 * @note Ao desabilitar, os flags de toque pendentes (_newtouch, _touchcount) 
 *       são zerados, mas o histórico de toques (_touchPoints[]) é preservado.
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
  _useinterrupt = false;
  
  // Reset de flags de toque pendentes, preservando histórico
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
 * @note Ao habilitar ou desabilitar, os flags de toque pendentes
 *       (_newtouch, _touchcount) são zerados. O histórico de toques
 *       (_touchPoints[]) é preservado.
 *
 * @param enable true para habilitar, false para desabilitar.
 * @retval true  Operação bem-sucedida.
 * @retval false Falha (pino inválido ou não suporta interrupção).
 */
bool FT::EnableInterrupt(bool enable)
{

  if (_ctp_intpin == 0xFF) return false;           // Nenhum pino configurado

  if (enable && !_interrupt_enabled) {
    
  #ifdef DIGITALPINTOINTERRUPT
    int8_t irq = digitalPinToInterrupt(_ctp_intpin);
	if (irq == NOT_AN_INTERRUPT) {          // Não é possível usar esse pino como interrupção
	  _interrupt_enabled = false;
      _useinterrupt = false;
	  return false;
	}
    attachInterrupt(irq, HandleInterruptStatic, FALLING);
  #else
    //fallback: cores antigos/alternativos aceitam o pino direto
    attachInterrupt(_ctp_intpin, HandleInterruptStatic, FALLING);
  #endif
    _interrupt_enabled = true;
    _useinterrupt = true;
	
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
    _useinterrupt = false;
  
  }

  // Reset de flags de toque pendentes, preservando histórico
  _newtouch = false;
  _touchcount = 0;

  return true;

}
