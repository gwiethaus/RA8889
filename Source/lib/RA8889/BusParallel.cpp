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


/**
 * @brief Inicializa o barramento paralelo e configura todos os pinos necessários.
 *
 * Este método deve ser chamado antes de qualquer operação de escrita ou leitura.
 * Ele configura os pinos de controle (RS, WR, RD, EN, CS), além do barramento de
 * dados conforme o modo definido (ex.: 8 ou 16 bits).
 *
 * O sinal WAIT é configurado como entrada com pull-up para permitir handshake
 * (READY) caso o dispositivo suporte esse recurso. O pino INT é configurado
 * somente se foi definido na configuração `_cfg`.
 *
 * Esta função é segura para múltiplas chamadas: após a primeira inicialização,
 * chamadas subsequentes não terão efeito.
 */
uint32_t Bus_Parallel::Init(void) 
{
  if (_parallel_init) return -1;
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
  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type);
  for (int i = 0; i < bits; i++) pinMode(_cfg.data_pins[i], _parallel_pindir);

  return 0;
}


/**
 * @brief Inicia uma operação de escrita no barramento paralelo.
 *
 * Esta função existe para manter compatibilidade com a interface genérica do
 * barramento (ex.: IBus). Diferentemente do barramento SPI, o barramento paralelo
 * não utiliza um mecanismo de início/fim explícito de transação: o controle é
 * feito diretamente pelos sinais WR, EN e WAIT a cada transferência.
 *
 * Portanto, nesta implementação, a função não executa nenhuma ação.
 *
 * @return Sempre retorna 0 (sem efeito).
 */
uint8_t  Bus_Parallel::StartWrite(void)
{
  return 0;
}


/**
 * @brief Finaliza uma operação de escrita no barramento paralelo.
 *
 * Assim como StartWrite(), esta função existe apenas para cumprir o contrato
 * definido pela interface IBus. O barramento paralelo não requer finalização
 * explícita, pois cada ciclo é concluído individualmente por sinais dedicados.
 *
 * Desta forma, esta função é propositalmente vazia.
 */
void  Bus_Parallel::EndWrite(void)
{
  return;
}


/**
 * @brief Realiza bloqueio do barramento pelo pino Chip Select (CS)
 *
 * Não serão executados bloqueio/desbloqueio de barramento pelo pino CS dentro 
 * de metodos/funções. Desta forma ocorrendo apenas uma única chamada de 
 * bloqueio  por esta função LockBus()
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param force_unlock Força o desbloquio do barramento caso por algum motivo estava bloqueado os o esquecimento do uso da função UnlockBus();
 *
 */
void Bus_Parallel::LockBus(bool force_unlock)
{
  if (force_unlock) {
    if (digitalRead(_cfg.pin_cs) == LOW) {
	  SetCS(1);  //para evitar alguma coisa mal resolvida
	  _lock_bus = false;
	}
  }
  if (_lock_bus) return;
  SetCS(0);
  _lock_bus = true;
}


/**
 * @brief Realiza desbloqueio do barramento pelo pino Chip Select (CS)
 *
 * Serão novamente executados bloqueio/desbloqueios de barramento pelo pino CS 
 * dentro de metodos/funções. Desta forma, é liberado o barramento por essa 
 * função LockBus().
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 */
void Bus_Parallel::UnlockBus(void)
{
  if (!_lock_bus) return;
  _lock_bus = false;
  SetCS(1);
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
 * @brief Controla o pino Chip Select (CS) do barramento Paralelo.
 *
 * Esta função ativa ou desativa o dispositivo Paralelo conectado ao pino CS.
 * É utilizada para garantir que apenas um dispositivo Paralelo esteja ativo
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
  if (_lock_bus) return;	
  level_cs == 0 ? digitalWrite(_cfg.pin_cs, LOW) : /*SS_RESET */  digitalWrite(_cfg.pin_cs, HIGH); /*SS_SET*/
}


/**
 * @brief Define o modo do barramento paralelo para envio de comandos ou dados.
 *
 * Esta função controla o sinal RS (Register Select), responsável por indicar ao
 * dispositivo externo (ex.: controladora gráfica RA8889) se o valor colocado no
 * barramento paralelo deve ser interpretado como um comando ou como dados.
 *
 * - `RS = 0` → O próximo valor escrito no barramento será tratado como **comando**.
 * - `RS = 1` → O próximo valor escrito no barramento será tratado como **dado**.
 *
 * @param level_rs Nível lógico desejado para RS:
 *  - `0` → Seleciona modo **comando**
 *  - `1` → Seleciona modo **dados**
 *
 * @note Esta função deve ser chamada antes de operações de escrita no barramento.
 *       Normalmente, o fluxo é: SetRS() → SetDataPinsDirection() → RwByte().
 *
 * @example
 * @code
 * // Envia um comando ao display
 * SetRS(0);
 * RwByte(0x2A);
 *
 * // Envia dados referentes ao comando
 * SetRS(1);
 * RwByte(0x10);
 * RwByte(0x20);
 * @endcode
 */
