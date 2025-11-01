#include <Arduino.h>
#include <BusParallel.hpp>
#include <Registers.hpp>
#include <Debug.hpp>

//Escrever as funcoes DataWrite e DataRead() do SPI e Parallel como sendo 16 bit de saida, 
//no caso do SPI so retorna 8 bits baixos. Para o spi pegar os 16 e 24 bits usa-se a funcao
// DataRead16, DataRead24, da emsma forma apra o paralelo
//Para o apralello leitura de 24 bits é caso especial poiis depende se o baramento éMPU8, MPU16 com colordepth 24 e isso pçor aqui nao dá apra prever
//apenas na aplicacao da biblitoca ra8889 e na no idus... uma solucacao precis ser pensada a respeito


/**
 * @brief Executa uma instrução NOP (No Operation) compatível com diversas arquiteturas.
 *
 * Esta macro realiza uma única instrução de *no-operation*, utilizada para criar atrasos extremamente curtos
 * (da ordem de nanossegundos). A duração exata do atraso depende diretamente da frequência de clock da CPU.
 *
 * - Em MCUs típicos:
 *   - @b 16 MHz → ~62,5 ns por NOP
 *   - @b 80 MHz → ~12,5 ns por NOP
 *   - @b 160 MHz → ~6,25 ns por NOP
 *
 * Essa abordagem é ideal para ajustes finos de temporização em protocolos paralelos, SPI bit-banged,
 * ou handshakes de hardware que exigem uma margem mínima de tempo entre pulsos.
 *
 * A macro é definida de forma condicional para garantir compatibilidade entre compiladores:
 * - GCC/Clang: usa `__asm__ __volatile__("nop")`
 * - IAR: usa `__no_operation()`
 * - MSVC: usa `__nop()`
 * - Outros: substituído por um bloco vazio (sem atraso efetivo)
 */
#if defined(__GNUC__)
  #define NOP() __asm__ __volatile__("nop")
#elif defined(__ICCAVR__)
  #define NOP() __no_operation()
#elif defined(_MSC_VER)
  #define NOP() __nop()
#else
  #define NOP() do {} while(0)
#endif


/**
 * @brief Configura o proctocolo de comunicação Paralela 8/16 bits para o display
 *
 * @verbatim
 * Usa structs internas do IBus (proposta “genérica”)
 * A área de código mostra exemplo de como usar o Config()
 * @endverbatim
 * 
 * @code
 * Bus_Parallel par;
 * RA8889 gfx(PIN_CS, PIN_RESET);
 * 
 * void setup() {
 *      
 *   IBus::ParallelBusConfig_t cfg;            //typedef structure
 *   cfg.type = PARALLEL8;
 *   cfg.data_pins[0] = 29;
 *   cfg.data_pins[1] = 23;
 *   cfg.data_pins[2] = 19;
 *   cfg.data_pins[3] = 18;
 *   cfg.data_pins[4] = 8;
 *   cfg.data_pins[5] = 10;
 *   cfg.data_pins[6] = 14;
 *   cfg.data_pins[7] = 5;
 *   cfg.freq_write = 40000000;
 *   
 *   par.Config(&cfg);                         // Grava a configuração
 *   gfx.setBus(par);                          // Seta o Bus Paralela
 *   gfx.Begin();                              // inicializa o display 
 * @endcode
 *
 * @param IBusConfig_t* cfg
 *        
 * @note None
 */
void Bus_Parallel::Config(const IBusConfig_t* cfg)
{
  const ParallelBusConfig_t* pcfg = static_cast<const ParallelBusConfig_t*>(cfg);
  _cfg = *pcfg;
}


void Bus_Parallel::Init(void) 
{
  if (_parallel_init) return;
  DEBUG_PRINT("Entrou em Bus_Parallel::Init", 0,false,true);
  
  pinMode(_cfg.pin_cs, OUTPUT);
  digitalWrite(_cfg.pin_cs, HIGH);
  
  pinMode(_cfg.pin_rs, OUTPUT);
  pinMode(_cfg.pin_wr, OUTPUT);
  pinMode(_cfg.pin_rd, OUTPUT);
  pinMode(_cfg.pin_en, OUTPUT);
  pinMode(_cfg.pin_wait, INPUT_PULLUP);        // WAIT é usado para handshake (READY) 
  if (_cfg.pin_int != 0xFF) {  // só configura se foi definido
    pinMode(_cfg.pin_int, INPUT_PULLUP);
  }

  _parallel_pindir = OUTPUT;
  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type) & 0x18;
  for (int i = 0; i < bits; i++) pinMode(_cfg.data_pins[i], _parallel_pindir);

  return;
}


