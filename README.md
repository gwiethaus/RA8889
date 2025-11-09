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
- ESP32 / ESP32-C3 / ESP32-S2 / ESP32-S3
- Display baseado em controlador RA8889
- Interface Comunicação SPI entre o modulo Display e a MCU
- Controlador de tela de toque FocalTech FT5x16 de comunciação I2C
- Controle de Painel de Display de vários fabricantes

# Problemas

- Preenchimento de uma tela completa 800x480x2bpp demora 453 ms, usando DMA através de SPI.write?Bytes(data,len) com uso da função WritePixels() em ESP32-S3 com uso de Arduino Core

# Novas implementações

Atravpés de uso de interface OOP podemos escolher facilmente o tipo de barramento de comunciação e configurar ele. Inicia-se este implementação que terá que sofrer mudança em todos os métodos onde se escreve ou lê o barrmaneto SPI que agora será via interface IBus. O usuário poderá escolher entre Bus I2C, SPI e Parallel para comunciaçlão do o display. Segue um exemplo modelo abaixo que já está pronto a estrutura basica para impelemtnar os métodos e classes:

## Controle do Tipo de Barramento

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

## Classe Tela de Toque FT

A tela te toque tem comuncação com barramento I²C com o MCU, com frequência de comunicação de 400kHz. Os recursos desta classe pode-se destacar até 5 multiplos toques simultaneamente com status de toque independente, cotnrole de eventos callback de usuário, cotnrole de debounce apra evitar falhas e ruidos de tela.

1️⃣ Configurando a Tela de toque

```
//ESP_32
#define  PIN_RESET      9  //Reset do RA8889
#define  PIN_RESET_CP   0  //Reset da Tela de toque não é conectado
#define  PIN_CS         10  
#define  PIN_INT        16 //interrupção da tela de toque 
#define  PIN_SDA        18 //I2C
#define  PIN_SCL        17 //I2C
#define  PIN_BLCONTROL  15 //External backlight control connected to this Arduino pin (quando nao estiver usando modulo display shield)

Bus_SPI bus_spi;
RA8889 gfx(PIN_CS, PIN_RESET);
FT touch(PIN_SDA, PIN_SCL, PIN_INT, PIN_RESET_CP);

void myInterrupt_cb(TouchEventInfo tevent, uint8_t idtouch, uint8_t ntouch){
//...
}

void setup() {
  Serial.begin(115200);
  
  IBus::SPIBusConfig_t cfg;
  cfg.spi_type = HOST_FSPI;
  cfg.pin_mosi = 11;
  cfg.pin_miso = 13;
  cfg.pin_sclk = 12;
  cfg.pin_cs   = 10;
  cfg.freq_write = 20000000;
  bus_spi.Config(&cfg);                        // Grava a configuração
  gfx.setBus(bus_spi);                         // Seta o Bus SPI
  bool b = gfx.Begin();                        //Inicializa o display
  gfx.FillScreen(clWhite);                     //Limpa a tela da ultima exibição apos power off
  gfx.DisplayOn(true);                         //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico
  gfx.GraphicMode();
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());

  b = touch.Begin();
  touch.AllowMultitouch(true);
  touch.OnTouch(myInterrupt_cb);
  touch.OnTouchEnable(true);
  touch.setDebounceTouch(false);
  touch.AllowMultitouch(false);             //Apenas um toque é permitido
  touch.setNumTouches(2);                   //valores maiores que 1 se multitouch está habilitado 
  ...
}

void loop() {
//...
}
```

1️⃣ Exemplo de loop usando polling com transições:

```
FT ft;
TouchEventInfo events[FT_MAX_TOUCHES];

void loop() {
    // 1️⃣ Chama poll do driver, que atualiza touchPoints e history
    uint8_t n = ft.ProcessTouchEvents(events, FT_MAX_TOUCHES);

    // 2️⃣ Para cada evento detectado, verifica a transição
    for (uint8_t i = 0; i < n; i++) {
        switch(events[i].transition) {
            case TouchEventInfo::TOUCH_DOWN:
                Serial.printf("Toque DOWN - ID:%u X:%u Y:%u\n", 
                               events[i].id, events[i].x, events[i].y);
                break;
            case TouchEventInfo::TOUCH_MOVE:
                Serial.printf("Toque MOVE - ID:%u X:%u Y:%u\n", 
                               events[i].id, events[i].x, events[i].y);
                break;
            case TouchEventInfo::TOUCH_UP:
                Serial.printf("Toque UP - ID:%u X:%u Y:%u\n", 
                               events[i].id, events[i].x, events[i].y);
                break;
            default:
                break;
        }
    }

    // 3️⃣ Opcional: faz alguma ação contínua ou delay curto
    delay(10);
}
```

🔹 Explicando o fluxo
- _newtouch e _touchcount são atualizados pela ISR e por getTouches().
- ProcessTouchEvents() compara cada touchPoint[i] com _history[i] para determinar transição:
  - TOUCH_DOWN → novo toque detectado
  - TOUCH_MOVE → posição mudou (debounce opcional)
  - TOUCH_UP → toque liberado ou timeout expirado
- Dentro do loop, você pode processar cada evento individualmente usando o array events[] retornado.
- _touchcount e _newtouch garantem que você não perca toques nem processe eventos repetidos.

3️⃣ While com múltiplos toques

