# RA8889

Desenvolvimento e Adaptação de Driver para uso de display com controladora RA8889 com tela de toque capacitivo baseado nos exemplos da BuyDisplay e RAIO. A comunicação entre o Mcirocontrolador Host e o Display se dá pela interface SPI.

O módulo shield ER-AS-5517 para ser utilziado em Arduino UNO/Mega/Duo apresenta o pino 14 dos display controle do backlight conectdo diretamente ao Vcc de 3,3V pasando pelo conversor de 5V e por isso não se pode ter o controle de luz de fundo do display. A saída do módulo pino 14 é ligado ao pino 14 do display. Quando não se utiliza o módulo shield e conecta-se diretamente ao ESP32 o pino BL_CONBTROL precisa estar ligado a uma saída de 3,3V de sinal e setando o pino de saída do ESP32 como saída. No display ER-TFT070-2-6105 baseado no controlador da RAIO RA8889 o controle de backlight precisa ser feito pelo usuário, enquanto que no módulo ER-TFTM070-5 baseado no RA8875 o seu pino 14 é mantido geralmetne solto, sem conexão com nada.

# Notas

Os driver para o uso da tela está preparado através do ajuste de configuração da SDRAM, PLL, ROM, Flash SPI e Fontes basicas. Permite a configuração do tipo de profundidde de cores de operação do display em 8 bits (RGB332), 16 (RGB565) ou 24 bits (RGB888). Possui tambem módulo de tela de toque capacitivo controlado pelo circuito FocalTech da série FT5xx6.

O microcontrolador de Display RA8889 possui recurso de manter a última tela antes do Power Off. No inicio parecia um Bug de que o Reset de Harware e de software não estavam ocorrendo adequadamente.

Durante alguns testes realizados, destacou que inicialmente após as configuraões do display, usando o método Begin(), e manter o display desligado. Foi feito o preenchimento da tela com algum padrão de fundo ou apresentação e então ligar o dsiplay pelo comando DisplayOn(). Esse recurso de aramzenanmento da ultima imagem de tela faz sentido quando precisa desligar a tela e religar ela sem a eprda de suas informações em memória.

Todos os métodos e propriedades tem como padrão o cabeçalho explicativo no formato Doxygen para gerar documentação.

# Hardware

- Arduino Uno/Mega
- ESP32 / ESP32-S3 (iniciando)
- Interface Comunicação SPI

# Novas implementações

Atravpés de uso de interface OOP podemos escolher facilmente o tipo de barramento de comunciação e configurar ele. Inicia-se este implementação que terá que sofrer mudança em todos os métodos onde se escreve ou lê o barrmaneto SPI que agora será via interface IBus. O usuário poderá escolher entre Bus I2C, SPI e Parallel para comunciaçlão do o display. Segue um exemplo modelo abaixo que já está pronto a estrutura basica para impelemtnar os métodos e classes:

```
Bus_SPI spi;
RA8889 gfx(PIN_CS, PIN_RESET);

void setup() {
     
  IBus::SPIBusConfig_t cfg;
  cfg.spi_type = FSPI_HOST;                 //Usando barramento FSPI
  cfg.pin_mosi = 23;                        //Pino MOSI - Master-Out, Slave-In
  cfg.pin_miso = 19;                        //Pino MISO - Master-In, Slave-Out
  cfg.pin_sclk = 18;                        //Pino Clock
  cfg.pin_cs   = 5;                         //Pino Chip Select
  cfg.freq_write = 40000000;                //Frequencia de escrita de 40MHz
  
  spi.Config(&cfg);                         //Grava a configuração
  gfx.setBus(spi);                          //Seta o Bus SPI
  gfx.Begin();                              //inicializa o display 
  gfx.setBacklignt(BL_PIN);                 //Seta pino do controle de backlight
  gfx.Backlight(true);                      //aciona o backlight
  gfx.DisplayOn(true);                      //liga o dsiplay exibindo conteudo da memória
  
}
```

Permitindo escolher o bus de uso do dsiplay, como SPI, I2C ou Paralelo bastando aepnas fazer isso:

```
Bus_I2C i2c;
RA8889 gfx(PIN_CS, PIN_RESET);

void setup() {
     
  IBus::I2CBusConfig_t cfg;
  cfg.pin_sda = 20;                         //Pino SDA
  cfg.pin_scl = 21;                         //Pino SCL
  cfg.addr    = 0x08                        //Endereço inicial do dispositivo slave
  cfg.pin_cs   = 5;                         //Pino Chip Select
  cfg.freq_write = 30000000;                //Frequencia de escrita de 30MHz
  
  spi.Config(&cfg);                         //Grava a configuração
  gfx.setBus(i2c);                          //Seta o Bus I2C
  gfx.Begin();                              //inicializa o display 
  gfx.setBacklignt(BL_PIN);                 //Seta pino do controle de backlight
  gfx.Backlight(true);                      //aciona o backlight
  gfx.DisplayOn(true);                      //liga o dsiplay exibindo conteudo da memória
  
}
```