/**
 * @brief Gera um pulso curto no pino EN do barramento paralelo.
 *
 * Essa função é usada principalmente por controladores gráficos (como o RA8876 ou RA8889)
 * para confirmar a escrita de dados ou comandos no barramento. O tempo entre a borda de subida
 * e descida é controlado por instruções NOP, garantindo uma temporização estável
 * independente de variações da função `delayMicroseconds()`.
 *
 * @note O tempo de pulso é da ordem de centenas de nanossegundos, variando conforme o clock da CPU.
 *
 * @code
 * // Exemplo de uso
 * PulseEN();  // Gera pulso curto de enable (~200 ns a 1 µs)
 * @endcode
 */
inline void Bus_Parallel::PulseEN()
{
  if (_cfg.pin_en == 0) return;
  digitalWrite(_cfg.pin_en, HIGH);
  NOP(); NOP(); NOP(); NOP();  ///< Ajuste fino de tempo (aprox. 4 ciclos de clock)
  digitalWrite(_cfg.pin_en, LOW);
}


/**
 * @brief Controla o pino Chip Select (CS) do barramento SPI.
 *
 * Esta função ativa ou desativa o dispositivo SPI conectado ao pino CS.
 * É utilizada para garantir que apenas um dispositivo SPI esteja ativo
 * no barramento por vez, evitando conflitos de comunicação.
 *
 * @verbatim
 * A lógica de controle do CS é:
 * - Nível LOW (0): Ativa o dispositivo (CS ativo).
 * - Nível HIGH (1): Desativa o dispositivo (CS inativo/liberado).
 * @endverbatim
 * 
 * @param level_cs O nível a ser definido para o pino CS:
 *        - 0: Ativa o dispositivo (CS em LOW).
 *        - 1: Desativa o dispositivo (CS em HIGH).
 *
 * @note O pino CS é tratado como "ativo baixo". Portanto, um nível LOW
 * (representado por 0) "bloqueia" o barramento para este dispositivo
 * específico, permitindo operações de escrita/leitura. Um nível HIGH
 * (representado por 1) "libera" o barramento para que outros dispositivos
 * possam utilizá-lo.
 */
void Bus_Parallel::SetCS(uint8_t level_cs)
{
  level_cs == 0 ? digitalWrite(_cfg.pin_cs, LOW) : /*SS_RESET */  digitalWrite(_cfg.pin_cs, HIGH); /*SS_SET*/
}


//Habilita forma de comando ou Dados
//0: commando
//1: Dados
void Bus_Parallel::SetRS(uint8_t level_rs)
{
  level_rs == 0 ? digitalWrite(_cfg.pin_rs, LOW) : digitalWrite(_cfg.pin_rs, HIGH);
}


// Adicione esta função à sua classe Bus_Parallel.
void Bus_Parallel::SetDataPinsDirection(int direction)
{
    if (_parallel_pindir == direction) return;
    // OTIMIZAÇÃO CRUCIAL:
    // O compilador (e a portabilidade) não sabe o que fazer sem o loop.
    // Assumimos que a sobrecarga desse loop UMA VEZ por transação é menor
    // do que a sobrecarga do pinMode() REPETIDO em cada bit.
    
    uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type);
    for (int i = 0; i < bits; i++) {
        pinMode(_cfg.data_pins[i], direction);
    }
	
	_parallel_pindir = direction;
}



/**
 * @brief Escreve um comando no RA8889 via barramento paralelo.
 * 
 * Esta função envia um byte de comando para o controlador gráfico RA8889
 * usando o barramento paralelo 8 bits. Garante que os pinos de controle
 * sejam configurados corretamente para que o chip interprete o byte
 * como comando, independente do estado anterior.
 * 
 * Fluxo da operação:
 * 1. Seleciona o chip (CS = LOW).
 * 2. Define RS = 0 para indicar que é um comando.
 * 3. WR = LOW → Ativa WR para iniciar a escrita.
 * 4. Configura os pinos de dados como saída e coloca o byte de comando.
 * 5. Gera um pulso de enable (EN) para que o RA8889 capture os dados.
 * 6. WR = HIGH → completa a escrita
 * 6. Libera o chip (CS = HIGH) ao final da operação.
 * 
 * @param cmd O byte de comando a ser enviado para o RA8889.
 */
