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

//Usado nas rotina de serviço de Interrupções ISR
#if defined(ESP32) || defined(ESP8266)
  #define ISR_ATTR IRAM_ATTR                   //somente ESP utilizam isso
#else
  #define ISR_ATTR                             //vazio em Arduinos AVR
#endif


//5 conjunto de registradores com 6 entradas cada
//cada conjunto de registradores define um toque
//permitindo até 5 toques simultâneos
//inicia no registrador 0x03 até 0x20
#define FT5316_TOUCH_ENTRY       6
#define FT5316_TOUCH_DATA_SIZE   FT5316_MAX_TOUCHES * FT5316_TOUCH_ENTRY

#define FT_TOUCH_TIMEOUT_MS   120   // tempo limite p/ considerar que um toque foi solto 
#define FT_TOUCH_DEBOUNCE_PX  3     // variação mínima para considerar movimento


/**
 * @brief Enumeração de eventos de toque lógicos
 */
enum class TouchEvent : uint8_t {
  Press    = 0,                                //Reg. 0x00, Pressionado a tela 
  Release  = 1,                                //Reg. 0x01, Solto a tela
  Change   = 2,                                //Reg. 0x02, Mudança de movimento e direção no toque sobre a tela 
  Unknown  = 3                                 //Desconehcido
};


/**
 * @brief Estrutura de ponto de toque
 */
struct TouchPoint {
    uint16_t x;         //Touch pos x
    uint16_t y;         //Touch pos y
    uint16_t id;        //Touch ID of Touch Point bit [3:0]
    uint8_t weight;     //Pressão sobre a tela
    TouchEvent event;   //Tipos de Evento

    void reset() {      //reset o TouchPoint
      x = 0;            
      y = 0; 
      id = 0; 
      weight = 0;
      event = TouchEvent::Unknown;
    }

};


/**
 * @brief Histórico interno para cada ponto de toque
 */
struct TouchHistory {
    bool active = false;        // estava ativo no último frame
    uint32_t lastSeen = 0;      // timestamp da última atualização
    TouchPoint prev{};          // último ponto conhecido

    void reset() {              //Para resetar o history 
      active = false;
      lastSeen = 0;
      prev = {0, 0, 0, 0, TouchEvent::Unknown};
    }
};


/**
 * @brief Transições de toque (escopo global)
 */
enum TouchTransition : uint8_t {
    TOUCH_NONE = 0,
    TOUCH_DOWN,
    TOUCH_MOVE,
    TOUCH_UP
};


/**
 * @brief Estrutura para evento processado com transição
 */
struct TouchEventInfo {
    uint16_t x;
    uint16_t y;
    uint16_t id;
    TouchEvent event;
    TouchTransition transition;

    void reset() {   //reset o TouchEventInfo
        x = 0;
        y = 0;
        id = 0;
        event = TouchEvent::Unknown;
        transition = TouchTransition::TOUCH_NONE;
    }    
};


/**
 * @brief Estrutura de buffer para armazenar eventos de toque.
 *
 * Este buffer segue um modelo simples de produtor/consumidor:
 * - O controlador de toque produz eventos e os armazena neste buffer.
 * - A aplicação (consumidor) lê os eventos e os processa, podendo marcá-los como consumidos.
 *
 * @note O tamanho máximo do buffer é definido por `FT5316_MAX_TOUCHES`.
 */
struct TouchEventBuffer {
  TouchEventInfo events[FT5316_MAX_TOUCHES];   /**< Array de eventos de toque. Cada posição representa um toque ativo ou registrado recentemente. */
  uint8_t count;                               /**< Número de eventos atualmente válidos no buffer. Deve ser usado para iterar sobre os eventos. */ 
  
  void reset() {
    count = 0;
    for (uint8_t i = 0; i < FT5316_MAX_TOUCHES; i++) {
      events[i].reset();
    }
  }
};


