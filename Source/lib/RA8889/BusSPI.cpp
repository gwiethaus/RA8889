#include <BusSPI.hpp>


//Isso nao se torna visivel ou acessivel pois esta declarado dentro de private
#if defined(SPI_ESP32_NATIVE)
spi_device_handle_t Bus_SPI::spi_handle = nullptr;
#endif


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
 * @brief Configura o proctocolo de comunicação SPI para o display
 *
 * @verbatim
 * Usa structs internas do IBus (proposta “genérica”)
 * A área de código mostra exemplo de como usar o Config()
 * @endverbatim
 * 
 * @code
 * Bus_SPI spi;
 * RA8889 gfx(PIN_CS, PIN_RESET);
 * 
 * void setup() {
 *      
 *   IBus::SPIBusConfig_t cfg;
 *   cfg.spi_type = SPI2_HOST;                 //Usando barramento SPI2
 *   cfg.pin_mosi = PIN_MOSI;
 *   cfg.pin_miso = PIN_MISO;
 *   cfg.pin_sclk = PIN_SCLK;
 *   cfg.pin_cs   = PIN_CS;
 *   cfg.freq_write = 40000000;                //Frequencia de escrita de 40MHz
 *   
 *   spi.Config(&cfg);                         //Grava a configuração
 *   gfx.setBus(spi);                          //Seta o Bus SPI
 *   gfx.Begin();                              //inicializa o display 
 * @endcode
 *
 * @param IBusConfig_t* cfg
 *        
 * @note None
 */
void Bus_SPI::Config(const IBusConfig_t* cfg) 
{
  //Cast seguro para SPIBusConfig
  const SPIBusConfig_t* scfg = static_cast<const SPIBusConfig_t*>(cfg);
  _cfg = *scfg;
}


/**
 * @brief Escolha do barramento de comunicação SPI para o display
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param SPIHost spihost
 *        
 * @note None
 */
void Bus_SPI::createSPI(SPIHost spihost)
{
#if defined(SPI_ARDUINO_CORE)

  //Se já existia um ponteiro de uma instância do objeto no heap, destrói
  if(spi) {
    delete spi;
    spi = nullptr;
	_spi_init = false;
  }

  #if defined(ESP32)  //familia do ESP32

    // Família ESP32
    switch (spihost) {

    #if HAS_HSPI
      case HOST_HSPI:
           spi =  new SPIClass(HSPI);
           break;
    #endif
    
    #if HAS_VSPI
      case HOST_VSPI:
           spi = new SPIClass(VSPI);
           break;
    #endif
    
    #if HAS_FSPI
       case HOST_FSPI:
            spi = new SPIClass(FSPI);
            break;
    #endif
    
       case HOST_SPI:
       default:
            spi = new SPIClass(); // SPI padrão
            break;
    }
  #else
    // Outros MCUs (Arduino, STM32, etc.)
    spi = new SPIClass();
  #endif

#elif defined(SPI_ESP32_NATIVE) 

  #warning "Method Bus_SPI::createSPI() not implemented for ESP32 with Native SPI"

#endif
}


/**
 * @brief Encontra a melhor frequência SPI dentro da faixa suportada (8–80 MHz).
 *
 * - Usa divisores inteiros de 1 a 10 → 80, 40, 26.6, 20, 16, 13.3, 11.4, 10, 8.9, 8 MHz
 * - Retorna a frequência real e o divisor escolhido.
 * - Se o valor pedido for maior que 80 MHz → retorna 80 MHz (div = 1)
 * - Se o valor pedido for menor que 8 MHz → retorna 8 MHz (div = 10)
 * - Caso contrário, escolhe o valor imediatamente abaixo do pedido.
 *
 * @param requested_hz  Frequência SPI desejada em Hz
 * @param div_out       Ponteiro opcional para retornar o divisor inteiro usado
 * @return uint32_t     Frequência SPI ajustada em Hz
 * 
 * @note Clock base do SPI (80 MHz no ESP32-S3) APB_CLK_FREQ 80000000UL
 * @see ESP-IDF: esp_clk_apb_freq()
 */
uint32_t Bus_SPI::Find_SPI_Frequence(uint32_t requested_hz, uint8_t* div_out)
{
  const uint8_t max_div = 10;
  uint32_t best_freq = SPI_CLOCK_SPEED_MAX;
  uint8_t best_div = 1;
  
  // Se pediu acima de 80 MHz → limita no máximo
  if (requested_hz >= SPI_CLOCK_SPEED_MAX) {
    best_div = 1;
    best_freq = SPI_CLOCK_SPEED_MAX;
  } 
  else {
    // Percorre divisores válidos (1 a 10)
    for (uint8_t div = 1; div <= max_div; div++) {
      uint32_t freq = SPI_CLOCK_SPEED_MAX / div;
      
      // quando a pedida é >= freq, achamos o divisor mais próximo abaixo
      if (requested_hz >= freq) {
        best_freq = freq;
        best_div = div;
        break;
      }
    }
    
    // Se for menor que o mínimo suportado (8 MHz)
    if (best_freq < (SPI_CLOCK_SPEED_MAX / max_div)) {
      best_freq = (SPI_CLOCK_SPEED_MAX / max_div);
      best_div = max_div;
    }
  }
  
  if (div_out) *div_out = best_div;
  return best_freq;
}


/**
 * @brief Inicializa o SPI para a comunicacao com o Display
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
 * @result codigo de erro, 0 sem erro, -1(0xFFFFFFFF) erro generico
 *
 * @note O pino CS é tratado como "ativo baixo". Portanto, um nível LOW
 * (representado por 0) "bloqueia" o barramento para este dispositivo
 * específico, permitindo operações de escrita/leitura. Um nível HIGH
 * (representado por 1) "libera" o barramento para que outros dispositivos
 * possam utilizá-lo.
 */
uint32_t Bus_SPI::Init(void)
{
#if defined(SPI_ARDUINO_CORE)
  if (_spi_init) return -1;
  
  pinMode(_cfg.pin_cs, OUTPUT);
  digitalWrite(_cfg.pin_cs, HIGH);
  _lock_bus = false;

  createSPI(static_cast<SPIHostType>(_cfg.spi_host));

  _spi_clockmax = (_cfg.freq_write > 0) ? _cfg.freq_write : SPI_CLOCK_SPEED_MAX;
  _spi_datamode  = SPI_MODE0;
  _spi_dataorder = MSBFIRST;

  spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
  _spi_transaction = true;

  // Determina qual SPI usar
  #if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
 
    spi->begin(_cfg.pin_sclk, _cfg.pin_miso, _cfg.pin_mosi, _cfg.pin_cs);

  #elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)

    spi->begin();

  #else

    #warning "SPI não implementada para esta plataforma"
    return -1;
  
  #endif

  _spi_startwrite = false;
  _spi_init = true;
  return 0;

#elif defined(SPI_ESP32_NATIVE)

  if (_spi_init) return static_cast<uint32_t>(ESP_FAIL);
  _lock_bus = false;

  _spi_datamode  = SPI_MODE0;
  _spi_dataorder = MSBFIRST;

  esp_err_t ret;
  //implementr mais tarde, agora ficara sempre ligado
  //uint32_t dma_buf_sz = _cfg.use_dma ? DMA_BUFFER_SIZE : (uint32_t)-1;
  uint32_t dma_buf_sz = DMA_BUFFER_SIZE;

  //-----------------------------------------------------
  // Ajuste da frequência para o S3
  //-----------------------------------------------------
  _spi_clockmax = (_cfg.freq_write > 0) ? _cfg.freq_write : SPI_CLOCK_SPEED_MAX;
  _spi_clockmax = Find_SPI_Frequence(_spi_clockmax, &_spi_divider_clock);

  //-----------------------------------------------------
  // 1. Configuração do barramento SPI (DMA sempre ativo)
  //-----------------------------------------------------
  spi_bus_config_t buscfg = {
  #if CONFIG_IDF_TARGET_ESP32                  // ESP32 Classic
      .miso_io_num     = _cfg.pin_miso,        // MISO
      .mosi_io_num     = _cfg.pin_mosi,        // MOSI
      .sclk_io_num     = _cfg.pin_sclk,        // CLK 
      .quadwp_io_num   = -1,                   // WP não usado
      .quadhd_io_num   = -1,                   // HD não usado
      .max_transfer_sz = dma_buf_sz,           // Deve ser DMA-capable
  #else                                        // ESP32-S2 / S3 / C3 
      .mosi_io_num     = _cfg.pin_mosi,        // MOSI
      .miso_io_num     = _cfg.pin_miso,        // MISO
      .sclk_io_num     = _cfg.pin_sclk,        // CLK
      //.data2_io_num    = -1,                   // WP, antigo quadwp_io_num
      //.data3_io_num    = -1,                   // HD, antigo quadhd_io_num
      // max_transfer_sz define o tamanho máximo que o DMA pode gerenciar de uma vez.
      .max_transfer_sz = dma_buf_sz            // Deve ser DMA-capable
    #endif
  };

  //2. Configuração do Dispositivo SPI
  spi_device_interface_config_t devcfg = {
  #if CONFIG_IDF_TARGET_ESP32                  // ESP32 clássico
     .clock_speed_hz = _spi_clockmax,          // Exemplo: 40 MHz
     .mode           = _spi_datamode,          // SPI Mode [0,1,2,3]
     .spics_io_num   = _cfg.pin_cs,            // CS pin
     .queue_size     = 3,                      // Pode enfileirar 3 transações
  #else                                        // ESP32-S2 / S3 / C3
    .command_bits     = 0,
    .address_bits     = 0,
    .dummy_bits       = 0,
    .mode             = _spi_datamode,         // SPI Mode [0,1,2,3]
    .duty_cycle_pos   = 128,                   // 0 = usar 50% automaticamente (equivalente ao antigo 128, mas tambem valido)
    .cs_ena_pretrans  = 0,                     // não manter CS antes
    .cs_ena_posttrans = 0,                     // não manter CS depois
    .clock_speed_hz   = _spi_clockmax,         // Exemplo: 40 MHz
    .input_delay_ns   = 0,                     // 0 = sem delay extra no MISO (display nem usa MISO normalmente); 50 ns na amostragem de MISO, ajudando estabilidade em cabos longos ou clock > 40 MHz.
    .spics_io_num     = -1,                    // <--- CHAVE 1: Desabilita o controle automático do CS, _cfg.pin_cs,
    .flags            = 0,                     // SEM SPI_DEVICE_HALFDUPLEX aqui, a não ser que seja necessário; SPI_DEVICE_NO_DUMMY insere byte dummy
    .queue_size       = 3,                     // =3 Pode enfileirar 3 transações
    .pre_cb           = nullptr,
    .post_cb          = nullptr
  #endif  
  };

  //Inicializa o barramento SPI e habilita o DMA automaticamente (SPI_DMA_CH_AUTO)
  ret = spi_bus_initialize(_cfg.spi_host, &buscfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) return static_cast<uint32_t>(ret);

  // Adiciona o dispositivo ao barramento
  ret = spi_bus_add_device(_cfg.spi_host, &devcfg, &spi_handle);
  if (ret != ESP_OK) return static_cast<uint32_t>(ret);

  //-----------------------------------------------------
  // 3. Configuração manual do pino CS
  //-----------------------------------------------------
  gpio_set_direction((gpio_num_t)_cfg.pin_cs, GPIO_MODE_OUTPUT);
  gpio_set_level((gpio_num_t)_cfg.pin_cs, 1);        // CS HIGH (inativo)

  //-----------------------------------------------------
  // 4. Alocação do buffer interno DMA-capable(SRAM interna)
  //    O buffer interno é usado para garantir que os dados 
  //    de Tx e Rx sejam DMA-capable.
  //Nota: Se o uso do framebuffe estiver na PSRAM, o 
  //      alinhamento será ignorado, caso contrário será 
  //      feito tasnferencias mais rapidas,s em afetar a
  //      quantidade de bytes que será enviado (sem a necessidade
  //      de ser multiplos de 4)
  //-----------------------------------------------------
  //dma_buffer = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
  dma_buffer = (uint8_t*)heap_caps_aligned_alloc(4, DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
  if (dma_buffer == nullptr) {
      printf("ERRO: Falha ao alocar dma_buffer_internal em SRAM DMA-capable.\n");
      return ESP_ERR_NO_MEM;
  }

  _spi_startwrite = false;
  _spi_init = true;
  return ESP_OK;
#endif  
}


/**
 * @brief Verifica e garante que exista uma transação SPI ativa.
 *
 * Esta função é utilizada em chamadas que não fazem parte de um bloco de escrita
 * volumosa (StartWrite/EndWrite), assegurando que ao menos uma transação esteja ativa.
 *
 * Caso nenhuma transação esteja em andamento, inicia uma nova automaticamente.
 *
 * É especialmente útil em comandos isolados, como leituras de status ou registros
 * de controle, prevenindo falhas por ausência de beginTransaction().
 *
 * @return void
 *
 * @note
 * - Se `_spi_startwrite == false` e `_spi_transaction == false`, inicia uma nova transação SPI.
 * - Não altera `_spi_startwrite`, pois seu objetivo é apenas manter o barramento ativo.
 */
void Bus_SPI::CheckTransaction()
{
#if defined(SPI_ARDUINO_CORE)
  if (!_spi_startwrite && !_spi_transaction) {
    spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
  	_spi_transaction = true;
  }
#elif defined(SPI_ESP32_NATIVE)
  // Não é necessário no SPI nativo do ESP32
  // As transações são gerenciadas pelo handle e DMA internamente
  #warning "Method Bus_SPI::CheckTransaction() not implemented for ESP32 with Native SPI"

#endif
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
void Bus_SPI::SetCS(uint8_t level_cs)
{
  if (_lock_bus) return;
#if defined(SPI_ARDUINO_CORE)
  level_cs == 0 ? digitalWrite(_cfg.pin_cs, LOW) : /*SS_RESET */  digitalWrite(_cfg.pin_cs, HIGH); /*SS_SET*/
  
#elif defined(SPI_ESP32_NATIVE)
  if (level_cs == 1) {NOP(); NOP(); NOP(); NOP(); NOP();}           //Requer de 10 a 500 ns de tempo antes de subir CS para termino dos dados no barramento SPI
  gpio_set_level((gpio_num_t)_cfg.pin_cs, level_cs ? 1 : 0);
  if (level_cs == 0) {NOP(); NOP(); NOP(); NOP(); NOP();}           //Requer de 10 a 500 ns de tempo apos de baixar CS para termino dos dados no barramento SPI

#endif
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
 * @param force_unlock Força o desbloquio do barramento caso por algum 
 *        motivo estava bloqueado pelo do uso da função UnlockBus();
 *
 * @see UnlockBus()
 */
void Bus_SPI::LockBus(bool force_unlock)
{
  if (force_unlock) {
    if (digitalRead(_cfg.pin_cs) == LOW) {
      SetCS(1);                                //Para evitar alguma coisa mal resolvida
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
 * @see LockBus()
 */
void Bus_SPI::UnlockBus(void)
{
  if (!_lock_bus) return;
  _lock_bus = false;
  SetCS(1);
}


/**
 * @brief Envia e recebe um único byte pelo barramento SPI.
 *
 * Esta função envia o byte fornecido pelo barramento SPI e retorna o byte
 * recebido durante a transmissão. Ideal para enviar comandos de 1 byte.
 *
 * Uso típico:
 * @code
 * SetCS(0);
 * RwByte(SPI_CMDWRITE);  // envia comando
 * RwBytes(data, len);    // envia bloco de dados
 * SetCS(1);
 * @endcode
 *
 * @param value Byte a ser enviado pelo barramento SPI.
 * @return Byte recebido durante a transmissão.
 *         Retorna 0 em caso de erro (somente no ESP32 nativo).
 *
 * @note - No Arduino Core, utiliza spi->transfer().  
 *       - No ESP32 nativo, utiliza spi_device_transmit() de forma bloqueante.  
 *       - Otimizado para 1 byte.
 */
uint8_t Bus_SPI::RwByte(uint8_t value)
{
#if defined(SPI_ARDUINO_CORE)

  uint8_t result;
  uint8_t rx_value = 0;
  result = spi->transfer(value);
  return result;

#elif defined(SPI_ESP32_NATIVE)

  //uint8_t rx_value[4] = {0,0,0,0};
  uint8_t rx_value = 0;

  // Cria uma transação de 1 byte para o SPI nativo do ESP32
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));

  t.length = sizeof(value) * 8;                // 8 bits (1 byte)
  t.rxlength = 8;                              // 8 bits (1 byte)
  //t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
  //t.tx_data[0] = value;
  t.tx_buffer = &value;                        // ponteiro para o byte a enviar
  t.rx_buffer = &rx_value;                     // reutiliza a mesma variável para receber apos envio

  //Original
  // Transmite de forma bloqueante (espera terminar)
  //esp_err_t err = spi_device_transmit(spi_handle, &t);
  //if (err != ESP_OK) return 0;                 // Retorna 0 em caso de erro

  //alteracao 
  esp_err_t err = spi_device_polling_transmit(spi_handle, &t);
  if (err != ESP_OK) return 0;                 // Retorna 0 em caso de erro

  ///so quando for masa de dados
  //esp_err_t err = spi_device_queue_trans(spi_handle, &t, portMAX_DELAY);          // Queue the DMA transfer
  //if (err != ESP_OK) return 0;                 // Retorna 0 em caso de erro
  //spi_transaction_t *ret;
  //spi_device_get_trans_result(spi_handle, &ret, portMAX_DELAY);   // Wait for transfer to complete

  return rx_value;
#endif  
}


/**
 * @brief Envia um buffer de dados pelo barramento SPI.
 *
 * Esta função envia um bloco de dados pelo SPI, utilizando o método mais rápido
 * disponível para cada plataforma:
 * 
 * - No Arduino Core (ESP32 / ESP8266), utiliza DMA interno se disponível.
 * - Em outros Arduinos, envia byte a byte ou usando transferência em bloco se suportado.
 * - No ESP32 nativo, utiliza DMA e fila de transações SPI, permitindo enviar
 *   blocos grandes de forma eficiente sem bloquear o processador enquanto a SPI transmite.
 *
 * O uso típico é:
 * @code
 * SetCS(0);
 * RwByte(SPI_CMDWRITE);   // comando único
 * RwBytes(data, len);     // bloco de dados via DMA/pipeline
 * SetCS(1);
 * @endcode
 *
 * @param data Ponteiro para o buffer de dados a ser enviado.
 * @param len  Quantidade de bytes a enviar.
 * 
 * @return Código de erro:
 *         - 0 (ESP_OK) se a transmissão foi bem-sucedida.
 *         - -1 (0xFFFFFFFF) ou outro código de erro genérico em caso de falha.
 *         - No ESP32 nativo, retorna ESP_ERR_INVALID_ARG se data == nullptr ou len == 0,
 *           e ESP_ERR_INVALID_STATE se o buffer DMA não estiver inicializado.
 *
 * @note - O DMA (Direct Memory Access) permite que o SPI transfira blocos grandes
 *         sem intervenção contínua do CPU. Cada chunk de até DMA_BUFFER_SIZE
 *         é enfileirado na fila SPI e processado em background.  
 *       - Se quiser, é possível enfileirar transações de forma assíncrona,
 *         usando 0 em vez de portMAX_DELAY, mas neste caso é necessário
 *         gerenciar quando a transação terminou.
 */
uint32_t Bus_SPI::RwBytes(const uint8_t* data, uint32_t len)
{
#if defined(SPI_ARDUINO_CORE)
  if (len == 0 || buffer == nullptr) return -1;
  #if defined(ESP32) || defined(ESP8266)
    // --- ESP32 / ESP8266 ---
    // Usa DMA interno e é o método mais rápido disponível.
    spi->writeBytes(data, len);
    //spi->transferBytes(data, nullptr, len);   //gera 461 ms
  #elif defined(ARDUINO_ARCH_SAM)
    // --- Arduino Due (ARM Cortex-M3) ---
    // Usa o método nativo de envio em bloco.
    for (size_t i = 0; i < len; i++) {
        spi->transfer(data[i]);
    }
  #elif defined(ARDUINO_ARCH_AVR)
    // --- AVR (UNO, Mega, Nano, etc.) ---
    // transfer(buf, len) nem sempre existe no AVR SPI padrão.
    // Então envia byte a byte.
    for (size_t i = 0; i < len; i++) {
        spi->transfer(data[i]);
    }
  #elif defined(ARDUINO_ARCH_RP2040)
    // --- Raspberry Pi Pico ---
    spi->transfer((void*)data, len);
  #else
    // --- Fallback genérico ---
    spi->transfer((void*)data, len);
  #endif
  return 0;

#elif defined(SPI_ESP32_NATIVE)  

  if (len == 0 || data == nullptr) return ESP_ERR_INVALID_ARG;
  if (dma_buffer == nullptr)       return ESP_ERR_INVALID_STATE; // Modulo não inicializado

  esp_err_t err;
  size_t remaining = len;
  const uint8_t* ptr = data;
  bool isinternal = esp_ptr_internal(ptr);

  while (remaining > 0) {
    
    size_t chunk = (remaining > DMA_BUFFER_SIZE) ? DMA_BUFFER_SIZE : remaining;
    const uint8_t* send_ptr = ptr;

    // Se o buffer estiver na PSRAM, copiar para o DMA-buffer
    if (!isinternal) {
      memcpy(dma_buffer, ptr, chunk);
      send_ptr = dma_buffer;
    }

    spi_transaction_t t = {};
    t.length = chunk * 8;
    t.tx_buffer = send_ptr;

    // Enfileira a transação (se não houver slot, espera)
    // usando spi_device_queue_trans(..., portMAX_DELAY) → se não houver espaço na fila, espera até ter.
    //o uso de 0 no lugar de portMAX_DELAY torna assincrono real
    spi_device_queue_trans(spi_handle, &t, portMAX_DELAY);

    // Espera terminar o DMA deste chunk antes de enviar o próximo
    // usando spi_device_get_trans_result(..., portMAX_DELAY) → espera até a DMA realmente terminar.
    //o uso de 0 no lugar de portMAX_DELAY torna assincrono real
    spi_transaction_t *ret;
    err = spi_device_get_trans_result(spi_handle, &ret, portMAX_DELAY);
    if (err != ESP_OK) return err;

    ptr       += chunk;
    remaining -= chunk;
  }

  return ESP_OK;

#endif  
}


/**
 * @brief Envia de 1 a 4 bytes (até 32 bits) pelo barramento SPI.
 *
 * Esta função envia os bytes menos significativos do valor @p data de forma consecutiva,
 * de acordo com o parâmetro @p len, na ordem LSB → MSB. 
 *
 * - No **Arduino Core**, cada byte é enviado individualmente via RwByte().
 * - No **ESP32 nativo**, os bytes são copiados para um buffer DMA-capable e enviados
 *   de forma mais eficiente via RwBytes(), aproveitando a fila de transações e DMA.
 *
 * O parâmetro @p len é automaticamente limitado entre 1 e 4 bytes, mesmo que o valor
 * informado esteja fora desse intervalo.
 *
 * @param data Valor de até 32 bits contendo os bytes a serem transmitidos.
 * @param len  Número de bytes a transmitir (1 a 4). Valores fora do intervalo
 *             serão ajustados automaticamente.
 *
 * @note
 * - Os bytes são transmitidos do menos significativo para o mais significativo (LSB → MSB),
 *   padrão compatível com controladoras gráficas como RA8889.
 * - Para dispositivos que utilizam convenção MSB → LSB, a ordem de envio deve ser ajustada.
 * - Em ESP32 nativo, a função usa RwBytes() e pode aproveitar DMA para performance otimizada.
 *
 * @see Bus_SPI::RwByte()
 * @see Bus_SPI::RwBytes()
 */
void Bus_SPI::RwByteN(uint32_t data, uint8_t len)
{
    len = (len < 1) ? 1 : (len > 4 ? 4 : len);
#if defined(SPI_ARDUINO_CORE)
    for (uint8_t i = 0; i < len; i++)
      RwByte((data >> (i * 8)) & 0xFF);

#elif defined(SPI_ESP32_NATIVE)
    uint8_t buf[4];
    for (uint8_t i = 0; i < len; i++)
        buf[i] = (data >> (i * 8)) & 0xFF;
    RwBytes(buf, len);   // RwBytes já usa DMA se disponível
#endif
}


/**
 * @brief Escreve comando 1 byte para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param cmd: command value for RA8889
 *
 * @note Note
 */
void Bus_SPI::CmdWrite(uint8_t cmd)
{
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_CMDWRITE);                        //0x00, Avisa Display que será um comando
  RwByte(cmd);                                 //Envia um comando de 1 byte para o Display
  SetCS(1);                                    //SS_SET
}


/**
 * @brief Escreve dados 1 byte para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param cmd: command value for RA8889
 *
 * @note Note
 */
void Bus_SPI::DataWrite(uint8_t data)
{
  CheckTransaction();
  SetCS(0);                                    //SS_RESET;
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever
  RwByte(data);                                //Envia um byte de Dado para o SPI
  SetCS(1);                                    //SS_SET;
}


/**
 * @brief Escreve de 1 a 4 bytes (até 32 bits) na controladora de display via SPI.
 *
 * Esta função envia consecutivamente 1 a 4 bytes, precedidos pelo prefixo de controle
 * SPI_DATAWRITE (0x80), que indica à controladora que os bytes subsequentes são dados.
 *
 * O valor de @p data é dividido em blocos de 8 bits e enviado na ordem LSB → MSB,
 * conforme o padrão SPI da maioria dos controladores gráficos (ex.: RA8889).
 *
 * O parâmetro @p step define a quantidade de bytes a transmitir e é automaticamente
 * limitado entre 1 e 4, mesmo que o valor informado esteja fora desse intervalo.
 *
 * @param data Valor de até 32 bits contendo os dados a serem transmitidos.
 *             Apenas os bytes menos significativos são utilizados, conforme @p step.
 * @param step Número de bytes a transmitir (1 a 4). Valores fora desse intervalo
 *             serão ajustados automaticamente.
 *
 * @note 
 * - A sequência de transmissão é:
 *   1. Verificação de transação ativa via CheckTransaction().
 *   2. Ativação do sinal CS (SetCS(0)).
 *   3. Envio do byte de controle SPI_DATAWRITE (0x80).
 *   4. Transmissão de @p step bytes de dados em ordem LSB → MSB via RwByteN().
 *   5. Desativação do sinal CS (SetCS(1)).
 * - Para controladoras que utilizam convenção MSB → LSB, ajuste a ordem no envio.
 * - Em ESP32 nativo, a função RwByteN() aproveita DMA para maior performance.
 *
 * @see Bus_SPI::CmdWrite()
 * @see Bus_SPI::RwByte()
 * @see Bus_SPI::RwByteN()
 * @see Bus_SPI::SetCS()
 * @see Bus_SPI::CheckTransaction()
 */
void Bus_SPI::DataWrite(uint32_t data, uint8_t step)
{
  CheckTransaction();
  SetCS(0);                                    //SS_RESET;
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever 
  RwByteN(data, step);                         //Envia de byte 1 até 4 de dados para o SPI
  SetCS(1);                                    //SS_SET;
}


/**
 * @brief Ler dados de 1 byte da controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note Note
 */
uint8_t Bus_SPI::DataRead(void)
{
  uint8_t temp;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  temp = RwByte(0x00);                         //
  SetCS(1);                                    //SS_SET
  return temp;
}


/**
 * @brief Lê dados da controladora RA8889 (ou similar) via barramento SPI.
 *
 * Esta função realiza a leitura de 1 a 4 bytes consecutivos a partir do registrador
 * de dados da controladora gráfica. O primeiro byte recebido corresponde à parte mais
 * alta (MSB) e o último byte à parte mais baixa (LSB). O valor final é composto em uma
 * variável de 32 bits no formato MSB→LSB (ex: 0xAABBCCDD).
 *
 * @verbatim
 * Exemplo de sequência:
 *  Bytes recebidos: 0xAA, 0xBB, 0xCC, 0xDD
 *  Valor final:     0xAABBCCDD
 * @endverbatim
 *
 * @param step Quantidade de bytes a ler (1 a 4). Valores fora do intervalo são
 *             automaticamente limitados.
 *
 * @return uint32_t Valor lido (até 32 bits), composto na ordem MSB→LSB.
 *
 * @note A comunicação SPI deve estar previamente configurada. A função controla
 *       automaticamente o sinal de chip select (CS) e envia o comando SPI_DATAREAD
 *       antes de iniciar a leitura dos bytes de dados.
 *
 * @see RwByte()
 * @see CheckTransaction()
 * @see SetCS()
 */
uint32_t Bus_SPI::DataRead(uint8_t step)
{
  uint32_t data;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  for (uint8_t i = 0; i < step; i++) {
    uint8_t byte_in = RwByte(0x00);
    data = (data << 8) | byte_in;              // MSB primeiro → desloca antes de somar
  }
  SetCS(1);                                    //SS_SET
  return data;
}


/**
 * @brief Lê dados da controladora RA8889 (ou similar) via barramento SPI.
 *
 * Esta função realiza a leitura de 1 a 4 bytes consecutivos a partir de um endereço
 * da controladora gráfica. O primeiro byte recebido corresponde à parte mais alta
 * (MSB) e o último byte à parte mais baixa (LSB). O valor final é composto em uma
 * variável de 32 bits no formato MSB→LSB (ex: 0xAABBCCDD).
 *
 * @verbatim
 * Exemplo de sequência:
 *  Bytes recebidos: 0xAA, 0xBB, 0xCC, 0xDD
 *  Valor final:     0xAABBCCDD
 * @endverbatim
 *
 * @param address Endereço ou comando a ser enviado antes da leitura dos dados.
 * @param step Quantidade de bytes a ler (1 a 4). Valores fora do intervalo são
 *             automaticamente limitados.
 *
 * @return uint32_t Valor lido (até 32 bits), alinhado conforme a ordem MSB→LSB.
 *
 * @note A comunicação SPI deve estar previamente configurada. A função controla
 *       automaticamente o sinal de chip select (CS).
 *
 * @see RwByte()
 * @see CheckTransaction()
 * @see SetCS()
 */
uint32_t Bus_SPI::DataRead(uint8_t address, uint8_t step)
{
  uint32_t data;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(address);                             //
  for (uint8_t i = 0; i < step; i++) {
    uint8_t byte_in = RwByte(0x00);
    data = (data << 8) | byte_in;              // MSB primeiro → desloca antes de somar
  }
  SetCS(1);                                    //SS_SET
  return data;
}


/** OK
 * @brief Ler o registardor de estado STSR da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note Note
 *
 * @return valor de estadado do STSR
 */
uint8_t Bus_SPI::StatusRead(void)
{
  uint8_t temp = 0;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_STATUSREAD);                      //0x40, Read Status SPI
  temp = RwByte(REG_STSR);                     //0x00, Read STSR Register
  SetCS(1);                                    //SS_SET
  return temp;
}


/** 
 * @brief Escrever em um registrador da controladora display
 *
 * @warning Não é permitido uso de LockBus()/UnlockBus()
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display, data: dados para escrever no registrador
 *       Não é necessário o uso interno da função CheckTransaction(), pois  
 *       CmdWrite() e DataRead() já possuem o CheckTransaction()
 * 
 * @return None
 *
 * @see LockBus()
 * @see UnlockBus()
 * @see CmdWrite();
 * @see DataWrite(data);
 */
void Bus_SPI::RegisterWrite(uint8_t reg, uint8_t data)
{
  CmdWrite(reg);
  DataWrite(data);
}


/** 
 * @brief Ler um registrador da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display
 *       Não é necessário o uso interno da função CheckTransaction(), pois  
 *       CmdWrite() e DataRead() já possuem o  CheckTransaction()
 *
 * @return dados do registrador
 */
uint8_t Bus_SPI::RegisterRead(uint8_t reg)
{
  CmdWrite(reg);
  return DataRead();
}


/** 
 * @brief Escreve um buffer de dados para SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @return None
 *
 * @see CheckTransaction()
 * @see SetCS()
 * @see RwBytes()
 */
void Bus_SPI::WriteBytes(const uint8_t* data, size_t len)
{
  if (!_spi_init || data == nullptr || len == 0) return;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_DATAWRITE);                       //Envia byte de controle SPI
  RwBytes(data, len);
  SetCS(1);                                    //SS_SET
}


/**
 * @brief Inicia um bloco de escrita SPI e garante uma transação ativa.
 *
 * Esta função inicia uma transação SPI se ainda não houver uma ativa,
 * configurando os parâmetros do barramento (clock, ordem de bits e modo SPI)
 * e marcando o início de um bloco de escrita de dados volumosos.
 *
 * É ideal para ser utilizada em operações de alto tráfego, como transferências
 * gráficas no LVGL, evitando o overhead de múltiplos beginTransaction().
 *
 * @return 0 SPI nao foi iniciado
 *         1 Já existe uma transação ativa
 *         2 Transação inciciada
 * @note
 * - Define os flags internos `_spi_transaction = true` e `_spi_startwrite = true`.
 * - Caso o SPI ainda não tenha sido inicializado (`_spi_init == false`), a função não executa.
 *
 * @code
 * bus->StartWrite();
 * bus->DrawPixels(...);
 * bus->EndWrite();
 * @endcode
 */
uint8_t Bus_SPI::StartWrite(void)
{
#if defined(SPI_ARDUINO_CORE)    

  uint8_t status = 0;
  
  if (!_spi_init) return status;

  if (!_spi_transaction) {
    spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
    _spi_transaction = true;
    status = 2;
  } else status = 3;

  if (!_spi_startwrite) {
    _spi_startwrite = true;
  }

  return status;

#elif defined(SPI_ESP32_NATIVE)

  #warning "Method Bus_SPI::StartWrite() not implemented for ESP32 with Native SPI"

#endif  
}


/**
 * @brief Finaliza um bloco de escrita SPI e encerra a transação ativa.
 *
 * Esta função encerra uma transação SPI iniciada por StartWrite(),
 * liberando o barramento e retornando o SPI ao estado ocioso.
 *
 * Deve ser utilizada após o término de transferências intensas,
 * como o envio de buffers de imagem para o display.
 *
 * @return void
 *
 * @note
 * - Finaliza a transação somente se `_spi_startwrite == true` e `_spi_transaction == true`.
 * - Define `_spi_transaction = false` e `_spi_startwrite = false` ao final.
 */
void Bus_SPI::EndWrite(void)
{
  if (!_spi_init) return;

#if defined(SPI_ARDUINO_CORE)    

  if (!_spi_startwrite) return;
  
  if (_spi_startwrite) {
    _spi_startwrite = false;
  }
  
  if (_spi_transaction) {
    spi->endTransaction();
   _spi_transaction = false;
  }

#elif defined(SPI_ESP32_NATIVE)

  #warning "Method Bus_SPI::EndWrite() not implemented for ESP32 with Native SPI"

#endif    
}