void Bus_Parallel::SetRS(uint8_t level_rs)
{
  digitalWrite(_cfg.pin_rs, (level_rs == 0) ? LOW : HIGH);
}


/**
 * @brief Configura a direção (INPUT ou OUTPUT) dos pinos do barramento paralelo.
 *
 * Esta função ajusta a direção elétrica dos pinos de dados do barramento paralelo,
 * definindo-os como entrada ou saída conforme necessário. A quantidade de pinos
 * configurados depende do tipo de barramento selecionado na configuração
 * (8 bits ou 16 bits).
 *
 * A função realiza uma otimização importante:
 * - O driver mantém o estado atual da direção em `_parallel_pindir`.
 * - Caso o mesmo modo seja solicitado novamente, nenhuma alteração é feita.
 *   Isso evita chamadas repetidas a pinMode(), que são operações custosas,
 *   especialmente quando realizadas a cada byte ou cada transação.
 *
 * @note Esta função deve ser chamada antes de operações de leitura ou escrita no
 * barramento. Em barramentos de 16 bits, todos os 16 pinos são configurados;
 * em barramentos de 8 bits, apenas os 8 pinos correspondentes.
 *
 * @param direction Direção desejada para os pinos:
 * - `OUTPUT` → habilita escrita para o barramento.
 * - `INPUT`  → habilita leitura do barramento.
 *
 * @example
 * @code
 * SetDataPinsDirection(OUTPUT); // prepara barramento para enviar dados
 * RwByte(0xAA);
 *
 * SetDataPinsDirection(INPUT);  // agora queremos ler dados do barramento
 * uint8_t value = RwByte(0);
 * @endcode
 */
void Bus_Parallel::SetDataPinsDirection(int direction)
{
    if (_parallel_pindir == direction) return;
    
    uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type);
    for (int i = 0; i < bits; i++) {
        pinMode(_cfg.data_pins[i], direction);
    }
	
	_parallel_pindir = direction;
}


/**
 * @brief Seleciona o canal alto (bits 15..8) do barramento paralelo, caso disponível.
 *
 * Esta função é utilizada em operações de escrita/leitura onde o barramento pode ser
 * de 8 ou 16 bits. Quando o barramento é de 16 bits, esta função habilita o canal de
 * bits mais altos (bits 15..8), de modo que a próxima chamada a RwByte() atuará sobre
 * esses bits.
 *
 * Após uma chamada a RwByte() com o canal alto ativo, o driver automaticamente retorna
 * para o canal baixo (bits 7..0).
 *
 * @warning Esta funcao apenas serve para barramento de 8 ou 16 atualmente
 *
 * Exemplo:
 * @code
 * RwByte(0x04);   // escreve no canal baixo (bits 7..0)
 * RwHighByte();   // seleciona canal alto se barramento for 16 bits
 * RwByte(0xFF);   // escreve nos bits 15..8 e retorna automaticamente ao canal baixo
 * @endcode
 *
 * Se o barramento for de 8 bits, esta função não tem efeito funcional.
 *
 * @see RwByte()
 *
 */
void Bus_Parallel::RwHighByte(void)
{
  _channel_high = (static_cast<uint8_t>(_cfg.parallel_type) >> 4) & 0x01; 
}


/**
 * @brief Lê ou escreve um byte no barramento paralelo (8 ou 16 bits).
 *
 * Esta função realiza a transferência de 1 byte no barramento de dados paralelo.
 * Caso a operação esteja configurada como saída (OUTPUT), o valor passado no
 * parâmetro `value` é colocado nos pinos correspondentes do barramento.
 *
 * Caso esteja configurada como entrada (INPUT), a função lê os pinos e retorna
 * um byte montado a partir do estado lógico desses pinos.
 *
 * Se o barramento for de 16 bits, o membro interno `_channel_high = 1` controla se
 * o byte será enviado/lido no grupo de bits mais altos (bits 15..8). Caso
 * `_channel_high = 0`, a operação ocorre sempre nos bits baixos (bits 7..0).
 *
 * Após a operação, `_channel_high` é sempre resetado para 0.
 *
 * @warning Apenas com sitema de barramento de 8 e 16 bits
 *
 * @param value Byte a ser escrito no barramento quando a direção estiver em OUTPUT.
 *              Ignorado em operações de leitura.
 *
 * @return 
 *   - Se a direção do barramento for OUTPUT: retorna o mesmo `value`.
 *   - Se a direção for INPUT: retorna um byte formado pelos pinos lidos.
 *
 * @see RwHighByte()
 */