/**
 * @typedef UserISR_t
 * 
 * @brief Tipo de função de callback definido pelo usuário para tratamento de eventos de toque.
 *
 * Este tipo de função é utilizado pelo driver FT para permitir que o usuário registre
 * uma rotina personalizada de tratamento de eventos de toque (touch events).  
 * 
 * Quando novos toques, movimentos ou liberações são detectados pelo driver, 
 * a função registrada é chamada automaticamente através do método @ref FT::Poll(),
 * recebendo as informações detalhadas do toque processado.
 *
 * @details
 * O callback é executado de forma síncrona durante o processamento dos eventos, 
 * e fornece ao usuário acesso a dados de alto nível — já tratados quanto a:
 * - Identificação do tipo de transição (TOUCH_DOWN, TOUCH_MOVE, TOUCH_UP)
 * - Coordenadas X e Y do toque
 * - ID de toque (para multitouch)
 * - Quantidade total de toques simultâneos ativos
 *
 * Isso permite que a aplicação implemente facilmente comportamentos de interface
 * (como botões, sliders e gestos) de forma independente da camada física do touch.
 *
 * @note
 * - Este typedef é declarado fora da classe @ref FT para facilitar seu uso em módulos externos.
 * - O driver garante que as chamadas de callback ocorram apenas quando eventos válidos
 *   forem detectados (ou seja, sem ruídos ou leituras espúrias).
 * - Para habilitar o uso do callback, deve-se registrar a função através de @ref FT::setUserCallback().
 *
 * @code
 * // Exemplo de uso:
 *
 * void MyTouchHandler(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch)
 * {
 *     switch (tevent.transition) {
 *         case TOUCH_DOWN:
 *             Serial.printf("Toque #%d pressionado em (%d, %d)\n",
 *                           idtouch, tevent.x, tevent.y);
 *             break;
 *
 *         case TOUCH_MOVE:
 *             Serial.printf("Toque #%d movido para (%d, %d)\n",
 *                           idtouch, tevent.x, tevent.y);
 *             break;
 *
 *         case TOUCH_UP:
 *             Serial.printf("Toque #%d liberado.\n", idtouch);
 *             break;
 *     }
 * }
 *
 * // Registro do callback:
 * FT ft;
 * ft.setUserCallback(MyTouchHandler);
 *
 * // Dentro do loop principal:
 * ft.Poll();   // O driver chamará MyTouchHandler() automaticamente
 * @endcode
 *
 * @param tevent   Estrutura @ref TouchEventInfo contendo dados do toque atual,
 *                 incluindo coordenadas, ID e tipo de transição.
 * @param idtouch  Índice (ID lógico) do toque atual dentro do conjunto multitouch.
 * @param ntouch   Quantidade total de toques simultâneos detectados no momento.
 *
 * @return Nenhum valor de retorno.
 */
typedef void (*UserISR_t)(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch);


/**
 * @class FT
 * 
 * @brief Driver para o controlador de toque capacitivo FocalTech FT5x16.
 *
 * Esta classe gerencia a leitura e processamento de eventos de toque de telas
 * capacitivas FT5x16, suportando múltiplos toques (multitouch), detecção
 * de pressionamento, movimento e liberação de toques, além de filtragem de
 * ruídos via debounce.
 *
 * ---
 *
 * @section funcionalidades_sec Funcionalidades principais
 *
 * - Suporte a múltiplos toques simultâneos (_maxmultitouch).
 * - Conversão dos dados brutos do controlador em eventos lógicos (`TOUCH_DOWN`, `TOUCH_MOVE`, `TOUCH_UP`).
 * - Histórico de toques por dedo (_history[]) para rastrear posição anterior, tempo e estado ativo.
 * - Controle opcional de **debounce** para filtrar microvariações de coordenadas (FT_TOUCH_DEBOUNCE_PX).
 * - Contagem de toques ativos e identificação de novos toques (_touchcount, _newtouch).
 * - Timeout automático para liberar toques que desapareceram fisicamente sem gerar `Release`.
 *
 * ---
 *
 * @section principais_metodos_sec Principais métodos
 *
 * - `uint8_t getTouches()`  
 *   Lê todos os toques atualmente ativos, atualiza `touchPoints[]` e `_touchcount`.  
 *   Considera o limite de toques permitido (_numtouchesallow) e aplica tratamento seguro em caso de falha de comunicação I2C.
 *
 * - `uint8_t ProcessTouchEvents(TouchEventInfo *events, uint8_t maxEvents)`  
 *   Processa os dados de `touchPoints[]` e gera transições lógicas de toque (`TOUCH_DOWN`, `TOUCH_MOVE`, `TOUCH_UP`)  
 *   usando o histórico `_history[]`. Inclui suporte a debounce e timeout.
 *
 * - `void setDebounceTouch(bool enable)` / `bool getDebounceTouch() const`  
 *   Ativa ou desativa o filtro de debounce em tempo de execução.
 *
 * - `void setNumTouches(uint8_t num)` / `uint8_t getNumTouches()`  
 *   Define ou consulta o número máximo de toques que o sistema irá processar, respeitando `_maxmultitouch`.
 *
 * ---
 *
 * @section uso_sec Uso típico
 *
 * ```cpp
 * FT touch;
 *
 * void setup() {
 *     touch.setNumTouches(5);        // Permite até 5 toques simultâneos
 *     touch.setDebounceTouch(true);  // Ativa filtro de debounce
 * }
 *
 * void loop() {
 *     uint8_t n = touch.getTouches();               // Atualiza touchPoints[]
 *     TouchEventInfo events[5];
 *     uint8_t count = touch.ProcessTouchEvents(events, 5); // Processa transições lógicas
 *
 *     for (uint8_t i = 0; i < count; i++) {
 *         handleTouch(events[i]);  // Função do usuário para tratar eventos
 *     }
 * }
 * ```
 *
 * ---
 *
 * @section nota_sec Notas importantes
 *
 * - "_history[]" deve ser mantido como **protegido** ou **privado**, pois registra
 *   o estado anterior de cada dedo e é essencial para detecção correta de transições.
 * - "_debouncetouch" permite alternar entre estabilidade e sensibilidade máxima.
 * - "_transition_time_ms" o timeout protege contra falhas na comunicação I2C, transição entre DOWN/MOVE ou remoção rápida de dedos.
 */