void Bus_Parallel::CmdWrite(uint8_t cmd)
{

  // Seleciona o chip (CS ativo em nível baixo)
  SetCS(0);

  // RS = 0 → indica comando
  SetRS(0);
  
  SetDataPinsDirection(OUTPUT);     //Seleciona o sentido das informações nos pinos
  
  digitalWrite(_cfg.pin_wr, LOW);  //WR ativo

  //uint8_t temp = static_cast<uint8_t>(_cfg.parallel_type) & 0x1;
  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type) & 0x18;
  //uint8_t bits = 8 * (1 << temp);              //Numero de bits (8/16)

  //Configura os pinos de dados como saída e escreve comando apenas no bit 7-0
  for (int i = 0; i < PARALLEL8; i++) {
	digitalWrite(_cfg.data_pins[i], (cmd >> i) & 0x01);
  }
  
  //Configura os pinos de dados como saída e escreve comando zerado a parte alta no bit 15-8
  //o comando possui apenas 8 bits, mesmo que o barramento seja de 16 bits, mas a cotnroladora RA8889 precisa receber todos os bits.
  cmd = 0;
  for (int i = PARALLEL8; i < bits; i++) {
    digitalWrite(_cfg.data_pins[i], (cmd >> i) & 0x01);
  }

  // Pulso de enable (EN) para gravar os dados
  PulseEN();
 
  //Completa a escrita: WR volta para HIGH
  digitalWrite(_cfg.pin_wr, HIGH);

  // Libera o barramento
  SetCS(1);

}


//se o barramento escolhido for de 8 bits esta funcao irá enviar valores de D15..D8 zerados
//void Bus_Parallel::DataWrite(uint16_t data)
//sera modificada, e no barramento spi tambem fazer o memso, so que lá o envio é 8 bits, e aqui pdoe ser tanto 8 como 16 dependnedo d configuracao dob arramento paralelo
void Bus_Parallel::DataWrite(uint8_t data)
{
  SetCS(0);
  delayMicroseconds(1);
  while (digitalRead(_cfg.pin_wait) == LOW);  // espera até WAIT = HIGH
  
  // RS = 1 → indica dados
  SetRS(1);

  SetDataPinsDirection(OUTPUT);     //Seleciona o sentido das informações nos pinos

  digitalWrite(_cfg.pin_wr, LOW);              //WR ativo

  //uint8_t temp = static_cast<uint8_t>(_cfg.parallel_type) & 0x1;
  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type) & 0x18;
  //uint8_t bits = 8 * (1 << temp);              //Numero de bits (8/16)

  //Configura os pinos de dados como saída e escreve comando apenas no bit 7-0
  for (int i = 0; i < PARALLEL8; i++) {
	digitalWrite(_cfg.data_pins[i], (cmd >> i) & 0x01);
  }
  
  //Configura os pinos de dados como saída e escreve comando zerado a parte alta no bit 15-8
  //o comando possui apenas 8 bits, mesmo que o barramento seja de 16 bits, mas a cotnroladora RA8889 precisa receber todos os bits.
  cmd = 0;
  for (int i = PARALLEL8; i < bits; i++) {
    pinMode(_cfg.data_pins[i], OUTPUT);
    digitalWrite(_cfg.data_pins[i], (cmd >> i) & 0x01);
  }

  // Pulso de enable (EN) para gravar os dados
  PulseEN();

  //Completa a escrita: WR volta para HIGH
  digitalWrite(_cfg.pin_wr, HIGH);

  SetCS(1);
  return;  
}
void Bus_Parallel::DataWrite8(uint8_t data) {DataWrite(data);}



//se o barramento escolhido for de 16 bits esta funcao irá enviar todos os valores de D15..D0
void Bus_Parallel::DataWrite16(uint16_t data)
{
  SetCS(0);
  delayMicroseconds(1);
  while (digitalRead(_cfg.pin_wait) == LOW);   // espera até WAIT = HIGH
  
  // RS = 1 → indica dados
  SetRS(1);

  SetDataPinsDirection(OUTPUT);     //Seleciona o sentido das informações nos pinos

  digitalWrite(_cfg.pin_wr, LOW);              //WR ativo

  //uint8_t temp = static_cast<uint8_t>(_cfg.parallel_type) & 0x1;
  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type) & 0x18;
  //uint8_t bits = 8 * (1 << temp);              //Numero de bits (8/16)

  //Configura os pinos de dados como saída e escreve comando apenas no bit 15-0
  for (int i = 0; i < PARALLEL16; i++) {
	digitalWrite(_cfg.data_pins[i], (cmd >> i) & 0x01);
  }

  // Pulso de enable (EN) para gravar os dados
  PulseEN();

  //Completa a escrita: WR volta para HIGH
  digitalWrite(_cfg.pin_wr, HIGH);

  SetCS(1);
}