# Futura versão

## Controle de Barramento SPI com transação Ativa e Controlada 

Será possível fazaer o controle quando se deseja que o barramento SPI fique sempre bloqueado apra oturos dispositivos deixando exclusivo par ao display ou permite que a cada processo de envio de dados ou leitura o barramento fica retido e depois liberado para oturos dispositivos. A segunda opção é útil quandos e deeja comaprtilhar o barramento a varios dispostiivos e ainda evitar overhead de transação causando sobrecarga no tempo de transmissao de dados e leitura. Esta técnica minimiza este risco. Muito usado para enviar principalmetne blocos de dados inteiros que o barramento precisa ficar retido ao dispositivo e depois liberado quando for necessário. Esta téncia é frequenctemente usada por algumas biblitoecas graficas como o LovyanGFX e TFT_eSPI, no entnado esta apresenta uma característica extra de poder escolher quando e como se deseja isso.

Demostrado abaixo, para melhor entendimento como é feito esta técnica de design através dos trechos de código abaixo que serão futuramente implemntados.

### Código da Classe RA8889:

```
void RA8889::StartSender() {
  _bus->startWrite();
}

void RA8889::EndtSender() {
  _bus->endWrite();
}
```

### Código da Clase Bus_SPI:

```
void Bus_SPI::Init(void) 
{
  if (_spi_init) return;

  pinMode(_cfg.pin_cs, OUTPUT);
  digitalWrite(_cfg.pin_cs, HIGH);

  createSPI(static_cast<SPIHostType>(_cfg.spi_type));

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
  return;
#endif
  _spi_init = true;
}

voi Bus_SPI::setTransaction(void) {
  /Se a transacao está ja ativa nao faça nada
  if (!_spi_transaction ) {                                                              
    spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
    _spi_ransaction = true;             //assume transacao ativa
  }
}

void Bus_SPI::CmdWrite(uint8_t cmd)
{ 
  //Se executou comando de finalziacao de transação e não tem nenhuma ativa, 
  //execute esta funcao para prevenir efeitos indesejados
  setTransaction();                           

  //se nao tiver controle de transcao, apenas uma transacao esta ativa 
  //precisando selecionar o barramento
  if (!_cfg.ctrl_trans) SetCS(0);              //SS_RESET
  RwByte(SPI_CMDWRITE);                        //0x00, Avisa Display que será um comando
  RwByte(cmd);                                 //Envia um comando de 1 byte para o Display
  if (!_cfg.ctrl_trans) SetCS(1);              //SS_SET
}

void Bus_SPI::startSender() {
  if (!_cfg.ctrl_trans) return;
  setTransaction(void)
  SetCS(0); 
}
   
void Bus_SPI::endSender() {
  if (!_cfg.ctrl_trans) return;
  if (_spi_ransaction) {
    spi->endTransaction();
   _spi_ransaction = false;
    SetCS(1);
  }
}
``` 

### Quando cfg.ctrl_trans = true:

Avisa que o controle será sempre por transacao. Ela inicia e finalzia sempre quando tem uma operaçãoo de escrita no barramento spi. Alem disso, StartSender() e EndSender() serão necessários sempre que há uma escrita ou leitura do barramento.

### Quando cfg.ctrl_trans = false:

A transação fica ativa sempre no inicio e nunca finaliza durante todo tempo de vida do software. Alem disso, StartSender() e EndSender() não terão efeito algum.

### Como usar

```
Bus_SPI bus_spi;
RA8889 gfx(PIN_CS, PIN_RESET);
...
void setup() {
...
...
  IBus::SPIBusConfig_t cfg;
  cfg.spi_type = HOST_FSPI;
  cfg.pin_mosi = 11;
  cfg.pin_miso = 13;
  cfg.pin_sclk = 12;
  cfg.pin_cs   = 10;
  cfg.ctrl_trans = true;
  cfg.freq_write = 20000000;
  bus_spi.Config(&cfg);                        // Grava a configuração
  gfx.setBus(bus_spi);                         // Seta o Bus SPI
...
...

//Trecho de uso sobre o LVGL, callback

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    gfx.StartSender();                            // <--- bloqueia o barramento
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.WritePixels(px_map, w * h);               // envia os pixels
    gfx.EndSender();                              // <--- libera o barramento

    lv_display_flush_ready(disp);
}
``` 


# Todo

- Transferencia de DMA
- Fontes de Usuário
- Melhorias das funções de uso fontes da CGROM
- Melhoria das funções IDEC
- Comunicação Paralela entre MCU/MPU e Display
- Configuração I2C entre MCU/MPU e Display
- Melhoria do Driver FocalTech FT5xx6