```
while (true) {
    if (ft.Touched()) {             // se algum toque novo existe
        uint8_t n = ft.ProcessTouchEvents(events, FT_MAX_TOUCHES);
        for (uint8_t i = 0; i < n; i++) {
            if (events[i].transition == TouchEventInfo::TOUCH_DOWN) {
                // faz ação de pressionamento
            } else if (events[i].transition == TouchEventInfo::TOUCH_UP) {
                // faz ação de liberação
            } else if (events[i].transition == TouchEventInfo::TOUCH_MOVE) {
                // faz ação de movimento
            }
        }
    }
}
```

4️⃣ Como usar no loop principal

```
FT ft;
TouchEventInfo events[FT_MAX_TOUCHES];

void loop() {
    uint8_t n = ft.Poll();

    for (uint8_t i = 0; i < n; i++) {
        switch (events[i].transition) {
            case TOUCH_DOWN:  Serial.println("Down"); break;
            case TOUCH_MOVE:  Serial.println("Move"); break;
            case TOUCH_UP:    Serial.println("Up"); break;
        }
    }

    // ... seu código da aplicação continua
}
```

5️⃣ Como usar na aplicação

```
FT ft;
TouchEventInfo events[FT_MAX_TOUCHES];

void loop() {
    uint8_t n = ft.getTouches();      // lê hardware
    uint8_t evCount = ft.processTouchEvents(events, FT_MAX_TOUCHES);

    for (uint8_t i = 0; i < evCount; i++) {
        switch (events[i].transition) {
            case TOUCH_DOWN:  Serial.println("Down"); break;
            case TOUCH_MOVE:  Serial.println("Move"); break;
            case TOUCH_UP:    Serial.println("Up"); break;
            default: break;
        }
    }
}
```

🔹 Benefícios desse pacote final
- _history mantém o estado temporal de cada toque.
- _touchcount e _newtouch são atualizados apenas nos eventos reais (Down/Up).
- processTouchEvents() transforma os dados do chip em eventos lógicos coerentes, incluindo movimentação e timeout.
- Totalmente compatível com seu enum class TouchEvent e TouchPoint.

## Controle de Barramento SPI com transação Ativa e Controlada 

Será possível fazaer o controle quando se deseja que o barramento SPI fique sempre bloqueado apra oturos dispositivos deixando exclusivo par ao display ou permite que a cada processo de envio de dados ou leitura o barramento fica retido e depois liberado para oturos dispositivos. A segunda opção é útil quandos e deeja comaprtilhar o barramento a varios dispostiivos e ainda evitar overhead de transação causando sobrecarga no tempo de transmissao de dados e leitura. Esta técnica minimiza este risco. Muito usado para enviar principalmetne blocos de dados inteiros que o barramento precisa ficar retido ao dispositivo e depois liberado quando for necessário. Esta téncia é frequenctemente usada por algumas biblitoecas graficas como o LovyanGFX e TFT_eSPI, no entnado esta apresenta uma característica extra de poder escolher quando e como se deseja isso.

# Todo

🟢 finalziado 🟡 parcial 🔴 não iniciado

## Geral

- 🟡 Wiki: Documentação Driver FT5x16
- 🟡 Wiki: Documentação Driver RA8889

## Drive RA8889
- 🟢 Transferencia de DMA entre MCU e RA8889
- 🟡 Fontes de Usuário
- 🟡 Melhorias das funções de uso fontes da CGROM
- 🟡 Melhoria das funções IDEC
- 🟡 Comunicação Paralela entre MCU/MPU e Display
- 🟡 Configuração I2C entre MCU/MPU e Display
- 🟡 Driver adaptado para RA8876 e RA8877 (Semelhante ao RA8889)
- 🟢 Controle de Transação SPI para transferência de volume de dados
- 🟢 DrawPixels() e WritePixels() para envio em massa de pixel a controladora por DMA
- 🟢 Seleção do barramento para SPI, I2C e Parallel
- 🟢 Controle de ativação de Barramento SPI

## Drive FT5x16
- 🟢 Melhoria do Driver FocalTech FT5xx6
- 🟢 Eventos callback de usuário para tela de toque
- 🟢 Tratamento de eventos de tela DOWN, UP, MOVE, NONE
- 🟢 Multitoques simultâneaos de 1 até 5 (hardware)
- 🟢 Ajuste pelo usuário do tempo de transição em milisegundos entre o evento DOWN e MOVE
- 🔴 Tratamento de gestos (gesture)

## Meus Emoticon

1️⃣ 2️⃣ 3️⃣ 4️⃣ 5️⃣ 6️⃣ 7️⃣ 8️⃣ 9️⃣ 🅰️ 🅱️ 🔄 ➡️ 🔁 
✅ 🟩 ⚠️ 💡 ⚙️ 🧩 📍 👉 👈 👇 👌 👍 👏 💪 👊 💬 📊 ⏰ 🔴 🟢 🟡 ⚫ 🔵 🔧 📘 🧱 🧮 📌 🧠 🚨 💯
🔹 ⚡ ✔️ ❌ 🔍 😄 😎 😊 💯 🚫 🚀 🧭 🔒 📈 👀 🎯 🏆 🔥 ⏱️ 💾 🐯 🎛️ 📁 📲 🎯 🧰 👨‍💻 📄 📝 🔘 📜 🪶 