//esta funcao nao cosnegue enviar apra o brramento, pois é de 24 bits
void Bus_Parallel::DataWrite24(uint32_t data)
{
  return;
}


/**
 * @brief Lê um byte (8 bits) de dados da controladora RA8889.
 * * Usada para ler registradores de estado ou dados, lendo apenas os 8 bits baixos (D[7:0]).
 * @return uint8_t O byte lido do barramento de dados (D[7:0]).
 */
uint8_t Bus_Parallel::DataRead8()
{
    uint8_t data = 0;
    
    // 1. Seleciona o chip (CS ativo em nível baixo)
    SetCS(0);

    // 2. RS = 1 → indica que leremos dados
    SetRS(1);

    // 3. Verifica o pino WAIT (espera se estiver ocupado)
    delayMicroseconds(1);
    while (digitalRead(_cfg.pin_wait) == LOW); 

    SetDataPinsDirection(INPUT);     //Seleciona o sentido das informações nos pinos
    
    // 5. Ativa o pino de Leitura (RD ativo em nível baixo) para habilitar os buffers de saída da RA8889
    // **NOTA:** É crucial que _cfg.pin_rd tenha sido adicionado à sua estrutura de configuração.
    digitalWrite(_cfg.pin_rd, LOW); 

    // 6. Pulso de Enable (EN) para o clock/latch, que coloca o dado no barramento
    PulseEN(); 

    // 7. Lê os dados no barramento (APENAS D[7:0])
    for (int i = 0; i < PARALLEL8; i++) {
        if (digitalRead(_cfg.data_pins[i]) == HIGH) {
            data |= (1 << i);
        }
    }
    
    // 8. Completa a leitura: RD volta para HIGH
    digitalWrite(_cfg.pin_rd, HIGH);

    // 9. Libera o chip
    SetCS(1);

    return data;
}


/**
 * @brief Lê uma palavra (16 bits) de dados da controladora RA8889.
 * * Usada APENAS se a interface paralela da MCU for configurada para 16 bits (PARALLEL16).
 * @return uint16_t A palavra de 16 bits lida do barramento de dados (D[15:0]).
 */
uint16_t Bus_Parallel::DataRead16()
{
    // Se a interface for 8 bits, não é possível ler 16 bits diretamente.
    // Retorna a leitura de 8 bits para evitar erro, embora o ideal seja erro/aviso.
    if (_cfg.parallel_type != PARALLEL16) {
        return static_cast<uint16_t>(DataRead8()); 
    }

    uint16_t data = 0;
    
    // 1. Seleciona o chip (CS ativo em nível baixo)
    SetCS(0);

    // 2. RS = 1 → indica que leremos dados
    SetRS(1);

    // 3. Verifica o pino WAIT (espera se estiver ocupado)
    delayMicroseconds(1);
    while (digitalRead(_cfg.pin_wait) == LOW); 

    SetDataPinsDirection(INPUT);     //Seleciona o sentido das informações nos pinos
    
    // 5. Ativa o pino de Leitura (RD ativo em nível baixo) para habilitar os buffers
    digitalWrite(_cfg.pin_rd, LOW); 

    // 6. Pulso de Enable (EN) para o clock/latch, que coloca o dado no barramento
    PulseEN();

    // 7. Lê os dados no barramento (D[15:0])
    for (int i = 0; i < PARALLEL16; i++) {
        if (digitalRead(_cfg.data_pins[i]) == HIGH) {
            data |= (1 << i);
        }
    }
    
    // 8. Completa a leitura: RD volta para HIGH
    digitalWrite(_cfg.pin_rd, HIGH);

    // 9. Libera o chip
    SetCS(1);

    return data;
}


uint8_t Bus_Parallel::StatusRead(void)
{
  return 0;
}


void Bus_Parallel::RegisterWrite(uint8_t reg, uint8_t data)
{
  return;
}


uint8_t Bus_Parallel::RegisterRead(uint8_t reg)
{
  return 0;
}