class FT {
  private:
    bool WriteRegister(uint8_t reg, uint8_t data);
    uint8_t ReadRegister(uint8_t reg);
    uint8_t TouchAddress(uint8_t reg, uint8_t *pBuf, uint8_t len);
    bool Enable(void);
    bool Disable(void);
  public:
    FT(uint8_t sdapin, uint8_t sclpin, uint8_t intpin, uint8_t rstpin);
    ~FT(void);
	void HardwareReset(void);
	uint8_t ReadChipID(void);
    bool Begin(uint8_t addr);
    void setDisplayArea(uint16_t width, uint16_t height);
    void setTouchArea(uint16_t width, uint16_t height, bool inverted_mount);
    bool EnableInterrupt(bool enable);
    void Pause(void);
    void Continue(void);
    void OnCallback(UserISR_t cb);
    void CallbackEnable(bool b);
    uint8_t AvailableTouch(void);
    bool Touched(void);
    void setNumTouches(uint8_t num);
    uint8_t getNumTouches(void);
    uint8_t getTouches(TouchPoint *pPoint);
    uint8_t getTouches(void);
    const TouchEventInfo& getTouch(uint8_t index) const;
    uint8_t getTouchCount(void) const;
    
    
    void Poll(void);
    bool SampleTouch(uint8_t index, uint16_t *x, uint16_t *y);
    void AllowMultitouch(bool enable);
    bool IsAllowMultitouch(void);
    void setDebounceTouch(bool enable);        // 👈 novo método
    bool getDebounceTouch() const;             // 👈 consulta o estado
  protected:
    static FT* _instanceft;                    //ponteiro estático global para a instância ativa
    UserISR_t _UserCallback = nullptr;         //Interrupt Service Request user (function pointer)
    volatile bool _callbackenable;             //callback do usuario conitua/pausa na execucao, enable/disable callback interrupt user, sem perder o ponteiro do callback
    bool _useinterrupt = false;                //usa interrupções interna para sinalizar, caso contrario atraves de getTouch, Poll()....
    bool _interrupt_enabled = false;           //Interrupções internas foi ativada ativadas
    volatile bool _newtouch = false;           //Marcado pela ISR, houve um ou mais toques
    volatile uint16_t _isrcounter = 0;         //numero de itenrrupç~eos produzidas/consumidas 
    TouchPoint touchPoints[FT5316_MAX_TOUCHES];//Matriz de estado dos toques de Hardware
    TouchHistory _history[FT5316_MAX_TOUCHES]; //Events Touch History
    TouchEventBuffer _eventBuffer;             //Buffer de Evento (Software) controle consumidor/produtor
    uint8_t _touchcount;                       //Numero de um ou mais toques detectados
    volatile uint8_t _lastState;

    uint16_t _width;                           //Total de pontos na horizontal da tela de toque
    uint16_t _height;                          //Total de pontos na vertical da tela de toque
    uint16_t _dispwidth;                       //Total de pontos na horizontal da tela de toque
    uint16_t _dispheight;                      //Total de pontos na vertical da tela de toque
    float scaleX = 0.0;                        //fator de escala X
    float scaleY = 0.0;                        //fator de escala Y

    bool _inverted_mount = false;              //Montagem tela de toque nao foi invertida
    bool _settoucharea;
    uint8_t _maxmultitouch;                    //Mximum multitouch allowed for this microcontroler
    uint8_t _numtouchesallow = 1;              //numero de toques deejados
    bool _allowmultitouch = false;             // true = multitouch, false = apenas single
    bool _debouncetouch = true;                // 👈 controle de debounce (padrão: ativo)
    uint16_t _transition_time_ms = 50;         //Tempo mínimo (em milissegundos) entre os eventos de toque DOWN e MOVE
	
    uint8_t _ctp_addr;                         //Touch I2C Adrress
    uint8_t _ctp_intpin = 255;                 //Touch interrupt pin
    uint8_t _ctp_rstpin = 0;                   //Reset pin
    uint8_t _ctp_sdapin = 0;                   //comuncaicao I2C, pino SDA com a tela de toque
    uint8_t _ctp_sclpin = 0;                   //comuncaicao I2C, pino SCL com a tela de toque
      
    
    void Reset(void);                          //Reseta o estado interno do touch controller.
	void Exchange(uint16_t &maior, uint16_t &minor);
    void ScaleFactor(void);
    uint32_t Dist(const TouchPoint &loc);
    uint32_t Dist(const TouchPoint &loc1, const TouchPoint &loc2);
    bool SameLoc(const TouchPoint &loc, const TouchPoint &loc2, uint32_t aproach);
    TouchEvent ToPointEvent(uint8_t event);
    void ISR_ATTR HandleInterrupt(void);
    static void IRAM_ATTR HandleInterruptStatic(void);
    uint8_t ProcessTouchEvents();
};

#endif //fim do include guard FT5316_HPP
