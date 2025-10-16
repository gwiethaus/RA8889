# RA8889

Desenvolvimento e Adaptação de Driver para uso de display com controladora RA8889 com tela de toque capacitivo baseado nos exemplos da BuyDisplay e RAIO. A comunicação entre o Mcirocontrolador Host e o Display se dá pela interface SPI.

O módulo shield ER-AS-5517 para ser utilziado em Arduino UNO/Mega/Duo apresenta o pino 14 dos display controle do backlight conectdo diretamente ao Vcc de 3,3V pasando pelo conversor de 5V e por isso não se pode ter o controle de luz de fundo do display. A saída do módulo pino 14 é ligado ao pino 14 do display. Quando não se utiliza o módulo shield e conecta-se diretamente ao ESP32 o pino BL_CONBTROL precisa estar ligado a uma saída de 3,3V de sinal e setando o pino de saída do ESP32 como saída. No display ER-TFT070-2-6105 baseado no controlador da RAIO RA8889 o controle de backlight precisa ser feito pelo usuário, enquanto que no módulo ER-TFTM070-5 baseado no RA8875 o seu pino 14 é mantido geralmetne solto, sem conexão com nada.

# Notas

Os driver para o uso da tela está preparado através do ajuste de configuração da SDRAM, PLL, ROM, Flash SPI e Fontes basicas. Permite a configuração do tipo de profundidde de cores de operação do display em 8 bits (RGB332), 16 (RGB565) ou 24 bits (RGB888). Possui tambem módulo de tela de toque capacitivo controlado pelo circuito FocalTech da série FT5xx6.

O microcontrolador de Display RA8889 possui recurso de manter a última tela antes do Power Off. No inicio parecia um Bug de que o Reset de Harware e de software não estavam ocorrendo adequadamente.
Após alguns testes, destacou que de inicio após as configuraões do display usando o método Begin() manter o display desligado. Preencher a tela com algum padrão de fundo ou apresentação e então lgiar o dsiplay pelo comando DisplayOn(). Esse recurso de aramzenanmento da ultima imagem de tela faz sentido quando precisa desligar a tela e religar ela sem a eprda de suas informações em memória.

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
  cfg.pin_sda = 23;                         //
  cfg.pin_scl = 18;                         //
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

# Todo

- Transferencia de DMA
- Fontes de Usuário
- Melhorias das funções de uso fontes da CGROM
- Melhoria das funções IDEC
- Comunicação Paralela entre MCU/MPU e Display
- Configuração I2C entre MCU/MPU e Display
- Melhoria do Driver FocalTech FT5xx6