uint8_t Bus_Parallel::RwByte(uint8_t value)
{
  uint8_t temp = 0;
  uint8_t d = 8 * _channel_high;
  
  if (_parallel_pindir == OUTPUT) {
    for (uint8_t i = 0; i < 8; i++)
	  digitalWrite(_cfg.data_pins[i + d], (value >> i) & 0x01);
    return value;
  } else if (_parallel_pindir == INPUT) {
    for (uint8_t i = 0; i < 8; i++)
	  temp |= (digitalRead(_cfg.data_pins[i + d])  & 0x01) << i;
  }
 
  return temp;
}


uint32_t Bus_Parallel::RwBytes(const uint8_t* data, uint32_t len)
{
  return 0;
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

  RwByte(cmd);                                 //escreve byte de comando
  RwHighByte();                                //acessa canal alto de 15..8 bits
  RwByte(0x00);                                //zero na parte alta

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

  while (digitalRead(_cfg.pin_wait) == LOW);  // espera até WAIT = HIGH
  
  // RS = 1 → indica dados
  SetRS(1);

  SetDataPinsDirection(OUTPUT);                //Seleciona o sentido das informações nos pinos

  digitalWrite(_cfg.pin_wr, LOW);              //WR ativo

  uint8_t bits = static_cast<uint8_t>(_cfg.parallel_type) & 0x18;

  RwByte(data);                                //escreve byte de dados
  RwHighByte();                                //acessa canal alto de 15..8 bits
  RwByte(0x00);                                //zero na parte alta, se for de 8 bits o barramento
  
  // Pulso de enable (EN) para gravar os dados
  PulseEN();

  //Completa a escrita: WR volta para HIGH
  digitalWrite(_cfg.pin_wr, HIGH);

  SetCS(1);
  return;  
}


/**
 * @brief Calcula o limite superior de um grupo baseado em tamanho de bloco.
 *
 * Esta função é utilizada para ajustar a quantidade total de bytes
 * que serão escritos no barramento, considerando tanto o tamanho 
 * real do dado (step) quanto a largura física do barramento (bus_bytes).
 *
 * O resultado sempre retorna o menor múltiplo de `step` que seja
 * maior ou igual a `i`.
 *
 * Exemplo para step = 3:
 *   i: 1 → 3  
 *   i: 2 → 3  
 *   i: 3 → 3  
 *   i: 4 → 6  
 *   i: 5 → 6  
 *   i: 6 → 6  
 *
 * Fórmula:
 *     result = ceil(i / step) * step
 * Implementado como:
 *     ((i - 1) / step) * step + step
 *
 * @param i     Quantidade base a ajustar (ex.: bytes do barramento).
 * @param step  Tamanho natural de agrupamento desejado (bytes de escrita).
 * @return Valor ajustado para o limite superior do grupo.
 */
inline uint32_t calc_group(uint32_t i, uint32_t step)
{
    return ((i - 1) / step) * step + step;
}


/**
 * @brief Escreve dados no barramento paralelo, ajustando automaticamente conforme
 *        largura real do barramento (8 ou 16 bits) e quantidade de bytes desejados (step).
 *
 * Esta função permite enviar de 1 até 4 bytes (8 a 32 bits) de dados para controladoras
 * gráficas ou periféricos que utilizam barramento paralelo configurável (ex.: 8, 16 bits, 
 * etc), com a seguinte organização:
 *
 *  - `parallel_type` define quantos bytes o barramento físico utiliza:
 *        PARALLEL8  → 1 byte por ciclo de escrita
 *        PARALLEL16 → 2 bytes por ciclo (byte baixo + byte alto)
 *
 *  - `step` define quantos bytes do valor `data` devem ser efetivamente utilizados.
 *    Bytes além de `step` serão automaticamente zerados na parte alta.
 *
 *  - Caso o dado (step) seja menor que o barramento, a parte alta é zerada:
 *        Ex.: step=1 e barramento=16 → escreve LOW e zera HIGH
 *
 *  - Caso step > barramento, a função passa para próxima palavra naturalmente.
 *
 * O parâmetro `step` determina quantos bytes do valor serão utilizados. Por exemplo:
 * - step = 0 → envia 1 byte  (8 bits)
 * - step = 1 → envia 2 bytes (16 bits)
 * - step = 2 → envia 3 bytes (24 bits)
 * - step = 3 → envia 4 bytes (32 bits)
 *
 * Quando o barramento possui largura maior que o número de bytes sendo enviados,
 * bytes adicionais são automaticamente zerados (preenchimento). Porém, quando
 * `step` é ímpar (como em envios de 24 bits), o último byte real deve ser mantido
 * sem zerar — e é aqui que entra o ajuste condicional:
 *
 *   (i == step) && (step % 2 != 0)
 *
 * Isso garante que o último byte útil **não será substituído por 0** em barramentos
 * onde o alinhamento de bytes poderia causar truncamento.
 *
 * Fluxo da função:
 *  1. Aguarda sinal WAIT estar liberado.
 *  2. Configura RS = 1 para indicar envio de dados.
 *  3. Define direção dos pinos como saída.
 *  4. Calcula quantos bytes devem ser enviados com ajuste via calc_group().
 *  5. Realiza a escrita sequencial via RwByte() e alternância via RwHighByte().
 *  6. Finaliza pulso de gravação e libera barramento.
 *
 * Esta lógica garante portabilidade e escalabilidade para futuras larguras
 * de barramento superiores (ex.: 24 ou 32 bits), sem alterar código estrutural.
 *
 * @param data Valor de até 32 bits a ser escrito no barramento.
 * @param step Quantidade de bytes significativos em `data` que devem ser enviados (1 a 4).
 * @param data Valor de até 32 bits a ser enviado pelo barramento.
 */
void Bus_Parallel::DataWrite(uint32_t data, uint8_t step)
{
  SetCS(0);

  while (digitalRead(_cfg.pin_wait) == LOW);   //Espera até WAIT = HIGH
  
  // RS = 1 → indica dados
  SetRS(1);

  SetDataPinsDirection(OUTPUT);                //Seleciona o sentido das informações nos pinos

  digitalWrite(_cfg.pin_wr, LOW);              //WR ativo

  uint8_t i= 0;
  uint8_t bus_bytes = static_cast<uint8_t>(_cfg.parallel_type) / 8;
  uint8_t numbytes = calc_group(bus_bytes, step);

  while (i < numbytes) {
    //if (i == step) && (step % 2 != 0) temp = 0 else temp = data;
    uint8_t temp = ((i == step) && (step % 2 != 0)) ? 0 : (data >> (i * 8));
    RwByte(temp); 
    RwHighByte();                              //Se o barramento for de 8-bits esta funcao nao terá efeito      
    i++;
  }
  
  // Pulso de enable (EN) para gravar os dados
  PulseEN();

  //Completa a escrita: WR volta para HIGH
  digitalWrite(_cfg.pin_wr, HIGH);

  SetCS(1);
  return;  
}


/**
 * @brief Lê um byte (8 bits) de dados da controladora RA8889.
 * * Usada para ler registradores de estado ou dados, lendo apenas os 8 bits baixos (D[7:0]).
 * @return uint8_t O byte lido do barramento de dados (D[7:0]).
 */
uint8_t Bus_Parallel::DataRead(void)
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


uint32_t DataRead(uint8_t address, uint8_t step)
{

  return 0;
}


/**
 * @brief Lê uma palavra (16 bits) de dados da controladora RA8889.
 * * Usada APENAS se a interface paralela da MCU for configurada para 16 bits (PARALLEL16).
 * @return uint16_t A palavra de 16 bits lida do barramento de dados (D[15:0]).
 */
uint32_t Bus_Parallel::DataRead(uint8_t step)
{
    // Se a interface for 8 bits, não é possível ler 16 bits diretamente.
    // Retorna a leitura de 8 bits para evitar erro, embora o ideal seja erro/aviso.
    if (_cfg.parallel_type != PARALLEL16) {
        return static_cast<uint16_t>(DataRead()); 
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







/** 
 * @brief Escreve um buffer de dados para Paralelo
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @return None
 *
 * @see SetCS()
 * @see SetRS()
 */
void Bus_Parallel::WriteBytes(const uint8_t* data, size_t len)
{
  if (!_parallel_init || data == nullptr || len == 0) return;
  SetCS(0);                                    //SS_RESET
  //escrever aqui codigo
  SetCS(1);                                    //SS_SET
}
