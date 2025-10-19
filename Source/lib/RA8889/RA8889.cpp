//baixar
//https://github.com/Lvkyky/RA8889
//https://github.com/wwatson4506/TeensyRA8889-SPI
//https://github.com/cycccc/zijia_ra8889_ceshi
//https://github.com/wwatson4506/TeensyRA8876-SPI
//https://github.com/wwatson4506/Ra8876LiteTeensy

/*
	Notas para Fazer:
	- Determinar a autoconfiguracao das portas MISO, MOSI, CLK da micrcontroladora
	- Determinar manual das portas MISO, MOSI, CLK da micrcontroladora
	- tipo de comunciacao SPI MODO 0, 1, 2 dependedo da microcontroladora
    - Sistema da porta de itnerrupção para a tela de toque do display
	- Verificar a funcao DrawEnable_AA() deve ser do RA8876, pois no RA8889 deve ser zero
	  
	Links:
	https://xod.io/libs/ivanmason/ra8876/
  https://www.youtube.com/@youtuberaio972
	
	Tarefas:
  
    //Escrita de Memoria de Display com MPU 16 bits / color depth 24bpp modo 2
    Estas funcoes com este padrao
	void RA8889::MPU16_24bpp_Mode2_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data)
	remover as linhas: 
	  ActiveWindow_XY(x, y);
      ActiveWindow_WidhtHeight(w, h);
    pois implica em formato diferente da janela afetando a janela ja pre existente.
    Verificar o impacto desta funcoes
    
    A funcao void RA8889::MPU16_24bpp_Mode2_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data)	
	é de escrita de memoria, e logo isnere um pixel na memoria.
	FAzer duas funcoes originario destas que detecta por macro mal mcu e color depth, colcaor dentro na nova funcao de escrita de memroia
	void MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint8_t *data_array)
    e entao depois de adaptado inserir dentro de DrawPixel e DrawPixels
	DrawPixels(x, y, w, h, *data); w e h não é da tela mas da area do pixel a transferir x, y vao incrementan do de acordo com w e h
	fazendo a varredura em *data.
	Outra funcao DrawPixels (x, y, count, *data) o controle tera que ser por x,y e count na posicao do buffer apra a posicao de tela que deseja os pixels.
	
Alteracoes #001:
----------------

void RA8889::setFontExternal(FontExternalParam param, bool enable = false)

param.scs_select
  Não precisa provavelmente o usuraio setar, pois deve ser fixo e na inicializacao do sistema
  a ROM nao fica mudando toda hora. Para verificar isso, fazer o teste com esta funcao ja fun cionando e trocar 
  o parametro param.scs_select=0 por param.scs_select=1 se funcionar nao entendi!!! agora se nao funcionar mais este valor é fixo apra a ROM da Fonte Genitop
  
  Assim se utilziar da variavel global _fnt_rom_genitor_scs para definir o scs da rom mda fonte genito.
  esta variavel deve setar no moemnto das configuracoes basicas da fonte na inicializacao do display
  
  outros parametros precisam ser verificado se é so caso a aprte ou se é fixo pelo sistema
  para o caso do seguinte:
  
  GTFont_CharacterParameter(_fnt_rom_genitor_scs,
                            BIT_SPI_DIV4, 
                            GTSERIAL_CGROM,
                            static_cast<uint8_t>(param.charset_select),
                            static_cast<uint8_t>(param.gt_width)                   
                           );
  
  A variavel foi definida globalmente como
  
  uint8_t _fnt_rom_genitor_clk_div = BIT_SPI_DIV4;
  
  talvez nao precise disso basta deixar a cosntante sempre em BIT_SPI_DIV4... estudar
 
Alteracoes #002:
----------------

tela manchada de pixels no power on:

Alteracoes retirou o HardwareReset() no metodo bool RA8889::Begin(void)
para tentar limapr a tela antes da incilizacao de qualqeur coisa.
Desta forma esta funcao deverá ser a primeira coisa a ser executado no dispositivo
verificando se isso surte efeito da tela com pixels aleatoris sempre quando dá power on

Construção #002
---------------
Objetivo: Fazendo as funções:
    
    void RA8889::setFontUser(FontUserParam param, bool enable = false)    


Construcao #003
---------------

Objetivo: Fonte de usuario

https://forum.pjrc.com/index.php?threads/ra8876-parallel-display-library-testing.75345/page-9
Baixar Arquivos: https://github.com/wwatson4506/TeensyRA8876Combined

Verificar se isto esta funcionando... dica de forum

Ok fixed userDefinedFont issue with this:
Code:
void RA8876_common::CGRAM_initial(uint32_t charAddr, const uint8_t *data, uint16_t count) {
    uint16_t i;
    uint16_t tmp = 0;
    
    graphicMode(true); // switch to graphic mode

    lcdRegWrite(RA8876_AW_COLOR);                                                  // 5Eh
    lcdDataWrite(RA8876_CANVAS_LINEAR_MODE << 2 | RA8876_CANVAS_COLOR_DEPTH_8BPP); // set memory to 8bpp and linear mode
    linearAddressSet(charAddr);                                                    // Set linear address (32 bit)
    // Set the start address for User Define Font, and write data.
    ramAccessPrepare();
    if(_bus_width == 8) {
        for (i = 0; i < count; i++) {
            checkWriteFifoNotFull();
            lcdDataWrite(*data);
            data++;
        }
    } else {
        for (i = 0; i < count/2; i++) {
            checkWriteFifoNotFull();
            tmp = (*data++ & 0xff);
            tmp |= (*data++ << 8);
            lcdDataWrite16(tmp);
        }
    }
    checkWriteFifoEmpty(); // If high speed mcu and without Xnwait check

    lcdRegWrite(RA8876_AW_COLOR); // 5Eh
    lcdDataWrite(RA8876_CANVAS_BLOCK_MODE << 2 | RA8876_CANVAS_COLOR_DEPTH_16BPP);
}
Works in 8-bit and 16-bit mode...


Construção #004
---------------

Objetivo: Luz de Fundo


Baseado no teese
https://github.com/wwatson4506/TeensyRA8876Combined
TeensyRA8876Combined-main
RA8876_common.cpp

//
// Turn Backlight ON/Off (true = ON)
//
void RA8876_common::backlight(boolean on) {
    if (on) {
        // Enable_PWM0_Interrupt();
        // Clear_PWM0_Interrupt_Flag();
        // Mask_PWM0_Interrupt_Flag();
        // Select_PWM0_Clock_Divided_By_2();
        // Select_PWM0();
        pwm_ClockMuxReg(0, RA8876_PWM_TIMER_DIV2, 0, RA8876_XPWM0_OUTPUT_PWM_TIMER0);
        // Enable_PWM0_Dead_Zone();
        // Auto_Reload_PWM0();
        // Start_PWM0();
        pwm_Configuration(RA8876_PWM_TIMER1_INVERTER_OFF, RA8876_PWM_TIMER1_AUTO_RELOAD, RA8876_PWM_TIMER1_STOP,
                          RA8876_PWM_TIMER0_DEAD_ZONE_ENABLE, RA8876_PWM_TIMER1_INVERTER_OFF,
                          RA8876_PWM_TIMER0_AUTO_RELOAD, RA8876_PWM_TIMER0_START);

        pwm0_Duty(0xffff);

    } else {
        pwm_Configuration(RA8876_PWM_TIMER1_INVERTER_OFF, RA8876_PWM_TIMER1_AUTO_RELOAD, RA8876_PWM_TIMER1_STOP,
                          RA8876_PWM_TIMER0_DEAD_ZONE_ENABLE, RA8876_PWM_TIMER1_INVERTER_OFF,
                          RA8876_PWM_TIMER0_AUTO_RELOAD, RA8876_PWM_TIMER0_STOP);
    }
}


Construção #005
---------------
Objetivo: Luz de Fundo

Baseado no teese
https://github.com/wwatson4506/TeensyRA8876Combined
TeensyRA8876Combined-main
TeensyRA8876Combined-main\Ra8876_t3\examples\gauges\gauges.ino

#define BACKLITE 7 //External backlight control connected to this Arduino pin

void setup() {
  //I'm guessing most copies of this display are using external PWM
  //backlight control instead of the internal RA8876 PWM.
  //Connect a Teensy pin to pin 14 on the display.
  //Can use analogWrite() but I suggest you increase the PWM frequency first so it doesn't sing.
  pinMode(BACKLITE, OUTPUT);
  digitalWrite(BACKLITE, HIGH);
  

Construção #006
---------------
Objetivo: Luz de Fundo

Baseado no teese
https://github.com/wwatson4506/TeensyRA8876Combined
TeensyRA8876Combined-main
TeensyRA8876Combined-main\Ra8876_t3\examples\ILI_Ada_FontTest4\ILI_Ada_FontTest4.ino

#define BACKLITE 7 //External backlight control connected to this Arduino pin

void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("Setup");
  ...
  tft.backlight(true);
  pinMode(BACKLITE, OUTPUT);
  digitalWrite(BACKLITE, HIGH);
  ...
  
Construcao #007
---------------
Objetivo: Luz de fundo

Baseado no teese
https://github.com/wwatson4506/TeensyRA8876Combined
TeensyRA8876Combined-main
TeensyRA8876Combined-main\Ra8876_t3\examples\ILI_GFX_FontTest\ILI_GFX_FontTest.ino

#define RA8876_CS 10
#define RA8876_RESET 9
#define BACKLITE 7 //External backlight control connected to this Arduino pin
RA8876_t3 tft = RA8876_t3(RA8876_CS, RA8876_RESET); //Using standard SPI pins

void setup() {
#ifdef BACKLITE
  pinMode(BACKLITE, OUTPUT);
  digitalWrite(BACKLITE, HIGH);
#endif
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("Setup");
  tft.begin();

#ifndef BACKLITE
  tft.backlight(true);
  tft.fillScreen(RED);
  delay(2000);
  tft.pwm0_Duty(0x0000);
  delay(2000);
  tft.pwm0_Duty(0x0ff0);
  delay(2000);
  tft.pwm0_Duty(0xffff);
  delay(2000);
#endif

  //tft.setRotation(1);
  tft.fillScreen(BLACK);

...


Construção #008
---------------
Objetivo: backlight
Baseado no RA8876

static esp_err_t panel_ra8876_tx_param(esp_lcd_panel_t *panel, int lcd_cmd, uint8_t param)
{
	ra8876_panel_t					*ra8876 = __containerof(panel, ra8876_panel_t, base);
	esp_lcd_panel_io_handle_t		io = ra8876->io;

 	panel_ra8876_wait(panel);
	return esp_lcd_panel_io_tx_param(io, lcd_cmd, (uint8_t[]) {	param }, 1);
}

#define RA8876_REG_TCMPB0L			0x88	// Timer 0 compare buffer register (TCMPB0L)
#define RA8876_REG_TCMPB0H			0x89	// Timer 0 compare buffer register (TCMPB0H)
esp_err_t esp_lcd_panel_set_backlight(esp_lcd_panel_t *panel, uint8_t level)
{
	esp_err_t						ret = ESP_OK;
	ra8876_panel_t					*ra8876 = __containerof(panel, ra8876_panel_t, base);
	uint16_t						raw_level;

	raw_level = (0xff * level);
	
	if (raw_level == ra8876->backlight_level)
		return ret;
	
	// set new backlight level
	ra8876->backlight_level = raw_level;
	panel_ra8876_tx_param(panel, RA8876_REG_TCMPB0L, raw_level & 0xff);
	panel_ra8876_tx_param(panel, RA8876_REG_TCMPB0H, (raw_level >> 8) & 0xff);

	return ret;
}

Construcao
----------


*/



// ---------- Detecta Arduino/ESP ----------
#if defined(__has_include)
  #if __has_include(<Arduino.h>)
    #define USING_ARDUINO_H
    #include <Arduino.h>
  #endif
#endif

// ---------- Verifica se dtostrf precisa ser definida ----------
#if !defined(USING_ARDUINO_H) || (defined(USING_ARDUINO_H) && !defined(__DTOSTRF_DEFINED__))
  #ifndef __DTOSTRF_DEFINED__
  #define __DTOSTRF_DEFINED__

  // Inclui apenas quando necessário
  #include <stdio.h>
  #include <math.h> // para lidar com -0.0, NaN e INF

  #endif
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>
#include <RA8889.hpp>
#include <ascii_table_8x12.h>
#include <ascii_table_16x24.h>
#include <ascii_table_32x48.h>
#include <Debug.hpp>

//================================================================================
//
// Funções de testes
//
//================================================================================


//não testado
//precisa estar configurado jumper J27 e J28 no modulo de display apra uso desta funcionaldiade
void RA8889::Brightness(uint16_t level)
{
	// Turn on PWM if not already active, map 8-bit to 16-bit PWM0 register
  PWM0_SetCompareBuffer(level);
	
	_bus->RegisterWrite(REG_PMUXR, BIT_PWM0_TIMER_DIV1 | BIT_XPWM0_OUTPUT_PWM_TIMER0);
	
	uint8_t temp = _bus->RegisterRead(REG_PCFGR);
	if (level)  {
		temp = BIT_PWM0_DEAD_ZONE_ENABLE | BIT_PWM0_AUTO_RELOAD | BIT_PWM0_START;
	} else {
		temp = 0;
	}
	_bus->RegisterWrite(REG_PCFGR, temp);
}


//================================================================================
//
// Funções/Macros auxiliares
//
//================================================================================


/**
 * @brief Subtração binária de 8 bits usando bitwise (borrow)
 * 
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
uint8_t Bin_subtract(uint8_t maior, uint8_t menor) {
  while (menor != 0) {
      int borrow = (~maior) & menor;
      menor = menor ^ maior;
      menor = borrow << 1;
  }
  return menor;
}


/**
 * @brief Subtração condicional
 * 
 * @verbatim
 * Apenas subtrai se 'a >= gt' --> ex. 'a > 9'
 * 
 * Uso em mapeamento onde valores possuem saltos e deseja colcoar em ordem ordinal
 * ou ordem sequencial numérica
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
#define MAPBREAKPOS 0x38    //posicao de quabra caso for maior que 16 os valores (uso no mapa de fontes - tipos de fontes)
uint16_t conditional_subtract(uint8_t a, uint8_t b, uint8_t gt) {
  return (a > gt) ? Bin_subtract(a, b) : a;
}


// ---------- Verifica se dtostrf já existe ----------
#if !defined(USING_ARDUINO_H) || (defined(USING_ARDUINO_H) && !defined(__DTOSTRF_DEFINED__))
  #ifndef __DTOSTRF_DEFINED__
  #define __DTOSTRF_DEFINED__

  #include <cstdio>
  #include <cmath> // para evitar problemas com -0.0

  inline char* dtostrf(double val, signed char width, unsigned char prec, char* buf) {
      if (!buf) return nullptr;

      // Tratamento especial para -0.0
      if (val == 0.0) val = 0.0;

      // Formata usando sprintf
      sprintf(buf, "%*.*f", width, prec, val);
      return buf;
  }

  #endif
#endif


/**
 * @brief Converte um número do tipo double para string formatada.
 * 
 * Esta função é equivalente à função `dtostrf` do Arduino.  
 * Caso esteja em um ambiente Arduino/ESP que já forneça dtostrf, a função nativa é usada.  
 * Caso contrário, esta implementação é usada para C++ puro, garantindo portabilidade.
 * 
 * @param val   O valor numérico a ser convertido.
 * @param width Largura mínima do campo resultante. Se negativo, alinha à esquerda.
 * @param prec  Número de casas decimais após o ponto.
 * @param buf   Ponteiro para o buffer de caracteres onde a string será armazenada.
 *              O buffer deve ser grande o suficiente para conter o resultado.
 * 
 * @return Retorna o mesmo ponteiro fornecido em `buf`. Retorna nullptr se `buf` for nulo.
 * 
 * @note Trata casos especiais:
 *       - Se `val` for -0.0, converte para 0.0.
 *       - Se `val` for NaN, escreve "nan" no buffer.
 *       - Se `val` for infinito, escreve "inf" no buffer.
 * 
 * @warning Esta função utiliza sprintf internamente. Para ambientes sem suporte a sprintf,
 *          deve-se fornecer um buffer adequado e garantir compatibilidade do compilador.
 * 
 * @example
 * char buffer[16];
 * dtostrf(3.14159, 6, 2, buffer); // buffer conterá "  3.14"
 */
#if defined(__DTOSTRF_DEFINED__)
inline char* dtostrf(double val, signed char width, unsigned char prec, char* buf) {
    if (!buf) return nullptr;

    // Corrige -0.0
    if (val == 0.0) val = 0.0;

    // Trata casos especiais
    if (isnan(val)) { sprintf(buf, "nan"); return buf; }
    if (isinf(val)) { sprintf(buf, "inf"); return buf; }

    // Formata o valor normalmente
    sprintf(buf, "%*.*f", width, prec, val);
    return buf;
}
#endif


/**
 * @brief Equivalente ao dtostrf()
 * 
 * 
 * @param width: largura mínima da string (incluindo sinal e ponto)
 * @param precision: casas decimais
 * @param *buffer: saída
 */
void my_dtostrf(float val, int width, int precision, char* buffer) {
    char temp[32];
    int pos = 0;

    // --- sinal ---
    if (val < 0) {
        temp[pos++] = '-';
        val = -val;
    }

    // --- parte inteira ---
    long intPart = (long)val;
    float fracPart = val - intPart;

    // converte inteiro para string
    char intStr[16];
    int intLen = 0;
    if (intPart == 0) intStr[intLen++] = '0';
    else {
        long n = intPart;
        while (n > 0) {
            intStr[intLen++] = '0' + (n % 10);
            n /= 10;
        }
        // inverte
        for (int i = 0; i < intLen / 2; i++) {
            char c = intStr[i];
            intStr[i] = intStr[intLen - 1 - i];
            intStr[intLen - 1 - i] = c;
        }
    }
    memcpy(temp + pos, intStr, intLen);
    pos += intLen;

    // --- parte decimal ---
    if (precision > 0) {
        temp[pos++] = '.';
        float f = fracPart * pow(10, precision) + 0.5f; // arredonda
        long fracInt = (long)f;
        char fracStr[16];
        for (int i = precision - 1; i >= 0; i--) {
            fracStr[i] = '0' + (fracInt % 10);
            fracInt /= 10;
        }
        memcpy(temp + pos, fracStr, precision);
        pos += precision;
    }

    temp[pos] = '\0';

    // --- aplica largura mínima ---
    int len = strlen(temp);
    int pad = (width > len) ? width - len : 0;
    if (pad > 0) {
        // espaços à esquerda
        for (int i = 0; i < pad; i++) buffer[i] = ' ';
        strcpy(buffer + pad, temp);
    } else {
        strcpy(buffer, temp);
    }
}


/** OK
 * @brief Formata um valor float em string usando máscara avançada similar a printf.
 * 
 * Esta função permite formatar números em ponto flutuante (`float`) em um buffer
 * de caracteres, seguindo uma máscara semelhante às usadas em `printf` (`%f`, `%e`, `%g`),
 * incluindo opções de largura mínima, casas decimais, sinal explícito, alinhamento e zero-padding.
 * 
 * @verbatim
 * 
 * Suporte a máscaras estendidas: %+6.2f, %-6.2f, %+06.2f, os símbolos significam:
 *  - "+" → força o sinal positivo
 *  - "-" → alinhamento à esquerda
 *  - "0" → preenchimento com zeros à esquerda
 * 
 * Para funcionalidade semelhante ao sprintf() podemos exemplificar para valor como -0.000123:
 *  - %12.3e (alinhamento à direita)   → " -1.230e-04" (2 espaços antes)
 *  - %-12.3e (alinhamento à esquerda) → "-1.230e-04 " (2 espaços depois)
 *
 * 
 * @param buffer  Ponteiro para o buffer de destino onde a string formatada será escrita.
 *                O buffer deve ser suficientemente grande para conter o resultado.
 * @param valor   Valor em ponto flutuante a ser formatado.
 * @param mascara Máscara de formatação (ex: "%+08.2f", "%-10.3g", "%06.1e").
 *                Suporta os seguintes elementos:
 *                - `%`      : início da máscara
 *                - `+`      : sinal explícito para valores positivos
 *                - `-`      : alinhamento à esquerda
 *                - `0`      : zero-padding à esquerda
 *                - `width`  : largura mínima do campo
 *                - `.prec`  : número de casas decimais
 *                - `f`, `e`, `E`, `g`, `G` : tipo de formatação
 * 
 * @note A função realiza os seguintes comportamentos especiais:
 *       - Para máscara `%g`/`%G` (`formatoAuto`), remove zeros à direita e ponto decimal se necessário.
 *       - Para `%e`/`%E` (`formatoCientifico`), converte o valor para notação científica.
 *       - Adiciona sinal positivo quando solicitado.
 *       - Aplica alinhamento à esquerda ou zero-padding conforme máscara.
 *       - Para valores negativos ou positivos com sinal, considera corretamente o preenchimento.
 * 
 * @warning O buffer deve ser grande o suficiente para conter a string resultante.  
 *          Recomenda-se pelo menos 80 caracteres para garantir segurança.
 * 
 * @example
 * char buf[50];
 * formatFloatAdvanced(buf, 3.14159f, "%+08.2f");     // buf -> "+003.14"
 * formatFloatAdvanced(buf, 0.0001234f, "%10.4g");    // buf -> "   0.0001234"
 * formatFloatAdvanced(buf, -12345.678f, "%-12.2f");  // buf -> "-12345.68   "
 * formatFloatAdvanced(buf, 123.456, "%10.2f");       // "    123.46"
 * formatFloatAdvanced(buf, 0.0000123, "%10.3e", );   // " 1.230e-05"
 * formatFloatAdvanced(buf, 12345678.9, "%+12.2g");   // "+1.23e+07"
 * formatFloatAdvanced(buf, 0.000123, "%-10.4g");     // "1.23e-04  "
 */
void formatDoubleAdvanced(char* buffer, double valor, const char* mascara)
{
  int largura = 0;
  int casas = -1;
  bool sinalPositivo = false;
  bool alinhamentoEsquerda = false;
  bool zeroPadding = false;
  bool formatoCientifico = false;
  bool formatoAuto = false;

  // --------------------------------------------------------------------------
  // 1. Analisa a máscara (ex: "%+06.2f")
  // --------------------------------------------------------------------------
  const char* p = mascara;

  if (*p == '%') p++;
  if (*p == '+') { sinalPositivo = true; p++; }
  if (*p == '-') { alinhamentoEsquerda = true; p++; }
  if (*p == '0') { zeroPadding = true; p++; }

  // Largura
  if (isdigit(*p)) {
    largura = atoi(p);
    while (isdigit(*p)) p++;
  }

  // Casas decimais
  if (*p == '.') {
    p++;
    casas = atoi(p);
    while (isdigit(*p)) p++;
  }

  // Tipo
  switch (*p) {
    case 'e': case 'E': formatoCientifico = true; break;
    case 'g': case 'G': formatoAuto = true; break;
    default: break; // assume 'f'
  }

  if (casas < 0) casas = 6;  // padrão
  if (largura <= 0) largura = 1;

  // --------------------------------------------------------------------------
  // 2. Decide entre notação científica ou decimal (%g)
  // --------------------------------------------------------------------------
  if (formatoAuto) {
    double absValor = fabs(valor);
    //if (absValor != 0.0f && (absValor < 0.0001f || absValor >= 1e7f)) {
    if (absValor != 0.0 && (absValor < 0.0001 || absValor >= 1e7)) {
      formatoCientifico = true;
    }
  }

  // --------------------------------------------------------------------------
  // 3. Formata valor bruto em buffer temporário
  // --------------------------------------------------------------------------
  char temp[50];
  temp[0] = '\0';

  if (formatoCientifico) {
    int expoente = 0;
    double mantissa = valor;

    if (mantissa != 0.0) {
      while (fabs(mantissa) >= 10.0) { mantissa /= 10.0; expoente++; }
      while (fabs(mantissa) < 1.0) { mantissa *= 10.0; expoente--; }
    }

    char mantissaStr[25];

    my_dtostrf(mantissa, 0, casas, mantissaStr);
    
    // ⚠️ Remove zeros SOMENTE para formato %g, não para %e
    if (formatoAuto) {
      int len = strlen(mantissaStr);
      while (len > 0 && mantissaStr[len - 1] == '0') mantissaStr[--len] = '\0';
      if (len > 0 && mantissaStr[len - 1] == '.') mantissaStr[--len] = '\0';
    }

    sprintf(temp, "%se%+03d", mantissaStr, expoente);
  } else {
    //dtostrf(valor, 0, casas, temp);
    my_dtostrf(valor, 0, casas, temp);
    if (formatoAuto) {
      // Remove zeros à direita e ponto decimal (%g)
      int len = strlen(temp);
      while (len > 0 && temp[len - 1] == '0') temp[--len] = '\0';
      if (len > 0 && temp[len - 1] == '.') temp[--len] = '\0';
    }
  }

  // --------------------------------------------------------------------------
  // 4. Adiciona sinal positivo, se solicitado
  // --------------------------------------------------------------------------
  char temp2[60];

  if (sinalPositivo && valor >= 0.0 && temp[0] != '+') {
    snprintf(temp2, sizeof(temp2), "+%s", temp);
    strcpy(temp, temp2);
  }

  // --------------------------------------------------------------------------
  // 5. Preenchimento e alinhamento
  // --------------------------------------------------------------------------
  int len = strlen(temp);
  int sinal = (temp[0] == '-' || temp[0] == '+') ? 1 : 0;

  if ((int)len < largura) {
    int pad = largura - len;
    char temp3[80];

    if (alinhamentoEsquerda) {
      // ex: %-10.2f
      strcpy(temp3, temp);
      for (int i = 0; i < pad; i++) strcat(temp3, " ");
      strcpy(buffer, temp3);
    }
    else if (zeroPadding) {
      // ex: %+08.2f  →  "-003.14" ou "+003.14"
      if (sinal) {
        temp3[0] = temp[0];
        for (int i = 0; i < pad; i++) temp3[sinal + i] = '0';
        strcpy(temp3 + sinal + pad, temp + sinal);
      } else {
        for (int i = 0; i < pad; i++) temp3[i] = '0';
        strcpy(temp3 + pad, temp);
      }
      temp3[sinal + pad + strlen(temp + sinal)] = '\0';
      strcpy(buffer, temp3);
    }
    else {
      // Preenche com espaços à esquerda
      memset(buffer, ' ', pad);
      buffer[pad] = '\0';
      strcat(buffer, temp);
    }
  } else {
    strcpy(buffer, temp);
  }

}


/** OLD
 * @brief Formata um valor double conforme máscara estilo printf (%+08.3f, %-10.2g, etc.)
 * 
 * Compatível com ambientes Arduino/AVR/ESP32, mesmo sem suporte a %f no printf.
 * 
 * @param buffer Buffer de saída (mínimo 80 bytes recomendados)
 * @param valor  Valor numérico a ser formatado
 * @param mascara Máscara de formatação (ex: "%+08.2f", "%-10.4g", "%e")
 */
void formatDoubleAdvanced2(char* buffer, double valor, const char* mascara)
{
    int largura = 0;
    int casas = -1;
    bool sinalPositivo = false;
    bool alinhamentoEsquerda = false;
    bool zeroPadding = false;
    bool formatoCientifico = false;
    bool formatoAuto = false;

    const char* p = mascara;
    if (*p == '%') p++;

    // Aceita modificadores em qualquer ordem: +, -, 0
    bool parsing = true;
    while (parsing) {
        switch (*p) {
            case '+': sinalPositivo = true; p++; break;
            case '-': alinhamentoEsquerda = true; p++; break;
            case '0': zeroPadding = true; p++; break;
            default: parsing = false; break;
        }
    }

    // Largura mínima
    if (isdigit(*p)) largura = strtol(p, (char**)&p, 10);

    // Casas decimais
    if (*p == '.') {
        p++;
        casas = strtol(p, (char**)&p, 10);
    }

    // Tipo
    switch (*p) {
        case 'e': case 'E': formatoCientifico = true; break;
        case 'g': case 'G': formatoAuto = true; break;
        default: break; // assume 'f'
    }

    if (casas < 0) casas = 6;  // padrão
    if (largura <= 0) largura = 1;

    if (formatoAuto) {
        double absValor = fabs(valor);
        if (absValor != 0.0 && (absValor < 0.0001 || absValor >= 1e7)) formatoCientifico = true;
    }

    // -------------------------------------------------
    // Buffer temporário único
    // -------------------------------------------------
    char temp[80] = {0};  // único buffer temporário seguro
    char* strNum = temp;  // ponteiro para construção da string

    if (formatoCientifico) {
        int expoente = 0;
        double mantissa = valor;

        if (mantissa != 0.0) {
            while (fabs(mantissa) >= 10.0) { mantissa /= 10.0; expoente++; }
            while (fabs(mantissa) < 1.0)   { mantissa *= 10.0; expoente--; }
        }

        // usa parte inicial do buffer para mantissa
        char* mantStr = temp;
        dtostrf(mantissa, 0, casas, mantStr);
        
        // remove zeros à direita se %g
        if (formatoAuto) {
            int len = strlen(mantStr);
            while (len > 0 && mantStr[len - 1] == '0') mantStr[--len] = '\0';
            if (len > 0 && mantStr[len - 1] == '.') mantStr[--len] = '\0';
        }

        // concatena expoente no mesmo buffer
        snprintf(temp, sizeof(temp), "%se%+03d", mantStr, expoente);
    }
    else {
        dtostrf(valor, 0, casas, temp);

        if (formatoAuto) {
            int len = strlen(temp);
            while (len > 0 && temp[len - 1] == '0') temp[--len] = '\0';
            if (len > 0 && temp[len - 1] == '.') temp[--len] = '\0';
        }
    }

    // -------------------------------------------------
    // Adiciona sinal positivo, se necessário
    // -------------------------------------------------
    if (sinalPositivo && valor >= 0.0 && temp[0] != '+') {
        // desloca tudo 1 posição à direita dentro do mesmo buffer
        size_t len = strlen(temp);
        if (len < sizeof(temp) - 1) {
            memmove(temp + 1, temp, len + 1);  // inclui '\0'
            temp[0] = '+';
        }
    }

    // -------------------------------------------------
    // Preenchimento e alinhamento
    // -------------------------------------------------
    int len = strlen(temp);
    int sinal = (temp[0] == '-' || temp[0] == '+') ? 1 : 0;

    if (len < largura) {
        int pad = largura - len;
        if (alinhamentoEsquerda) {
            // "%-10.2f" → esquerda
            memmove(buffer, temp, len + 1);
            for (int i = 0; i < pad; i++) buffer[len + i] = ' ';
            buffer[len + pad] = '\0';
        }
        else if (zeroPadding) {
            // "%+08.2f" → zeros à esquerda
            if (sinal) {
                buffer[0] = temp[0];
                for (int i = 0; i < pad; i++) buffer[sinal + i] = '0';
                memmove(buffer + sinal + pad, temp + sinal, len - sinal + 1);
            } else {
                for (int i = 0; i < pad; i++) buffer[i] = '0';
                memmove(buffer + pad, temp, len + 1);
            }
        }
        else {
            // espaço à esquerda
            int i;
            for (i = 0; i < pad; i++) buffer[i] = ' ';
            memmove(buffer + pad, temp, len + 1);
        }
    }
    else {
        memmove(buffer, temp, len + 1);
    }
}


/**
 * @brief Função auxiliar: escreve um inteiro positivo no buffer
 * 
 * 
 * @param val
 * @param *buf
 * @param minWidth
 */
static int intToStr(int val, char* buf, int minWidth) 
{
  char tmp[12]; // suporta até 32 bits
  int i = 0;
  if(val == 0) tmp[i++] = '0';
  while(val > 0) {
      tmp[i++] = '0' + (val % 10);
      val /= 10;
  }
  while(i < minWidth) tmp[i++] = '0'; // zero padding
  // inverte
  for(int j = 0; j < i; j++) buf[j] = tmp[i - j - 1];
  buf[i] = '\0';
  return i;
}


//================================================================================
//
// Funções de Interface
//
//================================================================================


//seta o bus atraves de implementacao de interface como Parallel, SPI e I2C
void RA8889::setBus(IBus& bus) 
{
  _bus = &bus;                                   //Ponteiro apra qual baramento de comunicação SPI/Parallel/I2C será usado
  //_bus->Init();                                //inicilaiza o baramento de comunicação SPI/Parallel/I2C
}


//================================================================================
//
// Funções Principais de Inicializacao
//
//================================================================================


/**
 * @brief Construtor da Classe RA8889
 *
 * @param None
 * 
 * @note None
 */
RA8889::RA8889(uint8_t cs, uint8_t rst)
{
  _bus           = nullptr;
  _xnreset       = rst;
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;
  _bpp           = COLOR_DEPTH;
  _mcu           = MCU;
  _colorfmt      = static_cast<uint8_t>(ePDATAColorFmt::RGB); //iniciar com o formato de cor RGB
  _usedma        = false;
  _fntparam_source_select  = eFontSource::InternalCGROM;      //Default do display
  _display_spi_clk_divider = BIT_SPI_DIV4;
}


//ESTUDAR A IMPLEMENTACAO
//Existe a resolucao maxima do display ativado
//esta funcao melhraor que podera acinar qualqeur uma das resolucoes permitidas
//no entanto nao podera passar do maximo e nem do minimo. Se estiver ok, retorna true que foi efetivado a escolha ou false que foi recusado
/**
 * @brief Define a resolução do display width x height
 *
 * @param None
 * 
 * @note Mesmo que o RA8889 consiga gerenciar todas estas resoluções
 *       fica limitado pela caracteristica de tela (screen) produzidos
 *       por um fabricante. Para o caso do modelo de display da 
 *       BuyDisplay ER-TFT070-2-6105, a controladora da tela (screen) é
*        o EK9713 800x480/800x600 (1200x960 pixel panel).
 */
void RA8889::PanelResolution(ePanelResolution resolution)
{
  if (resolution == ePanelResolution::r320x240)  {_displaywidth=320;  _displayheight=240;}
  if (resolution == ePanelResolution::r480x272)  {_displaywidth=480;  _displayheight=272;}
  if (resolution == ePanelResolution::r640x480)  {_displaywidth=640;  _displayheight=480;}
  if (resolution == ePanelResolution::r800x480)  {_displaywidth=800;  _displayheight=480;}
  if (resolution == ePanelResolution::r800x600)  {_displaywidth=800;  _displayheight=600;}
  if (resolution == ePanelResolution::r960x540)  {_displaywidth=960;  _displayheight=540;}
  if (resolution == ePanelResolution::r1024x600) {_displaywidth=1024; _displayheight=600;}
  if (resolution == ePanelResolution::r1024x768) {_displaywidth=1024; _displayheight=768;}
  if (resolution == ePanelResolution::r1280x768) {_displaywidth=1280; _displayheight=768;}
  if (resolution == ePanelResolution::r1280x800) {_displaywidth=1280; _displayheight=800;}
  if (resolution == ePanelResolution::r1366x768) {_displaywidth=1366; _displayheight=768;}
}


/**
 * @brief Inicializa configurações basicas do display RA8889
 *
 * @verbatim
 * Antes de utilizar as funcoes de display, é necessário configurar e iniciar 
 * o barramento Bus para trasnferencia de dados. Isso é realizado como exemplo no 
 * código a seguir.
 * @endverbatim
 *
 * @code
 * Bus_SPI spi;                                  //Classe Bus_SPI
 * RA8889 gfx(PIN_CS, PIN_RESET);                // 
 *
 * void setup() {
 *   auto cfg = spi.config();                    //Configuração SPI 
 *   cfg.spi_host = FSPI_HOST;                   //VSPI
 *   cfg.pin_mosi = 23;                          
 *   cfg.pin_miso = 19;
 *   cfg.pin_sclk = 18;
 *   cfg.pin_dc   = 21;
 *   cfg.freq_write = 40000000;                  //40MHz
 *   spi.config(cfg);                            //Grava as configurações
 *   
 *   gfx.setBus(spi);                            //seta o bus SPI para o RA8889
 *   gfx.Begin();                                // Aqui _Bus->Init() é chamado internamente
 * }
 * @endcode
 *
 * @param None
 *
 * @note None
 */
bool RA8889::Begin(void)
{
  if (!_bus) {
     DEBUG_PRINT("Bus not set!", 0, false, true);
     return false;
  }
  
  HardwareReset();                             //Hardware Reset
  DEBUG_PRINTD("HardwareReset()...", 0, false, 0, true);         //Debug

  _bus->Init();                                //inicializa comunicação SPI, I2C ou Parallel
  DEBUG_PRINT("_bus->Init() Sucesso", 0,false,true);

  #ifdef CHECK_RAIO_FAMILY
  //Verifica se é um RA8889
  if (ReadIDCode() == 0x89) { DEBUG_PRINT("RA8889 connect pass!",0,false, true); }
  else { 
    DEBUG_PRINT("RA8889 not found!",0,false,true);
    DEBUG_PRINT("ID Code: ",ReadIDCode(),true,true);
  return false;
  }
  #endif
  
  PLL_InitilizeWaitReady();
  DEBUG_PRINT("PLL_InitilizeWaitReady() Sucesso", 0,false,true);

  delay(100);

  // Aguarda até que a inicialização interna do RA8889 termine
  // Bit 1 do STSR (0x02) = 1 → inicialização em andamento
  // Bit 1 do STSR (0x02) = 0 → inicialização concluída
  while(_bus->StatusRead() & 0x02);
  DEBUG_PRINT("_bus->StatusRead() Sucesso", 0,false,true);
  
  //Inicializa as configurações basicas do display RA8889
  if (!Initialize()) {
    DEBUG_PRINT("RA8889 initial fail!",0,false,true);
    return false;
  } else {
    DEBUG_PRINT("RA8889 initial sucess!",0,false,true);
  }
  
  //Inicializa fonte

  Font_Init();

  return true;
}


/**
 * @brief mapa de recursos de caracteres no CI ROM
 *
 * @verbatim
 * https://gaotongfont.cn/zlxz/list_29.aspx?sjId=3&page=4
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::External_CGROM_CharSetResourceMap(void)
{
#if defined(CHIP_GT30L24T3Y)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_BIG5, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_GB2312, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_GB12345, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_UNICODE, MAPBREAKPOS, 0x88));
#elif defined(CHIP_GT20L24F6Y)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_1_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_2_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_3_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_4_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_5_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_7_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_8_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_9_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_10_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_11_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_14_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_15_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ISO8859_16_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_LGCATH, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_UNICODE, MAPBREAKPOS, 0x88));
#elif defined(CHIP_GT21L16T1W)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_BIG5, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_GB12345, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_JIS0208, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_UNICODE, MAPBREAKPOS, 0x88));
#elif defined(CHIP_GT21L24S1W)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_GB2312, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ASCII, MAPBREAKPOS, 0x88));
#elif defined(CHIP_GT30L24M1Z)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_GB18030, MAPBREAKPOS, 0x88));
#elif defined(CHIP_GT30L16U2W)
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_ASCII, MAPBREAKPOS, 0x88));
    _charsetresourceMap |= (1UL << conditional_subtract(BIT_UNICODE, MAPBREAKPOS, 0x88));    //Padrao 3.0, GB13000
#endif

}


/**
 * @brief Verifica se a determinada fonte existe no Genitop ROM
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 * 
 * @return true a fonte existe
 *
 * @note None
 */
bool RA8889::CheckFontExternal(uint8_t font_model)
{
  return _charsetresourceMap & (1UL << conditional_subtract(font_model, MAPBREAKPOS, 0x88));
}


/**
 * @brief Inicializa a configurações básicas do display RA8889.
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool RA8889::Initialize(void)
{
  //Configura clock Pixel/SDRAM/Core PLL
  
  PLL_Init();                                  //Prepare PLL circuit

  SDRAM_Init();                                //Inicializa a SDRAM

//Chip Configuration Register (CCR) [01h]
//Nota: Não é obrigatorio que o TFT seja o mesmo apdrão de dados da MCU. Por comodismo deixei o mesmo

#if defined(COLOR_DEPTH_16) || defined(COLOR_DEPTH_8)
  TFT_16bit();                                 //LCD Panel Data Bus 65K colors output format for source drive
#elif defined(COLOR_DEPTH_24)
  TFT_24bit();                                 //LCD Panel Data Bus 16.7M colors output format for source drive
#endif

//Ref. RAIO RA8876 / RA8877 AP Note_ Source Code 001_How to initial RA8876,RA8877. 
//If MCU I/F select SPI-3, SPI-4 or I2C, chip will only allow Host_Bus_8bit.
#ifdef COM_PARALLEL                            //Apenas para interface Paralela 8080/6800
  #ifdef MCU16
    HostDataBus_Select_16bit();                //MCU/MPU Host bus 16bit
  #endif
  #ifdef MCU8
    HostDataBus_Select_8bit();                 //MCU/MPU Host bus 8bit
  #endif
#endif

  //Mask_XnWAIT(false);                          //Wait no mask, Mask XnWAIT

//Memory Access Control Register (MACR) [02h]

  Select_MCU_ColorDepth();                       //MPU/MCU I/F 8/16 bits color depth 8/16/24 (mode1/mode2) to Host
  HostWriteMemoryDirection(MemoryDirection::LeftRight_TopBotom);
  
//Input Control Register (ICR) [03h]

  GraphicMode();
  MemorySelect_SDRAM();

//Display Configuration Register (DPCR) [12h]
//Panel scan Clock and Data Setting Register (PCSR) [13h]
//Horizontal Display Width Register (HDWR) [14h]
//Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
//Vertical Display Height Register 0(VDHR0) [1Ah]
//Vertical Display Height Register 1 (VDHR1) [1Bh]
//Horizontal Non-Display Period(HNDR) [16h]
//Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
//HSYNC Start Position Register (HSTR) [18h]
//HSYNC Pulse Width Register (HPWR) [19h]
//Vertical Non-Display Period Register 0(VNDR0) [0x1c]
//Vertical Non-Display Period Register 1(VNDR1) [0x1d]
//VSYNC Pulse Width Register (VPWR) [0x1f]

  LCD_SetPanel();                              //Configuração do Panel Screen LCD, de acordo com o tipo do fabricante

#ifdef COLOR_DEPTH_8
  Select_MainWindow_8bpp();
  Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  Memory_8bpp_BlockMode();                     //Set 16bpp Block mode
  
  PIP1_Window_ColorDepth_8bpp();
  PIP2_Window_ColorDepth_8bpp();
  BTE_S0_ColorDeph_8bpp();
  BTE_S1_ColorDeph_8bpp();
  BTE_Destination_ColorDeph_8bpp());
#endif

#ifdef COLOR_DEPTH_16
  Select_MainWindow_16bpp();
  Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  Memory_16bpp_BlockMode();                    //Set 16bpp Block mode
  Select_MainWindow_16bpp();

  PIP1_Window_ColorDepth_16bpp();
  PIP2_Window_ColorDepth_16bpp();
  BTE_S0_ColorDeph_16bpp();
  BTE_S1_ColorDeph_16bpp();
  BTE_Destination_ColorDeph_16bpp();
#endif

#ifdef COLOR_DEPTH_24
  Select_MainWindow_24bpp();
  Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  Memory_24bpp_BlockMode();                    //Set 16bpp Block mode
  
  PIP1_Window_ColorDepth_24bpp();
  PIP2_Window_ColorDepth_24bpp();
  BTE_S0_ColorDeph_24bpp();
  BTE_S1_ColorDeph_24bpp();
  BTE_Destination_ColorDeph_24bpp();
#endif

//Image buffer configure

  MainImage_StartAddress( LayerStartAddr(0) );
  MainImage_Width(_displaywidth); 
  MainWindow_StartXY(0,0);
  CanvasImage_StartAddr( LayerStartAddr(0) );
  CanvasImage_Width(_displaywidth);
  ActiveWindow_XY(0,0);
  ActiveWindow_WidhtHeight(_displaywidth, _displayheight);  
  CoreTask_WaitReady();

  return true;
}


/**
 * @brief Retorna a largura da tela
 *
 * @param None
 *
 * @note None
 */
uint16_t RA8889::Width(void)
{
  return _displaywidth;
}


/**
 * @brief Retorna o comprimento da tela
 *
 * @param None
 *
 * @note None
 */
uint16_t RA8889::Height(void)
{
  return _displayheight;
}


/**
 * @brief Inicializa a configuração da Tela LCD (Screen) de acordo com a 
 *        montagem feita pelo fornecedor do display.
 *        Informações baseada no IC do drive da tela utilziada no display.
 *
 * @param None
 *
 * @note None
 */
void RA8889::LCD_SetPanel(void)
{

#if defined(EK9713)                            //Fitipower EK9713 800x600/800x480
   
  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]
  
  HScanDirection_LeftToRight();                //HSCAN Left to Right
  VScanDirection_TopToBottom();                //VSCAN Top to Bottom
  PCLK_Falling();                              //LCD PCLK Falling
  PDATA_ColorRGB();                            //Select RGB output format

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_PolarityLow();
  VSYNC_PolarityLow();
  DE_PolarityHigh();

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);
  
  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;
	
  //Minha notas:
  //Estas funcoes são de caracteristica da tela LCD (Screen) gerenciado pelo
  //RA8889, no entanto o RA8889 controla o tipo de tela. Cada fabricante de 
  //tela possui seus determinados valores tipicos de Blanking horiz/Vert,  
  //HSYNC/VSYNC largura de pulso e front porch que precisam ser grenciados 
  //pelo RA8889.

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)
  
#elif defined(AT070TN92)                       //Innolux AT070TN92 800X480 7" TFT-LCD

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(eHSYNCPolarity::Low);
  VSYNC_Polarity(eVSYNCPolarity::Low);
  DE_Polarity(eDEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);
  
  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;
  
  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(AWT_1024600L7N03)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(eHSYNCPolarity::Low);
  VSYNC_Polarity(eVSYNCPolarity::Low);
  DE_Polarity(eDEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);
  
  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(AWY_800480T70N02)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(eHSYNCPolarity::Low);
  VSYNC_Polarity(eVSYNCPolarity::Low);
  DE_Polarity(eDEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);

  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(EJ080NA_05B)                     //Innolux EJ080NA-05B 800x600 8.0" TFT-LCD

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(eHSYNCPolarity::Low);
  VSYNC_Polarity(eVSYNCPolarity::Low);
  DE_Polarity(eDEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);

  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(ET0700B3DMA)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(eHSYNCPolarity::Low);
  VSYNC_Polarity(eVSYNCPolarity::Low);
  DE_Polarity(eDEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);

  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(G190SVT01)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_PolarityLow();
  VSYNC_PolarityLow();
  DE_PolarityHigh();
  
  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);

  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = 350;                               //valor diferente de LCD_VH=342 

  //[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR  
  //[18h]      : Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1) * 8
  //[19h]      : Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1) * 8
 
  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking) - 288
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
  //[1Eh]      : Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
  //[1Fh]      : Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking) - 16
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(LQ190E1LW52)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  //Display Configuration Register (DPCR) [12h]

  PCLK_Failing();
   
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);

  //Panel scan Clock and Data Setting Register (PCSR) [13h]
  
  HSYNC_PolarityLow();
  VSYNC_PolarityLow();
  DE_PolarityHigh();

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);

  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)

#elif defined(LQ035NC111)

  //Main/PIP Window Control Register (MPWCTR) [10h]
  
  Select_LCD_SyncMode();                      //Enable XVSYNC, XHSYNC, XDE.

  PCLK_Rising();
  
  VScanDirection_TopToBottom();
  PDATA_ColorFmt(ePDATAColorFmt::RGB);
  
  //Panel scan Clock and Data Setting Register (PCSR) [13h]
  
  HSYNC_PolarityLow();
  VSYNC_PolarityLow();
  DE_PolarityLow();
  
  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);
  
  //Seta a resolução do display baseado no painel
  
  _displaywidth  = LCD_HW;
  _displayheight = LCD_VH;

  //[16h][17h] : Figure 19-3 [HND]	Non Display or Back porch (pixels)		= (HNDR + 1) * 8 + HNDFTR  
  //[18h]      :	Figure 19-3 [HST]	Start Position or Front porch (pixels)	= (HSTR + 1) * 8
  //[19h]      :	Figure 19-3 [HPW]	Pulse Width(pixels)						= (HPW + 1) * 8
 
  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //[1Ch][1Dh] : Figure 19-3 [VND]	Non-Display Period(Line)	= (VNDR + 1)
  //[1Eh] :		 Figure 19-3 [VST]	Start Position(Line)		= (VSTR + 1)
  //[1Fh] :		 Figure 19-3 [VPW]	Pulse Width(Line)			= (VPWR + 1)
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)
  
#else
  #error "Nenhum formato de Painel LCD válido foi definido"
#endif

}


//================================================================================
// Funções SPI
//================================================================================


//Inicializa o SPI para a comunicacao com o Display RA8889
//void RA8889::SPI_Init(void)
//{
//  pinMode(_cs, OUTPUT);
//  spi.beginTransaction(SPISettings(_spi_clockmax, MSBFIRST, _spi_datamode));
//  spi.begin();
//  _spi_transaction = true;
//}


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
//void RA8889::SPISetCS(uint8_t level_cs)
//{
//  level_cs == 0 ? digitalWrite(_cs, LOW) : /*SS_RESET */  digitalWrite(_cs, HIGH); /*SS_SET*/
//}


/**
 * @brief Escrever alguma informação para barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param value: dados para SPI
 *
 * @note Note
 */
//uint8_t RA8889::SPIRwByte(uint8_t value)
//{
//  uint8_t result;
//  result = spi.transfer(value);
//  return result;
//}


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
//void RA8889::SPI_CmdWrite(uint8_t cmd)
//{
//  SPISetCS(0);                                 //SS_RESET
//  SPIRwByte(RA8889_SPI_CMDWRITE);              //0x00, Avisa Display que será um comando
//  SPIRwByte(cmd);                              //Envia um comando de 1 byte para o Display
//  SPISetCS(1);                                 //SS_SET
//}


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
//void RA8889::SPI_DataWrite(uint8_t data)
//{
//  SPISetCS(0);                                 //SS_RESET;
//  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
//  SPIRwByte(data);                             //Envia um byte de Dado para o SPI
//  SPISetCS(1);                                 //SS_SET;
//}
//void RA8889::SPI_DataWrite8(uint8_t data) {SPI_DataWrite(data);}


/**
 * @brief Escreve dados de 2 byte (16 bits) para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param data: dados de 2 bytes para display
 *
 * @note Note
 */
//void RA8889::SPI_DataWrite16(uint16_t data)
//{
//  SPISetCS(0);                                 //SS_RESET;
//  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
//  SPIRwByte(data);                             //Envia um byte menos significativo de Dado para o SPI
//  SPIRwByte(data >> 8);                        //Envia um byte mais significativo de Dado para o SPI
//  SPISetCS(1);                                 //SS_SET;
//}


/**
 * @brief Escreve dados de 3 byte (24 bits) para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param uint32_t data: dados de 3 bytes para display. A parte alta de bit 31-28 será truncado
 *
 * @note None
 */
//void RA8889::SPI_DataWrite24(uint32_t data)
//{
//  SPISetCS(0);                                 //SS_RESET;
//  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever 
//  SPIRwByte(data);                             //Envia byte 1 de Dado para o SPI
//  SPIRwByte(data >> 8);                        //Envia byte 2 de Dado para o SPI
//  SPIRwByte(data >> 16);                       //Envia byte 3 de Dado para o SPI
//  SPISetCS(1);                                 //SS_SET;
//}


//SPI_DataWritePixel
//void RA8889::SPI_DataWrite_Pixel(uint16_t data)
//{
//  SPISetCS(0);                                 //SS_RESET;
//  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
//  SPIRwByte(data);                             //Escreve a parte baixa da palavra
//  SPISetCS(1);                                 //SS_SET;
//											   
//  SPISetCS(0);                                 //SS_RESET;
//  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
//  SPIRwByte(data >> 8);                        //Escreve a parte alta da palavra
//  SPISetCS(1);                                 //SS_SET;
//}


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
//uint8_t RA8889::SPI_DataRead(void)
//{
//  uint8_t temp;
//  SPISetCS(0);                             //SS_RESET
//  SPIRwByte(RA8889_SPI_DATAREAD);          //0xc0, Leitura de dados
//  temp = SPIRwByte(0x00);                  //envia um dummy byte para receber dados
//  SPISetCS(1);                             //SS_SET
//  return temp;
//}


//uint16_t RA8889::DataRead16(uint8_t address)
//{
//  uint16_t data;
//  SPISetCS(0);                             //SS_RESET
//  spi.transfer(address);
//  data = spi.transfer(0x00);               //MSB
//  data <<= 8;                              //Shift 8 bits right
//  data |= SPI.transfer(0x00);              //LSB
//  SPISetCS(1);                             //SS_SET
//  return data;
//}


//================================================================================
// Comandos para o Display
//================================================================================


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
//uint8_t RA8889::StatusRead(void)
//{
//  uint8_t temp = 0;
//  SPISetCS(0);                            //SS_RESET
//  SPIRwByte(RA8889_SPI_STATUSREAD);       //0x40, Read Status SPI
//  temp = SPIRwByte(REG_STSR);             //0x00, Read STSR Register
//  SPISetCS(1);                            //SS_SET
//  return temp;
//}


/** 
 * @brief Escrever em um registrador da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display, data: dados para escrever no registrador
 *
 * @return None
 */
//void RA8889::RegisterWrite(uint8_t reg, uint8_t data)
//{
//  SPI_CmdWrite(reg);
//  SPI_DataWrite(data);
//}


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
 *
 * @return dados do registrador
 */
//uint8_t RA8889::RegisterRead(uint8_t reg)
//{
//  uint8_t temp;
//  SPI_CmdWrite(reg);
//  temp = SPI_DataRead();
//  return temp;
//}


/**
 * @brief Executa um reset de hardware no RA8889 através do pino RESET.
 *
 * @verbatim
 * Mantém o pino de reset (configurado em _xnreset) em nível baixo por 500 ms para
 * garantir que o chip seja reinicializado, depois volta a nível
 * alto por mais 500 ms para concluir o processo de reset.
 *
 * Aplicação:
 * Esse procedimento força o RA8889 a retornar ao estado inicial,
 * sendo útil quando o PLL ou a inicialização interna falham.
 * @endverbatim
 * 
 * @param None
 *
 * @return None 
 *
 * @note: Reset segundo o modelo de codigo da RAIO
 */
void RA8889::HardwareReset(void)
{
  pinMode(_xnreset, OUTPUT);
  
  //digitalWrite(_xnreset, HIGH);
  //delay(10);
  //digitalWrite(_xnreset, LOW);
  //delay(10);
  //digitalWrite(_xnreset, HIGH);
  //delay(50);

  digitalWrite(_xnreset, LOW);
  delay(500);
  digitalWrite(_xnreset, HIGH);
  delay(500);

}


/**
 * @brief Executa um reset por software no RA8889
 *
 * @verbatim
 * REG [0x00] 
 * bit [0] Software Reset (write)
 *     0: Normal operation.
 *     1: Software Reset.
 *     Software Reset only reset internal state machine. Configuration 
 *     Registers value won’t be reset. So all read-only flag in the 
 *     register will return to its initial value. User should have 
 *     proper action to make sure flag is in desired state. 
 *     Note: The bit will auto clear after reset.
 * bit [0] Warning condition flag (read)
 *     0: No warning operation occurred
 *     1: Warning condition occurred.
 *     Please check REG[E4h] bit 3 for more detail. (Read)
 * @endverbatim
 * 
 * @param None
 * 
 * @bug
 *   Apos isso o display nao exibe mais nada, mesmo que seja a primeira coisa que executou no display
 *   Correções para serem testados: colocou delayMicroseconds(100)
 * @return  1: time out sem completar, 0: software reset efetivado 
 */
uint8_t RA8889::SoftwareReset(void)
{
  uint8_t temp;
  uint8_t res = 1;                             //timeout as default
  _bus->RegisterWrite(REG_SRR, 0x01);                //0x00, Software Reset Register (SRR)
  temp = _bus->DataRead();
  temp |= 0x01;
  _bus->DataWrite(temp);
  delayMicroseconds(100);			           //it must wait 100us after Software_Reset.
  for (uint16_t i=0; i<1000;i++){
     if (_bus->RegisterRead(REG_SRR)&0x01 == 0x00) { //No Warning condition flag
		 res = 0;
		 break;
     }
  }
  return res;
}


/**
 * @brief Aguarde o sistema estaeja em modo de operação normal
 *        apos uma inicilização, ou termine de sair do estado de 
 *        economia de energia.
 *
 * @verbatim
 * Status Register (STSR)
 * bit [1] Operation mode status
 *         0b0 : Normal operation state  → inicialização concluída
 *         0b1 : Inhibit operation state → inicialização em andamento
 *               Inhibit operation state means internal reset event 
 *               keep running or initial display still running or chip 
 *               enter power saving state.
 * 
 * Aplicação: Até que a inicialização do IC (Core) tenha terminado uma 
 *            operação de inicialização (rest) ou um retorno de uma 
 *            economia de energia.
 * @endverbatim
 *
 * @param true:  IC em modo operação normal e pronto,
 *        false: IC ainda não concluiu a inicializacao
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::IC_WaitReady(void)
{
  static const uint32_t COUNTER = 1000000;     //de acordo com o uso, altere o valor de i.
  for(uint32_t i = 0; i < COUNTER; i++) {
    if( (_bus->StatusRead() & 0x02) == 0x00 ) return true;
	delayMicroseconds(1);
  }
  return false;
}


/**
 * @brief Aguarde até as tarefas do sistema estejam prontas
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [3] Core task is busy
 *                0b0 : task is done or idle.
 *                0b1 : task is busy
 *                      Following task is running:
 *                      BTE, Geometry engine, Serial flash DMA, Text write 
 *                      or Graphic write.
 *
 *        Aplicação: Quando o envio de certos comandos o controlador fica 
 *                   ocupado até o momento de terminar a tarefa. Envie este 
 *                   comando para aguardar até que a tarefa saia no modo 
 *                   ocupado (busy) para o modo ocioso (idle) ou feito (done)
 * @endverbatim
 *
 * @param None
 * 
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::CoreTask_WaitReady(void)
{
  static const uint32_t COUNTER = 1000000;	//Ajuste valor de i de acordo com a necessidade
  for(uint32_t i = 0; i < COUNTER; i++) {
    if((_bus->StatusRead() & 0x08) == 0x00) return true;
    delayMicroseconds(1);
  } 
  return false;
}


/**
 * @brief Aguarde até as tarefas de desenho grafico e texto estejam prontos
 *              
 * @verbatim
 * Status Register (STSR)
 * bit [3] Core task is busy
 *         0b0 : task is done or idle.
 *         0b1 : task is busy
 *               Following task is running:
 *               BTE, Geometry engine, Serial flash DMA, Text write 
 *               or Graphic write.
 *
 * Aplicação: Quando o envio de certos comandos o controlador fica 
 *            ocupado até o momento de terminar a tarefa. Envie este 
 *            comando para aguardar até que a tarefa saia no modo 
 *            ocupado (busy) para o modo ocioso (idle) ou feito (done)
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::Draw_WaitReady(void)  { return CoreTask_WaitReady(); }


/**
 * @brief Aguarde até que a memória de escrita FIFO tenha algo nela
 *              
 * @verbatim
 * Status Register (STSR)
 * bit [7] Host Memory Write FIFO full
 *         0b0 : Memory Write FIFO is not full.
 *         0b1 : Memory Write FIFO is full.
 * 
 * Only when Memory Write FIFO is not full, MPU may write another one 
 * pixel.
 *
 * Muito usado no caso do portão de escrita de memória tenha enviado os dados em uma FIFO interno
 * e tenha feito o procesamento quando os dados vão para processamento na memoria do diplay SDRAM.
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::Wait_WriteFIFO_NotFull(void)
{  
  static const uint32_t COUNTER = 10000;   //Ajuste valor de i de acordo com a necessidade
  for(uint16_t i = 0; i < COUNTER; i++) {
    if( (_bus->StatusRead() & 0x80) == 0x00 ) return true;
  }
  return false;
}


/**
 * @brief Aguarde até que a memória de escrita FIFO esteja livre para escrever
 *              
 * @verbatim
 * Status Register (STSR)
 * bit [6] Host Memory Write FIFO empty
 *         0b0 : Memory Write FIFO is not empty.
 *         0b1 : Memory Write FIFO is empty.
 * 
 * When Memory Write FIFO is empty, MPU may write 8bpp data 64 pixels, or 
 * 16bpp data 32 pixels, 24bpp data 16 pixels directly.
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
* 
 * @note None
 */
bool RA8889::Wait_WriteFIFO_Empty(void)
{
  static const uint32_t COUNTER = 10000;   //Ajuste valor de i de acordo com a necessidade
  for(uint16_t i = 0; i < COUNTER; i++) {
    if( (_bus->StatusRead() & 0x40) == 0x40 ) return true;
  }
  return false;
}


/**
 * @brief Aguarde até que a memória de leitura FIFO Não esteja cheio
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [5] Host Memory Read FIFO full
 *                0b0 : Memory Read FIFO is not full.
 *                0b1 : Memory Read FIFO is full.
 *        
 *        When Memory Read FIFO is full, MPU may read 8bpp data 32
 *        pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
 *
 *        Dizer que algo nao está cheio, quer dizer que está pela metade,
 *        nem vazio, nem cheio.
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::Wait_ReadFIFO_NotFull(void)
{
  static const uint32_t COUNTER = 10000;       //Ajuste valor de i de acordo com a necessidade
  for(uint16_t i = 0; i < COUNTER; i++) {
    if( (_bus->StatusRead() & 0x20) == 0x00 ) return true;
  }
  return false;
}


/**
 * @brief Aguarde até que a memória de leitura FIFO tenha algo para ler
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [4] Host Memory Read FIFO empty
 *                0b0 : Memory Read FIFO is not empty.
 *                0b1 : Memory Read FIFO is empty.
 * 
 *        Dizer que algo não está vazio, quer dizer que tem alguma coisa 
 *        ou esta completo (totalmente cheio)
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 *
 * @note None
 */
bool RA8889::Wait_ReadFIFO_NotEmpty(void)
{ 
  static const uint32_t COUNTER = 10000;       //Ajuste valor de i de acordo com a necessidade  
  for(uint16_t i = 0; i < COUNTER; i++) {
    if( (_bus->StatusRead() & 0x10) == 0x00 ) return true;
  }
  return false;
}


/**
 * @brief Verifica se é um RA8889
 *        ATENÇÂO: Ainda não foi testado este codigo
 *
 * @verbatim
 * Para ler o ID code precisa desabilitar o PLL, 0x01 bit 7 setando para 0.
 * 
 * A RAIO em alguns de seus chipos como o RA8876/RA8877 e RA8889 possuem 
 * páginas de registradores 0 e 1. Para poder ler o Id nestes controladores, 
 * precisa acessar a página de registradores 1.
 *
 * O RA8875 apresenta todos na mesma pagina de registradrores.
 *
 * Chip       ID esperado (REG 0xFF)
 * ---------------------------------
 * RA8875     0x75
 * RA8876     0x76
 * RA8877     0x77
 * RA8889     0x89
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @return value to RA8876/RA8877/RA8889
 */
uint8_t RA8889::ReadIDCode(void)
 { 
  uint8_t datareg = 0;
  uint8_t temp;
  temp = _bus->RegisterRead(REG_CCR);                  //Ler o registrador 0x01
  datareg = temp;                                //Salva o estado original do PLL
  CLRB(temp,7);                                  //Desligar o PLL momentaneamente
  _bus->RegisterWrite(REG_CCR, temp);                  //0x01, Chip Configuration Register (CCR)
  delay(1);
  PageSwitch(ePageReg::Page1);                   //Muda para pagina de registradores 1
  temp = _bus->RegisterRead(0xff);                     //Registrador não documentado, para ID do Chip
  PageSwitch(ePageReg::Page0);                   //Retorna para  pagina de registradores 1
  delay(1);
  _bus->RegisterWrite(REG_CCR, datareg);               //Devolve o status original do 0x01, Chip Configuration Register (CCR)
  return temp;                                   //O ID do RA8889, tem que retornar 0x89
 }


//================================================================================
//
// Funcoes PLL
//
//================================================================================


/** OK
 * @brief Aguarda até que o RA8889 finalize sua inicialização interna e o PLL esteja pronto.
 * 
 * @verbatim
 * Fluxo:
 *  - Verifica o registrador de status (STSR) para saber se a inicialização interna terminou.
 *  - Quando terminado, acessa o Chip Configuration Register (CCR) e checa se o PLL (bit 7) está pronto.
 *  - Se o PLL não estiver pronto, reconfigura-o e tenta novamente.
 *  - Caso o sistema não responda após várias tentativas, executa um reset de hardware e repete o processo.
 * 
 * A função só retorna quando o sistema está estável e pronto para operar.
 * @endverbatim
 */
void RA8889::PLL_InitilizeWaitReady(void)
{
  static const uint8_t COUNT_TIMEOUT = 5;     //implementei talvez precise de ajuste
  uint8_t count_timeout = 0;
  uint8_t temp = 0;
  bool system_ok = false;
  
  do {
    temp = _bus->StatusRead();               //Read Status Register STSR
    DEBUG_PRINT("_bus->StatusRead()", temp,true,true);
	if((temp & 0x02) == 0x00) {                //Veja se o bit 1 esta limpo (0x00=modo de operação normal, evento de inicialização interna terminou)

      delay(2);                                //MCU too fast, necessary
      _bus->CmdWrite(REG_CCR);                   //0x01, Access register Chip Configuration Register (CCR)
      delay(2);                                //MCU too fast, necessary
      temp = _bus->DataRead();                   //Leia o CCR 
      if((temp & 0x80) == 0x80) {              //Check CCR register's PLL is ready or not (bit 7 = 1) value=0x80
        system_ok = true;                      //PLL pronto
        count_timeout = 0;
      } else {
        delay(2);                              //MCU too fast, necessary
        _bus->CmdWrite(REG_CCR);                 //0x01, Access register Chip Configuration Register (CCR)
        delay(2);                              //MCU too fast, necessary
        _bus->DataWrite(0x80);                   //Reconfigura a frequencia do PLL
      }
	    
    } else {                          
        
	  system_ok = false;                         //A inicialização interna ainda está sendo feita
      count_timeout++;                           //fazer outra tentativa
	    
    }
	  
    if(system_ok==false && count_timeout==COUNT_TIMEOUT) {   //Sistema ainda nao está pronto e houve timeout
      HardwareReset();                           //*note1, envia um reset novamente
      count_timeout = 0;                         //zera o contador de timeout 
    }
	
  } while(system_ok==false);                     //faz enquanto não ficar pronto o sistema
}


//Habilita o PLL
void RA8889::PLL_Enable(void)
{
/* 1: PLL enable; cannot change PLL parameter.*/
  uint8_t temp;
  uint16_t i;

  _bus->CmdWrite(REG_CCR);                       //0x01, Envia comando Chip Configuration Register (CCR) 
  //removi isso nao faz sentido escrever comando REG_CCR e outro o SSR simultaneao sem enviar dados ou receber depois dele
  //_bus->CmdWrite(0x00);                          //Como o CCR possui tudo zerado por default ainda na inicilizacao e configuração do dispositivo, o bit 7 será zerado (inicia com 1 como default)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Habilita o PLL
  _bus->DataWrite(temp);

  delayMicroseconds(10);                       // PLL lock time = 1024 T OSC clocks, if OSC=10MHz, PLL lock time = 10 us.

  /*check PLL was ready ( Please according to your usage to modify. (Modifique de acordo com o uso)	*/
  for(i=0;i<1000;i++) {
    _bus->CmdWrite(REG_CCR);                       //0x01, Envia comando Chip Configuration Register (CCR) 
    temp = _bus->DataRead();                       //Leia o registrador
    if( (temp & 0x80)==0x80 ){break;}            //Veja se as configuracoes do PLL ficaram prontas para o uso
  }
}


//Desabilita o PLL
void RA8889::PLL_Disable(void)
{
/* 0: PLL disanable; can change PLL parameter.*/
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Envia comando Chip Configuration Register (CCR) 
  //removi isso nao faz sentido escrever comando REG_CCR e outro o SSR simultaneao sem enviar dados ou receber depois dele
  //_bus->CmdWrite(0x00);                          //Como o CCR possui tudo zerado por default ainda na inicilizacao e configuração do dispositivo, o bit 7 será zerado (inicia com 1 como default)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Disable PLL
  _bus->DataWrite(temp);
  delayMicroseconds(100);                      // PLL lock time = 1024 T OSC clocks, if OSC=10MHz, PLL lock time = 100 us.  
}


/**
 * @brief Configura o PLL do RA8889 para ajustar frequências de Pixel Clock, 
 *        SDRAM Clock e Core Clock
 * 
 * @verbatim
 * Configura o PLL do RA8889 para ajustar as frequências de:
 * - Pixel Clock (SCAN_FREQ)
 * - SDRAM Clock (DRAM_FREQ)
 * - Core Clock (CORE_FREQ)
 *
 * REG[05h] SCLK PLL Control Register 1 (PPLLC1) - SCAN or PIXEL Clock PLL
 * REG[07h] MCLK PLL Control Register 1 (MPLLC1) - MEMORY Clock PLL
 * REG[09h] CCLK PLL Control Register 1 (SPLLC1) - CORE or SYSTEM Clock PLL
 *
 * A função calcula automaticamente o divisor e multiplicador adequado
 * com base no cristal externo (OSC_FREQ) e nas frequências alvo.
 * 
 * Fluxo seguro:
 * 1. Desliga o PLL temporariamente
 * 2. Configura os registros de divisores/multiplicadores
 * 3. Habilita o PLL com os novos valores
 *
 *
 * (1) OSC_FREQ = 10MHz 
 * (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
 * (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 500MHz
 *     PLLDIVM:0
 *     PLLDIVN:1~63
 *     PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
 *     example:
 *       OSC_FREQ = 10MHz
 *       Set X_DIVK=2
 *       Set X_DIVM=0
 *       => (X_DIVN+1)=(XPLLx4)/10
 * @endverbatim
 *
 * @note  PLL (Phase-Locked Loop)
 * 
 * @param scanclk : PSCK Panel Scan clock frequency, unit: MHz
 * @param sdramclk: SDRAM clock frequency, unti: MHz	
 * @param coreclk : Core (system) clock frequency, unit: MHz 
 * @param xtalclk : Crystal clock, unit: MHz 
 */
void RA8889::PLL_ConfigClocks(uint8_t scanclk, uint8_t dramclk, uint8_t coreclk, uint8_t xtalclk)
{
  
  // ---------- Set Pixel/Scan Clock ----------
 
  if(scanclk>=63)
  {
    _bus->CmdWrite(REG_PPLLC1);                  //0x05 
    _bus->DataWrite(0x04);                       //PLL Divided by 4
    _bus->CmdWrite(REG_PPLLC2);                  //0x06
    _bus->DataWrite((scanclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=32)&&(scanclk<=62))         
  {                                            
    _bus->CmdWrite(REG_PPLLC1);                  //0x05    
    _bus->DataWrite(0x06);                       //PLL Divided by 8
    _bus->CmdWrite(REG_PPLLC2);                  //0x06
    _bus->DataWrite((scanclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=16)&&(scanclk<=31))         
  {                                            
	_bus->CmdWrite(REG_PPLLC1);                  //0x05     
	_bus->DataWrite(0x16);                       //PLL Divided by 16
	_bus->CmdWrite(REG_PPLLC2);                  //0x06
	_bus->DataWrite((scanclk*16/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=8)&&(scanclk<=15))          
  {                                            
	_bus->CmdWrite(REG_PPLLC1);                  //0x05    
	_bus->DataWrite(0x26);                       //PLL Divided by 32
	_bus->CmdWrite(REG_PPLLC2);                  //0x06
	_bus->DataWrite((scanclk*32/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>0)&&(scanclk<=7))            
  {                                            
	_bus->CmdWrite(REG_PPLLC1);                  //0x05    
	_bus->DataWrite(0x36);                       //PLL Divided by 64
	_bus->CmdWrite(REG_PPLLC2);                  //0x06
	_bus->DataWrite((scanclk*64/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }            
  
  // ---------- Set SDRAM clock ----------

  if(dramclk>=125)
  {
	_bus->CmdWrite(REG_MPLLC1);                  //0x07 
	_bus->DataWrite(0x02);                       //PLL Divided by 2
	_bus->CmdWrite(REG_MPLLC2);                  //0x08
	_bus->DataWrite((dramclk*2/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((dramclk>=63)&&(dramclk<=124))
  {
	_bus->CmdWrite(REG_MPLLC1);                  //0x07     
	_bus->DataWrite(0x04);                       //PLL Divided by 4
	_bus->CmdWrite(REG_MPLLC2);                  //0x08
	_bus->DataWrite((dramclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((dramclk>=31)&&(dramclk<=62))
  {           
	_bus->CmdWrite(REG_MPLLC1);                  //0x07     
	_bus->DataWrite(0x06);                       //PLL Divided by 8
	_bus->CmdWrite(REG_MPLLC2);                  //0x08
	_bus->DataWrite((dramclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if(dramclk<=30)
  {
	_bus->CmdWrite(REG_MPLLC1);                  //0x07   
	_bus->DataWrite(0x06);                       //PLL Divided by 8
	_bus->CmdWrite(REG_MPLLC2);                  //0x08
	_bus->DataWrite((30*8/xtalclk)-1);           //Deve ser de 1~63, 0 é proibido
  }
 
  // ---------- Set Core clock ----------
  
  if(coreclk>=125)
  {
	_bus->CmdWrite(REG_SPLLC1);                  //0x09
	_bus->DataWrite(0x02);                       //PLL Divided by 2
	_bus->CmdWrite(REG_SPLLC2);                  //0x0A
	_bus->DataWrite((coreclk*2/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((coreclk>=63)&&(coreclk<=124))     
  {
	_bus->CmdWrite(REG_SPLLC1);                  //0x09   
	_bus->DataWrite(0x04);                       //PLL Divided by 4
	_bus->CmdWrite(REG_SPLLC2);                  //0x0A
	_bus->DataWrite((coreclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((coreclk>=31)&&(coreclk<=62))
  {           
	_bus->CmdWrite(REG_SPLLC1);                  //0x09  
	_bus->DataWrite(0x06);                       //PLL Divided by 8
	_bus->CmdWrite(REG_SPLLC2);                  //0x0A
	_bus->DataWrite((coreclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if(coreclk<=30)
  {
	_bus->CmdWrite(REG_SPLLC1);                  //0x09   
	_bus->DataWrite(0x06);                       //PLL Divided by 8
	_bus->CmdWrite(REG_SPLLC2);                  //0x0A
	_bus->DataWrite((30*8/xtalclk)-1);           //Deve ser de 1~63, 0 é proibido
  }

}


/**
 * @brief Inicializa o PLL do RA8889 com as frequências padrão
 * 
 * @details
 * Esta função realiza a configuração completa do PLL do RA8889 utilizando
 * os valores padrão definidos por macros:
 * - SCAN_FREQ  : frequência do Pixel/Scan Clock
 * - DRAM_FREQ  : frequência do SDRAM Clock
 * - CORE_FREQ  : frequência do Core/System Clock
 * - OSC_FREQ   : frequência do cristal externo
 * 
 * O procedimento segue o fluxo seguro:
 * 1. Desabilita temporariamente o PLL via PLL_Disable()
 * 2. Configura os divisores e multiplicadores apropriados chamando
 *    PLL_ConfigClocks()
 * 3. Habilita o PLL com os novos valores via PLL_Enable()
 * 
 * Após a execução, o PLL estará pronto para fornecer as frequências de
 * operação ao RA8889.
 * 
 * @note Esta função depende de macros previamente definidas para as
 * frequências de operação e do cristal externo. Também imprime uma
 * mensagem no Serial indicando que a configuração foi concluída.
 */
void RA8889::PLL_Init(void)
{
  //PLL_Disable();  //O PLL so pode ser modificado com novos valores desligando antes
  PLL_ConfigClocks(SCAN_FREQ, DRAM_FREQ, CORE_FREQ, OSC_FREQ);
  //PLL_Enable();	

	_bus->CmdWrite(0x01);
	_bus->CmdWrite(0x00);
	delay(1);
	_bus->CmdWrite(0x80);

  DEBUG_PRINT("PLL Initialized",0,false, true);
}


//================================================================================
//
// Funcoes SDRAM
//
//================================================================================


/** OK
 * @brief Aguarda até que a SDRAM do RA8889 esteja pronta para acesso.
 *
 * Esta função realiza leituras repetidas do registrador de status (STSR) 
 * do RA8889 até que o bit 2 esteja definido como '1', indicando que a 
 * memória SDRAM já está inicializada e pronta para operações de leitura 
 * e escrita. 
 *
 * Uso típico: deve ser chamada após a inicialização da SDRAM ou 
 * antes de qualquer operação que dependa do acesso estável à memória.
 *
 * @verbatim 
 * Status Register (STSR) 
 * Bit [2] SDRAM ready for access
 *         0: SDRAM não está pronta para acesso.
 *         1: SDRAM pronta para acesso.
 * Before user check this bit staus , user must be set ”sdr_initdone” bit as 1
 * @endverbatim
 *
 * @param None
 *
 * @return true: sucesso, false: erro, tempo de espera muito longo (timeout)
 */
bool RA8889::SDRAM_WaitReady(void)
{
  static const uint32_t COUNTER = 1000000;
  for (unsigned long i = 0; i < COUNTER; i++) {
    delayMicroseconds(1);
    if ((_bus->StatusRead() & 0x04) == 0x04) return true;
  }
  return false;
}


/**
 * @brief Inicializa a memória SDRAM externa usada pelo RA8889.
 *
 * Esta função configura os parâmetros necessários para o controlador SDRAM do RA8889,
 * incluindo a latência CAS e o intervalo de refresh, de acordo com a frequência de operação
 * (64 MHz) e os requisitos típicos da SDRAM (8192 ciclos de refresh a cada 64 ms).
 *
 * Passos executados:
 *  - Define o modo de operação da SDRAM.
 *  - Configura a latência CAS (3 ciclos).
 *  - Calcula e ajusta o intervalo de refresh com base no clock do sistema.
 *  - Grava os valores de refresh nos registradores correspondentes.
 *  - Ativa a SDRAM e aguarda até que esteja pronta para uso.
 *
 * @note Deve ser chamada apenas uma vez na inicialização do sistema, 
 *       antes de qualquer acesso à memória SDRAM (framebuffer).
 */
void RA8889::SDRAM_Init(void)
{
  uint16_t Auto_Refresh;
  uint8_t  CAS_Latency;
	
  //0xe0, SDRAM attribute register (SDRAR)
  //Configura o modo da SDRAM
  //  0x29 = indica parâmetros como largura do barramento e o tipo de refresh.
  //  SDRAM Bank number (sdr_bank)      bit 5=1b     -> uso 4 bancos
  //  SDRAM Row addressing (sdr_row)    bit 4-3=01b  -> 4096 (A0-A11) 
  //  SDRAM Column addressing (sdr_col) bit 2-0=001b -> 512 (A0-A8)
  _bus->RegisterWrite(REG_SDRAR, 0x29);            
    
  //0xe1, SDRAM mode register & extended mode register (SDRMD)
  //Define a latência CAS (Column Address Strobe latency).
  //  0x03 = Define a latência CAS (Column Address Strobe latency)
  //  SDRAM CAS latency (sdr-caslat)    bit 2-0 CAS:2 010b=0x02 -> 2 ciclos CAS:3 011b=0x03 -> 3 ciclos  
  CAS_Latency = 0x03;
  _bus->RegisterWrite(REG_SDRMD, CAS_Latency);
  
  //Set SDRAM refresh interval via SDRAM auto refresh interval registers
  
  //Calcula o intervalo de refresh da SDRAM.
  // - A SDRAM precisa de 4096 ciclos de refresh em 64 ms.
  // - Esse cálculo pega o clock de 64 MHz, divide pelo número de linhas (4096)
  // - O -2 é um ajuste de margem para não ficar no limite.
  Auto_Refresh = (64 * DRAM_FREQ * 1000) / (4096);  //valores em MHz (foi testado no original e funciona bem)
  //Auto_Refresh = (64000000 / 8192) / (1000/60);
  Auto_Refresh-=2;                                  //Start [refresh] in advance to avoid just rachiong the limits
  
  //0xe2, SDRAM auto refresh interval (SDR_REF_ITVL0) - Byte low
  _bus->RegisterWrite(REG_SDR_REF_ITVL0, Auto_Refresh); //envia byte menos significativo da palavra
  
  //0xe3, SDRAM auto refresh interval (SDR_REF_ITVL1) - Byte high
  _bus->RegisterWrite(REG_SDR_REF_ITVL1, Auto_Refresh >> 8); //envia byte mais significativo da palavra
  
  //0xe4, SDRAM Control register (SDRCR)
  //  0x01 = Iniciar procedimento de inicialização da SDRAM (sdr_initdone)
  _bus->RegisterWrite(REG_SDRCR, 0x01);
  
  //Espera até que a SDRAM esteja pronta
  SDRAM_WaitReady();
  delay(1);
}


//================================================================================
//
// [0x01] Chip Configuration Register (CCR)
//
//================================================================================


/**
 * @brief Mask XnWAIT on XnCS deassert
 *
 * Mask XnWAIT on XnCS deassert
 * 0 : No mask
 *     XnWAIT keep assert if internal state keep busy and cannot
 *     accept next R/W cycle, no matter XnCS assert/deassert. If
 *     MCU cycle cannot be extended while XnWAIT keep low, user
 *     should poll XnWAIT and wait it goes high then start next
 *     access.
 * 1 : Mask
 *     XnWAIT deassert when XnCS deassert. Use in MCU cycle can
 *     be extended by XnWAIT automatically.
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 *            bit [6] Mask XnWAIT on XnCS deassert
 *            0b0: No mask
 *            XnWAIT keep assert if internal state keep busy and cannot accept 
 *            next R/W cycle, no matter XnCS assert/deassert. If MPU cycle 
 *            cannot be extended while XnWAIT keep low, user should poll 
 *            XnWAIT and wait it goes high then start next access.
 *            0b1: Mask
 *            XnWAIT deassert when XnCS deassert. Use in MPU cycle can be 
 *            extended by XnWAIT automatically.
 * @endverbatim
 *
 * Em outras palacras, quando bit=6 estiver reetado, independente da MCU soltar o 
 * xnCS, antes de começar a próxima escrita/leitura você precisa verificar se o 
 * XnWAIT já voltou para alto (liberado).
 *
 * @param mask
 *
 * @note None
 *
 */
void RA8889::XnWAIT_Mask(bool mask)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  mask ? SETB(temp,6) : CLRB(temp,6);      //Set/reset bit 6
  _bus->DataWrite(temp);	
}


/**
 * @brief Key Scan Enable/Disable
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [5] Key-Scan Enable/Disable
 *     0: Disable.
 *     1: Enable.
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::KeyScan_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  b ? SETB(temp,5) : CLRB(temp,5); 
  _bus->DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída 24-bits da interface (I/F) do painel TFT
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [4-3]
 *     0b00: Saída TFT 24-bits
 *     0b01: Saída TFT 18-bits
 *     0b10: Saída TFT 16-bits
 *     0b11: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 * @endverbatim
 *
 * @param None
 */
void RA8889::TFT_24bit(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  CLRB(temp,3);                                //Reset bit 3 
  _bus->DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída 18-bits da interface (I/F) do painel TFT
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [4-3]
 *     0b00: Saída TFT 24-bits
 *     0b01: Saída TFT 18-bits
 *     0b10: Saída TFT 16-bits
 *     0b11: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 * @endverbatim
 *
 * @param None
 */
void RA8889::TFT_18bit(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  SETB(temp,3);                                //Set bit 3 
  _bus->DataWrite(temp);
}


/** OK
 * @brief Configuração do pino de saída 18-bits da interface (I/F) do painel TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void RA8889::TFT_16bit(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  SETB(temp,4);                                //Set bit 4
  CLRB(temp,3);                                //Reset bit 3 
  _bus->DataWrite(temp);
}


/**
 * @brief Configuração do pino sem nenhuma saída interface (I/F) do painel TFT
 *
 * Desativa o uso de interface do painel de TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void RA8889::TFT_Without(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  SETB(temp,4);                                //Set bit 4
  SETB(temp,3);                                //Set bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída de interface (I/F) do painel TFT
 *
 *        Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *        bit [4-3] 0b00: Saída TFT 24-bits
 *                  0b01: Saída TFT 18-bits
 *                  0b10: Saída TFT 16-bits
 *                  0b11: Sem Nenhuma Saída TFT
 * 
 *        Outros pinos de saída TFT não utilizados são definidos como GPIO ou 
 *        função de tecla.
 *
 * @param None
 *
 * @note None
 */
void RA8889::TFT_Interface(TFTInterface mode)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = _bus->DataRead();
  CLRB(temp,4);
  CLRB(temp,3);
  temp |= static_cast<uint8_t>(mode);          
  _bus->DataWrite(temp);
}


/**
 * @brief I2C Master Enable/Disable
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [2] IIC master Interface Enable/Disable
 *     0: Disable (GPIO function)
 *     1: Enable (IIC master function)
 *     IIC master pins are shared with XKIN[0] & XKOUT[0].
 *     this bit has higher priority than Key-Scan Enable bit. ie. if IIC master and Key-Scan are enable simultaneously then XKIN[0]/XKOUT[0] will become IIC function & other XKIN/XKOUT pins still keep Key-scan function.
 * @endverbatim
 *
 * @param None
 */
void RA8889::I2CM_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  b ? SETB(temp,2) : CLRB(temp,2); 
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash or SPI Interface Enable/Disable
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [1] Serial Flash or SPI Interface Enable/Disable
 *     0: Disable (GPIO function)
 *     1: Enable (SPI master function)
 *
 * When SDR SDRAM 32bits bus function enable, this bit is ignored
 * & Serial flash pins become SDR SDRAM bus function.
 * @endverbatim
 *
 * @code
 * SFlashSPI_Enable(true);
 * @endcode
 *
 * @param None
 */
void RA8889::SFlashSPI_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = _bus->DataRead();
  b ? SETB(temp,1) : CLRB(temp,1); 
  _bus->DataWrite(temp);                         //Escreve de volta no CCR
}


/**
 * @brief Parallel Host Data Bus 8-bit Width Selection
 *
 * @verbatim
 * REG [0x01] Chip Configuration Register (CCR)
 * bit [0] Host Data Bus Width Selection
 *     0: 8-bit Parallel Host Data Bus
 *     1: 16-bit Parallel Host Data Bus
 * @endverbatim
 *
 * @param None
 *
 * @note Uso em MCU de 16-bit com: 
 *         - color depth 8-bpp
 *         - color depth 16-bpp
 *         - color depth 24-bpp
 */
void RA8889::HostDataBus_Select_8bit(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = _bus->DataRead();
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/** OK
 * @brief Parallel Host Data Bus 16-bit Width Selection
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [0]
 *   0b: 8-bit Parallel Host Data Bus
 *   1b: 16-bit Parallel Host Data Bus
 *
 * @param None
 *
 * @note Uso em MCU de 16-bit com: 
 *         - color depth 16-bpp
 *         - color depth 24-bpp Mode 1
 *         - color depth 24-bpp Mode 2
 */
void RA8889::HostDataBus_Select_16bit(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = _bus->DataRead();
  SETB(temp,0);                                //Set bit 0, 16-bit data bus
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x02] Memory Access Control Register (MACR)
//
//================================================================================


//outro nome sugerido: MCUDataFormatSDRAM()
/** OK
 * @brief MPU/MCU Read/Write data format when access memory data
 *
 * @verbatim
 * REG [0x02] Memory Access Control Register (MACR)
 * bit [7-6] Host Read/Write image Data Format
 *           MPU/MCU read/write data format when access memory data port.      
 *     0b0x: Direct write for all 8 bits MPU I/F, 16 bits MPU I/F with 
 *           16bpp, 16 bits MPU I/F with 24bpp data mode 1 and serial host 
 *           interface.
 *     0b10: Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
 *     0b11: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
 * @endverbatim
 *
 * @param uint8_t type: 0, 1 e 2
 *
 * @note type=0 (0b0x) - Uso em MCU de 8/16-bit:
 *                     - MCU 8-bit I/F with color depth 8-bpp 
 *                     - MCU 8-bit I/F with color depth 16-bpp
 *                     - MCU 8-bit I/F with color depth 24-bpp
 *                     - MCU 16-bit I/F with color depth 8-bpp data mode 2
 *                     - MCU 16-bit I/F with color depth 16-bpp
 *                     - MCU 16-bit I/F with color depth 24-bpp data Mode 1
 *                     - Serial Host Interface
 *       type=1 (0b10) - Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
 *                     - MCU 16-bit I/F with color depth 8-bpp data Mode 1
 *       type=2 (0b11) - Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
 *                     - MCU 16-bit I/F with color depth 24-bpp data Mode 2
 */
void RA8889::HostColorDepthFormat(uint8_t type)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = _bus->DataRead();
  
  if (type == 0) {                             //0xb: Direct write (for all 8 bits MPU I/F, 16 bits MPU I/F with 8bpp data mode 1 & 2, 16 bits MPU I/F with 16/24-bpp data mode 1 & serial host interface)
	  CLRB(temp,7);                            //Reset bit 7
	  CLRB(temp,6);                            //Reset bit 6
  } else if (type == 1) {                      //10b: Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
	  SETB(temp,7);                            //Set bit 7
	  CLRB(temp,6);                            //Reset bit 6
  } else if (type == 2) {                      //11b: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
	  SETB(temp,7);                            //Set bit 7
	  SETB(temp,6);                            //Set bit 6
  }
  _bus->DataWrite(temp);                         //Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
}


//outro nome sugerido: MCU_DataFormat_SDRAM
/**
 * @brief Select a data format for MCU read/write SDRAM.
 *
 * Esta função configura o formato de cor utilizado pelo host MCU/MPU
 * ao acessar a SDRAM do RA8889. A seleção é feita em tempo de compilação
 * através de diretivas `#define`, garantindo que apenas um modo válido
 * seja configurado.
 *
 * @verbatim
 * HostColorDepthFormat(0) -> Mode 0: MCU 8/16 bits, color depth 8/16/24 bpp (Mode 1)
 * HostColorDepthFormat(1) -> Mode 1: MCU 16 bits, color depth 8 bpp
 * HostColorDepthFormat(2) -> Mode 2: MCU 16 bits, color depth 24 bpp
 * @endverbatim
 *
 * @note É necessário definir um dos macros de configuração antes da compilação.
 *       Caso contrário, um erro será gerado no pré-processamento.
 *
 * Macros aceitos:
 * - MCU_8bit_COLORDEPTH_8bpp
 * - MCU_8bit_COLORDEPTH_16bpp
 * - MCU_8bit_COLORDEPTH_24bpp
 * - MCU_16bit_COLORDEPTH_8bpp_Mode2
 * - MCU_16bit_COLORDEPTH_16bpp
 * - MCU_16bit_COLORDEPTH_24bpp_Mode1
 * - MCU_16bit_COLORDEPTH_8bpp_Mode1
 * - MCU_16bit_COLORDEPTH_24bpp_Mode2
 */
void RA8889::Select_MCU_ColorDepth(void)
{
#if defined(MCU_8bit_COLORDEPTH_8bpp) || \
    defined(MCU_8bit_COLORDEPTH_16bpp) ||  \
    defined(MCU_8bit_COLORDEPTH_24bpp) || \
    defined(MCU_16bit_COLORDEPTH_8bpp_Mode2) || \
    defined(MCU_16bit_COLORDEPTH_16bpp) || \
    defined(MCU_16bit_COLORDEPTH_24bpp_Mode1)
  HostColorDepthFormat(0);                      //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
#elif defined(MCU_16bit_COLORDEPTH_8bpp_Mode1)
  HostColorDepthFormat(1);                      //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
#elif defined(MCU_16bit_COLORDEPTH_24bpp_Mode2)
  HostColorDepthFormat(2);                      //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
#else
  #error "Nenhum formato de MCU com color depth válido foi definido"
#endif
}


/** OK
 * @brief Host Read Memory Direction (Only for Graphic Mode)
 *        Video memory read direction setting
 *        
 *        Efeito somente no modo gráfico
 *
 *        bit [5-4] 0b00: Left to Right then Top to Bottom
 *                  0b01: Right to Left then Top to Bottom
 *                  0b10: Top to Bottom then Left to Right
 *                  0b11: Bottom to Top then Left to Right
 *
 * @param MemoryDirection::LeftRight_TopBotom 
 *        MemoryDirection::RightLeft_TopBotom
 *        MemoryDirection::TopBotom_LeftRight
 *        MemoryDirection::BotomTop_LeftRight 
 *
 *
 * @note Only for Graphic Mode. Graphics mode can be activated after using 
 *       this function.
 */
void RA8889::HostReadMemoryDirection(MemoryDirection direction)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = _bus->DataRead();
  temp &= ~(cSetb5 | cSetb4);                  //Reset bit 5 e 4
  temp |= (static_cast<uint8_t>(direction) << 4); //posiciona o valor para o bit 5 e 4
  _bus->DataWrite(temp);                         //Host Read Memory Direction
}


/** OK
 * @brief Host Write Memory Direction (Only for Graphic Mode)
 *        Video memory write direction setting
 *        
 *        Efeito somente no modo gráfico
 *
 *        REG [02h] Memory Access Control Register (MACR)
 *                  bit [2-1] 0b00: Left to Right then Top to Bottom (Original)
 *                            0b01: Right to Left then Top to Bottom (Horizontal flip)
 *                            0b10: Top to Bottom then Left to Right (Rotate right 90˚& Horizontal flip)
 *                            0b11: Bottom to Top then Left to Right (Rotate left 90˚)
 *
 * @param MemoryDirection::LeftRight_TopBotom 
 *        MemoryDirection::RightLeft_TopBotom
 *        MemoryDirection::TopBotom_LeftRight
 *        MemoryDirection::BotomTop_LeftRight 
 *
 *
 * @note Only for Graphic Mode. Ignored if canvas in linear addressing mode.
 */
void RA8889::HostWriteMemoryDirection(MemoryDirection direction)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MACR);                             //0x02, Memory Access Control Register (MACR)
  temp = _bus->DataRead();
  temp &= ~(cSetb2 | cSetb1);                         //Reset bit 2 e 1
  temp |= (static_cast<uint8_t>(direction) << 1);     //posiciona o valor para o bit 2 e 1
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x03] Input Control Register (ICR)
//
//================================================================================


/**
 * @brief Seleciona o nível lógico que a interrupção fica ativa
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [7] Output to MPU Interrupt pin’s active level
 *                         0b0 : active low.
 *                         0b1 : active high.
 *
 * @param eInterruptLevel::Low
 *        eInterruptLevel::High
 *
 */
void RA8889::Interrupt_ActiveLevel(eInterruptLevel level)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                //Reset bit 7
  temp |= static_cast<uint8_t>(level);         //Define o nível
  _bus->DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) with de-bounce
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [6] External interrupt input (XPS[0] pin) de-bounce
 *                         0b0 : without de-bounce
 *                         0b1 : enable de-bounce (1024 OSC clock)	
 *
 * @param None
 *
 * @note None
 */
void RA8889::ExtInterrupt_Debounce(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       
  SETB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) without de-bounce
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [6] External interrupt input (XPS[0] pin) de-bounce
 *                          0b0 : without de-bounce
 *                          0b1 : enable de-bounce (1024 OSC clock)	
 *
 * @param None
 *
 * @note None
 */
void RA8889::ExtInterrupt_NoDebounce(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       
  CLRB(temp,6);                                //Reset bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) trigger type
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [5-4] External interrupt input (XPS[0] pin) trigger type
 *                            0b00 : low level trigger
 *                            0b01 : falling edge trigger
 *                            0b10 : high level trigger
 *                            0b11 : rising edge trigger
 *                            
 *
 * @param None
 *
 * @note None
 */
void RA8889::ExtInterrupt_InputLevelTrigger(eInterrupLevelTrigger leveltrg)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();     
  CLRB(temp,5);                                //Reset bit 5
  CLRB(temp,4);                                //Reset bit 4
  temp |= (static_cast<uint8_t>(leveltrg) << 3); 
  _bus->DataWrite(temp);
}


/**
 * @brief Select LVDS Data Format VESA Standard
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [3] FPD-Link Data Format / LVDS Data Format
 *                          0b0 : Format 1 (VESA format) --- use with displays expecting the 2 MSB to be transmitted over the 4th data channel Y3.
 *                          0b1 : Format 2 (JEIDA format) --- use with displays expecting the 2 LSB to be transmitted over the 4th data channel Y3.
 *                            
 *
 * @param None
 *
 * @note RA8877 only
 */
void RA8889::LVDS_DataFormat_VESA(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       
  CLRB(temp,3);                                //Reset bit 3 
  _bus->DataWrite(temp);
}


/**
 * @brief Select LVDS Data Format JEIDA Standard
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [3] FPD-Link Data Format / LVDS Data Format
 *                          0b0 : Format 1 (VESA format) --- use with displays expecting the 2 MSB to be transmitted over the 4th data channel Y3.
 *                          0b1 : Format 2 (JEIDA format) --- use with displays expecting the 2 LSB to be transmitted over the 4th data channel Y3.
 *                            
 *
 * @param None
 *
 * @note RA8877 only
 */
void RA8889::LVDS_DataFormat_JEIDA(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       
  SETB(temp,3);                                //Set bit 3
  _bus->DataWrite(temp);
}


/** OK
 * @brief Muda o display para modo grafico
 *
 * @verbatim
 * REG [0x03] Input Control Register (ICR)
 * bit [2] Text Mode Enable
 *     0 : Graphic mode.
 *     1 : Text mode.
 *     Before toggle this bit user must make sure core task busy bit 3 
 *     (Status Register - STSR) in status register is done or idle.
 *     This bit always 0 (in graphic mode) if canvas' address mode is linear 
 *     mode.
 * @endverbatim
 *
 * @param None
 *
 * @return true se o display já estava em modo grafico
 *
 * @note None
 *
 */
bool RA8889::GraphicMode(void){
  uint8_t temp;
  bool res;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  res = (temp & 0x4) == 0;                     //bit [2]=0, Graphic mode
  CLRB(temp,2);                                //Reset bit 2
  _bus->DataWrite(temp);                         //Ativa modo grafico
  return res;
}


/**
 * @brief Verifica se o modo grafico está ativo
 *
 * @param None
 *
 * @note None
 *
 * @return true: Modo Grafico, false: Modo texto
 */
bool RA8889::IsGraphicMode(void){
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  return ((temp == cSetb2) == 0x00);           //Verificar bit 2 está desligado
}


/**
 * @brief Muda o display para modo texto
 *
 * @verbatim
 * REG [0x03] Input Control Register (ICR)
 * bit [2] Text Mode Enable
 *     0 : Graphic mode.
 *     1 : Text mode.
 *     Before toggle this bit user must make sure core task busy bit 3 
 *     (Status Register - STSR) in status register is done or idle.
 *     This bit always 0 (in graphic mode) if canvas' address mode is linear 
 *     mode.
 * @endverbatim
 *
 * @param None
 *
 * @return true se o display já estava em modo texto
 *
 * @note None
 *        
 */
bool RA8889::TextMode(void)
{
  uint8_t temp;
  bool res;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  res = (temp & 0x4);                          //bit [2]=1, Text mode
  SETB(temp,2);                                //Set bit 2
  _bus->DataWrite(temp);                         //Ativa o modo texto
  return res;
}


/** OK
 * @brief Seleciona o destino da porta de memória do RA8889 para a SDRAM.
 *
 * Configura os bits [1:0]=00b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note Image buffer (SDRAM) for image data, pattern (palette), user-characters. 
 *        
 */
void RA8889::MemorySelect_SDRAM(void)
{
  uint8_t temp = 0;
  _bus->CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();               //Lê valor atual do registrador
  CLRB(temp,1);                        //Reset bit 1
  CLRB(temp,0);                        //Reset bit 0
  _bus->DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para Tabela Gama.
 *
 * Configura os bits [1:0]=01b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note Tabela Gama para cores Vermelho/Verde/Azul.
 *        
 */
void RA8889::MemorySelect_GammaTable(void)
{
  uint8_t temp = 0;
  _bus->CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();               //Lê valor atual do registrador
  CLRB(temp,1);                        //Reset bit 1
  SETB(temp,0);                        //Set bit 0
  _bus->DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para Cursor Gráfico.
 *
 * Configura os bits [1:0]=10b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note RAM do Cursor Gráfico (aceita apenas dados MPU de 8 bits, leitura e 
 *       gravação de dados de registradores normais semelhantes), não suporta 
 *       leitura de RAM do Cursor Gráfico. Contém 4 conjuntos de cursores 
 *       gráficos. Cada conjunto tem 128x16 bits. O usuário precisa especificar 
 *       o conjunto de cursores gráficos de destino e continuar a gravação de 
 *       256 bytes..
 */
void RA8889::MemorySelect_GraphicCursorRAM(void)
{
  uint8_t temp = 0;
  _bus->CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();               //Lê valor atual do registrador
  SETB(temp,1);                        //Set bit 1  
  CLRB(temp,0);                        //Clear bit 0
  _bus->DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para RAM de Palette.
 *
 * Configura os bits [1:0]=11b do registrador ICR (0x03)
  *
 * @param Nenhum
 *
 * @note RAM do Cursor Gráfico (aceita apenas dados MPU de 8 bits, leitura e 
 *       gravação de dados de registradores normais semelhantes), não suporta 
 *       leitura de RAM do Cursor Gráfico. Contém 4 conjuntos de cursores 
 *       gráficos. Cada conjunto tem 128x16 bits. O usuário precisa especificar 
 *       o conjunto de cursores gráficos de destino e continuar a gravação de 
 *       256 bytes..
 */
void RA8889::MemorySelect_ColorPaletteRAM(void)
{
  uint8_t temp = 0;
  _bus->CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();               //Lê valor atual do registrador
  SETB(temp,1);                        //Set bit 1
  SETB(temp,0);                        //Set bit 0
  _bus->DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889.
 *
 * @verbatim
 * REG [0x03] Input Control Register (ICR)
 *     bit [1-0] Memory port Read/Write Destination Selection
 *     0b00 : Image buffer (SDRAM) for image data, pattern, user-characters. Support Read-modify-Write.
 *     0b01 : Gamma table for Color Red/Green/Blue. Each color’s gamma table has 256 bytes. User need specify desired gamma table and continuous write 256 bytes.
 *     0b10 : Graphic Cursor RAM (only accept low 8-bits MPU data, similar normal register data r/w.), not support Graphic Cursor RAM read. It contains 4 graphic cursor sets. Each set has 128x16 bits. User need specify target graphic cursor set and continue write 256 bytes.
 *     0b11 : Color palette RAM. It is 64x12 bits SRAM, so even address’ data only low 4 bits are valid. Not support Color palette RAM read. User need continue write 128 bytes.
 * 
 * Configura este bit para escolher o destino da porta de memoria do display.
 * Ao enviar dados para o display ele é feito através de uma porta, esta porta precisa ser selecionada
 * através deste registrador bit [1-0].
 *
 * Por exemplo, para escrever um pixel na tela é enviado para esse registrador o valor 0b00 e entao se escreve 
 * os dados para o buffer de imagem através da porta no registrdor 0x04 Memory Data Read/Write Port (MRWDP). 
 * Este é a passagem usada entre o registador (buffer de dados) e a memoria do display.
 *
 * Exemplo: MemoryPort_Select(MemoryPortDest::SDRAM);
 *          
 * @endverbatim
 *
 * @param dest Destino da porta de memória (SDRAM, Tabela Gama, Cursor Grafico e Palete)
 *
 */
void RA8889::MemoryPort_Select(MemoryPortDest dest)
{
  uint8_t temp;
  _bus->CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,1);                                //Reset bit 1
  CLRB(temp,0);                                //Reset bit 0 
  temp |= static_cast<uint8_t>(dest);          //
  _bus->DataWrite(temp);                         //Atualiza registrador
}


//================================================================================
//
// PAGE 0
// [0x0B] Interrupt Enable Register (INTEN)
//
//================================================================================


/**
 * @brief Interrupt Resume Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [7] Wakeup/resume Interrupt Enable
 *         0: Disable.
 *         1: Enable.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_Resume_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,7) : CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief External Interrupt Input Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [6] External Interrupt (PS[0] pin) Enable
 *         0: Disable.
 *         1: Enable.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::ExtInterrupt_Input_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,6) : CLRB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt I2C Master Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [5] I2C Master Interrupt Enable
 *         0: Disable.
 *         1: Enable.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_I2CM_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,5) : CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt Vertical Synchronization time base Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [4] Vsync time base interrupt Enable Bit
 *         0: Disable Interrupt.
 *         1: Enable Interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_VSync_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,4) : CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt Key Scan Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [3] Key Scan Interrupt Enable Bit
 *         0: Disable Key scan interrupt.
 *         1: Enable Key scan interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_KeyScan_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,3) : CLRB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt Serial Flash DMA, Draw Task, BTE Process Complete, etc. Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [2] Serial flash DMA Complete | Draw task finished | 
 *         BTE Process Complete etc. Interrupt Enable
 *         0: Disable interrupt.
 *         1: Enable interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_ClearMultiEventTask_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,2) : CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt PWM Timer 1 Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [1] PWM timer 1 Interrupt Enable Bit
 *         0: Disable interrupt.
 *         1: Enable interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_PWM1_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,1) : CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt PWM Timer 0 Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 * bit [0] PWM timer 0 Interrupt Enable Bit
 *         0: Disable interrupt.
 *         1: Enable interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::Interrupt_PWM0_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();
  b ? SETB(temp,0) : CLRB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0x0B] Interrupt Enable Register (INTEN)
//
//================================================================================


/**
 * @brief IDEC Interrupt Enable/Disable
 *
 * @verbatim
 * PAGE 1 REG [0x0B] Interrupt Enable Register (INTEN)
 *        bit [0] IDEC Interrupt Enable Bit
 *                0: Disable Interrupt.
 *                1: Enable Interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void RA8889::IDEC_InterruptEnable(bool b)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);                 //Troca para Pagina 1
  _bus->CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = _bus->DataRead();                       //
  b ? SETB(temp,0) : CLRB(temp,0);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);                 //Troca para Pagina 1
}


//================================================================================
//
// PAGE 0 
// [0x0C] Interrupt Event Flag Register (INTF)
// *If you received an interrupt but cannot identify it on Interrupt Event Flag 
// Register, please check SPI master status register’s interrupt flag bits 
// REG[BAh].
//
//================================================================================


/**
 * @brief Read Interrupt Status
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [7] Read Function ..Resume Interrupt Status
 *         0: No Resume interrupt happens.
 *         1: Resume interrupt happens.
 * Bit [6] Read Function .. PS[0] pin Interrupt Status
 *         0: No PS[0] pin interrupt happens.
 *         1: PS[0] pin interrupt happens.
 * Bit [5] Read Function .. I2C master Interrupt Status
 *         0: No I2C master interrupt happens.
 *         1: I2C master interrupt happens.
 * Bit [4] Read Function .. Vsync Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * Bit [3] Read Function ..Key Scan Interrupt Status
 *         0: No Key Scan interrupt happens.
 *         1: Key Scan interrupt happens.
 * Bit [2] Read Function..Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * Bit [1] Read Function..Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * Bit [0] Read Function..Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * @endverbatim
 *                         
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
uint8_t RA8889::Interrupt_Status(void)
{
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  return _bus->DataRead();
}


/**
 * @brief Aguarde até o VSync termine de fazer o sincronismo vertical
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [4] Vsync Time base interrupt flag
 *         Write Function .. Vsync Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the Vsync interrupt.
 *         Read Function .. Vsync Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note É enviado um comando de verificaçãod a interrupção, pois pode estar 
 *       no meio do caminho ainda fazendo o VSync, antes de atualizar a tela ou
 *       escrever novos dados. 
 */
void RA8889::VSYNC_WaitReady(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  _bus->DataWrite(0x10);                         //Set bit 4, clear VSync interrupt status, solicita que verifique se o VSync está pronto
  do {
    temp = _bus->DataRead();                     //Leia o status
  } while ( (temp & 0x10) == 0x00);            //Aguarde ate que seja resetado o bit, terminou o retraço
}


/**
 * @brief Clear Resume Interrupt Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [7] Wakeup/resume Interrupt flag
 *         Write Function ➔ Wakeup/resume Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear Wakeup/resume interrupt.
 *         
 *         Read Function ➔ Wakeup/resume Interrupt Status
 *         0: No Wakeup/resume interrupt happens.
 *         1: Wakeup/resume interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 * 
 */
void RA8889::Interrupt_ClearResume_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Clear External Interrupt Input (PS[0] pin) Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [6] External Interrupt (PS[0] pin) flag
 *         Write Function ➔ XPS[0] pin edge Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the XPS[0] pin edge interrupt.
 *         
 *         Read Function ➔ XPS[0] pin Interrupt Status
 *         0: No XPS[0] pin interrupt happens.
 *         1: XPS[0] pin interrupt happens.
 * @endverbatim
 *                           
 * @param None
 *
 * @note None
 * 
 */
void RA8889::ExtInterrupt_ClearInput_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,6);                                //Resetar interrupção
  _bus->DataWrite(temp);
}


/**
 * @brief Clear I2C Master Interrupt Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [5] IIC master Interrupt flag
 *         Write Function➔ IIC master Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the IIC master interrupt.
 *
 *         Read Function ➔ IIC master Interrupt Status
 *         0: No IIC master interrupt happens.
 *         1: IIC master interrupt happens.
 * @endverbatim
 *                           
 * @param None
 *
 * @note None
 * 
 */
void RA8889::Interrupt_ClearI2CM_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,5);                                //Resetar interrupção
  _bus->DataWrite(temp);                         
}


/**
 * @brief Clear Interrupt Vertical Synchronization Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [4] Vsync Time base interrupt flag
 *         Write Function ➔Vsync Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the Vsync interrupt.
 *         Read Function ➔ Vsync Interrupt Status
 *         0: No Vsync interrupt happens.
 *         1: Vsync interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 * 
 */
void RA8889::Interrupt_ClearVSync_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,4);                                //Resetar interrupção
  _bus->DataWrite(temp);                         //Limpa status de interrupção VSync
}


/**
 * @brief Clear Interrupt Key Scan Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [3] Key Scan Interrupt flag
 *         Write Function ➔ Key Scan Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the Key Scan interrupt.
 *
 *         Read Function ➔ Key Scan Interrupt Status
 *         0: No Key Scan interrupt happens.
 *         1: Key Scan interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 * 
 */
void RA8889::Interrupt_ClearKeyScan_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,3);                                //Clear interrupt status
  _bus->DataWrite(temp);
}


/**
 * @brief Check for Interrupt Key Scan Occurred
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [3] Key Scan Interrupt flag
 *         Write Function ➔ Key Scan Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear the Key Scan interrupt.
 *
 *         Read Function ➔ Key Scan Interrupt Status
 *         0: No Key Scan interrupt happens.
 *         1: Key Scan interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note Para limpar a interrupção de teclas (Key Scan) após tratamento, use a funcao
 *       Interrupt_ClearKeyScan_Flag()
 */
bool RA8889::Interrupt_IsKeyPressed(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  return (temp & 0x08);                        //Recebeu a interrupção
}


/**
 * @brief  Clear Interrupt Serial Flash DMA, Draw Task, BTE Process Complete, etc. Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [2] Serial flash DMA Complete | Draw task finished | 
 *         BTE Process Complete | etc. Interrupt flag
 *         Write Function➔ Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear interrupt.
 *         Read Function➔Interrupt Status
 *         0: No interrupt happens.
 *         1: interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note De acordo com a requisição de evento de Serial Flash DMA, Draw Task 
 *       ou processos completos de BTE ou outros este flag é setado ou pode 
 *       ser limpo para qualquer um destes casos.
 */
void RA8889::Interrupt_ClearMultiEventTask_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Clear Interrupt PWM 0 Timer Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [0] PWM 0 timer Interrupt flag
 *         Write Function ➔ Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear PWM0 interrupt.
 *
 *         Read Function ➔ Interrupt Status
 *         0: No PWM0 interrupt happens.
 *         1: PWM0 interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::Interrupt_ClearPWM0_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
}


/**
 * @brief Clear Interrupt PWM 1 Timer Flag
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 * Bit [1] PWM 1 timer Interrupt flag
 *         Write Function ➔ Interrupt Clear Bit
 *         0: No operation.
 *         1: Clear PWM1 interrupt.
 *
 *         Read Function ➔ Interrupt Status
 *         0: No PWM1 interrupt happens.
 *         1: PWM1 interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::Interrupt_ClearPWM1_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,1);
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0x0C] Interrupt Event Flag Register (INTF)
// *If you received an interrupt but cannot identify it on Interrupt Event Flag 
// Register, please check SPI master status register’s interrupt flag bits 
// REG[BAh].
//
//================================================================================


/**
 * @brief Clear IDEC Interrupt Flag
 *        
 * PAGE 1 REG [0x0c] Interrupt Event Flag Register (INTF)
 *                    Bit [0] IDEC Interrupt flag
 *                            Write Function➔Interrupt Clear Bit
 *                            0: No operation.
 *                            1: Clear IDEC interrupt.
 *                            Read Function➔Interrupt Status
 *                            0: No IDEC interrupt happens.
 *                            1: IDEC interrupt happens.
 *
 * @param None
 *
 * @note None
 */
void RA8889::IDEC_ClearInterrupt_Flag(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0x0D] Mask Interrupt Flag Register (MINTFR)
//
// *** If you masked certain interrupt flag, then RA8889 neither assert 
// interrupt event to MPU nor checked it on Interrupt Flag Register. But if 
// you unmasked certain interrupt flag and disable this interrupt then MPU 
// won’t be informed by XnINTR but you still may check it on interrupt Flag 
// Register.
//
//================================================================================


/**
 * @brief Enable/Disable the mask for the Resume Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [7] Mask Wakeup/Resume Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_ResumeInterrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,7) : CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the External Interrupt Input Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [6] Mask External Interrupt (XPS[0] pin) Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_ExtInterruptInput_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,6) : CLRB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the I2C Master Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [5] Mask IIC Master Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_I2CMInterrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,5) : CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the VSync Time Base Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [4] Mask Vsync time base interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_VsyncInterrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,4) : CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the Key Scan Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [3] Mask Key Scan Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_KeyScanInterrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,3) : CLRB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the Generic Task System (Serial flash DMA 
 *        Complete | Draw task finished | BTE Process Complete etc) Interrupt 
 *        Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [2] Mask Serial flash DMA Complete | Draw task finished | BTE Process Complete etc. Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_GenericInterrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,2) : CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the PWM Timer 1 Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [1] Mask PWM timer 1 Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_PWM1Interrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,1) : CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable/Disable the mask for the PWM Timer 0 Interrupt Flag on the XnINTR pin.
 *
 * @verbatim 
 * REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * bit [0] Mask PWM timer 0 Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *
 * @param b
 *        true  - Mask interrupt, disable XnINTR output.
 *        false - Unmask interrupt, enable XnINTR output.
 *
 * @note None
 */
void RA8889::XnINTR_PWM0Interrupt_Mask(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MINTFR);                    //0x0d, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,0) : CLRB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0x0D] Mask Interrupt Flag Register (MINTFR)
//
//================================================================================


/**
 * @brief IDEC Unmask Interrupt Flag
 *        
 * @verbatim
 * PAGE 1 REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 *            Bit [0] Mask IDEC Interrupt Flag
 *                    0: Unmask.
 *                    1: Mask.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::IDEC_Unmask_Interrupt(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_MINTFR);           //0x0d, page 1, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  CLRB(temp,0);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief IDEC Mask Interrupt Flag
 *        
 * @verbatim
 * PAGE 1 REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 *            Bit [0] Mask IDEC Interrupt Flag
 *                    0: Unmask.
 *                    1: Mask.
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::IDEC_Mask_Interrupt(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_MINTFR);           //0x0d, page 1, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief Enable/Disable the mask for the IDEC Interrupt Flag.
 *        
 * @verbatim
 * PAGE 1 REG [0x0d] Mask Interrupt Flag Register (MINTFR)
 * Bit [0] Mask IDEC Interrupt Flag
 *     0: Unmask (IRQ output active)
 *     1: Mask (IRQ output disabled)
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::IDEC_Interrupt_Mask(bool b)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_MINTFR);           //0x0d, page 1, Mask Interrupt Flag Register (MINTFR)
  temp = _bus->DataRead();
  b ? SETB(temp,0) : CLRB(temp,0);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// [0x0E] Pull- high control Register (PUENR)
//
//================================================================================


/**
 * @brief IDEC GPIO F Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [5] GPIO-F[7:0] Pull-high Enable (XPDAT[23:19, 15:13])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOF_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO F Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [5] GPIO-F[7:0] Pull-high Enable (XPDAT[23:19, 15:13])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOF_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO E Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [4] GPIO-E[7:0] Pull- high Enable (XPDAT[12:10, 7:3])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    * Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOE_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO E Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [4] GPIO-E[7:0] Pull- high Enable (XPDAT[12:10, 7:3])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    * Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOE_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO D Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [3] GPIO-D[7:0] Pull- high Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    * Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOD_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO D Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [3] GPIO-D[7:0] Pull- high Enable (XPDAT[18, 2, 17, 16, 9, 8, 1,0])
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 *                    * Only available when XPDAT configure as GPIO function
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOD_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO C Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [2] GPIO-C[4:0] Pull- high Enable (XnSFCS1, XnSFCS0, XMISO, XMOSI , XSCK)
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOC_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC GPIO C Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [2] GPIO-C[4:0] Pull- high Enable (XnSFCS1, XnSFCS0, XMISO, XMOSI , XSCK)
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::GPIOC_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC XDB[15:8] Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [1] XDB[15:8] Pull- high Enable
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::XDB15_8_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC XDB[15:8] Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [1] XDB[15:8] Pull- high Enable
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::XDB15_8_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC XDB[7:0] Pull-Up Enable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [0] XDB[7:0] Pull- high Enable
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::XDB7_0_PullUp_Enable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
}


/**
 * @brief IDEC XDB[7:0] Pull-Up Disable
 *        
 * @verbatim
 * REG [0x0e] Pull- high control Register (PUENR)
 *            Bit [0] XDB[7:0] Pull- high Enable
 *                    0: Pull-Up Disable
 *                    1: Pull-Up Enable
 * @endverbatim
 *                         
 * @param None
 *
 * @note None
 */
void RA8889::XDB7_0_PullUp_Disable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PUENR);                     //0x0e, Pull- high control Register (PUENR)
  temp = _bus->DataRead();
  CLRB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
//
//================================================================================


//registrador PSFSR (0x0F), mostrando bit, XPDAT, função GPIO e função teclado (KOUT/KIN). Pode ser usada no cabeçalho ou Doxygen para referência rápida:

/**
 * @brief Tabela resumida do registrador PSFSR (0x0F) – PIO/Key Function Select Register
 *
 * Esta tabela mostra a correspondência entre os bits do registrador,
 * os pinos XPDAT, e suas funções possíveis como GPIO ou teclado matricial.
 *
 * Bit  | XPDAT   | GPIO   | Teclado (KOUT/KIN)
 * -----------------------------------------------
 * 7    | XPDAT[18] | GPIO-D7 | KOUT[4]
 * 6    | XPDAT[17] | GPIO-D5 | KOUT[2]
 * 5    | XPDAT[16] | GPIO-D4 | KOUT[1]
 * 4    | XPDAT[9]  | GPIO-D3 | KOUT[3]
 * 3    | XPDAT[8]  | GPIO-D2 | KIN[3]
 * 2    | XPDAT[2]  | GPIO-D6 | KIN[4]
 * 1    | XPDAT[1]  | GPIO-D1 | KIN[2]
 * 0    | XPDAT[0]  | GPIO-D0 | KIN[1]
 *
 * Observações:
 * - Bits setados em 0 → GPIO (entrada/saída digital)
 * - Bits setados em 1 → função de teclado matricial (KOUT para saída, KIN para entrada)
 * - Essa tabela consolida todas as funções XPDAT conhecidas até o momento.
 */


/**
 * @brief Configura o pino XPDAT[18] como GPIO-D7.
 *
 * @verbatim
 * REG [0x0f] PSFSR (PIO/Key Function Select Register)
 *            Bit [7] controla a função alternativa do pino XPDAT[18]:
 *                    0: GPIO-D7 (uso como porta digital normal)
 *                    1: KOUT[4] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D7:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[4]:
 *  - O pino passa a ser gerado automaticamente pelo
 *    controlador de teclado interno do RA8889.
 *  - É usado em conjunto com os registradores de key-scan.
 *
 * Essa função força o modo GPIO-D7, limpando o bit 7 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o teclado matricial não
 *       estiver em uso, liberando o pino para I/O digital.
 */
void RA8889::XPDAT18_GPIO_D7_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[18] como KOUT[4].
 *
 * @verbatim
 * REG [0x0f] PSFSR (PIO/Key Function Select Register)
 *            Bit [7] controla a função alternativa do pino XPDAT[18]:
 *                     0: GPIO-D7 (uso como porta digital normal)
 *                     1: KOUT[4] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D7:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[4]:
 *  - O pino passa a ser controlado pelo módulo interno de teclado,
 *    funcionando como linha de varredura (scan) para detecção de teclas.
 *  - Deve ser usado junto aos registradores de configuração de teclado.
 *
 * Essa função força o modo KOUT[4], setando o bit 7 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o recurso de teclado
 *       matricial do RA8889 estiver habilitado.
 */
void RA8889::XPDAT18_KOUT4_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[17] como GPIO-D5.
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [6] controla a função alternativa do pino XPDAT[17]:
 *                    0: GPIO-D5 (uso como porta digital normal)
 *                    1: KOUT[2] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D5:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[2]:
 *  - O pino passa a ser gerado automaticamente pelo
 *    controlador de teclado interno do RA8889.
 *  - É usado em conjunto com os registradores de key-scan.
 *
 * Essa função força o modo GPIO-D5, limpando o bit 6 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o teclado matricial não
 *       estiver em uso, liberando o pino para I/O digital.
 */
void RA8889::XPDAT17_GPIO_D5_Mode(void)
{
    /*
    XPDAT[17] – not scan function select
    0: GPIO-D5
    1: KOUT[2]
    */
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[17] como KOUT[2].
 *
 * @verbatim
 * REG [0x0f] PSFSR (PIO/Key Function Select Register)
 *            Bit [6] controla a função alternativa do pino XPDAT[17]:
 *                    0: GPIO-D5 (uso como porta digital normal)
 *                    1: KOUT[2] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D5:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[2]:
 *  - O pino passa a ser controlado pelo módulo interno de teclado,
 *    funcionando como linha de varredura (scan) para detecção de teclas.
 *  - Deve ser usado junto aos registradores de configuração de teclado.
 *
 * Essa função força o modo KOUT[2], setando o bit 6 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o recurso de teclado
 *       matricial do RA8889 estiver habilitado.
 */
void RA8889::XPDAT17_KOUT2_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[16] como GPIO-D4.
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [5] controla a função alternativa do pino XPDAT[16]:
 *                    0: GPIO-D4 (uso como porta digital normal)
 *                    1: KOUT[1] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D4:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[1]:
 *  - O pino passa a ser gerado automaticamente pelo
 *    controlador de teclado interno do RA8889.
 *  - É usado em conjunto com os registradores de key-scan.
 *
 * Essa função força o modo GPIO-D4, limpando o bit 5 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o teclado matricial não
 *       estiver em uso, liberando o pino para I/O digital.
 */
void RA8889::XPDAT16_GPIO_D4_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[16] como KOUT[1].
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [5] controla a função alternativa do pino XPDAT[16]:
 *                    0: GPIO-D4 (uso como porta digital normal)
 *                    1: KOUT[1] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D4:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[1]:
 *  - O pino passa a ser controlado pelo módulo interno de teclado,
 *    funcionando como linha de varredura (scan) para detecção de teclas.
 *  - Deve ser usado junto aos registradores de configuração de teclado.
 *
 * Essa função força o modo KOUT[1], setando o bit 5 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o recurso de teclado
 *       matricial do RA8889 estiver habilitado.
 */
void RA8889::XPDAT16_KOUT1_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[9] como GPIO-D3.
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [4] controla a função alternativa do pino XPDAT[9]:
 *                    0: GPIO-D3 (uso como porta digital normal)
 *                    1: KOUT[3] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D3:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[3]:
 *  - O pino passa a ser gerado automaticamente pelo
 *    controlador de teclado interno do RA8889.
 *  - É usado em conjunto com os registradores de key-scan.
 *
 * Essa função força o modo GPIO-D3, limpando o bit 4 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o teclado matricial não
 *       estiver em uso, liberando o pino para I/O digital.
 */
void RA8889::XPDAT9_GPIO_D3_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[9] como KOUT[3].
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *           Bit [4] controla a função alternativa do pino XPDAT[9]:
 *                   0: GPIO-D3 (uso como porta digital normal)
 *                   1: KOUT[3] (linha de saída do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D3:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KOUT[3]:
 *  - O pino passa a ser controlado pelo módulo interno de teclado,
 *    funcionando como linha de varredura (scan) para detecção de teclas.
 *  - Deve ser usado junto aos registradores de configuração de teclado.
 *
 * Essa função força o modo KOUT[3], setando o bit 4 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o recurso de teclado
 *       matricial do RA8889 estiver habilitado.
 */
void RA8889::XPDAT9_KOUT3_Mode(void)
{
    /*
    XPDAT[9] – not scan function select
    0: GPIO-D3
    1: KOUT[3]
    */
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[8] como GPIO-D2.
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [3] controla a função alternativa do pino XPDAT[8]:
 *                    0: GPIO-D2 (uso como porta digital normal)
 *                    1: KIN[3]  (linha de entrada do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D2:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KIN[3]:
 *  - O pino é usado pelo controlador de teclado interno como linha
 *    de entrada (Key Input) na varredura de teclas.
 *  - O valor lido será refletido nos registradores de key-scan.
 *
 * Essa função força o modo GPIO-D2, limpando o bit 3 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o teclado matricial não
 *       estiver em uso, liberando o pino para I/O digital.
 */
void RA8889::XPDAT8_GPIO_D2_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[8] como KIN[3].
 *
 * @verbatim
 * REG [0x0F] PSFSR (PIO/Key Function Select Register)
 *            Bit [3] controla a função alternativa do pino XPDAT[8]:
 *                    0: GPIO-D2 (uso como porta digital normal)
 *                    1: KIN[3]  (linha de entrada do engine de teclado matricial)
 *
 * Quando selecionado como GPIO-D2:
 *  - A direção do pino deve ser configurada em REG[0x12] (PIO_DIR).
 *  - O valor de entrada/saída é manipulado em REG[0x10] (PIO_DATA).
 *
 * Quando selecionado como KIN[3]:
 *  - O pino passa a ser controlado pelo módulo interno de teclado,
 *    funcionando como linha de entrada (Key Input).
 *  - Deve ser usado junto aos registradores de configuração e leitura
 *    de teclas.
 *
 * Essa função força o modo KIN[3], setando o bit 3 de REG[0x0F].
 * @endverbatim
 *
 * @param None
 *
 * @note Deve ser usada apenas quando o recurso de teclado
 *       matricial do RA8889 estiver habilitado.
 */
void RA8889::XPDAT8_KIN3_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[2] como GPIO-D6.
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *            Bit 2 → Seleção de função para XPDAT[2]:
 *                    0: GPIO-D6 (função de propósito geral)
 *                    1: KIN[4]  (entrada de teclado matricial)
 *
 * Esta função força o bit [2] para 0, definindo XPDAT[2] 
 * como saída/entrada digital genérica (GPIO-D6).
 * @endverbatim
 *
 * @param None
 * @note Altera apenas o bit 2 do registrador PSFSR, preservando os demais bits.
 */
void RA8889::XPDAT2_GPIO_D6_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[2] como entrada de teclado KIN[4].
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *            Bit 2 → Seleção de função para XPDAT[2]:
 *                    0: GPIO-D6 (função de propósito geral)
 *                    1: KIN[4]  (entrada de teclado matricial)
 *
 * Esta função força o bit [2] para 1, habilitando o modo de entrada 
 * de teclado KIN[4] no pino XPDAT[2].
 * @endverbatim
 *
 * @param None
 * @note Altera apenas o bit 2 do registrador PSFSR, preservando os demais bits.
 */
void RA8889::XPDAT2_KIN4_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[1] como GPIO-D1.
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *            Bit [1] → XPDAT[1]:
 *                      0: GPIO-D1 (uso como porta digital normal)
 *                      1: KIN[2]  (linha de entrada do teclado matricial)
 * @endverbatim
 *
 * @param None
 */
void RA8889::XPDAT1_GPIO_D1_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[1] como entrada de teclado KIN[2].
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *            Bit [1] XPDAT[1] –GPIO or Key function select
 *                    0: GPIO-D1
 *                    1: KIN[2]
 * @endverbatim
 *
 * @param None
 */
void RA8889::XPDAT1_KIN2_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[0] como GPIO-D0.
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *           Bit [0] XPDAT[0] –GPIO or Key function select
 *                   0: GPIO-D0
 *                   1: KIN[1]
 * @endverbatim
 *
 * @param None
 */
void RA8889::XPDAT0_GPIO_D0_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  CLRB(temp,0);
  _bus->DataWrite(temp);
}


/**
 * @brief Configura o pino XPDAT[0] como entrada de teclado KIN[1].
 *
 * @verbatim
 * REG [0x0F] PDAT for PIO/Key Function Select Register (PSFSR)
 *           Bit [0] XPDAT[0] –GPIO or Key function select
 *                   0: GPIO-D0
 *                   1: KIN[1]
 * @endverbatim
 *
 * @param None
 */
void RA8889::XPDAT0_KIN1_Mode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PSFSR);                     //0x0f, PDAT for PIO/Key Function Select Register (PSFSR)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x10] Main/PIP Window Control Register (MPWCTR)
//
//================================================================================


/**
 * @brief Habilita o Picture-in-Picure (PIP 1) do display
 *        PIP: Para entender, é uma pequena área de iamgem (janelinha) sobre 
 *        outra área de imagem semelhante a uma janelinha da TV no canto 
 *        esquerdo, enquanto vai zapeando os canais nas janelas principal.
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [7] PIP 1 window Enable/Disable
 *                0b0: PIP 1 window disable.
 *                0b1: PIP 1 window enable
 *
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
void RA8889::PIP1_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  b ? SETB(temp,7) : CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Habilita o Picture-in-Picure (PIP 2) do display
 *        PIP: Para entender, é uma pequena área de iamgem (janelinha) sobre 
 *        outra área de imagem semelhante a uma janelinha da TV no canto 
 *        esquerdo, enquanto vai zapeando os canais nas janelas principal.
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [6] PIP 2 window Enable/Disable
 *                0b0: PIP 2 window disable.
 *                0b1: PIP 2 window enable
 *
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
void RA8889::PIP2_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  b ? SETB(temp,6) : CLRB(temp,6);
  _bus->DataWrite(temp);
}


/**
 * @brief Select Configure PIP 1 or 2 Window’s parameters
 *
 * @verbatim
 * REG[10h] Main/PIP Window Control Register (MPWCTR)
 * bit [4] Select Configure PIP 1 or 2 Window’s parameters
 *         PIP window’s parameter including Color Depth, starting address,
 *         image width, display coordinates, window coordinates, window
 *         width, and window height.
 *         0b0: To configure PIP 1’s parameters.
 *         0b1: To configure PIP 2’s parameters.
 * @endverbatim
 * 
 * @param ePIPSelect::PIP1 : To configure PIP 1’s parameters
 * @param ePIPSelect::PIP2 : To configure PIP 2’s parameters.
 *
 * @note None
 */
void RA8889::PIP_Select_Parameter(ePIPSelect pip)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  temp |= static_cast<uint8_t>(pip);           //Converte enum para uint8_t
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona Janela de imagem principal para 8bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Window Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void RA8889::Select_MainWindow_8bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  CLRB(temp,3);                                //Reset bit 3
  CLRB(temp,2);                                //Reset bit 2
  _bus->DataWrite(temp);                         //Set main windows image to 8bpp
}


/** OK
 * @brief Seleciona Janela de imagem principal para 16bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void RA8889::Select_MainWindow_16bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  CLRB(temp,3);                                //Reset bit 3
  SETB(temp,2);                                //Set bit 2
  _bus->DataWrite(temp);                         //Set main windows image to 16bpp
}


/**
 * @brief Seleciona Janela de imagem principal para 24bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Window Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void RA8889::Select_MainWindow_24bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  SETB(temp,3);                                //Set bit 3
  CLRB(temp,2);                                //Reset bit 2
  _bus->DataWrite(temp);                         //Set main windows image to 16bpp
}


/**
 * @brief Controlar o sinal de modo de sincronização do painel LCD habilitando
 *        o modo de sincronização XVSYNC, XHSYNC, XDE
 *
 * @verbatim
 * REG [0x10] Main/PIP Window Control Register (MPWCTR)
 * bit [0] To Control panel’s synchronous signals
 *         0b0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
 *         0b1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::Select_LCD_SyncMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);                         //Enable XVSYNC, XHSYNC, XDE
}


/**
 * @brief Controlar o sinal de modo de sincronização do painel LCD habilitando
 *        apenas o modo de sincronização XDE
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [0] To Control panel’s synchronous signals
 *                0b0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
 *                0b1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
 *
 * @param None
 *
 */
void RA8889::Select_LCD_DEMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = _bus->DataRead();
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);                         //Only XDE enable, XVSYNC & XHSYNC in idle state
}


//================================================================================
//
// [0x11] PIP Window Color Depth Setting (PIPCDEP)
//
//================================================================================


/**
 * @brief PIP 1 Window Color Depth Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [3-2] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP1_Window_ColorDepth(eColorDepthBPP bpp)
{
  uint8_t temp;
  uint8_t bit;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  temp &= ~(cSetb3 | cSetb2);                  //Reset bit 3 e 2
  bit = static_cast<uint8_t>(bpp);             //
  bit = (bit >> 3)-1;                          //transforma 8,16,24 em 0, 1, 2
  bit = bit << 2;                              //posiciona no bit 3 e 2
  temp |= bit;                                 //cobina os bits
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 1 Window Color Depth 8 bpp Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [3-2] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP1_Window_ColorDepth_8bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  CLRB(temp,3);                                //Reset bit 
  CLRB(temp,2);                                //Reset bit 2
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 1 Window Color Depth 16 bpp Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [3-2] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP1_Window_ColorDepth_16bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  CLRB(temp,3);                                //Reset bit 
  SETB(temp,2);                                //Set bit 2
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 1 Window Color Depth 24 bpp Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [3-2] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP1_Window_ColorDepth_24bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  SETB(temp,3);                                //Set bit 3
  CLRB(temp,2);                                //Reset bit 2 
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 2 Window Color Depth Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [1-0] PIP 2 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP2_Window_ColorDepth(eColorDepthBPP bpp)
{
  uint8_t temp;
  uint8_t bit;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  temp &= ~(cSetb1 | cSetb0);                  //Reset bit 1 e 0
  bit = static_cast<uint8_t>(bpp);             //
  temp |= (bit >> 3)-1;                        //transforma 8,16,24 em 0, 1, 2
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 2 Window Color Depth 8 bpp
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [1-0] PIP 2 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP2_Window_ColorDepth_8bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  CLRB(temp,1);                                //Reset bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 2 Window Color Depth 16 bpp
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [1-0] PIP 2 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP2_Window_ColorDepth_16bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  CLRB(temp,1);                                //Reset bit 1
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);                         
}


/**
 * @brief PIP 2 Window Color Depth 24 bpp
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [1-0] PIP 2 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void RA8889::PIP2_Window_ColorDepth_24bpp(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = _bus->DataRead();
  SETB(temp,1);                                //Set bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);                         
}


//================================================================================
//
// [0x12] Display Configuration Register (DPCR)
//
//================================================================================


/**
 * @brief Panel Scan Clock PCLK Rising
 *
 * @verbatim
 * PCLK Inversion:
 * bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *         0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 * @endverbatim
 *
 * @param None
 * 
 * @note None
 */
void RA8889::PCLK_Rising(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,7);                                //Reset bit 7 
  _bus->DataWrite(temp);
}


/**
 * @brief Panel Scan Clock PCLK Failing
 *
 * @verbatim
 * PCLK Inversion:
 * bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *         0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 * @endverbatim
 *
 * @param None
 * 
 * @note None
 */
void RA8889::PCLK_Falling(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/** OK
 * @brief Panel Scan Clock PCLK Edge Type
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * PCLK Inversion:
 * bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *         0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 * @endverbatim
 *
 * @param None
 * 
 * @note None
 */
void RA8889::PCLK_EdgeType(ePCLKEdge edge)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,7);                                //Reset bit 7
  temp |= static_cast<uint8_t>(edge);          
  _bus->DataWrite(temp);
}


/**
 * @brief Liga o display
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [6] Display ON/OFF
 *         0b0: Display Off.
 *         0b1: isplay On.
 * @endverbatim
 *
 * @param on: true, liga display, false: desliga display 
 *
 * @note None
 *       
 */
void RA8889::DisplayOn(bool on)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  on ? SETB(temp,6) : CLRB(temp,6);            //Set/Reset bit 6
  _bus->DataWrite(temp);
  CoreTask_WaitReady();
}

/**
 * @brief Display Test Color Bar On/Off
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [5] Display Test Color Bar
 *     0b: Disable.
 *     1b: Enable.
 * @endverbatim
 *
 * @param b
 *
 * @note None
 *       
 */
void RA8889::DisplayTestBar(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);             //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();
  b ? SETB(temp,5) : CLRB(temp,5);    //Bit 5, set/reset
  _bus->DataWrite(temp);
}


/**
 * @brief Horizontal Scan Direction Left to Right
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [4] 0b0: Horizontal Scan Left to Right
 *         0b1: Horizontal Scan Right to Left
 * @endverbatim
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void RA8889::HScanDirection_LeftToRight (void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,4);                                //reset bit 4
  _bus->DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Horizontal Scan Direction Left to Right
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 *        bit [4] 0b0: Horizontal Scan Left to Right
 *                0b1: Horizontal Scan Right to Left
 * @endverbatim
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void RA8889::HScanDirection_RightToLeft (void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Horizontal Scan Direction Left to Right / Right to Left
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [4] 0b0: Horizontal Scan Left to Right
 *         0b1: Horizontal Scan Right to Left
 * @endverbatim
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void RA8889::HorizontalScanDirection (HSCANDir direction)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,4);                                //Reset bit 4
  temp |= static_cast<uint8_t>(direction);     //Define o destino
  _bus->DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Vertical Scan Direction Top to Bottom
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 *        bit [3] 0b0: Vertical Scan from top to bottom
 *                0b1: Vertical Scan from bottom to top
 * @endverbatim
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 *       
 */
void RA8889::VScanDirection_TopToBottom(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,3);                                //Reset bit 3
  _bus->DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Vertical Scan Direction Bottom to Top
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [3] 0b0: Vertical Scan from top to bottom
 *         0b1: Vertical Scan from bottom to top
 * @endverbatim
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 *       
 */
void RA8889::VScanDirection_BottomToTop(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,3);                                //Set bit 3
  _bus->DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Vertical Scan Direction Top to Bottom / Bottom to Top
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * bit [3] 0b0: Vertical Scan from top to bottom
 *         0b1: Vertical Scan from bottom to top
 * @endverbatim
 *
 * @param dir: VSCANDir::TopToBottom
 *             VSCANDir::BottomToTop
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 */
void RA8889::VerticalScanDirection(VSCANDir direction)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,3);                                //Reset bit 3
  temp |= static_cast<uint8_t>(direction);     //Define o destino
  _bus->DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Set the type of parallel data output sequence in Color RGB format
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * Parallel XPDAT[23:0] Output Sequence:
 * bit [2-0] 0b000 : RGB
 *           0b001 : RBG
 *           0b010 : GRB
 *           0b011 : GBR
 *           0b100 : BRG
 *           0b101 : BGR
 *           0b110 : Gray
 *           0b111 : Send out idle state (all 0 or 1, black or white color).
 * @endverbatim
 *
 * @param None
 * 
 * @note None
 */
void RA8889::PDATA_ColorRGB(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  temp &= ~(cSetb2 | cSetb1 | cSetb0);         //Reset bit 2, 1 e 0, format RGB =0b000 
  _bus->DataWrite(temp);
}


/**
 * @brief Set the type of parallel data output sequence in RGB format
 *
 * @verbatim
 * REG[0x12] Display Configuration Register (DPCR)
 * Parallel XPDAT[23:0] Output Sequence:
 * bit [2-0] 0b000 : RGB
 *           0b001 : RBG
 *           0b010 : GRB
 *           0b011 : GBR
 *           0b100 : BRG
 *           0b101 : BGR
 *           0b110 : Gray
 *           0b111 : Send out idle state (all 0 or 1, black or white color).
 * @endverbatim
 *
 * @param fmt: Formato de cor (PDATAColorFmt)
 * 
 * @note None
 */
void RA8889::PDATA_ColorFmt(ePDATAColorFmt fmt)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  temp &= ~(cSetb2 | cSetb1 | cSetb0);         //Reset bit 2, 1 e 0 
  temp |= static_cast<uint8_t>(fmt);
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x13] Panel scan Clock and Data Setting Register (PCSR)
//
//================================================================================


/**
 * @brief HSYNC Polarity Active Low
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 * 
 * @note None
 */
void RA8889::HSYNC_PolarityLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief HSYNC Polarity Active High
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 * 
 * @note None
 */
void RA8889::HSYNC_PolarityHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief HSYNC Polarity
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param HSYNCPolarity::Low
 *        HSYNCPolarity::High
 *
 * @note None
 */
void RA8889::HSYNC_Polarity(eHSYNCPolarity val)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,7);                                //Reset bit 7
  temp |= static_cast<uint8_t>(val);
  _bus->DataWrite(temp);
}


/** OK
 * @brief VSYNC Polarity Active Low
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 *
 * @note None
 */
void RA8889::VSYNC_PolarityLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,6);                                //Reset bit 6
  _bus->DataWrite(temp);
}


/** OK
 * @brief VSYNC Polarity Active High
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 *
 * @note None
 */
void RA8889::VSYNC_PolarityHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,6);                                //Set bit 6
  _bus->DataWrite(temp);
}


/** OK
 * @brief VSYNC Polarity
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param VSYNCPolarity::Low
 *        VSYNCPolarity::High
 *
 * @note None
 */
void RA8889::VSYNC_Polarity(eVSYNCPolarity val)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,6);                                //Reset bit 6
  temp |= static_cast<uint8_t>(val);
  _bus->DataWrite(temp);
}


/**
 * @brief Data Enable Polarity Low Active
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param None
 *
 * @note None
 */
void RA8889::DE_PolarityLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  SETB(temp,5);                                //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Data Enable Polarity High
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param None
 *
 * @note None
 */
void RA8889::DE_PolarityHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,5);                                //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Data Enable Polarity
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param DataEnable::High
 *        DataEnable::Low
 *
 * @note None
 */
void RA8889::DE_Polarity(eDEPolarity val)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();                       //Lê valor atual do registrador
  CLRB(temp,5);                                //Reset bit 5
  temp |= static_cast<uint8_t>(val);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [4] XDE IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “DE” output is low.
 *     1 : Pin “DE” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::DE_IdleStateLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [4] XDE IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “DE” output is low.
 *     1 : Pin “DE” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::DE_IdleStateHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  SETB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [3] XPCLK IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “PCLK” output is low.
 *     1 : Pin “PCLK” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::PCLK_IdleStateLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  CLRB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [3] XPCLK IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “PCLK” output is low.
 *     1 : Pin “PCLK” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::PCLK_IdleStateHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  SETB(temp,3);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [2] XPDAT IDLE STATE
 *     (Must use with reg[12h] bit2-0 Parallel XPDAT[23:0] Output Sequence to send out idle state)
 *     0 : Pins “PDAT[23:0]” output is low.
 *     1 : Pins “PDAT[23:0]” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::PDAT_IdleStateLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [2] XPDAT IDLE STATE
 *     (Must use with reg[12h] bit2-0 Parallel XPDAT[23:0] Output Sequence to send out idle state)
 *     0 : Pins “PDAT[23:0]” output is low.
 *     1 : Pins “PDAT[23:0]” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::PDAT_IdleStateHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [1] XHSYNC IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “HSYNC” output is low.
 *     1 : Pin “HSYNC” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::HSYNC_IdleStateLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [1] XHSYNC IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “HSYNC” output is low.
 *     1 : Pin “HSYNC” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::HSYNC_IdleStateHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  SETB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [0] XVSYNC IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “VSYNC” output is low.
 *     1 : Pin “VSYNC” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::VSYNC_IdleStateLow(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  CLRB(temp,0);
  _bus->DataWrite(temp);
}


/**
 * @brief 
 *        
 * @verbatim
 * REG [0x13] Panel scan Clock and Data Setting Register (PCSR)
 * bit [0] XVSYNC IDLE STATE
 *     (in power saving mode or DISPLAY OFF )
 *     0 : Pin “VSYNC” output is low.
 *     1 : Pin “VSYNC” output is high.
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::VSYNC_IdleStateHigh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x14] Horizontal Display Width Register (HDWR)
// [0x15] Horizontal Display Width Fine Tune Register (HDWFTR)
// [0x1a] Vertical Display Height Register 0(VDHR0)
// [0x1b] Vertical Display Height Register 1 (VDHR1)
//
//================================================================================


/** OK
 * @brief Define a resolução vertical e horizontal do display
 *        
 * @verbatim
 * REG [0x14] Horizontal Display Width Setting (HDWR) Bit[7:0]
 *
 * REG [0x15] Horizontal Display Width Fine Tuning (HDWFTR) [3:0]
 * The register specifies the LCD panel horizontal display width in
 * the unit of 8 pixels resolution.
 * Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR
 *
 * REG [0x1A] Vertical Display Height Bit[7:0] (low register) 
 * Vertical Display Height(Line) = VDHR + 1
 * 
 * REG [0x1B] Vertical Display Height Bit[10:8] (high register)
 * Vertical Display Height(Line) = VDHR + 1
 * @endverbatim
 *
 * @param wx Width horizontal
 *        hy Height vertical
 *
 * @note None
 */
void RA8889::HorizontalWidth_VerticalHeight(uint16_t wx, uint16_t hy)
{
  uint8_t temp;
  
  if (wx < 8) {
    _bus->CmdWrite(REG_HDWR);                    //0x14, Horizontal Display Width Register (HDWR)
    _bus->DataWrite(0x00);                       //
    
    _bus->CmdWrite(REG_HDWFTR);                  //0x15, Horizontal Display Width Fine Tune Register (HDWFTR)
    _bus->DataWrite(wx);                         //
    
    temp = hy - 1;
    _bus->CmdWrite(REG_VDHR0);                   //0x1a, Vertical Display Height Register 0(VDHR0)
    _bus->DataWrite(temp);                       //Store bit [7-0]
    
    temp = (hy - 1) >> 8;                      //
    _bus->CmdWrite(REG_VDHR1);                   //0x1b, Vertical Display Height Register 1 (VDHR1)
    _bus->DataWrite(temp);                       //Store bit [2-0]
  } else {
    temp = (wx / 8) - 1;                       //exemplo: temp = (800/8)-1 = 99
    _bus->CmdWrite(REG_HDWR);                    //0x14, Horizontal Display Width Register (HDWR)
    _bus->DataWrite(temp);                       //
    
    temp = wx % 8;                             //temp = 800 % 8 = 0
    _bus->CmdWrite(REG_HDWFTR);                  //0x15, Horizontal Display Width Fine Tune Register (HDWFTR)
    _bus->DataWrite(temp);                       //
    
    temp = hy - 1;                             //temp = 480 - 1 = 479 = 0x01df -> low(0xdf) 
    _bus->CmdWrite(REG_VDHR0);                   //0x1a, Vertical Display Height Register 0(VDHR0)
    _bus->DataWrite(temp);                       //Store bit [7-0]
    
    temp = (hy - 1) >> 8;                      //temp = 480 - 1 = 479 = 0x01df -> high(0x01)
    _bus->CmdWrite(REG_VDHR1);                   //0x1b, Vertical Display Height Register 1 (VDHR1)
    _bus->DataWrite(temp);                       //Store bit [2-0]
  }
}


//================================================================================
//
// [0x16] Horizontal Non-Display Period Register (HNDR)
// [0x17] Horizontal Non-Display Period Fine Tune Register (HNDFTR)
//
//================================================================================


/** OK
 * @brief Define o Período Horizontal de Não Exibição do LCD
 *        
 * @verbatim
 * REG [0x16] Horizontal Non-Display Period(HNDR) Bit[4:0]
 * This register specifies the horizontal non-display period. Also called "back porch".
 * Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
 *
 * REG [0x17] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
 * This register specifies the fine tuning for horizontal non-display
 * period; it is used to support the SYNC mode panel. Each level of
 * this modulation is 1-pixel.
 * Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
 * @endverbatim
 *
 * @param hbpd HS Back Porch (Blanking)
 *
 * @note None
 */
void RA8889::Horizontal_NonDisplay(uint16_t hbpd)
{
  uint8_t temp;
  if (hbpd < 8) {
    _bus->CmdWrite(REG_HNDR);                    //0x16, Horizontal Non-Display Period Register (HNDR)
    _bus->DataWrite(0x00);                       //
    
    _bus->CmdWrite(REG_HNDFTR);                  //0x17, Horizontal Non-Display Period Fine Tune Register (HNDFTR)
    _bus->DataWrite(hbpd);
  } else {
    temp = (hbpd / 8) - 1;                     // 
    _bus->CmdWrite(REG_HNDR);                    //0x16, Horizontal Non-Display Period Register (HNDR)
    _bus->DataWrite(temp);
    
    temp = hbpd % 8;                           //
    _bus->CmdWrite(REG_HNDFTR);                  //0x17, Horizontal Non-Display Period Fine Tune Register (HNDFTR)
    _bus->DataWrite(temp);
  }
}


//================================================================================
//
// [0x18] HSYNC Start Position Register (HSTR)
// [0x19] HSYNC Pulse Width Register (HPWR)
//
//================================================================================


/** OK
 * @brief 
 *        
 * @verbatim
 * REG [0x18] HSYNC Start Position Register (HSTR)
 * bit [4:0] HSYNC Start Position
 *           The starting position from the end of display area to the
 *           beginning of HSYNC. Each level of this modulation is 8-pixel.
 *           Also called front porch.
 *           HSYNC Start Position(pixels) = (HSTR + 1) * 8
 * @endverbatim
 *
 * @param hfpd: HS Front Porch
 *
 * @note None
 */
void RA8889::HSYNC_StartPosition(uint16_t hfpd)
{
  uint8_t temp;
  if (hfpd < 8) {
    _bus->CmdWrite(REG_HSTR);                    //0x18, HSYNC Start Position Register (HSTR)
    _bus->DataWrite(0x00);                       //
  } else {
    temp = (hfpd / 8) - 1;                     //
    _bus->CmdWrite(REG_HSTR);                    //0x18, HSYNC Start Position Register (HSTR)
    _bus->DataWrite(temp);                       //
  }
}


/** OK
 * @brief 
 *        
 * @verbatim
 * REG [0x19] HSYNC Pulse Width Register (HPWR) 
 * bit [4:0] HSYNC Pulse Width(HPW)
 *           The period width of HSYNC.
 *           HSYNC Pulse Width(pixels) = (HPW + 1) x 8
 * @endverbatim
 *
 * @param hspw: HS Pulse Width
 *
 * @note None
 */
void RA8889::HSYNC_PulseWidth(uint16_t hspw)
{
  uint16_t temp;
  if(hspw < 8) {
    _bus->CmdWrite(REG_HPWR);                    //0x19, HSYNC Pulse Width Register (HPWR)
    _bus->DataWrite(0x00);                       //
  } else {
    temp = (hspw / 8) - 1;
    _bus->CmdWrite(REG_HPWR);                    //0x19, HSYNC Pulse Width Register (HPWR)
    _bus->DataWrite(temp);                       //
  }
}


//================================================================================
//
// [0x1c] Vertical Non-Display Period Register 0(VNDR0)
// [0x1d] Vertical Non-Display Period Register 1(VNDR1)
//
//================================================================================


/** OK
 * @brief Define o Período Vertical de Não Exibição do LCD
 *        
 * @verbatim
 * REG [0x1c] Vertical Non-Display Period Bit[7:0]
 * Vertical Non-Display Period(Line) = (VNDR + 1)
 *
 * REG [0x1d] Vertical Non-Display Period Bit[9:8]
 * Vertical Non-Display Period(Line) = (VNDR + 1)
 * @endverbatim
 *
 * @param vbpd VS Back Porch (Blanking)
 *
 * @note None
 */
void RA8889::Vertical_NonDisplay(uint16_t vbpd)
{
  uint16_t temp;
  temp = vbpd - 1;
  _bus->CmdWrite(REG_VNDR0);                     //0x1c, Vertical Non-Display Period Register 0(VNDR0)       
  _bus->DataWrite(temp);                         //
  _bus->CmdWrite(REG_VNDR1);                     //0x1d, Vertical Non-Display Period Register 1(VNDR1)
  _bus->DataWrite(temp >> 8);	                   //
}


//================================================================================
//
// [0x1e] VSYNC Start Position Register (VSTR)
//
//================================================================================


/** OK
 * @brief 
 *        
 * @verbatim
 * REG [18h] HSYNC Start Position[4:0]
 * The starting position from the end of display area to the
 * beginning of HSYNC. Each level of this modulation is 8-pixel.
 * Also called front porch.
 * HSYNC Start Position(pixels) = (HSTR + 1) * 8
 * @endverbatim
 *
 * @param vfpd: VS Front Porch
 *
 * @note None
 */
void RA8889::VSYNC_StartPosition(uint16_t vfpd)
{
    uint8_t temp;
    temp = vfpd - 1;
    _bus->CmdWrite(REG_VSTR);                    //0x1e, VSYNC Start Position Register (VSTR)
    _bus->DataWrite(temp);
}


//================================================================================
//
// [0x1f] VSYNC Pulse Width Register (VPWR)
//
//================================================================================


/** OK
 * @brief 
 *        
 * @verbatim
 * REG [0x1f] VSYNC Pulse Width[5:0]
 * The pulse width of VSYNC in lines.
 * VSYNC Pulse Width(Line) = (VPWR + 1)
 * @endverbatim
 *
 * @param vspw: VS Pulse Width
 *
 * @note None
 */
void RA8889::VSYNC_PulseWidth(uint8_t vspw)
{
  uint8_t temp;
  temp = vspw - 1;
  _bus->CmdWrite(REG_VPWR);                //0x1f, VSYNC Pulse Width Register (VPWR)
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x20] Main Image Start Address 0 (MISA0)
// [0x21] Main Image Start Address 1 (MISA1)
// [0x22] Main Image Start Address 2 (MISA2)
// [0x23] Main Image Start Address 3 (MISA3)
//
//================================================================================


/**
 * @brief Page Layer Start Address
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param layer: camada 0..MAX_LAYER-1 (layer 0, layer 1, layer 2....)
 *
 * @note  Page (image buffer) Configure: 
 *        The maximum number of pages is based on SDRAM capacity and color 
 *        depth and width and height of one page.
 *        For example we used 128Mbit SDRAM that capacity =  16Mbyte
 *        The SDRAM is divided into several image buffers and the maximum 
 *        number of image buffers is limited by the memory size. For example : 
 *        page_size = 800*600*2byte(16bpp) = 960000byte, 
 *        maximum number = 16/0.96 = 16.6
 *        
 *        Vertical mulit page application
 *        
 *        Se o valor for alem do numero de paginas permitida pelo display, ira 
 *        retornar o endreço da primeira pagina 0, ou seja endreço 0
 *
 * @return Page layer start address
 */
uint32_t RA8889::LayerStartAddr(uint8_t layer)
{
  if (layer > MAX_LAYER-1) return 0;
  return _displaywidth * _displayheight * (_bpp / 8) * layer;   //ex. 800x480 * (16 (16bpp)/8) * 1 = 768000 = 0xbb800
  DEBUG_PRINT("LayerStartAddr, _bpp: ",_bpp,true,true);    //Debug
}


/**
 * @brief Main Image Start Address
 *        
 * @verbatim
 * REG [0x20] Main Image Start Address[7:2]   : Deve ser divisível por 4. O bit [1:0] está vinculado a “0” internamente.
 * REG [0x21] Main Image Start Address[15:8]
 * REG [0x22] Main Image Start Address [23:16]
 * REG [0x23] Main Image Start Address [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */
void RA8889::MainImage_StartAddress(uint32_t addr)
{
  _bus->RegisterWrite(REG_MISA0, addr);              //0x20, Main Image Start Address 0 (MISA0)
  _bus->RegisterWrite(REG_MISA1, addr >> 8);         //0x21, Main Image Start Address 1 (MISA1)
  _bus->RegisterWrite(REG_MISA2, addr >> 16);        //0x22, Main Image Start Address 2 (MISA2)
  _bus->RegisterWrite(REG_MISA3, addr >> 24);	       //0x23, Main Image Start Address 3 (MISA3)
}


//================================================================================
//
// [0x24] Main Image Width 0 (MIW0)
// [0x25] Main Image Width 1 (MIW1)
//
//================================================================================


/** OK
 * @brief Main Image Width
 *
 * @verbatim
 * REG [0x24] Main Image Width [7:0]
 * REG [0x25] Main Image Width [12:8]
 *
 * Unit: Pixel.
 * It must be divisible by 4. MIW Bit [1:0] tie to “0” internally.
 * The value is physical pixel number.
 * Maximum value is 8188 pixels
 * @endverbatim
 *
 * @param wx
 *
 * @note None
 */
void RA8889::MainImage_Width(uint16_t Wx)
{
  _bus->RegisterWrite(REG_MIW0, Wx);                 //0x24, Main Image Width 0 (MIW0) 
  _bus->RegisterWrite(REG_MIW1, Wx >> 8);            //0x25, Main Image Width 1 (MIW1)
}


//================================================================================
//
// PAGE 0
// [0x26] Main Window Upper-Left corner X-coordinates 0 (MWULX0)
// [0x27] Main Window Upper-Left corner X-coordinates 1 (MWULX1)
// [0x28] Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
// [0x29] Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
//
//================================================================================


/** OK
 * @brief Main Windows Start Position
 *
 * @verbatim
 * REG [0x26] Main Window Upper-Left corner X-coordinates 0 (MWULX0)
 * Main Window Upper-Left corner X-coordination [7:0] 
 *
 * REG [0x27] Main Window Upper-Left corner X-coordinates 1 (MWULX1)
 * Main Window Upper-Left corner X-coordination [12:8]  
 * 
 * Reference Main Image coordinates.
 * It must be divisible by 4. MWULX Bit [1:0] tie to "0" internally.
 * X-axis coordination plus Horizontal display width cannot large 
 * than 8188.
 *
 * REG [0x28] Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
 * Main Window Upper-Left corner Y-coordination [7:0]
 *
 * REG [0x29] Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
 * Main Window Upper-Left corner Y-coordination [12:8]
 * 
 * Reference Main Image coordinates.
 * Range is between 0 and 8191.
 * @endverbatim
 *
 * @param wx, hy : coordinate (x, y) in pixel
 *
 * @note None
 */
void RA8889::MainWindow_StartXY(uint16_t wx, uint16_t hy)  
{
  _bus->RegisterWrite(REG_MWULX0, wx);               //0x026, Main Window Upper-Left corner X-coordinates 0 (MWULX0)
  _bus->RegisterWrite(REG_MWULX1, wx >> 8);          //0x027, Main Window Upper-Left corner X-coordinates 1 (MWULX1)
  _bus->RegisterWrite(REG_MWULY0, hy);               //0x028, Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
  _bus->RegisterWrite(REG_MWULY1, hy >> 8);          //0x029, Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
}


//================================================================================
//
// PAGE 0
// [0x2A] PIP 1 or 2 Window Display Upper-Left corner X-coordinates 0 (PWDULX0)
// [0x2B] PIP 1 or 2 Window Display Upper-Left corner X-coordinates 1 (PWDULX1)
// [0x2C] PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 0 (PWDULY0)
// [0x2D] PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 1 (PWDULY1)
//
//================================================================================


/**
 * @brief Define a posição inicial da janela PIP (Picture-in-Picture).
 *
 * Esta função configura as coordenadas do canto superior esquerdo
 * da janela PIP1 ou PIP2, em relação à janela principal do display.
 * Os valores fornecidos em pixels são escritos nos registradores
 * correspondentes (0x2A a 0x2D).
 *
 * PIP Window Display Upper-Left corner X-coordinate [12:8][7:0]
 * Reference Main Window coordinate.
 * Unit: Pixel
 * It must be divisible by 4. PWDULX Bit [1:0] tie to “0” internally.
 * X-axis coordinate should less than horizontal display width.
 * According to bit of Select Configure PIP 1 or 2 Window’s parameters.
 * Function bit will be configured for relative PIP window.
 *
 * PIP Window Display Upper-Left corner Y-coordinate [12:8][7:0]
 * Reference Main Window coordinate.
 * Unit: Pixel
 * Y-axis coordinate should less than vertical display height.
 * According to bit of Select Configure PIP 1 or 2 Window’s parameters.
 * Function bit will be configured for relative PIP window.
 *  
 * @verbatim
 * REG [0x2a] PIP 1 or 2 Window Display Upper-Left corner X-coordinates 0 (PWDULX0)
 * bit [7-2] PIP Window Display Upper-Left corner X-coordinates [7:2]
 *           Reference Main Window coordinates.
 *           Unit: Pixel
 *           It must be divisible by 4. PWDULX Bit [1:0] ties to “0” internally.
 *           X-axis coordinates should be less than horizontal display width.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 *
 * REG [0x2b] PIP 1 or 2 Window Display Upper-Left corner X-coordinates 1 (PWDULX1)
 * bit [4-0] PIP Window Display Upper-Left corner X-coordinates [12:8]
 *           Reference Main Window coordinates.
 *           Unit: Pixel
 *           It must be divisible by 4. PWDULX Bit [1:0] ties to “0” internally.
 *           X-axis coordinates should be less than horizontal display width.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x2c] PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 0 (PWDULY0)
 * bit [7-0] PIP Window Display Upper-Left corner Y-coordinates [7:0]
 *           Reference Main Window coordinates.
 *           Unit: Pixel
 *           Y-axis coordinates should be less than vertical display height.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x2d] PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 1 (PWDULY1)
 * bit [4-0] PIP Window Display Upper-Left corner Y-coordinates [12:8]
 *           Reference Main Window coordinates.
 *           Unit: Pixel
 *           Y-axis coordinates should be less than vertical display height.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * @endverbatim
 *
 * @param Wx Coordenada X inicial da janela PIP (em pixels).
 * @param Hy Coordenada Y inicial da janela PIP (em pixels).
 *
 * @note Aplica-se à configuração da PIP 1 ou 2, de acordo com os
 *       bits de seleção ativos no registrador de controle PIP.
 */
void RA8889::PIP_Display_StartXY(uint16_t Wx, uint16_t Hy)
{
    _bus->CmdWrite(REG_PWDULX0);                 //0x2a, PIP 1 or 2 Window Display Upper-Left corner X-coordinates 0 (PWDULX0)
    _bus->DataWrite(Wx);
    _bus->CmdWrite(REG_PWDULX1);                 //0x2b, PIP 1 or 2 Window Display Upper-Left corner X-coordinates 1 (PWDULX1)
    _bus->DataWrite(Wx >> 8);
    _bus->CmdWrite(REG_PWDULY0);                 //0x2c, PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 0 (PWDULY0)
    _bus->DataWrite(Hy);
    _bus->CmdWrite(REG_PWDULY1);                 //0x2d, PIP 1 or 2 Window Display Upper-Left corner Y-coordinates 1 (PWDULY1)
    _bus->DataWrite(Hy >> 8);
}


//================================================================================
//
// PAGE 0
// [0x2E] PIP 1 or 2 Image Start Address 0 (PISA0)
// [0x2F] PIP 1 or 2 Image Start Address 1 (PISA1)
// [0x30] PIP 1 or 2 Image Start Address 2 (PISA2)
// [0x31] PIP 1 or 2 Image Start Address 3 (PISA3)
//
//================================================================================


/**
 * @brief Define o endereço inicial da imagem para a janela PIP (Picture-in-Picture).
 *
 * Esta função configura os registradores PISA0–PISA3, que juntos formam o 
 * endereço inicial de 32 bits na memória de vídeo (GRAM) de onde o conteúdo 
 * da janela PIP 1 ou 2 será lido.
 *
 * @verbatim
 * REG [0x2e] PIP 1 or 2 Image Start Address 0 (PISA0)
 * bit [7-2] PIP Image Start Address[7:2]
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 *           It must be divisible by 4. Bit [1:0] tie to “0” internally.
 *
 * REG [0x2f] PIP 1 or 2 Image Start Address 1 (PISA1)
 * bit [7-0] PIP Image Start Address [15:8]
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x30] PIP 1 or 2 Image Start Address 2 (PISA2)
 * bit [7-0] PIP Image Start Address [23:16]
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x31] PIP 1 or 2 Image Start Address 3 (PISA3)
 * bit [4-0] PIP Image Start Address [31:24]
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * Observação:
 *   O endereço deve estar alinhado em múltiplos de 4 bytes, 
 *   pois os dois bits menos significativos são ignorados.
 * @endverbatim
 *
 * @param addr Endereço inicial de 32 bits na memória de vídeo (GRAM)
 *             onde a imagem da janela PIP será armazenada.
 *
 * @note Essa configuração é relativa à janela PIP selecionada. 
 *       É necessário configurar previamente qual PIP (1 ou 2) será afetado.
  */
void RA8889::PIP_Image_StartAddress(uint32_t addr)
{
  _bus->CmdWrite(REG_PISA0);             //0x2e, PIP 1 or 2 Image Start Address 0 (PISA0)
  _bus->DataWrite(addr);
  _bus->CmdWrite(REG_PISA1);             //0x2f, PIP 1 or 2 Image Start Address 1 (PISA1)
  _bus->DataWrite(addr >> 8);
  _bus->CmdWrite(REG_PISA2);             //0x30, PIP 1 or 2 Image Start Address 2 (PISA2)
  _bus->DataWrite(addr >> 16);
  _bus->CmdWrite(REG_PISA3);             //0x31, PIP 1 or 2 Image Start Address 3 (PISA3)
  _bus->DataWrite(addr >> 24);
}


//================================================================================
//
// PAGE 1
// [0x2E] AVI shadow pip start address 0 (avi_spip_sadr0)
// [0x2F] AVI shadow pip start address 1 (avi_spip_sadr1)
// [0x30] AVI shadow pip start address 2 (avi_spip_sadr2)
// [0x31] AVI shadow pip start address 3 (avi_spip_sadr3)
//
//================================================================================


/**
 * @brief AVI Shadow PIP (Picture-in-Picture) Start Address
 *
 * @verbatim
 * PAGE 1 REG [0x2e] AVI shadow pip start address 0 (avi_spip_sadr0)
 * bit [7-2] memory start address [7:2] for shadow image
 * bit [1-0] Fix at 0
 *
 * PAGE 1 REG [0x2f] AVI shadow pip start address 1 (avi_spip_sadr1)
 * bit [7-0] memory start address [15:8] for shadow image
 *
 * PAGE 1 REG [0x30] AVI shadow pip start address 2 (avi_spip_sadr2)
 * bit [7-0] memory start address [23:16] for shadow image
 *
 * PAGE 1 REG [0x31] AVI shadow pip start address 3 (avi_spip_sadr3)
 * bit [7-0] memory start address [31:24] for shadow image
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */
void RA8889::AVI_ShadowPIP_StartAddress(uint32_t addr)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_AVI_SPIP_SADR0);            //0x2e, page 1, AVI shadow pip start address 0 (avi_spip_sadr0)
  _bus->DataWrite(addr);
  _bus->CmdWrite(REG_AVI_SPIP_SADR1);            //0x2f, page 1, AVI shadow pip start address 1 (avi_spip_sadr1)
  _bus->DataWrite(addr >> 8);
  _bus->CmdWrite(REG_AVI_SPIP_SADR2);            //0x30, page 1, AVI shadow pip start address 2 (avi_spip_sadr2)
  _bus->DataWrite(addr >> 16);
  _bus->CmdWrite(REG_AVI_SPIP_SADR3);            //0x31, page 1, AVI shadow pip start address 3 (avi_spip_sadr3)
  _bus->DataWrite(addr >> 24);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0 
// [0x32] PIP 1 or 2 Image Width 0 (PIW0)
// [0x33] PIP 1 or 2 Image Width 1 (PIW1)
//
//================================================================================


/**
 * @brief  Define a largura da imagem de origem para a janela PIP (Picture-in-Picture).
 *
 * Essa função programa os registradores PIW0 (0x32) e PIW1 (0x33) para indicar
 * quantos pixels horizontais a imagem do PIP possui na memória gráfica.
 * 
 * Diferença importante:
 *   - **Janela PIP**: região da tela onde a imagem será mostrada (definida por posição e tamanho).
 *   - **Imagem Width (PIW)**: largura real da imagem armazenada na RAM, que será usada
 *     como base para a leitura dos dados do PIP.
 * 
 * Restrições impostas pelo hardware:
 *   - O valor deve ser múltiplo de 4 (bits [1:0] são fixados em 0 internamente).
 *   - Deve ser menor que a largura horizontal total do display principal.
 *   - O valor corresponde ao número físico de pixels que compõem cada linha da imagem na RAM.
 * 
 * @verbatim
 * REG [0x32] PIP 1 or 2 Image Width 0 (PIW0)
 * bit [7-2] PIP Image Width [7:2]
 *           Unit: Pixel.
 *           It must be divisible by 4. PIW Bit [1:0] ties to “0” internally.
 *           The value is physical pixel number.
 *           This width should be less than horizontal display width.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s 
 *           parameters. Function bit will be configured for relative PIP 
 *           window.
 *
 * REG [0x33] PIP 1 or 2 Image Width 1 (PIW1)
 * bit [4-0] PIP Image Width [12:8]
 *           Unit: Pixel.
 *           It must be divisible by 4. PIW Bit [1:0] ties to “0” internally.
 *           The value is physical pixel number.
 *           This width should be less than horizontal display width.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s 
 *           parameters. Function bit will be configured for relative PIP 
 *           window.
 * 
 * @endverbatim
 *
 * @param Wx Largura da imagem de origem do PIP em pixels (múltiplo de 4).
 *
 * @note Esse valor não é necessariamente igual à largura da janela PIP mostrada na tela;
 *       ele apenas informa ao RA8889 como percorrer os dados de imagem armazenados.
 */
void RA8889::PIP_Image_Width(uint16_t Wx)
{
  _bus->CmdWrite(REG_PIW0);          //0x32, PIP 1 or 2 Image Width 0 (PIW0)
  _bus->DataWrite(Wx);
  _bus->CmdWrite(REG_PIW1);          //0x33, PIP 1 or 2 Image Width 1 (PIW1)
  _bus->DataWrite(Wx >> 8);
}


//================================================================================
//
// PAGE 0 
// [0x34] PIP 1 or 2 Window Image Upper-Left corner X-coordinates 0 (PWIULX0)
// [0x35] PIP 1 or 2 Window Image Upper-Left corner X-coordinates 1 (PWIULX1)
// [0x36] PIP 1 or 2 Window Image Upper-Left corner Y-coordinates (PWIULY0)
// [0x37] PIP 1 or 2 Window Image Upper-Left corner Y-coordinates 1 (PWIULY1)
// 
//================================================================================


/**
 * @brief  Define a posição inicial (X, Y) da imagem dentro da janela PIP (Picture-in-Picture).
 *
 * Essa função configura os registradores PWIULX0/1 (0x34/0x35) e PWIULY0/1 (0x36/0x37)
 * para indicar o pixel da imagem de origem que será exibido no canto superior-esquerdo
 * da janela PIP.
 *  
 * Diferença importante:
 *   - **Janela PIP**: região da tela onde a imagem será mostrada.
 *   - **Imagem PIP**: pixels da imagem de origem armazenada na RAM.
 *   - Essa função controla qual pixel da imagem de origem será mapeado para o canto
 *     superior-esquerdo da janela.
 *
 * Restrições impostas pelo hardware:
 *   - Coordenadas X e Y devem ser divisíveis por 4 (bits [1:0] são fixados em 0 internamente para X).
 *   - X + largura da imagem ≤ 8188 pixels.
 *   - Y + altura da janela ≤ 8191 pixels.
 * 
 * @verbatim
 * REG [0x34] PIP 1 or 2 Window Image Upper-Left corner X-coordinates 0 (PWIULX0)
 * bit [7-2] PIP 1 or 2 Window Image Upper-Left corner X-coordinates [7:2]
 *           Reference PIP Image coordinates.
 *           Unit: Pixel
 *           It must be divisible by 4. PWIULX Bit [1:0] tie to “0” internally.
 *           X-axis coordinates plus PIP image width must be less than or equal to 8188.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 *
 * REG [0x35] PIP 1 or 2 Window Image Upper-Left corner X-coordinates 1 (PWIULX1)
 * bit [4-0] PIP Window Image Upper-Left corner X-coordinates [12:8]
 *           Reference PIP Image coordinates.
 *           Unit: Pixel
 *           It must be divisible by 4. PWIULX Bit [1:0] ties to “0” internally.
 *           X-axis coordinates plus PIP image width must be less than or equal to 8188.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x36] PIP 1 or 2 Window Image Upper-Left corner Y-coordinates (PWIULY0)
 * bit [7-0] PIP Windows Display Upper-Left corner Y-coordinates [7:0]
 *           Reference PIP Image coordinates.
 *           Unit: Pixel
 *           Y-axis coordinates plus PIP window height should be less than or equal to 8191.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x37] PIP 1 or 2 Window Image Upper-Left corner Y-coordinates 1 (PWIULY1)
 * bit [4-0] PIP Windows Image Upper-Left corner Y-coordinates [12:8]
 *           Reference PIP Image coordinates.
 *           Unit: Pixel
 *           Y-axis coordinates plus PIP window height should be less than or equal to 8191.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * @endverbatim
 *
 * @param Wx Coordenada X do pixel inicial da imagem PIP (múltiplo de 4).
 * @param Hy Coordenada Y do pixel inicial da imagem PIP.
 *
 * @note Essa função não altera o tamanho da janela PIP nem a largura da imagem;
 *       apenas define o ponto da imagem de origem que será renderizado no canto
 *       superior-esquerdo da janela.
 */
void RA8889::PIP_WindowImage_StartXY(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_PWIULX0);                   //0x34, PIP 1 or 2 Window Image Upper-Left corner X-coordinates 0 (PWIULX0)
  _bus->DataWrite(Wx);
  _bus->CmdWrite(REG_PWIULX1);                   //0x35, PIP 1 or 2 Window Image Upper-Left corner X-coordinates 1 (PWIULX1)
  _bus->DataWrite(Wx >> 8);
  _bus->CmdWrite(REG_PWIULY0);                   //0x36, PIP 1 or 2 Window Image Upper-Left corner Y-coordinates (PWIULY0)
  _bus->DataWrite(Hy);
  _bus->CmdWrite(REG_PWIULY1);                   //0x37, PIP 1 or 2 Window Image Upper-Left corner Y-coordinates 1 (PWIULY1)
  _bus->DataWrite(Hy >> 8);
}


//================================================================================
//
// PAGE 0 
// [0x38] PIP 1 or 2 Window Width 0 (PWW0)
// [0x39] PIP 1 or 2 Window Width 1 (PWW1)
// [0x3a] PIP 1 or 2 Window Height 0 (PWH0)
// [0x3b] PIP 1 or 2 Windows Height 1 (PWH1)
// 
//================================================================================


/**
 * @brief Configura a largura e altura da janela PIP (Picture-in-Picture) na tela.  
 *
 * Essa função grava nos registradores PWW0/1 (0x38/0x39) e PWH0/1 (0x3A/0x3B) 
 * os valores de largura e altura da janela PIP selecionada. 
 * 
 * Diferença importante:
 *   - **Janela PIP (Window Width/Height)**: define a área visível do PIP na tela.
 *   - **Imagem PIP**: pixels da imagem de origem que serão renderizados na janela.
 *   - Essa função **não altera a imagem**, apenas define o tamanho da janela na tela.
 *
 * Restrições impostas pelo hardware:
 *   - Largura (Wx) deve ser múltiplo de 4, máximo 2044 pixels.
 *   - Altura (Hy) máxima 2047 pixels.
 *   - A resolução horizontal é múltiplo de 8 pixels, vertical em linhas de 1 pixel.
 *
 * @verbatim
 * REG [0x38] PIP 1 or 2 Window Width 0 (PWW0)
 * bit [7-2] PIP Window Width [7:2]
 *           Unit: Pixel.
 *           It must be divisible by 4. PWW Bit [1:0] ties to “0” internally.
 *           The value is physical pixel number. Maximum value is 2044 pixels.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 *
 * REG [0x39] PIP 1 or 2 Window Width 1 (PWW1)
 * bit [2-0] PIP Window Width [10:8]
 *           Unit: Pixel.
 *           It must be divisible by 4. The value is physical pixel number.
 *           Maximum value is 2044 pixels.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x3a] PIP 1 or 2 Window Height 0 (PWH0)
 * bit [7-0] PIP Window Height [7:0]
 *           Unit: Pixel
 *           The value is physical pixel number. Maximum value is 2047 pixels.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * REG [0x3b] PIP 1 or 2 Windows Height 1 (PWH1)
 * bit [2-0] PIP Window Height [10:8]
 *           Unit: Pixel
 *           The value is physical pixel number. Maximum value is 2047 pixels.
 *           According to the bit of Select Configure PIP 1 or 2 Window’s parameters. Function bit will be configured for relative PIP window.
 * 
 * Note: 
 *   - The PIP windows sizes and start positions are specified in 8 pixel resolution (horizontal) and 1 line resolution (vertical).
 *   - Above multiple bytes registers from 20h to 3Bh only take effect while the MSB is written.
 *     For example: To program Main Image Start Address from 20h to 23h and this address take effect while the register [23h] is written.
 * @endverbatim
 *
 * @param Wx Largura da janela PIP em pixels (múltiplo de 4).
 * @param Hy Altura da janela PIP em pixels.
 *
 * @note Essa função não altera a posição da janela nem os dados da imagem,
 *       apenas a área da tela que a janela PIP ocupa.
 */
void RA8889::PIP_Window_WidthHeight(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_PWW0);                      //0x38, PIP 1 or 2 Window Width 0 (PWW0)
  _bus->DataWrite(Wx);
  _bus->CmdWrite(REG_PWW1);                      //0x39, PIP 1 or 2 Window Width 1 (PWW1)
  _bus->DataWrite(Wx >> 8);
  _bus->CmdWrite(REG_PWH0);                      //0x3a, PIP 1 or 2 Window Height 0 (PWH0)
  _bus->DataWrite(Hy);
  _bus->CmdWrite(REG_PWH1);                      //0x3b, PIP 1 or 2 Windows Height 1 (PWH1)
  _bus->DataWrite(Hy >> 8);
}


//================================================================================
//
// PAGE 0 
// [0x3c] Graphic / Text Cursor Control Register (GTCCR)
// 
//================================================================================


/**
 * @brief Enable or disable gamma correction output
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [7] Gamma correction Enable
 *     0: Disable
 *     1: Enable
 * Gamma correction is the last output stage.
 * @endverbatim
 *
 * @param b true to enable gamma correction, false to disable
 *
 * @note Use this function after configuring the gamma table for each color.
 */
void RA8889::GammaCorrection_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  b ? SETB(temp,7) : CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Select gamma table for Blue component for MPU write
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [6-5] Gamma table select for MPU write gamma data
 *     0b00: Gamma table for Blue
 *     0b01: Gamma table for Green
 *     0b10: Gamma table for Red
 *     0b11: NA
  * @endverbatim
 *
 * @param None
 *
 * @note This function only selects the table. You must write gamma data afterward.
 */
void RA8889::GammaTableforBlue(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  CLRB(temp,6);
  CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Select gamma table for Green component for MPU write
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [6-5] Gamma table select for MPU write gamma data
 *     0b00: Gamma table for Blue
 *     0b01: Gamma table for Green
 *     0b10: Gamma table for Red
 *     0b11: NA
  * @endverbatim
 *
 * @param None
 *
 * @note This function only selects the table. You must write gamma data afterward.
 */
void RA8889::GammaTableforGreen(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  CLRB(temp,6);
  SETB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Select gamma table for Red component for MPU write
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [6-5] Gamma table select for MPU write gamma data
 *     0b00: Gamma table for Blue
 *     0b01: Gamma table for Green
 *     0b10: Gamma table for Red
 *     0b11: NA
  * @endverbatim
 *
 * @param None
 *
 * @note This function only selects the table. You must write gamma data afterward.
 */
void RA8889::GammaTableforRed(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  SETB(temp,6);
  CLRB(temp,5);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable or disable Graphic Cursor
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [4] Graphic Cursor Enable
 *     0 : Graphic Cursor disable.
 *     1 : Graphic Cursor enable.
 * @endverbatim
 *
 * @param b true to enable, false to disable
 *
 *  @note Cursor Graphic has priority over Text Cursor if both enabled.
 */
void RA8889::CursorGraphic_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  b ? SETB(temp,4) : CLRB(temp,4);
  _bus->DataWrite(temp);
}


/**
 * @brief Select Graphic Cursor Set 1
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [3-2] Graphic Cursor Selection Bit
 *     Select one from four graphic cursor types. (00b to 11b)
 *     0b00 : Graphic Cursor Set 1.
 *     0b01 : Graphic Cursor Set 2.
 *     0b10 : Graphic Cursor Set 3.
 *     0b11 : Graphic Cursor Set 4.
 * @endverbatim
 *
 * @param 
 *
 * @note None
 */
void RA8889::CursorGraphic_Set1(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  CLRB(temp,3);
  CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Select Graphic Cursor Set 2
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [3-2] Graphic Cursor Selection Bit
 *     Select one from four graphic cursor types. (00b to 11b)
 *     0b00 : Graphic Cursor Set 1.
 *     0b01 : Graphic Cursor Set 2.
 *     0b10 : Graphic Cursor Set 3.
 *     0b11 : Graphic Cursor Set 4.
 * @endverbatim
 *
 * @param 
 *
 * @note None
 */
void RA8889::CursorGraphic_Set2(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  CLRB(temp,3);
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Select Graphic Cursor Set 3
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [3-2] Graphic Cursor Selection Bit
 *     Select one from four graphic cursor types. (00b to 11b)
 *     0b00 : Graphic Cursor Set 1.
 *     0b01 : Graphic Cursor Set 2.
 *     0b10 : Graphic Cursor Set 3.
 *     0b11 : Graphic Cursor Set 4.
 * @endverbatim
 *
 * @param 
 *
 * @note None
 */
void RA8889::CursorGraphic_Set3(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  SETB(temp,3);
  CLRB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Select Graphic Cursor Set 4
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [3-2] Graphic Cursor Selection Bit
 *     Select one from four graphic cursor types. (00b to 11b)
 *     0b00 : Graphic Cursor Set 1.
 *     0b01 : Graphic Cursor Set 2.
 *     0b10 : Graphic Cursor Set 3.
 *     0b11 : Graphic Cursor Set 4.
 * @endverbatim
 *
 * @param 
 *
 * @note None
 */
void RA8889::CursorGraphic_Set4(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  SETB(temp,3);
  SETB(temp,2);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable or disable Text Cursor
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [1] Text Cursor Enable
 *     0 : Disable.
 *     1 : Enable.
 * 
 *     Text cursor & Graphic cursor cannot be enabled simultaneously. If they are enabled at the same time, the priority of Graphic cursor is higher than Text cursor.
 * @endverbatim
 *
 * @param b true to enable, false to disable
 *
 * @note None
 */
void RA8889::CursorText_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  b ? SETB(temp,1) : CLRB(temp,1);
  _bus->DataWrite(temp);
}


/**
 * @brief Enable or disable Text Cursor Blinking
 *
 * @verbatim
 * REG [0x3c] Graphic / Text Cursor Control Register (GTCCR)
 * bit [0] Text Cursor Blinking Enable
 *     0 : Disable.
 *     1 : Enable.
 * @endverbatim
 *
 * @param b true to enable blinking, false to disable
 *
 * @note None
 */
void RA8889::CursorText_Blinking_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTCCR);                     //0x3c,  Graphic / Text Cursor Control Register (GTCCR)
  temp = _bus->DataRead();
  SETB(temp,0);
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 0 
// [0x3d] Blink Time Control Register (BTCR)
// 
//================================================================================


/**
 * @brief Text Cursor Blink Time Setting
 *
 * @verbatim
 * REG [0x3d] Blink Time Control Register (BTCR)
 * bit [7-0] Text Cursor Blink Time Setting (Unit: frame time)
 *     0x00 : 1 frame time.
 *     0x01 : 2 frames time.
 *     0x02 : 3 frames time.
 *     ...
 *     0xff : 256 frames time.
 * @endverbatim
 *
 * @param frames (Unit: Frame)
 *
 * @note None
 */
void RA8889::CursorText_BlinkingTimeFrames(uint8_t frames)
{
  _bus->CmdWrite(REG_BTCR);                      //0x3d, Blink Time Control Register (BTCR)
  _bus->DataWrite(frames);
}


//================================================================================
//
// PAGE 0 
// [0x3e] Text Cursor Horizontal Size Register (CURHS)
// [0x3f] Text Cursor Vertical Size Register (CURVS)
//
//================================================================================


/**
 * @brief Set Text Cursor size (width and height)
 *
 * @verbatim
 * REG [0x3e] Text Cursor Horizontal Size Register (CURHS)
 * bit [4-0] Text Cursor Horizontal Size Setting[4:0]
 *           Unit : Pixel
 *           Zero-based number. Value “0” means 1 pixel. Note : When character is enlarged, the cursor setting will multiply the same times as the character enlargement.
 *           Default: 0x07, Width of 8 pixel
 * 
 * REG [0x3f] Text Cursor Vertical Size Register (CURVS)
 * bit [4-0] Text Cursor Vertical Size Setting[4:0]
 *           Unit : Pixel
 *           Zero-based number. Value “0” means 1 pixel. Note : When character is enlarged, the cursor setting will multiply the same times as the character enlargement.
 *           Default: 0x00, Height of 1 pixel
 * 
 *  Note: When character enlargement is used, the cursor size is multiplied by the same factor.
 * @endverbatim
 *
 * @param Wx Cursor horizontal size (0-31, zero-based)
 * @param Hy Cursor vertical size (0-31, zero-based)
 *
 * @note Only affects the text cursor; graphic cursor is unaffected.
 *       Wx, Hy iniciam em 0
 */
void RA8889::CursorText_Dimensions(uint8_t Wx, uint8_t Hy)
{
  _bus->CmdWrite(REG_CURHS);      //0x3, Text Cursor Horizontal Size Register (CURHS)
  _bus->DataWrite(Wx & 0x1f);     //Garante que só 5 bits são usados
  _bus->CmdWrite(REG_CURVS);      //0x3, Text Cursor Vertical Size Register (CURVS)
  _bus->DataWrite(Hy & 0x1f);     //Garante que só 5 bits são usados
}


//================================================================================
//
// PAGE 0 
// [0x40] Graphic Cursor Horizontal Position Register 0 (GCHP0)
// [0x41] Graphic Cursor Horizontal Position Register 1 (GCHP1)
// [0x42] Graphic Cursor Vertical Position Register 0 (GCVP0)
// [0x43] Graphic Cursor Vertical Position Register 1 (GCVP1)
//
//================================================================================


/**
 * @brief Set Graphic Cursor Position on screen
 *
 * @verbatim
 * REG [0x40] Graphic Cursor Horizontal Position Register 0 (GCHP0)
 * bit [7-0] Graphic Cursor Horizontal Location[7:0]
 *           Please refer to the Main Window coordinates.
 * 
 * REG [0x41] Graphic Cursor Horizontal Position Register 1 (GCHP1)
 * bit [4-0] Graphic Cursor Horizontal Location[12:8]
 *           Please refer to the Main Window coordinates.
 *           
 * REG [0x42] Graphic Cursor Vertical Position Register 0 (GCVP0)
 * bit [7-0] Graphic Cursor Vertical Location[7:0]
 *           Please refer to the Main Window coordinates.
 * 
 * REG [0x43] Graphic Cursor Vertical Position Register 1 (GCVP1)
 * bit [4-0] Graphic Cursor Vertical Location[12:8]
 *           Please refer to the Main Window coordinates.
 * 
 * Note: Position is referenced to the Main Window coordinates.
 *       Maximum coordinate is 8191 (13 bits).
 * 
 * Observações:
 *   - A posição é referenciada ao Main Window.
 *   - É um cursor gráfico de até 13 bits por coordenada, ou seja, valores de 0 a 8191.
 *   - O registrador divide a coordenada em byte baixo (0–7) e byte alto (8–12).
 * @endverbatim
 *
 * @param X Horizontal position (0–8191)
 * @param Y Vertical position (0–8191)
 *
 * @note Requires Graphic Cursor to be enabled via CursorGraphic_Enable().
 */
void RA8889::CursorGraphic_Position(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_GCHP0);                     //0x40, Graphic Cursor Horizontal Position Register 0 (GCHP0)
  _bus->DataWrite(Wx & 0xff);                    //byte baixo
  _bus->CmdWrite(REG_GCHP1);                     //0x41, Graphic Cursor Horizontal Position Register 1 (GCHP1)
  _bus->DataWrite((Wx >> 8) & 0x1f);             //byte alto (apenas 5 bits)
  _bus->CmdWrite(REG_GCVP0);                     //0x42, Graphic Cursor Vertical Position Register 0 (GCVP0)
  _bus->DataWrite(Hy & 0xff);                    //Byte baixo
  _bus->CmdWrite(REG_GCVP1);                     //0x43, Graphic Cursor Vertical Position Register 1 (GCVP1)
  _bus->DataWrite((Hy >> 8) & 0x1f);             //byte alto (apenas 5 bits)
}


//================================================================================
//
// PAGE 0 
// [0x44] Graphic Cursor Color 0 (GCC0)
//
//================================================================================


/**
 * @brief Set Graphic Cursor Color 0
 *
 * @verbatim
 * REG [0x44] Graphic Cursor Color 0 (GCC0)
 * bit [7-0] Graphic Cursor Color 0 with 256 Colors
 *           RGB Format [7:0] = RRRGGGBB.
 * @endverbatim
 *
 * @param color 8-bit RGB color value in RRRGGGBB format
 *
 * @note This sets the first color of the Graphic Cursor.
 *       Combinando Color 0 e Color 1, você pode ter cursores bicolores ou com efeitos de transparência, dependendo do Set selecionado.
 */
void RA8889::CursorGraphic_Color0(uint8_t color)
{
    _bus->RegisterWrite(0x44, color);       //0x44, Graphic Cursor Color 0 (GCC0)
}


//================================================================================
//
// PAGE 0 
// [0x45] Graphic Cursor Color 1 (GCC1)
//
//================================================================================


/**
 * @brief Set Graphic Cursor Color 1
 *
 * @verbatim
 * REG [0x44] Graphic Cursor Color 1 (GCC1)
 * bit [7-0] Graphic Cursor Color 1 with 256 Colors
 *           RGB Format [7:0] = RRRGGGBB.
 * @endverbatim
 *
 * @param color 8-bit RGB color value in RRRGGGBB format
 *
 * @note This sets the second color of the Graphic Cursor.
 *       Combinando Color 0 e Color 1, você pode ter cursores bicolores ou com efeitos de transparência, dependendo do Set selecionado.
 */
void RA8889::CursorGraphic_Color1(uint8_t color)
{
  _bus->RegisterWrite(REG_GCC1, color);        //0x45, Graphic Cursor Color 1 (GCC1)
}


//================================================================================
//
// PAGE 0 
// [0x46] PAGE Switch
//
//================================================================================


/**
 * @brief PAGE Switch
 *
 *        REG [46h] PAGE Switch
 *                  bit [0] Page switch, and SPIM bus switch
 *                          0: page 0, SPIM bus 0, for lower 256 register setting (Default)
 *                          1: page 1, SPIM bus 1, for the register settings of media decoder
 *
 * @param ePageReg::Page0, ePageReg::Page1
 *
 * @note RA8889 has two page-page0 / page1 registers. Users can switch 
 *       page1 / page0 at REG [46h] bit0 of page0 / page1. Default is Page 0
 */
void RA8889::PageSwitch(ePageReg pr)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PAGE_SWITCH);              //0x46, PAGE Switch
  temp = _bus->DataRead();
  CLRB(temp,0);                               //Reset bit 0
  temp |= static_cast<uint8_t>(pr);           //Converte enum para uint8_t  
  _bus->DataWrite(temp);
  delay(1);
}


/**
 * @brief SPI Master Selectable Bus Mode (Default)
 *
 * @details
 * Configura o RA8889 para o modo "Selectable Bus" no SPI mestre (Page 0).  
 * Esta funcionalidade é **não documentada oficialmente** no datasheet da RAiO.  
 * Experimentos e códigos de terceiros indicam que:
 * - Bit 1 do registrador 0x46 é usado
 * - 0: modo "selecionável", permitindo flexibilidade no uso dos sinais CS do SPI.
 * - Observado como **compatível com RA8876**, ou seja, o SPI se comporta de forma semelhante
 *   ao RA8876 em termos de clock e seleção de barramento.
 *
 * @note
 * Informação baseada em **códigos open-source e experimentos práticos**; comportamento exato
 * pode variar entre versões do RA8889.
 */
void RA8889::SPIM_SelectableBusMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PAGE_SWITCH);               //0x46, Page 0, Page Switch
  temp = _bus->DataRead();
  CLRB(temp,1);                                //Reset bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief SPI Master Fixed Bus Mode
 *
 * @details
 * Configura o RA8889 para o modo "Fixed Bus" no SPI mestre (Page 0).  
 * Esta funcionalidade é **não documentada oficialmente** no datasheet da RAiO.  
 * Experimentos indicam que:
 * - Bit 1 do registrador 0x46 é usado
 * - 1: modo "fixo", agrupando CS em dois barramentos lógicos:
 *      - Bus0: CS0 e CS1
 *      - Bus1: CS2 e CS3
 *
 * @note
 * Útil quando múltiplos dispositivos SPI precisam ser isolados entre grupos de CS.
 * Informação baseada em **códigos open-source e testes práticos**, sem confirmação oficial.
 */
void RA8889::SPIM_FixedBusMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PAGE_SWITCH); //0x46, Page 0, Page Switch
  temp = _bus->DataRead();
  SETB(temp,1);                  //Set bit 1
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1 
// [0x46] PAGE Switch
//
//================================================================================


/**
 * @brief SPI Master Frequency Clock Eliminator Divided by 2 (Default)
 *        Modo compativel com RA8876
 *
 * @verbatim
 * Essas funções permitem que você controle a velocidade do SPI do RA8889, 
 * escolhendo entre modo seguro compatível com RA8876 (dividido por 2) ou 
 * modo rápido (dividido por 1), dependendo da necessidade de transferir 
 * muitos dados rapidamente ou manter compatibilidade com códigos antigos.
 *
 * PAGE 1 REG [0x46] Page Switch
 *                   bit [2] PS8876 Fsck(REG[BBh]) compatible mode
 *                           Spi master frequency eliminator 
 *                   0: Fsck = Fcore / ((divisor + 1) * 2)
 *                      User don’t need modify old program parameter (Default)
 *                   1: Fsck = Fcore / (divisor * 2)
 *                      When user need to use SPI_DIVSOR = 0,
 *                      Fsck = Fcore, set this to 1
 *                   Note: this bit is available only on page1
 *
 * Essas funções controlam a frequência do SPI master do RA8889, permitindo escolher entre:
 *  - Modo seguro compatível com RA8876 (dividido por 2)
 *  - Modo rápido (dividido por 1)
 * @endverbatim
 *
 * @param None
 *
 * @note  Objetivo de configura o SPI master para dividir a frequência do clock por 2 usando o eliminador de frequência.
 *        0: SPIM_clock = ( Core clock / 2 ) / ( frequency eliminator )
 *           Altera a forma como o clock SPI mestre é calculado, garantindo compatibilidade com códigos que foram originalmente escritos para RA8876
 *        1: SPIM_clock = ( Core clock / 1 ) / ( frequency eliminator )
 *           Modo RA8889 padrão
 */
void RA8889::SPIM_ClockDivided_2(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_PAGE_SWITCH);               //0x46, Page 1, Page Switch
  temp = _bus->DataRead();
  CLRB(temp,2);                                //Seleciona Divisor 2 / modo compatível RA8876
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief SPI Master Frequency Clock Eliminator Divided by 1
 *        Modo r[apido
 *
 * @verbatim
 * Essas funções permitem que você controle a velocidade do SPI do RA8889, 
 * escolhendo entre modo seguro compatível com RA8876 (dividido por 2) ou 
 * modo rápido (dividido por 1), dependendo da necessidade de transferir 
 * muitos dados rapidamente ou manter compatibilidade com códigos antigos.
 *
 * PAGE 1 REG [0x46] Page Switch
 *                   bit [2] PS8876 Fsck(REG[BBh]) compatible mode
 *                           Spi master frequency eliminator 
 *                   0: Fsck = Fcore / ((divisor + 1) * 2)
 *                      User don’t need modify old program parameter (Default)
 *                   1: Fsck = Fcore / (divisor * 2)
 *                      When user need to use SPI_DIVSOR = 0,
 *                      Fsck = Fcore, set this to 1
 *                   Note: this bit is available only on page1
 *
 * Essas funções controlam a frequência do SPI master do RA8889, permitindo escolher entre:
 *  - Modo seguro compatível com RA8876 (dividido por 2)
 *  - Modo rápido (dividido por 1)
 * @endverbatim
 *
 * @param None
 *
 * @note  Objetivo de configura o SPI master para dividir a frequência do clock por 1 usando o eliminador de frequência.
 *        0: SPIM_clock = ( Core clock / 2 ) / ( frequency eliminator )
 *           Altera a forma como o clock SPI mestre é calculado, garantindo compatibilidade com códigos que foram originalmente escritos para RA8876
 *        1: SPIM_clock = ( Core clock / 1 ) / ( frequency eliminator )
 *           Modo RA8889 padrão
 */
void RA8889::SPIM_ClockDivided_1(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_PAGE_SWITCH);               //0x46, Page 1, Page Switch
  temp = _bus->DataRead();
  SETB(temp,2);                                //Seleciona Divisor 1 / modo rápido
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// [0x50] Canvas Start address 0 (CVSSA0)
// [0x51] Canvas Start address 1 (CVSSA1)
// [0x52] Canvas Start address 2 (CVSSA2)
// [0x53] Canvas Start address 3 (CVSSA3)
//
//================================================================================


/** OK
 * @brief 
 *
 * @verbatim
 *        REG [50h] Canvas Start address 0 (CVSSA0)
 *                  Start address of Canvas [7:0]
 *        REG [51h] Canvas Start address 1 (CVSSA1)
 *                  Start address of Canvas [15:8]
 *        REG [52h] Canvas Start address 2 (CVSSA2)
 *                  Start address of Canvas [23:16]
 *        REG [53h] Canvas Start address 3 (CVSSA3)
 *                  Start address of Canvas [31:24]
 * @endverbatim
 *
 * @param addr: endereço
 *
 * @note None
 */
void RA8889::CanvasImage_StartAddr(uint32_t addr)
{
  _bus->RegisterWrite(REG_CVSSA0, addr);             //0x50, Canvas Start address 0 (CVSSA0)
  _bus->RegisterWrite(REG_CVSSA1, addr >> 8);        //0x51, Canvas Start address 1 (CVSSA1)
  _bus->RegisterWrite(REG_CVSSA2, addr >> 16);       //0x52, Canvas Start address 2 (CVSSA2)
  _bus->RegisterWrite(REG_CVSSA3, addr >> 24);       //0x53, Canvas Start address 3 (CVSSA3)
}


//================================================================================
//
// [0x54] Canvas image width 0 (CVS_IMWTH0)
// [0x55] Canvas image width 1 (CVS_IMWTH1)
//
//================================================================================


/** OK
 * @brief 
 *
 * @verbatim
 *        REG [54h] Canvas image width 0 (CVS_IMWTH0)
 *                  Canvas image width [7:2]
 *                  The bits are Canvas image width.
 *                  Unit: Pixel, it is 4 pixel resolutions.
 *                  Width = Set Value
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [55h] Canvas image width 1 (CVS_IMWTH1)
 *                  Canvas image width [12:8]
 *                  The bits are Canvas image width
 *                  Ignored if canvas is in linear addressing mode.
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note None
 */
void RA8889::CanvasImage_Width(uint16_t Wx)
{
  _bus->RegisterWrite(REG_CVS_IMWTH0, Wx);           //0x54, Canvas image width 0 (CVS_IMWTH0)
  _bus->RegisterWrite(REG_CVS_IMWTH1, Wx >> 8);      //0x55, Canvas image width 1 (CVS_IMWTH1)
}


//================================================================================
//
// [0x56] Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
// [0x57] Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
// [0x58] Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
// [0x59] Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
//
//================================================================================


/** OK
 * @brief 
 *        
 * @verbatim
 *        REG [56h] Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
 *                  Active Window Upper-Left corner X-coordination [7:0]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  X-axis coordinates plus Active Window width cannot be larger than 8188.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [57h] Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
 *                  Active Window Upper-Left corner X-coordination [12:8]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  X-axis coordinates plus Active Window width cannot be larger than 8188.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [58h] Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
 *                  Active Window Upper-Left corner Y-coordination [7:0]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  Y-axis coordinates plus Active Window height cannot be larger than 8191.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [59h] Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
 *                  Active Window Upper-Left corner Y-coordination [12:8] 
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  Y-axis coordinates plus Active Window height cannot large than 8191.
 *                  Ignored if canvas is in linear addressing mode.
 * @endverbatim
 *
 * @param Wx, Hy: coordenada x,y do ponto inicial da janela
 *
 * @note None
 */
void RA8889::ActiveWindow_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_AWUL_X0, Wx);              //0x56, Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
  _bus->RegisterWrite(REG_AWUL_X1, Wx >> 8);         //0x57, Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
  _bus->RegisterWrite(REG_AWUL_Y0, Hy);              //0x58, Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
  _bus->RegisterWrite(REG_AWUL_Y1, Hy >> 8);         //0x59, Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
}


//================================================================================
//
// [0x5A] Active Window Width 0 (AW_WTH0)
// [0x5B] Active Window Width 1 (AW_WTH1)
// [0x5C] Active Window Height 0 (AW_HT0)
// [0x5D] Active Window Height 1 (AW_HT1)
//
//================================================================================


/** OK
 * @brief 
 *
 * @endverbatim
 *        REG [0x5a] Active Window Width 0 (AW_WTH0)
 *                   Width of Active Window [7:0]
 *        REG [0x5b] Active Window Width 1 (AW_WTH1)
 *                   Width of Active Window [12:8]
 *        REG [0x5c] Active Window Height 0 (AW_HT0)
 *                   Height of Active Window [7:0]
 *        REG [0x5d] Active Window Height 1 (AW_HT1)
 *                   Height of Active Window [12:8]
 * @endverbatim
 *
 * @param wx: width, hy: height
 *
 * @note None
 */
void RA8889::ActiveWindow_WidhtHeight(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_AW_WTH0, Wx);              //0x5a, Active Window Width 0 (AW_WTH0)
  _bus->RegisterWrite(REG_AW_WTH1, Wx >> 8);         //0x5b, Active Window Width 1 (AW_WTH1)
  _bus->RegisterWrite(REG_AW_HT0, Hy);               //0x5c, Active Window Height 0 (AW_HT0)
  _bus->RegisterWrite(REG_AW_HT1, Hy >> 8);          //0x5d, Active Window Height 1 (AW_HT1)
}


//================================================================================
//
// [0x5E] Color Depth of Canvas & Active Window (AW_COLOR)
//
//================================================================================


/** OK
 * @brief Block Mode X-Y Coordinates Addressing
 *        
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [2] Canvas addressing mode
 *         0b0: Block mode (X-Y coordinates addressing)
 *         0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Memory_BlockMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  CLRB(temp,2);                             //Reset bit 2
  _bus->DataWrite(temp);
}
void RA8889::Memory_XYMode(void) { Memory_BlockMode(); }


/**
 * @brief Check for Block Mode X-Y Coordinates Addressing
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [2] Canvas addressing mode
 *         0b0: Block mode (X-Y coordinates addressing)
 *         0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool RA8889::Memory_IsBlockMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  return (temp &= 0x04) == 0x00;             //Test bit 2
}
bool RA8889::Memory_IsXYMode(void) { return Memory_IsBlockMode(); }


/**
 * @brief Linear Mode Addressing
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [2] Canvas addressing mode
 *         0b0: Block mode (X-Y coordinates addressing)
 *         0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Memory_LinearMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  SETB(temp,2);                              //Set bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief Check for Linear Mode Addressing
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [2] Canvas addressing mode
 *         0b0: Block mode (X-Y coordinates addressing)
 *         0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool RA8889::Memory_IsLinearMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  return (temp &= 0x04) == 0x04;             //Test bit 2
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 8bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [1-0] Canvas image’s color depth & memory R/W data width
 *           In Block Mode:
 *           00: 8bpp
 *           01: 16bpp
 *           1x: 24bpp
 *           In Linear Mode:
 *           x0: 8-bits memory data read/write.
 *           x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void RA8889::Memory_8bpp_BlockMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  CLRB(temp,1);                              //Reset bit 1
  CLRB(temp,0);                              //Reset bit 2
  _bus->DataWrite(temp);                       //Set block mode x-y 8bpp
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 16bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [1-0] Canvas image’s color depth & memory R/W data width
 *           In Block Mode:
 *           00: 8bpp
 *           01: 16bpp
 *           1x: 24bpp
 *           In Linear Mode:
 *           x0: 8-bits memory data read/write.
 *           x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void RA8889::Memory_16bpp_BlockMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  CLRB(temp,1);                              //Reset bit 1
  SETB(temp,0);                              //Set bit 0
  _bus->DataWrite(temp);                       //Set block mode x-y 16bpp
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 24bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [1-0] Canvas image’s color depth & memory R/W data width
 *           In Block Mode:
 *           00: 8bpp
 *           01: 16bpp
 *           1x: 24bpp
 *           In Linear Mode:
 *           x0: 8-bits memory data read/write.
 *           x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void RA8889::Memory_24bpp_BlockMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = _bus->DataRead();
  SETB(temp,1);                              //Set bit 1
  _bus->DataWrite(temp);                       //Set block mode x-y 24bpp
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 * bit [1-0] Canvas image’s color depth & memory R/W data width
 *           In Block Mode:
 *           00: 8bpp
 *           01: 16bpp
 *           1x: 24bpp
 *           In Linear Mode:
 *           x0: 8-bits memory data read/write.
 *           x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param colordepth:  Profundidade de cores
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void RA8889::Memory_ColorDepth_BlockMode(eColorDepthBPP colordepth)
{
  if (colordepth == eColorDepthBPP::bpp8) Memory_8bpp_BlockMode();
  if (colordepth == eColorDepthBPP::bpp16) Memory_16bpp_BlockMode();
  if (colordepth == eColorDepthBPP::bpp24) Memory_24bpp_BlockMode();
}


//================================================================================
//
// [0x5F] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
// [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
// [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
// [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
//
//================================================================================


/**
 * @brief Set Graphic Read/Write Position X,Y
 *
 * @verbatim
 * User should program proper active window related parameters before configure this register.
 *            
 * REG [0x5f] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
 * bit [7~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [7:0]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Horizontal Position 0 [7:0]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel   
 * 
 * REG [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
 * bit [7~5] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [15:13]
 *           Unit: Byte
 *           When Canvas In Block mode: NA
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * bit [4~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [12:8]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Horizontal Position 1 [12:8]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 *
 * REG [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
 * bit [7~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [23:16]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Vertical Position 0 [7:0]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 *
 * REG [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
 * bit [7~5] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [31:29]
 *           Unit: Byte
 *           When Canvas In Block mode:NA
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * bit [4~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [28:24]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Vertical Position 1 [12:8]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note        
 * REG[5Eh].bit[3] Não existe a funcionalidade descrita por alguns autores 
 * no RA8889/8877/8876/8875/8870
 *
 * Nota: REG[0x5E].bit2 = AW_COLOR Linear/Block mode (em RA8889 family).
 * Quando Linear Mode = 1, registradores DMA (C0..C3) são interpretados como
 * Destination Address [7:0]..[31:2] na SDRAM (32-bit). 
 * Quando em Block Mode = 0, registrador de DMR (C0..C3) são interpretados como
 * coordenadas X e Y a partir de 0,0. Não existe REG[0x5E].bit3 com essa função.
 *
 * Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 * verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 * Original da RAIO:
 * 
 * REG[5Eh] bit3, Select to read back Graphic Read/Write position.
 *          When DPRAM Linear mode:        Graphic Read/Write Position [31:24][23:16][15:8][7:0]
 *          When DPRAM Active window mode: Graphic Read/Write
 *          Horizontal Position [12:8][7:0],
 *          Vertical Position [12:8][7:0].
 *          Reference Canvas image coordinate. Unit: Pixel
 *
 * Use este para posicionar o local de um pixel na tela
 */
void RA8889::GotoPixel_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_CURH0);       //0x5f, Graphic Read/Write position Horizontal Position Register 0 (CURH0)
  _bus->DataWrite(Wx);             //byte baixo de x
  _bus->CmdWrite(REG_CURH1);       //0x60, Graphic Read/Write position Horizontal Position Register 1 (CURH1)
  _bus->DataWrite(Wx >> 8);        //byte alto de x
							     
  _bus->CmdWrite(REG_CURV0);       //0x61, Graphic Read/Write position Vertical Position Register 0 (CURV0)
  _bus->DataWrite(Hy);             //byte baixo de y
  _bus->CmdWrite(REG_CURV1);       //0x62, raphic Read/Write position Vertical Position Register 1 (CURV1)
  _bus->DataWrite(Hy >> 8);        //byte alto de y
}


/**
 * @brief Set Graphic Read/Write Position Linear
 *
 * @verbatim
 * User should program proper active window related parameters before configure this register.
 *            
 * REG [0x5F] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
 * bit [7~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [7:0]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Horizontal Position 0 [7:0]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel         
 * 
 * REG [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
 * bit [7~5] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [15:13]
 *           Unit: Byte
 *           When Canvas In Block mode: NA
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * bit [4~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [12:8]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Horizontal Position 1 [12:8]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 *
 * REG [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
 * bit [7~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [23:16]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Vertical Position 0 [7:0]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 *
 * REG [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
 * bit [7~5] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [31:29]
 *           Unit: Byte
 *           When Canvas In Block mode:NA
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * bit [4~0] Write: Set Graphic Read/Write position
 *           When Canvas In Linear mode:
 *           Memory Read/Write address [28:24]
 *           Unit: Byte
 *           When Canvas In Block mode:
 *           Graphic Read/Write Vertical Position 1 [12:8]
 *           Please refer to the Canvas image coordinates.
 *           Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note        
 * REG[5Eh].bit[3] Não exsite a funcionalidade descrita por alguns autores 
 * no RA8889/8877/8876/8875/8870
 *
 * Nota: REG[0x5E].bit2 = AW_COLOR Linear/Block mode (em RA8889 family).
 * Quando Linear Mode = 1, registradores DMA (C0..C3) são interpretados como
 * Destination Address [7:0]..[31:2] na SDRAM (32-bit). 
 * Quando em Block Mode = 0, registrador de DMR (C0..C3) são interpretados como
 * coordenadas X e Y a partir de 0,0. Não existe REG[0x5E].bit3 com essa função.
 *
 * Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 * verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 * Original da RAIO:
 * 
 * REG[5Eh] bit3, Select to read back Graphic Read/Write position.
 *          When DPRAM Linear mode:        Graphic Read/Write Position [31:24][23:16][15:8][7:0]
 *          When DPRAM Active window mode: Graphic Read/Write
 *          Horizontal Position [12:8][7:0],
 *          Vertical Position [12:8][7:0].
 *          Reference Canvas image coordinate. Unit: Pixel
 */
void RA8889::GotoPixel_Linear(uint32_t addr)
{
  _bus->CmdWrite(REG_CURH0);       //0x5f, Graphic Read/Write position Horizontal Position Register 0 (CURH0)
  _bus->DataWrite(addr);           //bit [7..0] do endreço
  _bus->CmdWrite(REG_CURH1);       //0x60, Graphic Read/Write position Horizontal Position Register 1 (CURH1)
  _bus->DataWrite(addr >> 8);      //bit [15..8] do endereço
  _bus->CmdWrite(REG_CURV0);       //0x61, Graphic Read/Write position Vertical Position Register 0 (CURV0)
  _bus->DataWrite(addr >> 16);     //bit [23..16] do endereço
  _bus->CmdWrite(REG_CURV1);       //0x62, raphic Read/Write position Vertical Position Register 1 (CURV1)
  _bus->DataWrite(addr >> 24);     //bit [31..24] do endereço
}
void RA8889::GotoLinearAddr(uint32_t addr) { GotoPixel_Linear(addr); }


//================================================================================
//
// [0x63] Text Write X-coordinates Register 0 (F_CURX0)
// [0x64] Text Write X-coordinates Register 1 (F_CURX1)
// [0x65] Text Write Y-coordinates Register 0 (F_CURY0)
// [0x66] Text Write Y-coordinates Register 1 (F_CURY1)
//
//================================================================================


/**
 * @brief Set Text Write Position X-Y
 *
 * @verbatim
 * User should program proper active window related parameters before configure this register.
 *            
 * REG [0x63] Text Write X-coordinates Register 0 (F_CURX0)
 *            bit [7~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write X-coordinates [7:0]
 *                      The setting of the horizontal cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 *   
 * REG [0x64] Text Write X-coordinates Register 1 (F_CURX1)
 *            bit [4~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write X-coordinates [12:8]
 *                      The setting of the horizontal cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * REG [0x65] Text Write Y-coordinates Register 0 (F_CURY0)
 *            bit [7~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write Y-coordinates [7:0]
 *                      The setting of the vertical cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * REG [0x66] Text Write Y-coordinates Register 1 (F_CURY1)
 *            bit [4~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write Y-coordinates [12:8]
 *                      The setting of the vertical cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note  Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 *        verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 */
void RA8889::GotoText_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_F_CURX0);                   //0x63, Text Write X-coordinates Register 0 (F_CURX0)
  _bus->DataWrite(Wx);                           //Text Write X-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURX1);                   //0x64, Text Write X-coordinates Register 1 (F_CURX1)
  _bus->DataWrite(Wx >> 8);                      //Text Write X-coordinate [12:8]
  _bus->CmdWrite(REG_F_CURY0);                   //0x65, Text Write Y-coordinates Register 0 (F_CURY0)
  _bus->DataWrite(Hy);                           //Text Write Y-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURY1);                   //0x66, Text Write Y-coordinates Register 1 (F_CURY1)
  _bus->DataWrite(Hy >> 8);                      //Text Write Y-coordinate [12:8]
}


//================================================================================
//
// [0x67] Draw Line / Triangle Control Register 0 (DCR0)
//
//================================================================================


/**
 * @brief Enable/Disable Drawing
 * 
 * @verbatim
 * REG [0x67] Draw Line / Triangle Control Register 0 (DCR0)
 * bit [0] Must set 0
 * @endverbatim
 * 
 * @param b: true habilita, false: desabilita a linha de desenhos
 *
 * @note Não está documentado no manual. indica apenas que o bit[0] precisa 
 *       ser zero. Não existe descrição para o RA8875/RA8876/RA8877/RA8889
 *       Com base na experiência de programadores.
 */ 
void RA8889::DrawEnable_AA(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR0);           //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = _bus->DataRead();
  b ? SETB(temp,0) : CLRB(temp,0);
  _bus->DataWrite(temp);
}


/**
 * @brief Ativa o Modo de desenho de Linha
 *
 * @verbatim
 * Draw Line Start Signal Write Function
 *
 * REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *          bit [7] Draw Line / Triangle Start Signal 
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0 : Drawing function complete.
 *                  0b1 : Drawing function is processing.
 *          bit [1] Draw Triangle or Line Select Signal
 *                  0b0: Draw Line
 *                  0b1: Draw Triangle
 * @endverbatim
 *
 * @param None
 *
 * @note Antes de executar esta função precisa entrar com os valores em registradores de coordenadas
 *       através das funções Point1_XY(), Point2_XY().
 */ 
void RA8889::LineMode_Start(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR0);                      //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Start draw function
  CLRB(temp,1);                                //Reset bit 1, Select Draw Line
  _bus->DataWrite(temp);                         
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de Triangulo
 *        
 * @verbatim
 * Draw Triangle Start Signal Write Function Non-Fill
 *
 * REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *          bit [7] Draw Line / Triangle Start Signal 
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0 : Drawing function complete.
 *                  0b1 : Drawing function is processing.
 *          bit [5] Fill function for Triangle Signal
 *                  0b0: Non fill.
 *                  0b1: Fill 
 *          bit [1] Draw Triangle or Line Select Signal
 *                  0b0: Draw Line
 *                  0b1: Draw Triangle
 * @endverbatim
 *
 * @param None
 *
 * @note Antes de executar esta função precisa entrar com os valores em registradores de coordenadas
 *       através das funções Point1_XY(), Point2_XY() e Point3_XY().
*/ 
void RA8889::TriangleMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR0);                      //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Draw Triangle
  fill ? SETB(temp,5) : CLRB(temp,5);          //Set bit 5, Com preenchimento do triangulo
  SETB(temp,1);                                //Set bit 1, Select Draw Triangle
  _bus->DataWrite(temp);
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


//================================================================================
//
// [0x68] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
// [0x69] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
// [0x6a] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
// [0x6b] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
// [0x6c] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
// [0x6d] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
// [0x6e] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
// [0x6f] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
// [0x70] Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
// [0x71] Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
// [0x72] Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
// [0x73] Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
//
//================================================================================


/** OK
 * @brief Line Start Point
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void RA8889::Point1_XY(uint16_t wx, uint16_t hy)
{
  _bus->CmdWrite(REG_DLHSR0);                    //0x68, Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
  _bus->DataWrite(wx);                           
  _bus->CmdWrite(REG_DLHSR1);                    //0x69, Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
  _bus->DataWrite(wx >> 8);                      
  _bus->CmdWrite(REG_DLVSR0);                    //0x6a, Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
  _bus->DataWrite(hy);                           
  _bus->CmdWrite(REG_DLVSR1);                    //0x6b, Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
  _bus->DataWrite(hy >> 8);                      
}
void RA8889::Line_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }


/** OK
 * @brief Line End Point
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void RA8889::Point2_XY(uint16_t wx, uint16_t hy)
{
  _bus->CmdWrite(REG_DLHER0);                    //0x6c, Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
  _bus->DataWrite(wx);                           //
  _bus->CmdWrite(REG_DLHER1);                    //0x6d, Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
  _bus->DataWrite(wx >> 8);                      //
  _bus->CmdWrite(REG_DLVER0);                    //0x6e, Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
  _bus->DataWrite(hy);                           //
  _bus->CmdWrite(REG_DLVER1);                    //0x6f, Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
  _bus->DataWrite(hy >> 8);                      //
}
void RA8889::Line_Point2XY(uint16_t wx, uint16_t hy) { Point2_XY(wx, hy); }


/**
 * @brief Triangle Point 1
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void RA8889::Triangle_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }


/**
 * @brief Triangle Point 2
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void RA8889::Triangle_Point2XY(uint16_t wx, uint16_t hy)  { Point2_XY(wx, hy); }


/**
 * @brief Triangle Point 3
 *
 * @verbatim
 *        REG [70h] Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
 *                  bit [7~0] Draw Triangle Point 3 X-coordination [7:0]
 *        REG [71h] Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
 *                  bit [7~0] Draw Triangle Point 3 X-coordination [12:8]
 *        REG [72h] Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
 *                  bit [7~0] Draw Triangle Point 3 Y-coordination [7:0]
 *        REG [73h] Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
 *                  bit [7~0] Draw Triangle Point 3 Y-coordination [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void RA8889::Point3_XY(uint16_t wx, uint16_t hy)
{
  _bus->CmdWrite(REG_DTPH0);                     //0x70, Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
  _bus->DataWrite(wx);                           
  _bus->CmdWrite(REG_DTPH1);                     //0x71, Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
  _bus->DataWrite(wx >> 8);                      
  _bus->CmdWrite(REG_DTPV0);                     //0x72, Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
  _bus->DataWrite(hy);                           
  _bus->CmdWrite(REG_DTPV1);                     //0x73, Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
  _bus->DataWrite(hy >> 8);                      
}
void RA8889::Triangle_Point3XY(uint16_t wx, uint16_t hy) { Point3_XY(wx, hy); }


/**
 * @brief Square Start Point
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void RA8889::Square_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }


/**
 * @brief Square End Point
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void RA8889::Square_Point2XY(uint16_t wx, uint16_t hy) { Point2_XY(wx, hy); }


//================================================================================
//
// [0x76] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
//
//================================================================================


/**
 * @brief Ativa o Modo de desenho de Circulo / Elipse
 *        
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche figura, false não preenche figura
 *
 * @note None
 */
void RA8889::CircleMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp &= ~(cSetb5 | cSetb4);                  //Reset bit 5-4, Draw Circle / Ellipse
  temp &= ~(cSetb1 | cSetb0);                  //Reset bit 1-0, bottom-left Ellipse Curve
  _bus->DataWrite(temp);                         //0b1n00 xx00, n=0/1
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}
void RA8889::EllipseMode_Start(bool fill) { CircleMode_Start(fill); }


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Abaixo
 *
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::CurveLeftDownMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  CLRB(temp,5);                                //Reset bit 5, Draw Circle / Ellipse Curve   
  SETB(temp,4);                                //Set bit 4, Draw Circle / Ellipse Curve
  temp &= ~(cSetb1 | cSetb0);                  //Reset bit 1-0, bottom-left Ellipse Curve
  _bus->DataWrite(temp);                         //0b1n01 xx00   n=1/0 
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Acima
 *
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::CurveLeftUpMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  CLRB(temp,5);                                //Reset bit 5, Draw Circle / Ellipse Curve   
  SETB(temp,4);                                //Set bit 4, Draw Circle / Ellipse Curve
  CLRB(temp,1);                                //Reset bit 1, upper-left Ellipse Curve  
  SETB(temp,0);                                //Set bit 0, upper-left Ellipse Curve 
  _bus->DataWrite(temp);                         //0b1n01 xx01   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Direita e Acima
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::CurveRightUpMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead(); 
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  CLRB(temp,5);                                //Reset bit 5, Draw Circle / Ellipse Curve   
  SETB(temp,4);                                //Set bit 4, Draw Circle / Ellipse Curve
  SETB(temp,1);                                //Set bit 1, upper-right Ellipse Curve
  CLRB(temp,0);                                //Reset bit 0, upper-right Ellipse Curve
  _bus->DataWrite(temp);                         //0b1n01 xx10   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Abaixo
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::CurveRightDownMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();  
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  CLRB(temp,5);                                //Reset bit 5, Draw Circle / Ellipse Curve   
  SETB(temp,4);                                //Set bit 4, Draw Circle / Ellipse Curve
  SETB(temp,1);                                //Set bit 1, bottom-right Ellipse Curve
  SETB(temp,0);                                //Set bit 0, bottom-right Ellipse Curve
  _bus->DataWrite(temp);                         //0b1n01 xx11   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/** OK
 * @brief Ativa o Modo de desenho de quadrado
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::SquareMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  SETB(temp,5);                                //Set bit 5, Draw Square.
  CLRB(temp,4);                                //Reset bit 4, Draw Square.
  _bus->DataWrite(temp);                         //0b1n10 xxxx   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular no canto quadrado
 *        
 * @verbatim  
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim  
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void RA8889::CircleSquareMode_Start(bool fill)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = _bus->DataRead();  
  SETB(temp,7);                                //Set bit 7, Start the drawing function
  fill ? SETB(temp,6) : CLRB(temp,6);          //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  SETB(temp,5);                                //Set bit 5, Draw Circle Square
  SETB(temp,4);                                //Set bit 4, Draw Circle Square
  _bus->DataWrite(temp);                         //0b1n11 xxxx   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


//================================================================================
//
// [0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
// [0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
// [0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
// [0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
// [0x7b] Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
// [0x7c] Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
// [0x7d] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
// [0x7e] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)
//
//================================================================================


/**
 * @brief Raio do círculo                           Rx = Ry
 *        Raio da elipse                            Rx > Ry or Rx < Ry
 *        Raio da curva circular no canto quadrado  Rx = Ry, Rx > Ry or Rx < Ry
 *
 * @verbatim
 * REG[0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
 *           bit [7~0] Draw Circle/Ellipse/Circle Square Major radius [7:0]
 * REG[0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
 *           bit [4~0] Draw Circle/Ellipse/Circle Square Major radius [12:8]
 *
 * Unit: Pixel
 * To draw a circle needs to set major axis equal to minor radius.
 * 
 * REG[0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
 *           bit [7~0] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
 * REG[0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
 *           bit [4~0] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
 *
 * Unit: Pixel
 * To draw a circle needs to set major axis equal to minor radius.
 * @endverbatim
 *
 * @param radius: raio do circulo
 *
 * @note No criculo o R = Rx = Ry e na elipse Rx > Ry ou Rx < Ry
 */
void RA8889::Radius_RxRy(uint16_t Rx, uint16_t Ry)
{
  _bus->CmdWrite(REG_ELL_A0);                    //0x77, Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
  _bus->DataWrite(Rx);                           //
  _bus->CmdWrite(REG_ELL_A1);                    //0x78, Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
  _bus->DataWrite(Rx >> 8);                      //
  
  _bus->CmdWrite(REG_ELL_B0);                    //0x79, Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
  _bus->DataWrite(Ry);                           //
  _bus->CmdWrite(REG_ELL_B1);                    //0x7a, Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
  _bus->DataWrite(Ry >> 8);                      //
}
void RA8889::CircleRadius_R(uint16_t R) { Radius_RxRy(R, R); }
void RA8889::EllipseRadius_RxRy(uint16_t Rx, uint16_t Ry) { Radius_RxRy(Rx, Ry); }
void RA8889::CircleSquareRadius_RxRy(uint16_t Rx, uint16_t Ry) { Radius_RxRy(Rx, Ry); }


/**
 * @brief Posição do Centro do Círculo/Elipse/Circulo do quadrado
 *
 * @verbatim
 *        REG[0x7b] Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Center X-coordinates [7:0]
 *        REG[0x7c] Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Center X-coordinates [12:8]
 *        REG[0x7d] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Center Y-coordinates [7:0]
 *        REG[0x7e] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Center Y-coordinates [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel
 * @endverbatim
 *
 * @param Wx, Hy: coordenada central (x,y)
 *
 * @note 
 */
 void RA8889::Center_XY(uint16_t Wx, uint16_t Hy)
 {
  _bus->CmdWrite(REG_DEHR0);      //0x7b, Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
  _bus->DataWrite(Wx);            //
  _bus->CmdWrite(REG_DEHR1);      //0x7c, Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR1)
  _bus->DataWrite(Wx >> 8);       //
						   
  _bus->CmdWrite(REG_DEVR0);      //0x7d, Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
  _bus->DataWrite(Hy);            //
  _bus->CmdWrite(REG_DEVR1);      //0x7e, Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR1)
  _bus->DataWrite(Hy >> 8);       //
 }
void RA8889::CircleCenter_XY(uint16_t Wx, uint16_t Hy) {Center_XY(Wx, Hy);}
void RA8889::EllipseCenter_XY(uint16_t Wx, uint16_t Hy) {Center_XY(Wx, Hy);}


//================================================================================
//
// [0x84] PWM Prescaler Register (PSCLR)
//
//================================================================================


/**
 * @brief PWM Prescaler 1 to 256
 *
 * @verbatim
 * REG[0x84] PWM Prescaler Register (PSCLR)
 *           bit [7~0] PWM Prescaler Register
 *           These 8 bits determine prescaler value for Timer 0 and 1.
 *           Time base is “Core_Freq / (Prescaler + 1)”
 *           Fcore: Core Frequency
 * @endverbatim
 *
 * @param Wx, Hy: coordenada central (x,y)
 *
 * @note These 8 bits determine prescaler value for Timer 0 and 1
 */
 void RA8889::PWM_Prescaler(uint8_t prescaler)
{
  prescaler = prescaler - 1;                   //0..255
  _bus->CmdWrite(REG_PSCLR);                     //0x84, PWM Prescaler Register (PSCLR)
  _bus->DataWrite(prescaler);
}


//================================================================================
//
// [0x85] PWM clock Mux Register (PMUXR)
//
//================================================================================


/**
 * @brief Select MUX input for PWM Timer 1.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *           bit [7-6] Select 2nd clock divider’s MUX input for PWM Timer 1
 *                     0b00 = 1
 *                     0b01 = 1/2
 *                     0b10 = 1/4
 *                     0b11 = 1/8
 * @endverbatim
 *
 * @param eDividerClock::X1
 *        eDividerClock::X2
 *        eDividerClock::X4
 *        eDividerClock::X8
 *
 * @note Selecione a entrada MUX do 2º divisor de clock para o PWM Timer 1.
 */
void RA8889::PWM1_ClockDividedBy(eDividerClock divider)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Rest bit 7 and 6
  temp |=  static_cast<uint8_t>(divider);
  _bus->DataWrite(temp);
}


/**
 * @brief Select MUX input for PWM Timer 0.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *           bit [5-4] Select 2nd clock divider’s MUX input for PWM Timer 0
 *                     0b00 = 1/1
 *                     0b01 = 1/2
 *                     0b10 = 1/4
 *                     0b11 = 1/8
 * @endverbatim
 *
 * @param eDividerClock::X1
 *        eDividerClock::X2
 *        eDividerClock::X4
 *        eDividerClock::X8
 *
 * @note Selecione a entrada MUX do 2º divisor de clock para o PWM Timer 0.
 */
void RA8889::PWM0_ClockDividedBy(eDividerClock divider)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb5 | cSetb4);                  //Reset bit 5 and 4
  temp |=  static_cast<uint8_t>(divider);
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[1] para atuar como saída do sinal de indicação de erro.
 *
 *        Esta função ajusta o registrador PMUXR (0x85), bits [3-2], de modo que o pino XPWM[1]
 *        seja direcionado para indicar condições de erro detectadas pelo RA8889,
 *        como insuficiência de dados no FIFO de largura de banda de varredura
 *        ou acesso de memória fora do intervalo permitido (REG[00h] bit[1:0]).
 *        
 *        Ao selecionar esta opção, o XPWM[1] deixa de funcionar como saída de PWM/clock
 *        e passa a refletir diretamente o estado do "System Error Flag".
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [3-2] XPWM[1] pin function control
 *                      0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *                      0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *                      0b11: XPWM[1] output oscillator clock
 *            If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note Se o bit XTEST[0] estiver configurado como alto, o pino XPWM[1] é forçado
 *       a atuar como entrada de clock de varredura do painel, independentemente desta configuração.
 */
void RA8889::PWM1_Select_ErrorFlag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  CLRB(temp,3);                                //Reset bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Select Output Oscillator PWM Timer 1
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 * bit [3-2] XPWM[1] pin function control
 *           0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *           0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *           0b11: XPWM[1] output oscillator clock
 * If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::PWM1_Select(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  SETB(temp,3);                                //Set bit 3 
  CLRB(temp,2);                                //Reset bit 2 
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[1] para atuar como uma saída do clock do oscilador interno.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [3-2] XPWM[1] pin function control
 *                      0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *                      0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *                      0b11: XPWM[1] output oscillator clock
 *            If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::PWM1_Select_OscillatorClock(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  SETB(temp,3);                                //Set bit 3
  SETB(temp,2);                                //Set bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[0] para atuar como um GPIO-C[7].
 *        
 *        Ajusta o registrador PMUXR (0x85), bits [1-0], para multiplexar o pino XPWM[0]
 *        como um pino de propósito geral (GPIO) ao invés de saída PWM ou clock.
 *
 *        Esse modo é utilizado quando você precisa controlar XPWM[0] manualmente
 *        como pino digital, por exemplo para sinais de controle ou I/O externo.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::PWM0_Select_GPIOC7(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  CLRB(temp,1);                                //Reset bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Select Output Oscillator PWM Timer 0
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::PWM0_Select(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  SETB(temp,1);                                //Set bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[0] para atuar como uma saída do clock interno do núcleo (core clock).
 *
 *        Ajusta o registrador PMUXR (0x85), bits [1-0], para que o pino XPWM[0] seja
 *        multiplexado MUX como saída do sinal de clock principal do RA8889, em vez de
 *        funcionar como GPIO-C[7] ou como saída do PWM Timer 0.
 *
 *        Esse modo é normalmente utilizado para depuração ou como referência de clock
 *        para dispositivos externos. 
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
  * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::PWM0_Select_CoreClock(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = _bus->DataRead();                       
  SETB(temp,1);                                //Set bit 1
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x86] PWM Configuration Register (PCFGR)
//
//================================================================================


/**
 * @brief Ativa ou desativa o inversor de saída do PWM Timer 1 (XPWM[1]).
 *
 * @verbatim
 * Ajusta o registrador PCFGR (0x86), bit 6, para definir se a saída do PWM1
 * será invertida ou não.  
 *
 *
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [6] PWM Timer 1 output inverter on/off
 *            Determine the output inverter on/off for Timer 1.        
 *                    0b0: Inverter off
 *                    0b1: Inverter on for PWM1
 * @endverbatim
 *
 * @param on
 *        true  - Ativa o inversor (PWM1 invertido)
 *        false - Desativa o inversor (PWM1 normal)
 * @note O inversor afeta apenas a forma de onda do PWM Timer 1 na saída XPWM[1].
 */
void RA8889::PWM1_InverterOn(boolean on)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  on ? SETB(temp,6) : CLRB(temp,6);            //Set/Reset bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o modo Auto Reload PWM Timer 1
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [5] PWM Timer 1 auto reload on/off
 *            Determine auto reload on/off for Timer 1.        
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void RA8889::PWM1_Select_AutoReload(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  SETB(temp,5);                                //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o modo One Shot PWM Timer 1
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [5] PWM Timer 1 auto reload on/off
 *            Determine auto reload on/off for Timer 1.
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void RA8889::PWM1_Select_OneShot(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  CLRB(temp,5);                                //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Inicia o PWM Timer 1 (XPWM[1]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 4, para iniciar o PWM Timer 1.
 *        Em modo Interval (auto reload), o timer continuará rodando até que o bit seja zerado.
 *        Em modo One-shot, o bit é limpo automaticamente ao final do ciclo.
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [4] PWM Timer 1 start/stop
 *            Determine start/stop for Timer 1.        
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note É possível ler o mesmo bit para verificar se o Timer 1 está ativo ou parado.
 */
void RA8889::PWM1_StartTimer(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Para o PWM Timer 1 (XPWM[1]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 4, para parar o PWM Timer 1.
 *        Em modo Interval (auto reload), o timer será interrompido até que o bit seja novamente setado.  
 *        Em modo One-shot, o bit se limpa automaticamente ao final do ciclo.  
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [4] PWM Timer 1 start/stop
 *            Determine start/stop for Timer 1.        
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note É possível ler o mesmo bit para verificar se o Timer 1 está ativo ou parado.
 */
void RA8889::PWM1_StopTimer(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  CLRB(temp,4);                                //Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Habilita ou desabilita a Dead Zone do PWM Timer 0.
 *
 *        Ajusta o registrador PCFGR (0x86), bit 3, para ativar ou desativar
 *        o intervalo de Dead Zone no PWM Timer 0.
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [3] PWM Timer 0 Dead zone enable
 *            Determine the dead zone operation.       
 *                    0b0: Disable
 *                    0b1: Enable
 * @endverbatim
 *
 * @param None
 * @note Quando habilitado, há um curto período em que ambos os sinais complementares
 *       do PWM ficam desligados, prevenindo condução simultânea em H-bridges ou MOSFETs.
 */
void RA8889::PWM0_DeadZoneEnable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  b ? SETB(temp,3) : CLRB(temp,3);             //Set/Reset bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Ativa ou desativa o inversor de saída do PWM Timer 0 (XPWM[0]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 2, para definir se a saída do PWM0
 *        será invertida ou não.
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [2] PWM Timer 0 output inverter on/off
 *            Determine the output inverter on/off for Timer 0.        
 *                    0b0: Inverter off
 *                    0b1: Inverter on for PWM0
 * @endverbatim
 *
 * @param on
 *        true  - Ativa o inversor (PWM1 invertido)
 *        false - Desativa o inversor (PWM1 normal)
 * @note O inversor afeta apenas a forma de onda do PWM Timer 0 na saída XPWM[0].
 */
void RA8889::PWM0_InverterOn(bool on)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  on ? SETB(temp,2) : CLRB(temp,2);            //Set/Reset bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o modo Auto Reload PWM Timer 0
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 * bit [1] PWM Timer 0 auto reload on/off
 * Determine auto reload on/off for Timer 0.        
 *         0b0: One-shot
 *         0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void RA8889::PWM0_Select_AutoReload(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  SETB(temp,1);                                //Set bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o modo One Shot PWM Timer 0
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 * bit [1] PWM Timer 0 auto reload on/off
 * Determine auto reload on/off for Timer 0.
 *         0b0: One-shot
 *         0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void RA8889::PWM0_Select_OneShot(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  CLRB(temp,1);                                //Reset bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief Inicia o PWM Timer 0 (XPWM[0]).
 *
 *        Configura o bit [0] do registrador PCFGR (0x86) para "1",
 *        habilitando o funcionamento do PWM Timer 0.  
 *        O Timer 0 pode ser roteado para o pino XPWM[0] (dependendo da configuração
 *        do registrador PMUXR) e passa a gerar o sinal PWM de acordo com os
 *        parâmetros previamente configurados (período, duty cycle, etc.).
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [0] PWM Timer 0 start/stop
 *            Determine start/stop for Timer 0.
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note Esta função apenas inicia o Timer; para realmente gerar sinal PWM,
 *       é necessário que os registradores de configuração de frequência,
 *       duty cycle e roteamento da saída já estejam ajustados.
 */
void RA8889::PWM0_StartTimer(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Para o PWM Timer 0 (XPWM[0]).
 *
 *        Configura o bit [0] do registrador PCFGR (0x86) para "0",
 *        habilitando o funcionamento do PWM Timer 0.  
 *        O Timer 0 pode ser roteado para o pino XPWM[0] (dependendo da configuração
 *        do registrador PMUXR) e passa a gerar o sinal PWM de acordo com os
 *        parâmetros previamente configurados (período, duty cycle, etc.).
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [0] PWM Timer 0 start/stop
 *            Determine start/stop for Timer 0.
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note Esta função apenas inicia o Timer; para realmente gerar sinal PWM,
 *       é necessário que os registradores de configuração de frequência,
 *       duty cycle e roteamento da saída já estejam ajustados.
 */
void RA8889::PWM0_StopTimer(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = _bus->DataRead();                       
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0x87] Timer 0 Dead zone length register [DZ_LENGTH]
//
//================================================================================


/**
 * @brief Configura o tempo morto (Dead Zone) do PWM Timer 0.
 *
 *@verbatim
 * REG [0x87] Timer 0 Dead zone length register [DZ_LENGTH]
 *            bit [7-0] Timer 0 Dead zone length register
 *            This byte is used to determine the dead zone length. The 1 unit 
 *            time of the dead zone length is equal to the whole cycle of 
 *            timer 0.
 * @endverbatim
 *
 * @param len: Número de ciclos de período do Timer 0 que definem a duração do 
 *        tempo morto. Faixa: 0–255.
 *
 * @note Length expressa que se trata da “duração” do tempo morto, em ciclos. 
 *       "Timer 0 Dead zone length" → isso é só um parâmetro adicional do PWM 
 *       Timer 0, que define o tempo do “gap” morto na comutação.
 */
void RA8889::PWM0_DeadZoneLength(uint8_t len)
{
  _bus->CmdWrite(REG_DZ_LENGTH);                 //0x87, Timer 0 Dead zone length register [DZ_LENGTH]
  _bus->DataWrite(len);
}


//================================================================================
//
// [0x88] Timer 0 compare buffer register [TCMPB0L]
// [0x89] Timer 0 compare buffer register [TCMPB0H]
//
//================================================================================


/**
 * @brief Configura o valor do Compare Buffer (Duty Cycle) do PWM Timer 0.
 *
 *@verbatim
 * REG [0x88] Timer 0 compare buffer register [TCMPB0L]
 * bit [7-0] Timer 0 compare buffer register --- Low Byte
 *     Compare buffer register is 16 bits in total. When timer counter 
 *     is eqaual to or less than compare buffer register, PWM 0 will 
 *     output high level if PWM Timer 0 output inverter on/off bit is 
 *     off.
 *
 * REG [0x89] Timer 0 compare buffer register [TCMPB0H]
 * bit [7-0] Timer 0 compare buffer register --- High Byte
 *     Compare buffer register is 16 bits in total. When timer counter 
 *     is eqaual to or less than compare buffer register, PWM 0 will 
 *     output high level if PWM Timer 0 output inverter on/off bit is 
 *     off.
 *
 * Como o Timer funciona com ele:
 *
 *   O PWM Timer 0 conta de 0 até o valor máximo do período configurado. A 
 *   cada ciclo:
 *
 *   - Se o contador ≤ valor do Compare Buffer, o pino XPWM[0] é colocado em 
 *     nível alto (ou invertido se o bit de inversão estiver ativo).
 *
 *   - Quando o contador ultrapassa o valor do Compare Buffer, o pino vai para 
 *     nível baixo.
 *
 *   Recebe Wx (0 a 65535) → quanto maior o valor, maior o tempo em que o PWM 
 *   fica em nível alto durante o ciclo. 
 *
 *   Em outras palavras: o Compare Buffer define o duty cycle do PWM.
 * @endverbatim
 *
 * @param duty Valor de 16 bits para o compare buffer do Timer 0.
 *
 * @example
 *       WX = 1024
 *       pwm0 = 7.5MHz/WX = 7.3KHz
 * 
 * @note When timer counter equal or less than compare buffer register will 
 *       cause PWM out high level if inv_on bit is off. 
 *       Este buffer determina o ponto em que o PWM muda de nível durante o ciclo.
 *       É útil para ajustar o duty cycle do PWM com precisão.
 */
void RA8889::PWM0_SetCompareBuffer(uint16_t duty)
{   
  _bus->CmdWrite(REG_TCMPB0L);                   //0x88, Timer 0 compare buffer register [TCMPB0L]
  _bus->DataWrite(duty);                           
  _bus->CmdWrite(REG_TCMPB0H);                   //0x89, Timer 0 compare buffer register [TCMPB0H]
  _bus->DataWrite(duty >> 8);                      
}
void RA8889::PWM0_Duty(uint16_t duty) {PWM0_SetCompareBuffer(duty);}

//================================================================================
//
// [0x8A] Timer 0 count buffer register [TCNTB0L]
// [0x8B] Timer 0 count buffer register [TCNTB0H]
//
//================================================================================


/**
 * @brief Set Clock per Period from Tiemr 0 (Count Buffer PWM Timer 0)
 *
 *@verbatim
 * REG [0x8a] Timer 0 count buffer register [TCNTB0L]
 *            bit [7-0] Timer 0 count buffer register --- Low Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, the PWM timer will reload the value of Count buffer register to the counter.
 *            The current value of the timer counter (TCNT0) can be read back when the PWM timer starts.
 *
 * REG [0x8b] Timer 0 count buffer register [TCNTB0H]
 *            bit [7-0] Timer 0 count buffer register --- High Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, the PWM timer will reload the value of Count buffer register to the counter.
 *            The current value of the timer counter (TCNT0) can be read back when the PWM timer starts.
 * @endverbatim
 *
 * @param clock_per_period
 *
 * @example
 *       pwm_timer_clock = 7.5MHz
 *       clock_per_period = 1024
 *       pwm0 = pwm_timer_clock / clock_per_period = 7.3KHz
 * 
 * @note Count buffer register total has 16 bits.
 *       When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
 *       It may read back timer counter's real time value when PWM timer start.
 * 
 */
void RA8889::PWM0_SetCountBuffer(uint16_t clock_per_period)
{
  _bus->CmdWrite(REG_TCNTB0L);                   //0x8a, Timer 0 count buffer register [TCNTB0L]
  _bus->DataWrite(clock_per_period);             
  _bus->CmdWrite(REG_TCNTB0H);                   //0x8b, Timer 0 count buffer register [TCNTB0H]
  _bus->DataWrite(clock_per_period >> 8);        
}
void RA8889::PWM0_ClocksPerPeriod(uint16_t clock_per_period) {PWM0_SetCountBuffer(clock_per_period);}


//================================================================================
//
// [0x8C] Timer 1 compare buffer register [TCMPB1L]
// [0x8D] Timer 1 compare buffer register [TCMPB1H]
//
//================================================================================


/**
 * @brief Configura o valor do Compare Buffer (Duty Cycle) do PWM Timer 1.
 *
 *@verbatim
 * REG [0x8c] Timer 1 compare buffer register [TCMPB1L]
 *            bit [7-0] Compare buffer register is 16 bits in total. When timer counter is equal to or less than the value of compare buffer register and inv_on bit is off, PWM will output high level.
 *
 * REG [0x8d] Timer 1 compare buffer register [TCMPB1H]
 *            bit [7-0] Timer 0 compare buffer register --- High Byte
 *            Compare buffer register is 16 bits in total. When timer counter is equal to or less than the value of compare buffer register and inv_on bit is off, PWM will output high level.
 *
 * Como o Timer funciona com ele:
 *
 *   O PWM Timer 1 conta de 0 até o valor máximo do período configurado. A 
 *   cada ciclo:
 *
 *   - Se o contador ≤ valor do Compare Buffer, o pino XPWM[1] é colocado em 
 *     nível alto (ou invertido se o bit de inversão estiver ativo).
 *
 *   - Quando o contador ultrapassa o valor do Compare Buffer, o pino vai para 
 *     nível baixo.
 *
 *   Recebe Wx (0 a 65535) → quanto maior o valor, maior o tempo em que o PWM 
 *   fica em nível alto durante o ciclo. 
 *
 *   Em outras palavras: o Compare Buffer define o duty cycle do PWM.
 * @endverbatim
 *
 * @param duty Valor de 16 bits para o compare buffer do Timer 1.
 *
 * @example
 *       WX = 1024
 *       pwm0 = 7.5MHz/WX = 7.3KHz
 * 
 * @note When timer counter equal or less than compare buffer register will 
 *       cause PWM out high level if inv_on bit is off. 
 *       Este buffer determina o ponto em que o PWM muda de nível durante o ciclo.
 *       É útil para ajustar o duty cycle do PWM com precisão.
 */
void RA8889::PWM1_SetCompareBuffer(uint16_t duty)
{
  _bus->CmdWrite(REG_TCMPB1L);                   //0x8c, Timer 1 compare buffer register [TCMPB1L]
  _bus->DataWrite(duty);                         
  _bus->CmdWrite(REG_TCMPB1H);                   //0x8d, Timer 1 compare buffer register [TCMPB1H]
  _bus->DataWrite(duty >> 8);                    
}
void RA8889::PWM1_Duty(uint16_t duty) {PWM1_SetCompareBuffer(duty);}

//================================================================================
//
// [0x8E] Timer 1 count buffer register [TCNTB1L]
// [0x8F] Timer 1 count buffer register [TCNTB1H]
//
//================================================================================


/**
 * @brief Set Clock per Period from Tiemr 1 (Count Buffer PWM Timer 1)
 *
 *@verbatim
 * REG [0x8e] Timer 1 count buffer register [TCNTB1L]
 *            bit [7-0] Timer 1 count buffer register --- Low Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, PWM timer will reload the value of Count buffer register.
 *            The current value of the timer counter (TCNT1) can be read back when the PWM timer starts.
 *
 * REG [0x8f] Timer 1 count buffer register [TCNTB1H]
 *            bit [7-0] Timer 1 count buffer register --- High Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, PWM timer will reload the value of Count buffer register.
 *            The current value of the timer counter (TCNT1) can be read back when the PWM timer starts.
 * @endverbatim
 *
 * @param clock_per_period
 *
 * @example
 *       pwm_timer_clock = 7.5MHz
 *       clock_per_period = 1024
 *       pwm0 = pwm_timer_clock / clock_per_period = 7.3KHz
 *
 * @note Count buffer register total has 16 bits.
 *       When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
 *       It may read back timer counter's real time value when PWM timer start.
 */
void RA8889::PWM1_SetCountBuffer(uint16_t clock_per_period)
{
  _bus->CmdWrite(REG_TCNTB1L);                   //0x8e, Timer 1 count buffer register [TCNTB1L]
  _bus->DataWrite(clock_per_period);             
  _bus->CmdWrite(REG_TCNTB1H);                   //0x8f, Timer 1 count buffer register [TCNTB1H]
  _bus->DataWrite(clock_per_period >> 8);        
}
void RA8889::PWM1_ClocksPerPeriod(uint16_t clock_per_period) {PWM1_SetCountBuffer(clock_per_period);}


//================================================================================
//
// [0x90] BTE Function Control Register 0 (BTE_CTRL0)
//
//================================================================================


/**
 * @brief BTE function Enable/Disable
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [4] BTE Function Enable / Status
 *            Write:
 *            0: BTE function is disabled.
 *            1: BTE function is enabled.
 *            Read:
 *            0: BTE function is idle.
 *            1: BTE function is busy.
 *
 *            Note: When BTE function is enabled, it’s not allowed to access 
 *            the memory space of the active window in canvas.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_Enable(bool b)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_CTRL0);                 //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = _bus->DataRead();
  b ? SETB(temp,4) : CLRB(temp,4);             //Set/Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Wait until Core Task (BTE) is Ready
 *
 *@verbatim
 * Status Register (STSR)
 * 
 * bit [3] Core task is busy (fontwr_busy)
 *        Following task is running:
 *        BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
 *        0: task is done or idle.
 *        1: task is busy.
 *        While User change canvas relative setting & switch text 
 *        mode or graphic mode must make sure core task is done. 
 *        Note: BTE, Geometry drawing & Serial flash DMA also may 
 *        check each start bit. Under text mode, if user wants to 
 *        change rotate attribute, character line gap, 
 *        character-to-character space, foreground color, 
 *        background color and Text/graphic mode setting, he must 
 *        make sure core_busy (fontwr_busy) status bit is low.
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8876/RA8877/RA8889
 */
void RA8889::BTE_WaitReady(void) 
{
    while (_bus->StatusRead() & 0x08) delayMicroseconds(1);
}


/**
 * @brief Wait until Core Task (BTE) is Ready
 *
 * @verbatim
 * Status Register (STSR)
 * 
 * bit [3] Core task is busy (fontwr_busy)
 *        Following task is running:
 *        BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
 *        0: task is done or idle.
 *        1: task is busy.
 *        While User change canvas relative setting & switch text 
 *        mode or graphic mode must make sure core task is done. 
 *        Note: BTE, Geometry drawing & Serial flash DMA also may 
 *        check each start bit. Under text mode, if user wants to 
 *        change rotate attribute, character line gap, 
 *        character-to-character space, foreground color, 
 *        background color and Text/graphic mode setting, he must 
 *        make sure core_busy (fontwr_busy) status bit is low.
 *
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [4] BTE Function Enable / Status
 *            Write:
 *            0: BTE function is disabled.
 *            1: BTE function is enabled.
 *            Read:
 *            0: BTE function is idle.
 *            1: BTE function is busy.
 *
 *            Note: When BTE function is enabled, it’s not allowed to access 
 *            the memory space of the active window in canvas.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8876/RA8877/RA8889. Block Transfer Engine (BTE)
 */
void RA8889::BTE_DualWaitReady(void) 
{
  uint8_t temp;
  
  //Case1: using BTE Function Control Register
  
  do {
    _bus->CmdWrite(REG_BTE_CTRL0);               //0x090, Seleciona o registro BTE Function Control
    temp = _bus->DataRead();                     //
  	delayMicroseconds(1);                      //Libera CPU parcialmente
  } while (temp & 0x10);                       //Continua enquanto BTE estiver ocupado
  
  //Case2: using STSR Status Register
  
  do {
    temp = _bus->StatusRead();                 //Ler STSR (status geral do core) 
  	delayMicroseconds(1);                      //Libera CPU parcialmente
  } while (temp & 0x08);                       //Continua enquanto o core estiver ocupado
}


/**
 * @brief Pattern Format 8x8
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [0] PATTERN Format
 *            0b0: 8x8
 *            0b1: 16x16
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_PatternFormat8X8(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_CTRL0);          //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = _bus->DataRead();
  CLRB(temp,0);                         //Reset bit 0
  _bus->DataWrite(temp);
} 


/**
 * @brief Pattern Format 16x16
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [0] PATTERN Format
 *            0b0: 8x8
 *            0b1: 16x16
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_PatternFormat16X16(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_CTRL0);          //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = _bus->DataRead();
  SETB(temp,0);                         //Set bit 0
  _bus->DataWrite(temp);
} 


//================================================================================
//
// [0x91] BTE Function Control Register1 (BTE_CTRL1)
//
//================================================================================


/**
 * @brief BTE ROP Code
 *
 * @verbatim
 * REG [0x91] BTE Function Control Register1 (BTE_CTRL1)
 *            bit [7-4] BTE ROP Code Bit[3:0] or color expansion starting bit
 *            
 *            a. ROP is the acronym for Raster Operation. Some BTE operations 
 *               can be combined with ROP operations. (Please refer to Section 
 *               2.7)
 *
 *            Code        Description              enum eROPCode
 *            ----------------------------------------------     
 *            0b0000 0x0  0 ( Blackness )          Blackness
 *            0b0001 0x1  ~S0・~S1 or ~ ( S0+S1 )   NotS0_And_NotS1
 *            0b0010 0x2  ~S0・S1                   NotS0_And_S1
 *            0b0011 0x3  ~S0                      NotS0
 *            0b0100 0x4  S0・~S1                   S0_And_NotS1
 *            0b0101 0x5  ~S1                      NotS1
 *            0b0110 0x6  S0^S1                    S0_Xor_S1
 *            0b0111 0x7  ~S0+~S1 or ~ ( S0・S1 )   NotS0_Or_NotS1
 *            0b1000 0x8  S0・S1                    S0_And_S1
 *            0b1001 0x9  ~ ( S0^S1 )              Not_S0_Xor_S1
 *            0b1010 0xa  S1                       S1
 *            0b1011 0xb  ~S0+S1                   NotS0_Or_S1
 *            0b1100 0xc  S0                       S0
 *            0b1101 0xd  S0+~S1                   S0_Or_NotS1
 *            0b1110 0xe  S0+S1                    S0_Or_S1
 *            0b1111 0xf  1 ( Whiteness )          Whiteness
 *
 *            b. If BTE operation code function are color expansion with or 
 *               without chroma key (08h / 09h / Eh / Fh), then these bits 
 *               stand for starting bit on BTE window left boundary. MSB 
 *               stands for left most pixel. For 8-bits MPU, value should 
 *               within 0 to 7. For 16-bits MPU, value should within 0 to 15.
 * @endverbatim
 *
 * @param code
 *
 * @note ROP is the acronym for Raster Operation.
 */
void RA8889::BTE_ROPCode(eROPCode code)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_CTRL1);                 //BTE Function Control Register1 (BTE_CTRL1)
  temp = _bus->DataRead();                       
  temp &= 0x0f;                                //Limpar os bits [7-4]
  temp |= (static_cast<uint8_t>(code) << 4);   
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Operation Code
 *
 * @verbatim
 * REG [0x91] BTE Function Control Register1 (BTE_CTRL1)
 *            bit [3-0] BTE Operation Code Bit[3:0]
 *            RA8889 builds in 2D BTE Engine which provides 13 BTE functions. Some of BTE Operations can be combined with the
 *            0b0000 MPU Write with ROP.
 *              S0: comes from MPU data
 *              S1: comes from memory
 *              D:  According to ROP write to memory
 *            0b0001 Reserved
 *            0b0010 Memory Copy with ROP
 *              S0: comes from memory.
 *              S1: comes from memory
 *              D:  According to ROP Write to memory
 *            0b0011 Reserved
 *            0b0100 MPU Write w/ chroma keying (w/o ROP)
 *              S0: comes from MPU data
 *              If MPU data doesn’t match with chroma key color (specified by background color) then writes to destination.
 *            0b0101 Memory Copy (move) w/ chroma keying (w/o ROP)
 *              S0 comes from memory, and S1 is useless.
 *              If S0 data doesn’t match with chroma key color (specified by background color) then S0 data will write to destination.
 *            0b0110 Pattern Fill with ROP
 *              Pattern is specified by S0.
 *            0b0111 Pattern Fill with chroma keying
 *              Pattern is specified by S0.
 *              If S0 data doesn’t match with chroma key color (specified by background color) then writes to destination.
 *            0b1000 MPU Write w/ Color Expansion
 *              S0 comes from MPU data and convert to specified color & color depth then write to destination.
 *            0b1001 MPU Write w/ Color Expansion and chroma keying
 *              S0 comes from MPU data and If the data bit is “1” then convert to specified foreground color & color depth then write to destination.
 *            0b1010b Memory Copy with opacity
 *              S0, S1 & D: locate in memory
 *            0b1011 MPU Write with opacity
 *              S0: comes from MPU data
 *              S1: comes from memory
 *              D: According to Alpha blending operation write to memory
 *            0b1100 Solid Fill
 *              Destination data comes from register.
 *            0b1101 Reserved
 *            0b1110 Memory Copy w/ Color Expansion
 *              S0 & D locate in memory and S1 is useless
 *              S0 must be pre-loaded into memory with 8bpp or 16bpp color depth via MPU write or DMA, thus S0 color depth should follow that color depth.
 *            0b1111 Memory Copy w/ Color Expansion and chroma keying
 *              S0 & D locate in memory and S1 is useless.
 *              S0 must be pre-loaded into memory with 8bpp or 16bpp color depth via MPU write or DMA, thus S0 color depth should follow that color depth.
 *              If S0 data bit=0 then no data will be written into D. If S0 data bit=1 then foreground color data will be written to D.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_OperationCode(eBTEOpCode opcode)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_CTRL1);                 //BTE Function Control Register1 (BTE_CTRL1)
  temp = _bus->DataRead();                       
  temp &= 0xf0;                                //Limpa os bits de 3-0
  temp |= static_cast<uint8_t>(opcode);        //Coloca opcode
  _bus->DataWrite(temp);                         
}


//================================================================================
//
// [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Color Depth
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [6-5] S0 Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param bpp:
 *        eColorDepthBPP::bpp8
 *        eColorDepthBPP::bpp16
 *        eColorDepthBPP::bpp24
 *
 * @note None
 */
void RA8889::BTE_S0_ColorDeph(eColorDepthBPP bpp)
{ 
  uint8_t temp;
  uint8_t bit;
  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  temp &= ~(cSetb6 | cSetb5);                     //Reseta bits 6 e 5
  bit = static_cast<uint8_t>(bpp);
  bit = (bit >> 3)-1;                          //transforma 8,16,24 em 0, 1, 2
  temp |= bit << 5;                            //posiciona no bit 6 e 5
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Source 0 (S0) Color Depth 8 bpp
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [6-5] S0 Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S0_ColorDeph_8bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,6);                                   //Reset bits 6
  CLRB(temp,5);                                   //Reset bits 5
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Source 0 (S0) Color Depth 16 bpp
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [6-5] S0 Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S0_ColorDeph_16bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,6);                                   //Reset bits 6
  SETB(temp,5);                                   //Set bits 5
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Source 0 (S0) Color Depth 24 bpp
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [6-5] S0 Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S0_ColorDeph_24bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  SETB(temp,6);                                   //Set bits 6
  CLRB(temp,5);                                   //Reset bits 5
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 256 Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color (8bpp)
 *            0b001 : 64k Color (16bpp)
 *            0b010 : 16M Color (24bpp)
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_8bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  CLRB(temp,3);                                //Reset bit 3
  CLRB(temp,2);                                //Reset bit 2
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Source 1 (S1) Color Depth 64k Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_16bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                  //Reset bit 4
  CLRB(temp,3);                                  //Reset bit 3
  SETB(temp,2);                                  //Set bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 16M Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_24bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                   //Reset bit 4
  SETB(temp,3);                                   //Set bit 3
  CLRB(temp,2);                                   //Reset bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth Constant Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_Constant(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,4);                                   //Reset bit 4
  SETB(temp,3);                                   //Set bit 3
  SETB(temp,2);                                   //Set bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 8-bit Alpha Blend
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_8bitAlpha(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  SETB(temp,4);                                   //Set bit 4
  CLRB(temp,3);                                   //Reset bit 3
  CLRB(temp,2);                                   //Reset bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 16-bit Alpha Blend
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S1_ColorDeph_16bitAlpha(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  SETB(temp,4);                                   //Set bit 4
  CLRB(temp,3);                                   //Reset bit 3
  SETB(temp,2);                                   //Set bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief BTE Destination (D) Color Depth
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [1-0] Destination Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param bpp:
 *        eColorDepthBPP::bpp8  = 256 Color
 *        eColorDepthBPP::bpp16 = 64k Color  
 *        eColorDepthBPP::bpp24 = 16M Color
 *
 * @note None
 */
void RA8889::BTE_Destination_ColorDeph(eColorDepthBPP bpp)
{ 
  uint8_t temp;
  uint8_t bit;
  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb1 | cSetb0);                  //Reseta bits 1 e 0
  bit = static_cast<uint8_t>(bpp);             
  temp |= (bit >> 3)-1;                        //transforma 8,16,24 em 0, 1, 2
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Destination (D) Color Depth 16bpp
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [1-0] Destination Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_Destination_ColorDeph_16bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  CLRB(temp,1);                                   //Reset bits 1
  SETB(temp,0);                                   //Set bits 0
  _bus->DataWrite(temp);
} 


/**
 * @brief BTE Destination (D) Color Depth 24bpp
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [1-0] Destination Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_Destination_ColorDeph_24bpp(void)
{ 
  uint8_t temp;
  _bus->CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = _bus->DataRead();
  SETB(temp,1);                                   //Set bits 1
  CLRB(temp,0);                                   //Reset bits 0
  _bus->DataWrite(temp);
} 


//================================================================================
//
// [0x93] Source 0 memory start address 0 (S0_STR0)
// [0x94] Source 0 memory start address 1 (S0_STR1)
// [0x95] Source 0 memory start address 2 (S0_STR2)
// [0x96] Source 0 memory start address 3 (S0_STR3)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Memory Start Address
 *
 * @verbatim
 * REG [0x93] Source 0 memory start address 0 (S0_STR0)
 *            bit [7-2] Source 0 memory start address [7:2]
 *            bit [1-0] Fix at 0 [1:0]
 *
 * REG [0x94] Source 0 memory start address 1 (S0_STR1)
 *            bit [7-0] Source 0 memory start address [15:8]
 *
 * REG [0x95] Source 0 memory start address 2 (S0_STR2)
 *            bit [7-0] Source 0 memory start address [23:16]

 * REG [0x96] Source 0 memory start address 3 (S0_STR3)
 *            bit [7-0] Source 0 memory start address [31:24]
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void RA8889::BTE_S0_MemoryStartAddress(uint32_t addr)
{
  _bus->RegisterWrite(REG_S0_STR0, addr);            //0x93, Source 0 memory start address 0 (S0_STR0)
  _bus->RegisterWrite(REG_S0_STR1, addr >> 8);       //0x94, Source 0 memory start address 1 (S0_STR1)
  _bus->RegisterWrite(REG_S0_STR2, addr >> 16);      //0x95, Source 0 memory start address 2 (S0_STR2)
  _bus->RegisterWrite(REG_S0_STR3, addr >> 24);      //0x96, Source 0 memory start address 3 (S0_STR3)
}


//================================================================================
//
// [0x97] Source 0 image width 0 (S0_WTH0)
// [0x98] Source 0 image width 1 (S0_WTH1)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Image Width
 *
 * @verbatim
 * REG [0x97] Source 0 image width 0 (S0_WTH0)
 *            bit [7-2] Source 0 image width [7:2]
 *            bit [1-0] Fix to 00 [1:0]
 *
 *            Unit: Pixel.
 *            It must be divisible by 4. S0_WTH Bit [1:0] is internally fixed to 0.
 *            This value is the physical pixel number.
 *
 * REG [0x98] Source 0 image width 1 (S0_WTH1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 image width [12:8]
 *            Unit: Pixel.
 *            It must be divisible by 4. S0_WTH Bit [1:0] is internally fixed to 0.
 *            This value is the physical pixel number.
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void RA8889::BTE_S0_ImageWidth(uint16_t Wx)
{
  _bus->RegisterWrite(REG_S0_WTH0, Wx);              //0x97, Source 0 image width 0 (S0_WTH0)
  _bus->RegisterWrite(REG_S0_WTH1, Wx >> 8);         //0x98, Source 0 image width 1 (S0_WTH1)
}


//================================================================================
//
// [0x99] Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
// [0x9A] Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
// [0x9B] Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
// [0x9C] Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Window Upper-Left Corner X,Y Cooordinates
 *
 * @verbatim
 * REG [0x99] Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
 *            bit [7-0] Source 0 Window Upper-Left corner X-coordinates [7:0]
 *            The register is source 0 window upper-left corner x-coordinates
 *
 * REG [0x9a] Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 Window Upper-Left corner X-coordinates [12:8]
 *            The register is source 0 window upper-left corner x-coordinates
 *
 * REG [0x9b] Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
 *            bit [7-0] Source 0 Window Upper-Left corner Y-coordinates [7:0]
 *            The register is source 0 window upper-left corner y-coordinates
 *
 * REG [0x9c] Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 Window Upper-Left corner Y-coordinates [12:8]
 *            The register is source 0 window upper-left corner y-coordinates
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::BTE_S0_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_S0_X0, Wx);                //0x99, Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
  _bus->RegisterWrite(REG_S0_X1, Wx >> 8);           //0x9a, Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
  _bus->RegisterWrite(REG_S0_Y0, Hy);                //0x9b, Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
  _bus->RegisterWrite(REG_S0_Y1, Hy >> 8);           //0x9c, Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
}


//================================================================================
//
// [0x9D] Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
// [0x9E] Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
// [0x9F] Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
// [0xA0] Source 1 memory start address 3 (S1_STR3)
//
//================================================================================


/**
 * @brief BTE Source 1 (S1) Memory Start Address
 *
 * @verbatim
 * REG [0x9d] Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
 *            bit [7-0] Source 1 memory start address [7:2]
 *            If source 1 is set as constant color, then the register is defined as red element of this constant color. Otherwise, the register is defined as the start address (bit[7:2]) of source 1 and bit[1:0] must be set to 0.
 *
 * REG [0x9e] Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
 *            bit [7-0] Source 1 memory start address [15:8]
 *            If source 1 is set as constant color, then the register is defined as green element of this constant color.
 * 
 * REG [0x9f] Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
 *            bit [7-0] Source 1 memory start address [23:16]
 *            If source 1 is set as constant color, then the register is defined as blue element of this constant color.
 *
 * REG [0xa0] Source 1 memory start address 3 (S1_STR3)
 *            bit [7-0] Source 1 memory start address [31:24]
 *            If source 1 is set as constant color, then the register is not used.
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void RA8889::BTE_S1_MemoryStartAddress(uint32_t addr)
{
  _bus->RegisterWrite(REG_S1_STR0_RED, addr);        //0x9d, Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
  _bus->RegisterWrite(REG_S1_STR1_GREEN, addr >> 8); //0x9e, Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
  _bus->RegisterWrite(REG_S1_STR2_BLUE, addr >> 16); //0x9f, Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
  _bus->RegisterWrite(REG_S1_STR3, addr >> 24);      //0xa0, Source 1 memory start address 3 (S1_STR3)
}


/**
 * @brief BTE Source 1 (S1) Constant Color R3G3B2 (256 color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-5] Red element para 256 cores (8bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-5] Green element para 256 cores (8bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-5] Blue element para 256 cores (8bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R3G3B2
 *
 * @note None
 */
void RA8889::S1_ConstantColor_256(uint8_t color)
{
  _bus->CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  _bus->DataWrite(color);                     //RRRGGGBB >> 0 = RRRgggbbb para colocar na posicao [7:5]

  _bus->CmdWrite(REG_S1_STR1_GREEN);          //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  _bus->DataWrite(color << 3);                //RRRGGGBB << 3 GGGbb000 para colocar na posicao [7:5]

  _bus->CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  _bus->DataWrite(color << 6);                //RRRGGGBB << 6 gg000000 para colocar na posicao [7:5]
}


/**
 * @brief BTE Source 1 (S1) Constant Color R5G6B5 (65k color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-3] Red element para 65k cores (16bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-2] Green element para 65k cores (16bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-3] Blue element para 65k cores (16bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R5G6B5
 *
 * @note None
 */
void RA8889::S1_ConstantColor_65k(uint16_t color)
{
  _bus->CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  _bus->DataWrite(color >> 8);                //RRRRRGGGGGGBBBBB >> 8 = 00000000RRRRRggg para colocar na posicao [7:3]
  _bus->CmdWrite(REG_S1_STR1_GREEN);          //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  _bus->DataWrite(color >> 3);                //RRRRRGGGGGGBBBBB >> 3 = 000rrrrrGGGGGGbb para colocar na posicao [7:2]
  _bus->CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  _bus->DataWrite(color << 3);                //RRRRRGGGGGGBBBBB << 3 = rrggggggBBBBB000 para colocar na posicao [7:3]
}


/**
 * @brief BTE Source 1 (S1) Constant Color R8G8B8 (16.7M color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-0] Red element para 16M cores (24bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-0] Green element para 16M cores (24bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-0] Blue element para 16M cores (24bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R8G8B8
 *
 * @note None
 */
void RA8889::S1_ConstantColor_16M(uint32_t color) 
{
  _bus->CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  _bus->DataWrite(color >> 16);               //RRRRRRRRGGGGGGGGBBBBBBBB >> 16 = 0000000000000000RRRRRRRR para colocar na posicao [7:0]
  _bus->CmdWrite(REG_S1_STR1_GREEN);          //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  _bus->DataWrite(color >> 8);                //00000000rrrrrrrrGGGGGGGG >> 8 = 00000000rrrrrrrrGGGGGGGG para colocar na posicao [7:0]
  _bus->CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  _bus->DataWrite(color);                     //RRRRRRRRGGGGGGGGBBBBBBBB >> 0 = rrrrrrrrggggggggBBBBBBBB para colocar na posicao [7:0]
}


//================================================================================
//
// PAGE 1
// [0xA0] Video Control (VC)
//
//================================================================================


/**
 * @brief Media Error Flag
 *
 * @verbatim
 * PAGE 1 REG [0xa0] Video Control (VC)
 * bit [7] Media error, indicate either the unsupported image format or
 *         header format error. When media error occurs, the bit is set
 *         to 1.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
uint8_t RA8889::Media_Error_Flag(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_VC1);                       //0xa0, page 1, Video Control (VC)
  temp = _bus->DataRead();
  temp &= 0x80;                                //
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
  return temp;
}


/**
 * @brief Check Media Decoder Busy / Wait Media Decoder Ready
 *
 * @verbatim
 * PAGE 1 REG [0xa0] Video Control (VC)
 * bit [6] Media decoder busy
 *     0: Media decoder is free
 *     1: Media decoder is busy
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Media_DecodeBusy(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  //case4: check AVI
  _bus->CmdWrite(REG_VC1);                       //0xa0, page 1, Video Control (VC)
  do {                                         
    temp = _bus->DataRead();                     
  } while (temp & 0x40);                       //Aguarda enquanto estiver ocupado
  PageSwitch(ePageReg::Page1);
}
void RA8889::Media_DecodeWaitReady(void) { Media_DecodeBusy(); }


/**
 * @brief Media Decode Reading for Busy
 *
 * @verbatim
 * PAGE 1 REG [0xa0] Video Control (VC)
 * bit [6] Media decoder busy
 *     0: Media decoder is free
 *     1: Media decoder is busy
 * @endverbatim
 *
 * @param None
 *
 * @return 
 * 0x00: Media Decode ist free
 * 0x40: Media Decode ist Busy
 *
 * @note None
 */
uint8_t RA8889::MediaDecodeBusy(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  //case4: check AVI
  _bus->CmdWrite(REG_VC1);                       //0xa0, page 1, Video Control (VC)
  temp = _bus->DataRead();                       
  temp &= 0x40;                                //Veja se bit esta em bussy/idle
  PageSwitch(ePageReg::Page0);
  return temp;
}


/**
 * @brief Media FIFO Empty
 *
 * @verbatim
 * PAGE 1 REG [0xa0] Video Control (VC)
 * bit [5] Media fifo empty
 *     0: Media FIFO is not empty
 *     1: Media FIFO is empty
 * @endverbatim
 *
 * @param None
 *
 * @return 
 * 0x20: Media Fifo ist Empty
 * 0x00: Media Fifo ist not Empty
 *
 * @note None
 */
uint8_t RA8889::Media_Fifo_Empty(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_VC1);                       //0xa0, page 1, Video Control (VC)
  temp = _bus->DataRead();                       
  temp &= 0x20;                                //Verifica bit 5
  PageSwitch(ePageReg::Page0);
  return temp;
}


//================================================================================
//
// PAGE 0
// [0xA1] Source 1 image width 0 (S1_WTH0)
// [0xA2] Source 1 image width 1 (S1_WTH1)
//
//================================================================================


/**
 * @brief Source 1 (S1) Image Width
 *
 * @verbatim
 * REG [0xa1] Source 1 image width 0 (S1_WTH0)
 *            bit [7-2] Source 1 image width [7:2]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. S1_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *            bit [1-0] Fix to 00
 *
 * REG [0xa2] Source 1 image width 1 (S1_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 image width [12:8]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. S1_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note Bit [1:0] tie to "0" internally.
 */
void RA8889::BTE_S1_ImageWidth(uint16_t Wx)
{
  _bus->RegisterWrite(REG_S1_WTH0, Wx);              //0xa1, Source 1 image width 0 (S1_WTH0)
  _bus->RegisterWrite(REG_S1_WTH1, Wx >> 8);         //0xa2, Source 1 image width 1 (S1_WTH1)
}


//================================================================================
//
// PAGE 0
// [0xA3] Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
// [0xA4] Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
// [0xA5] Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
// [0xA6] Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
//
//================================================================================


/**
 * @brief BTE Source 1 (S1) Window Upper-Left Corner X,Y Cooordinates
 *
 * @verbatim
 * REG [0xa3] Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
 *            bit [7-0] Source 1 Window Upper-Left corner X-coordinates [7:0]
 *            The register is source 1 window upper-left corner x-coordinates
 *
 * REG [0xa4] Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 Window Upper-Left corner X-coordinates [12:8]
 *            The register is source 1 window upper-left corner x-coordinates
 *
 * REG [0xa5] Source 0 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
 *            bit [7-0] Source 1 Window Upper-Left corner Y-coordinates [7:0]
 *            The register is source 1 window upper-left corner y-coordinates
 *
 * REG [0xa6] Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 Window Upper-Left corner Y-coordinates [12:8]
 *            The register is source 1 window upper-left corner y-coordinates
 * @endverbatim
 *
 * @param Wx, Hy, coordinate x,y
 *
 * @note None
 */
void RA8889::BTE_S1_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_S1_X0, Wx);                //0xa3, Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
  _bus->RegisterWrite(REG_S1_X1, Wx >> 8);           //0xa4, Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
  _bus->RegisterWrite(REG_S1_Y0, Hy);                //0xa5, Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
  _bus->RegisterWrite(REG_S1_Y1, Hy >> 8);           //0xa6, Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
}


//================================================================================
//
// [0xA7] Destination memory start address 0 (DT_STR0)
// [0xA8] Destination memory start address 1 (DT_STR1)
// [0xA9] Destination memory start address 2 (DT_STR2)
// [0xAA] Destination memory start address 3 (DT_STR3)
//
//================================================================================


/**
 * @brief BTE Destination (D) Memory Start Address
 *
 * @verbatim
 * REG [0xa7] Destination memory start address 0 (DT_STR0)
 *            bit [7-2] Destination memory start address [7:2]
 *            bit [1-0] Fix at 0
 *
 * REG [0xa8] Destination memory start address 1 (DT_STR1)
 *            bit [7-0] Destination memory start address [15:8]
 *
 * REG [0xa9] Destination memory start address 2 (DT_STR2)
 *            bit [7-0] Destination memory start address [23:16]
 *
 * REG [0xaa] Destination memory start address 3 (DT_STR3)
 *            bit [7-0] Destination memory start address [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note Bit [1:0] tie to "0" internally.
 *       Destination memory start address cannot set within from source 0|1 
 *       start address to source 0|1’s 
 *       (image width) * (image height) * (color depth[1|2|3])
 */
void RA8889::BTE_Destination_MemoryStartAddress(uint32_t addr) 
{
  _bus->RegisterWrite(REG_DT_STR0, addr);       //0xa7, Destination memory start address 0 (DT_STR0)
  _bus->RegisterWrite(REG_DT_STR1, addr >> 8);  //0xa8, Destination memory start address 0 (DT_STR1)
  _bus->RegisterWrite(REG_DT_STR2, addr >> 16); //0xa9, Destination memory start address 0 (DT_STR2)
  _bus->RegisterWrite(REG_DT_STR3, addr >> 24); //0xaa, Destination memory start address 0 (DT_STR3)
}


//================================================================================
//
// PAGE 1 
// [0xA1] Media Image Height High Byte (MIHH)
// [0xA2] Media Image Height Low Byte (MIHL)
// [0xA3] Media Image Width High Byte (MIWH)
// [0xA4] Media Image Width Low Byte (MIWL)
// [0xA5] Video Frame Period Byte3 (VFPB3)
// [0xA6] Video Frame Period Byte2 (VFPB2)
// [0xA7] Video Frame Period Byte1 (VFPB1)
// [0xA8] Video Frame Period Byte0 (VFPB0)
//
//================================================================================


/**
 * @brief Extract Media Image Height from Header
 *
 * @verbatim
 * PAGE 1 REG [0xa1] Media Image Height High Byte (MIHH)
 * bit [7-0] Image height extracted from media (BMP/JPEG/AVI) header Height[15:8]
 *
 * PAGE 1 REG [0xa2] Media Image Height Low Byte (MIHL)
 * bit [7-0] Image height extracted from media (BMP/JPEG/AVI) header Height[7:0]
 *
 * A leitura é feita em duas etapas:
 * 1. Lê o byte alto (MIHH), deslocando-o para a parte alta da palavra (bits 15..8).
 * 2. Lê o byte baixo (MIHL) e combina com o valor anterior (bits 7..0).
 *
 * Dessa forma, retorna a altura completa de 16 bits da imagem.
 * @endverbatim
 *
 * @param None
 *
 * @return
 *
 * @note None
 */
uint16_t RA8889::Media_HeaderImageHeight(void)
{
  uint16_t temp = 0;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_MIHH);                      //0xa1, page 1, Media Image Height High Byte (MIHH)
  temp = (uint16_t)_bus->DataRead();             //Ler o byte da parte alta da palavra
  temp <<= 8;                                  //Desloca valor na parte alta
  _bus->CmdWrite(REG_MIHL);                      //0xa2, page 1, Media Image Height Low Byte (MIHL)
  temp |= (uint16_t)_bus->DataRead();            //Junta aos valores da parte baixa
  PageSwitch(ePageReg::Page0);                 
  return temp;                                 //Retorna valor de 16 bits do media image height
}


/**
 * @brief Extract Media Image Width from Header
 *
 * @verbatim
 * PAGE 1 REG [0xa3] Media Image Width High Byte (MIWH)
 * bit [7-0] Image width extracted from media (BMP/JPEG/AVI) header Width[15:8]
 *
 * PAGE 1 REG [0xa4] Media Image Width Low Byte (MIWL)
 * bit [7-0] Image width extracted from media (BMP/JPEG/AVI) header Width[7:0]
 *
 * A leitura é feita em duas etapas:
 * 1. Lê o byte alto (MIWH), deslocando-o para a parte alta da palavra (bits 15..8).
 * 2. Lê o byte baixo (MIWL) e combina com o valor anterior (bits 7..0).
 *
 * Dessa forma, retorna a altura completa de 16 bits da imagem.
 * @endverbatim
 *
 * @param None
 *
 * @return
 *
 * @note None
 */
uint16_t RA8889::Media_HeaderImageWidth(void)
{
  uint16_t temp = 0;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_MIWH);                 //0xa3, page 1, Media Image Width High Byte (MIWH)
  temp = (uint16_t)_bus->DataRead();        //Ler o byte da parte alta da palavra  
  temp <<= 8;                             //Desloca valor na parte alta
  _bus->CmdWrite(REG_MIWL);                 //0xa4, page 1, Media Image Width Low Byte (MIWL)
  temp |= (uint16_t)_bus->DataRead();       //Junta aos valores da parte baixa
  PageSwitch(ePageReg::Page0);                 
  return temp;                            //Retorna valor de 16 bits do media image width
}


/**
 * @brief AVI Video Frame Period from Header
 *
 * @verbatim
 * PAGE 1 REG [0xa5] Video Frame Period Byte3 (VFPB3)
 * bit [7-0] Video Frame Period extracted from AVI header VFPB[31:24]
 *
 * PAGE 1 REG [0xa6] Video Frame Period Byte2 (VFPB2)
 * bit [7-0] Video Frame Period extracted from AVI header VFPB[23:16]
 *
 * PAGE 1 REG [0xa7] Video Frame Period Byte1 (VFPB1)
 * bit [7-0] Video Frame Period extracted from AVI header VFPB[15:8]
 *
 * PAGE 1 REG [0xa8] Video Frame Period Byte0 (VFPB0)
 * bit [7-0] Video Frame Period extracted from AVI header VFPB[7:0]
 * @endverbatim
 *
 * @param None
 *
 * @return uint32_t valor do Video Frame Period lido do cabeçalho
 *
 * @note None
 */
uint32_t RA8889::AVI_HeaderFramePeriod(void)
{
  uint32_t temp = 0;
  PageSwitch(ePageReg::Page1);

  _bus->CmdWrite(REG_VFPB3);                     //0xa5, page 1, Video Frame Period Byte3 (VFPB3)
  temp = (uint32_t)_bus->DataRead() << 24;

  _bus->CmdWrite(REG_VFPB2);                     //0xa6, page 1, Video Frame Period Byte2 (VFPB2)
  temp |= (uint32_t)_bus->DataRead() << 16;

  _bus->CmdWrite(REG_VFPB1);                     //0xa7, page 1, Video Frame Period Byte1 (VFPB1)
  temp |= (uint32_t)_bus->DataRead() << 8;
  
  _bus->CmdWrite(REG_VFPB0);                     //0xa8, page 1, Video Frame Period Byte0 (VFPB0)
  temp |= (uint32_t)_bus->DataRead();

  PageSwitch(ePageReg::Page0);
  return temp;                                 //retorna 32 bits AVI Video Frame Period
}


//================================================================================
//
// PAGE 1
// [0xA9] Video Control (VC)
//
//================================================================================


/**
 * @brief IDEC Reset
 *
 * @verbatim
 * PAGE 1 REG [0xa9] Video Control (VC)
 * bit [1] Must set 1
 * bit [0] Idec reset , clear Idec circuit
 *     1: no active
 *     0: reset
 *
 * @endverbatim
 *
 * @param None
 *
 * @return None
 *
 * @note None
 */
void RA8889::IDEC_Reset(void)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_VC2);                  //0xa9, page 1, Video Control (VC)
  _bus->DataWrite(0x02);                    //bit1=1 (fixo), bit0=0 (reset active)
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0 
// [0xAB] Destination image width 0 (DT_WTH0)
// [0xAC] Destination image width 1 (DT_WTH1)
//
//================================================================================


/**
 * @brief Destination (D) Image Width
 *
 * @verbatim
 * REG [0xab] Destination image width 0 (DT_WTH0)
 *            bit [7-2] Destination image width [7:2]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. DT_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *            bit [1-0] Fix at 00
 *
 * REG [0xac] Destination image width 1 (DT_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination image width [12:8]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. DT_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void RA8889::BTE_Destination_ImageWidth(uint16_t Wx)
{
  _bus->RegisterWrite(REG_DT_WTH0, Wx);              //0xab, Destination image width 0 (DT_WTH0)
  _bus->RegisterWrite(REG_DT_WTH1, Wx >> 8);         //0xac, Destination image width 1 (DT_WTH1)
}


//================================================================================
//
// [0xAD] Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
// [0xAE] Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
// [0xAF] Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
// [0xB0] Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
//
//================================================================================


/**
 * @brief Destination (D) Windows Upper-Left Corner X,Y Coordinates
 *
 * @verbatim
 * REG [0xad] Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
 *            bit [7-0] Destination Window Upper-Left corner X-coordinates [7:0]
 *
 * REG [0xae] Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination Window Upper-Left corner X-coordinates [12:8]
 *
 * REG [0xaf] Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
 *            bit [7-0] Destination Window Upper-Left corner Y-coordinates [7:0]
 *
 * REG [0xb0] Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination Window Upper-Left corner Y-coordinates [12:8]
 * @endverbatim
 *
 * @param Wx, Hy
 *
 * @note None
 */
void RA8889::BTE_Destination_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_DT_X0, Wx);                //0xad, Destination Window Upper-Left corner X-coordinates 0 [7:0] (DT_X0)
  _bus->RegisterWrite(REG_DT_X1, Wx >> 8);           //0xae, Destination Window Upper-Left corner X-coordinates 1 [12:8] (DT_X1)
  _bus->RegisterWrite(REG_DT_Y0, Hy);                //0xaf, Destination Window Upper-Left corner Y-coordinates 0 [7:0] (DT_Y0)
  _bus->RegisterWrite(REG_DT_Y1, Hy >> 8);           //0xb0, Destination Window Upper-Left corner Y-coordinates 1 12:8] (DT_Y1)
}


//================================================================================
//
// [0xB1] BTE Window Width 0 (BTE_WTH0)
// [0xB2] BTE Window Width 1 (BTE_WTH1)
// [0xB3] BTE Window Height 0 (BTE_HIG0)
// [0xB4] BTE Window Height 1 (BTE_HIG1)
//
//================================================================================


/**
 * @brief BTE Window Size (width, height)
 *
 * @verbatim
 * REG [0xb1] BTE Window Width 0 (BTE_WTH0)
 *            bit [7-0] BTE Window Width Setting[7:0]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb2] BTE Window Width 1 (BTE_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] BTE Window Width Setting [12:8]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb3] BTE Window Height 0 (BTE_HIG0)
 *            bit [7-0] BTE Window Height Setting[7:0]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb4] BTE Window Height 1 (BTE_HIG1)
 *            bit [7-5] N/A
 *            bit [4-0] BTE Window Height Setting [12:8]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 * @endverbatim
 *
 * @param Wx, Hy: Width and Height size
 *
 * @note None
 */
void RA8889::BTE_WindowSize(uint16_t Wx, uint16_t Hy)
{
  _bus->RegisterWrite(REG_BTE_WTH0, Wx);       //0xb1, BTE Window Width 0 [7:0] (BTE_WTH0)
  _bus->RegisterWrite(REG_BTE_WTH1, Wx >> 8);  //0xb2, BTE Window Width 1 [12:8] (BTE_WTH1)
  _bus->RegisterWrite(REG_BTE_HIG0, Hy);       //0xb3, BTE Window Height 0 [7:0] (BTE_HIG0)
  _bus->RegisterWrite(REG_BTE_HIG1, Hy >> 8);  //0xb4, BTE Window Height 1 [12:8] (BTE_HIG1)
}


//================================================================================
//
// [0xB5] Alpha Blending (APB_CTRL)
//
//================================================================================


/**
 * @brief Window Alpha Blending Effect for S0 & S1
 *
 * @verbatim
 * REG [0xab] Alpha Blending (APB_CTRL)
 *            bit [7-4] N/A
 *            bit [5-0] Window Alpha Blending effect for S0 & S1
 *            The value of alpha in the color code ranges from 0.0 to 1.0,
 *            where 0.0 represents a fully transparent color, and 1.0
 *            represents a fully opaque color.
 *            00h: 0
 *            01h: 1/32
 *            02h: 2/32
 *            :
 *            1Eh: 30/32
 *            1Fh: 31/32
 *            2Xh: 1
 *            
 *            Output Effect = (S0 image x (1 - alpha setting value))
 *                          + (S1 image x alpha setting value)
 * @endverbatim
 *
 * @param value
 *
 * @note Bit [5:4] from Window Alpha Blending effect bit [5-0] overlap in [7-4] N/A
 */
void RA8889::BTE_AlphaBlendingEffect(uint8_t value)
{ 
  _bus->CmdWrite(REG_APB_CTRL);                  //0xb5, Alpha Blending (APB_CTRL)
  _bus->DataWrite(value);  
}


//================================================================================
//
// PAGE 0
// [0xB6] Serial flash DMA Controller REG (DMA_CTRL)
// [0xB7] Serial Flash/ROM Controller Register (SFL_CTRL)
//
//================================================================================


/**
 * @brief Serial Flash DMA Start
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 * bit [0] Write: DMA Start Bit
 *         Set to 1 by MPU and reset to 0 automatically
 *         The bit cannot start when fontwr_busy is 1. On 
 *         the contrary, if DMA is enabled, the text mode & 
 *         sending character code are disabled.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_DMA_Start(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Aguarde até que o Serial flash DMA Controller esteja pronto/ocioso
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [0] Read: DMA Busy Check Bit
 *                    0: Idle
 *                    1: Bsy
 *                    *** about DMA transfer of serial flash, its 
 *                    destination starting address, destination image 
 *                    width, color depth & address mode in SDRAM are 
 *                    followed by Canvas’ setting and only operate in 
 *                    graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_DMA_WaitReady(void)
{
  uint8_t temp;
  //Case 1: Using DMA Function Control Register
  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  do {
    temp = _bus->DataRead();
  } while (temp & 0x01);                       //Enquanto estiver em Busy 
}


/**
 * @brief Select Serial Flash/ROM I/F Access Font Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 * bit [6] Serial Flash / ROM Access Mode
 *     0b0: Font mode – for external CGROM
 *     0b1: DMA mode – for CGRAM, pattern, boot start image or OSD
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_Select_FontMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,6);                                //Reset bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Access DMA Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 * bit [6] Serial Flash / ROM Access Mode
 *     0b0: Font mode – for external CGROM
 *     0b1: DMA mode – for CGRAM, pattern, boot start image or OSD
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_Select_DMAMode(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,6);                                //Set bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Address 24-bit Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 * bit [5] Serial Flash / ROM Access Mode
 *         0b0: 24 bits address mode
 *         0b1: 32 bits address mode
 *         
 *         If user wants to use 32 bits address mode, user 
 *         must manual send EX4B command (B7h) to serial 
 *         flash then set this bit to 1.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_Select_24bitAddress(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,5);                                //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Address 32-bit Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 * bit [5] Serial Flash / ROM Access Mode
 *     0b0: 24 bits address mode
 *     0b1: 32 bits address mode
 *     
 *     If user wants to use 32 bits address mode, user 
 *     must manual send EX4B command (B7h) to serial 
 *     flash then set this bit to 1.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::SFI_Select_32bitAddress(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,5);                                //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Single Data Without Dummy Cycles Normal Read Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 *   000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 *   010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 *   1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 *   xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 *   xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Select_SFI_SingleData_03h(void)
{
  uint8_t temp;

  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bit 7 and 6, Use REG [0xb7] bit [3-0]
  _bus->DataWrite(temp);
  
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  _bus->DataWrite(temp);                         
}


/**
 * @brief Select Serial Flash/ROM for Single Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary: 
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
 void RA8889::Select_SFI_SingleData_0Bh(void)
{
  uint8_t temp;

  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bit 7 and 6, Use REG [0xb7] bit [3-0]
  _bus->DataWrite(temp);
  
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  SETB(temp,2);                                //Set bit 2. Set x read command code = 0Bh. 8 dummy cycles inserted between address and data.
  _bus->DataWrite(temp);                         
}


/**
 * @brief Select Serial Flash/ROM Single Data 16 Dummy Cycles Fast Read Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 *            Note: Not serial flash support above read command, please 
 *            according to serial flash’s datasheet to select proper read 
 *            command.
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Select_SFI_SingleData_1Bh(void)
{
  uint8_t temp;

  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bit 7 and 6, Use REG [0xb7] bit [3-0]
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  SETB(temp,3);                                //Set bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Dual Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Select_SFI_DualData_3Bh(void)
{
  uint8_t temp;

  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bit 7 and 6, Use REG [0xb7] bit [3-0]
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  SETB(temp,1);                                //Set bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Dual Data 4 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas aplicado ao RA8876 e RA8877, esse modo 1 BBh não existe no RA8889
 *
 * @result None
 */
void RA8889::Select_SFI_DualData_BBh(void)
{
  uint8_t temp;

  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bit 7 and 6, Use REG [0xb7] bit [3-0]
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  SETB(temp,1);                                //Set bit 1
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Quad Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 *
 * Summary:
 *   01b: 4x read command code – 6Bh.
 *   10b: 4x read command code – EBh.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Select_SFI_QuadData_6Bh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= 0x3F;
  SETB(temp,6);                                //Set bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Quad Data 4 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * 
 * Summary:
 *   01b: 4x read command code – 6Bh.
 *   10b: 4x read command code – EBh.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Select_SFI_QuadData_EBh(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = _bus->DataRead();                       
  temp &= 0x3F;
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM 0 I/F is Selected
 *        
 * @verbatim                  
 * REG [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                    0b0: Serial Flash/ROM 0 I/F is selected.
 *                    0b1: Serial Flash/ROM 1 I/F is selected.
 *                    Note: when page1 B7h bit 7 = 1, then serial flash 
 *                    chip select 2,3
 * 
 * Compatibilidade:
 * - RA8876 / RA8877: apenas CS0/CS1 disponíveis, função compatível
 * - RA8889: CS0/CS1 selecionáveis usando apenas Page0:B7
 *           Para acessar CS2/CS3, é necessário usar as funções específicas
 *           SFI_SelectROM_CS2() / SFI_SelectROM_CS3(), que combinam Page1:B7 + Page0:B7
 * 
 * Diferença RA8889:
 * - Page0:B7 manipula apenas CS0/CS1
 * - Page1:B7 permite selecionar o "bloco" de CS2/CS3, expandindo o acesso a 4 chips
 * @endverbatim
 *
 * @param None
 *
 * @note Compativel com  RA8876/RA8877/RA8889
 *       Se usar o RA8889 esta função funciona o mesmo que a função SFI_SelectROM_CS0()
 * @result None
 */
void RA8889::SFI_Select_ROM0(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM 1 I/F is Selected
 *        
 * @verbatim                  
 * REG [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                    0b0: Serial Flash/ROM 0 I/F is selected.
 *                    0b1: Serial Flash/ROM 1 I/F is selected.
 *                    Note: when page1 B7h bit 7 = 1, then serial flash 
 *                    chip select 2,3
 *
 * Compatibilidade:
 * - RA8876 / RA8877: apenas CS0/CS1 disponíveis, função compatível
 * - RA8889: CS0/CS1 selecionáveis usando apenas Page0:B7
 *           Para acessar CS2/CS3, é necessário usar as funções específicas
 *           SFI_SelectROM_CS2() / SFI_SelectROM_CS3(), que combinam Page1:B7 + Page0:B7
 * 
 * Diferença RA8889:
 * - Page0:B7 manipula apenas CS0/CS1
 * - Page1:B7 permite selecionar o "bloco" de CS2/CS3, expandindo o acesso a 4 chips
 * @endverbatim
 *
 * @param None
 *
 * @note Compativel com  RA8876/RA8877/RA8889
 *       Se usar o RA8889 esta função funciona o mesmo que a função SFI_SelectROM_CS0()
 * @result None
 */
void RA8889::SFI_Select_ROM1(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM Waveform Mode 0
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [4] RA8875 compatible mode
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 *                    
 *                    Serial Flash/ROM Waveform Mode
 *                    In the RA8875 compatible mode,
 *                    
 *                    Data are read on the clock's falling edge (high->low transition) and data are changed on a falling edge (high->low transition).
 *                    0: For Mode 0, SPI clock park on low when idle.
 *                    1: For Mode 3, SPI clock park on high when idle.
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 0, REG 0xB7 bit [4] é sempre 1.
 *       Apartir do RA8886/RA8887 modo compatibildiade com o RA8875.
 *
 * @result None
 */
void RA8889::SFI_Select_WaveformMode0(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, RA8876/RA8877, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM Waveform Mode 3
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [4] RA8875 compatible mode
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 *                    
 *                    Serial Flash/ROM Waveform Mode
 *                    In the RA8875 compatible mode,
 *                    
 *                    Data are read on the clock's falling edge (high->low transition) and data are changed on a falling edge (high->low transition).
 *                    0: For Mode 0, SPI clock park on low when idle.
 *                    1: For Mode 3, SPI clock park on high when idle.
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 0, REG 0xB7 bit [4] é sempre 1.
 *       Apartir do RA8886/RA8887 modo compatibildiade com o RA8875.
 *
 * @result None
 */
void RA8889::SFI_Select_WaveformMode3(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, RA8876/RA8877, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0xB6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
//
//================================================================================


/**
 * @brief Select Serial Flash/ROM # I/F
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [7-6] IDEC Serial Flash/ROM I/F # Select
 *      0b00: Serial Flash/ROM 0 I/F is selected
 *      0b01: Serial Flash/ROM 1 I/F is selected
 *      0b10: Serial Flash/ROM 2 I/F is selected
 *      0b11: Serial Flash/ROM 3 I/F is selected
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_SFI_Select_ROM(eSFIROM sfirom)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0x3E;                                  //if bit0=1, IDEC will start .
  temp |= static_cast<uint8_t>(sfirom) << 6;     //Shift to bit 7-6
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief Font/DMA Serial Flash (SPI Bus 0) sck and data bus select
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [4] FONT/DMA serial flash sck and data bus select
 *     0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
 *     1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::Font_DMA_Select_Bus0(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xEE;                                  //if bit0=1, IDEC will start .
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief Font/DMA Serial Flash (SPI Bus 1) sck and data bus select
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [4] FONT/DMA serial flash sck and data bus select
 *     0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
 *     1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::Font_DMA_Select_Bus1(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xEE;                                  //if bit0=1, IDEC will start .
  SETB(temp,4);                                  //Set bit 4
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC sck and data bus select (SPI Bus 0)
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [3] IDEC sck and data bus select
 *     0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
 *     1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Select_Bus0(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF6;                                  //if bit0=1, IDEC will start
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC sck and data bus select (SPI Bus 1)
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [3] IDEC sck and data bus select
 *     0: SPI bus 0 is selected and the relative pins (xmosi, xmiso, xsio2, xsio3) are active.
 *     1: SPI bus 1 is selected and the relative pins (xsp1_msio0, xsp1_msio1, xsp1_msio2, xsp1_msio3) are active.
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Select_Bus1(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF6;                                  //if bit0=1, IDEC will start .
  SETB(temp,3);                                  //Set bit 3
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Dstination Color Depth 8 bpp
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [2-1] IDEC destination Color depth:
 *     00: 8 bit
 *     01: 16 bit
 *     10: 24 bit
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Destination_ColorDepth_8bpp(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF8;                                  //if bit0=1, IDEC will start
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Dstination Color Depth 16 bpp
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [2-1] IDEC destination Color depth:
 *     00: 8 bit
 *     01: 16 bit
 *     10: 24 bit
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Destination_ColorDepth_16bpp(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF8;                                  //if bit0=1, IDEC will start
  SETB(temp,1);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Dstination Color Depth 24 bpp
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [2-1] IDEC destination Color depth:
 *     00: 8 bit
 *     01: 16 bit
 *     10: 24 bit
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Destination_ColorDepth_24bpp(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF8;                                  //if bit0=1, IDEC will start
  SETB(temp,2);
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Dstination Color Depth
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [2-1] IDEC destination Color depth:
 *     00: 8 bit
 *     01: 16 bit
 *     10: 24 bit
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note bit0=0 para não reiniciar o IDEC.
 *
 * @result None
 */
void RA8889::IDEC_Destination_ColorDepth(eColorDepthBPP bpp)
{
  uint8_t temp;
  uint8_t bit;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0xF8;                                  //if bit0=1, IDEC will start
  bit = static_cast<uint8_t>(bpp);             
  bit = (bit >> 3)-1;                            //transforma 8,16,24 em 0, 1, 2
  temp |= (bit << 1);                            //posiciona nos bits [2-1]
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Start Decoding
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_Starts_Decoding(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  SETB(temp,0);                                  //Set bit 0
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page1);
}


/**
 * @brief IDEC Wait to Ready
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting address, destination image width, color depth & address mode in SDRAM are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_WaitReady(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  do {
    _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
    temp = _bus->DataRead();
  } while (temp & 0x01);
  PageSwitch(ePageReg::Page0);
}

/**
 * @brief IDEC Reading for Busy
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb6] Serial flash AVI/JPG/BMP (IDEC_CTRL0)
 * bit [0] 
 *     Write Function: IDEC Start Bit
 *       Set to 1 by MPU and reset to 0 automatically
 *     It cannot start when fontwr_busy is 1. And if IDEC is enabled, serial 
 *     flash I/F can’t be set as text mode & send character code.
 *     Read Function: IDEC Busy Check Bit
 *       0: Idle
 *       1: Busy
 *     When the serial flash I/F is in IDEC mode, its destination starting 
 *     address, destination image width, color depth & address mode in SDRAM 
 *     are followed by Canvas’ setting and only operated in graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result 0: IDEC ist Idle
 *         1: IDEC ist Busy 
 */
uint8_t RA8889::IDEC_Busy(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL0);                  //0xb6, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL0)
  temp = _bus->DataRead();
  temp &= 0x01;                                  //Verifica apenas o Bit 0
  PageSwitch(ePageReg::Page1);
  return temp;
}


//================================================================================
//
// PAGE 1
// [0xB7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
//
//================================================================================


/**
 * @brief Serial Flash/ROM I/F Chip Select 0 (CS0)
 *        Seleciona o Serial Flash/ROM Chip Select 0 (CS0) no RA8889
 *        
 * @verbatim                  
 * PAGE 0 REG [0xb7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                   bit [7] 0: Serial Flash/ROM 0 I/F is selected.
 *                           1: Serial Flash/ROM 1 I/F is selected.
 *                           Note: when page1 B7h bit 7 = 1 , then serial flash chip select 2,3
 *
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 *                   bit [7] Page 1 FONT/DMA Serial Flash/ROM I/F # Select
 *                           This BIT needs to be used with PAGE0 REG [B7h] bit7
 * 
 *                           Page0 REG[B7h]Bit7 = 0    Page0 REG[B7h]Bit7 = 1
 *   Page1 REG[B7h]Bit7 = 0  CS0                       CS1
 *   Page1 REG[B7h]Bit7 = 1  CS2                       CS3  
 *
 * Combinação para CS0:
 * - Page1:B7 = 0 (bloco CS0/CS1)
 * - Page0:B7 = 0 (primeiro do bloco → CS0)
 *
 * Compatibilidade:
 * - RA8876 / RA8877: Page0:B7 suporta CS0/CS1
 * - RA8889: Page1 + Page0 permite CS0 a CS3
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas para o RA8889
 *
 * @result None
 */
void RA8889::SFI_SelectROM_CS0 (void)
{
  uint8_t temp;

  PageSwitch(ePageReg::Page1);                 //Troca para a Pagina 1
  _bus->CmdWrite(REG_IDEC_CTRL1);                //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);                 //Troca para a Pagina 0

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, page 0, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM I/F Chip Select 1 (CS1)
 *        Seleciona o Serial Flash/ROM Chip Select 1 (CS1) no RA8889
 *        
 * @verbatim                  
 * PAGE 0 REG [0xb7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                   bit [7] 0: Serial Flash/ROM 0 I/F is selected.
 *                           1: Serial Flash/ROM 1 I/F is selected.
 *                           Note: when page1 B7h bit 7 = 1 , then serial flash chip select 2,3
 *
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 *                   bit [7] Page 1 FONT/DMA Serial Flash/ROM I/F # Select
 *                           This BIT needs to be used with PAGE0 REG [B7h] bit7
 * 
 *                           Page0 REG[B7h]Bit7 = 0    Page0 REG[B7h]Bit7 = 1
 *   Page1 REG[B7h]Bit7 = 0  CS0                       CS1
 *   Page1 REG[B7h]Bit7 = 1  CS2                       CS3  
 *
 * Combinação para CS1:
 * - Page1:B7 = 0 (bloco CS0/CS1)
 * - Page0:B7 = 1 (segundo do bloco → CS1)
 *
 * Compatibilidade:
 * - RA8876 / RA8877: Page0:B7 suporta CS0/CS1
 * - RA8889: Page1 + Page0 permite CS0 a CS3
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas para o RA8889
 *
 * @result None
 */
void RA8889::SFI_SelectROM_CS1 (void)
{
  uint8_t temp;

  PageSwitch(ePageReg::Page1);                 //Troca para a Pagina 1
  _bus->CmdWrite(REG_IDEC_CTRL1);                //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);                 //Troca para a Pagina 0

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, page 0, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM I/F Chip Select 2 (CS2)
 *        Seleciona o Serial Flash/ROM Chip Select 2 (CS2) no RA8889
 *
 * @verbatim                  
 * PAGE 0 REG [0xb7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                   bit [7] 0: Serial Flash/ROM 0 I/F is selected.
 *                           1: Serial Flash/ROM 1 I/F is selected.
 *                           Note: when page1 B7h bit 7 = 1 , then serial flash chip select 2,3
 *
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 *                   bit [7] Page 1 FONT/DMA Serial Flash/ROM I/F # Select
 *                           This BIT needs to be used with PAGE0 REG [B7h] bit7
 * 
 *                           Page0 REG[B7h]Bit7 = 0    Page0 REG[B7h]Bit7 = 1
 *   Page1 REG[B7h]Bit7 = 0  CS0                       CS1
 *   Page1 REG[B7h]Bit7 = 1  CS2                       CS3  
 *
 * Combinação para CS2:
 * - Page1:B7 = 1 (bloco CS2/CS3)
 * - Page0:B7 = 0 (primeiro do bloco → CS2)
 *
 * Compatibilidade:
 * - RA8876 / RA8877: não possuem Page1 → CS2/CS3 não disponíveis
 * - RA8889: Page1 + Page0 permite CS2/CS3 
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas para o RA8889
 *
 * @result None
 */
void RA8889::SFI_SelectROM_CS2(void)
{
  uint8_t temp;

  PageSwitch(ePageReg::Page1);                 //Troca para a Pagina 1
  _bus->CmdWrite(REG_IDEC_CTRL1);                //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);                 //Troca para a Pagina 0

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, page 0, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  CLRB(temp,7);                                //Reset bit 7
  _bus->DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM I/F Chip Select 3 (CS3)
 *        Seleciona o Serial Flash/ROM Chip Select 3 (CS3) no RA8889
 *
 * @verbatim                  
 * PAGE 0 REG [0xb7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                   bit [7] 0: Serial Flash/ROM 0 I/F is selected.
 *                           1: Serial Flash/ROM 1 I/F is selected.
 *                           Note: when page1 B7h bit 7 = 1 , then serial flash chip select 2,3
 *
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 *                   bit [7] Page 1 FONT/DMA Serial Flash/ROM I/F # Select
 *                           This BIT needs to be used with PAGE0 REG [B7h] bit7
 * 
 *                           Page0 REG[B7h]Bit7 = 0    Page0 REG[B7h]Bit7 = 1
 *   Page1 REG[B7h]Bit7 = 0  CS0                       CS1
 *   Page1 REG[B7h]Bit7 = 1  CS2                       CS3  
 *
 * Combinação para CS3:
 * - Page1:B7 = 1 (bloco CS2/CS3)
 * - Page0:B7 = 1 (segundo do bloco → CS3)
 *
 * Compatibilidade:
 * - RA8876 / RA8877: não possuem Page1 → CS2/CS3 não disponíveis
 * - RA8889: Page1 + Page0 permite CS2/CS3 
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas para o RA8889
 *
 * @result None
 */
void RA8889::SFI_SelectROM_CS3(void)
{
  uint8_t temp;

  PageSwitch(ePageReg::Page1);                 //Troca para a Pagina 1
  _bus->CmdWrite(REG_IDEC_CTRL1);                //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);                 //Troca para a Pagina 0

  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, page 0, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = _bus->DataRead();
  SETB(temp,7);                                //Set bit 7 
  _bus->DataWrite(temp);
}


/**
 * @brief Seleciona o chip serial flash/ROM desejado (CS0 a CS3) de forma rápida
 *
 * @param cs_num Número do chip select (0 a 3)
 *
 * @note Esta versão escreve diretamente os bits 7 dos registradores sem ler antes,
 *       ideal quando você sabe que apenas quer definir o CS e não precisa preservar outros bits.
 *       Apenas para RA8889.
 */
inline void RA8889::SFI_SelectROM_CS_Fast(uint8_t cs_num)
{
  if(cs_num > 3) return; // segurança

  // Page1:B7 → define par CS (0/1 ou 2/3)
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL1);                //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  _bus->DataWrite((cs_num >= 2) ? 0x80 : 0x00);  // bit7 = 1 para CS2/3, 0 para CS0/1

  // Page0:B7 → define dentro do par (0 = primeiro, 1 = segundo)
  PageSwitch(ePageReg::Page0);
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, page 0, Serial Flash/ROM Controller Register (SFL_CTRL)
  _bus->DataWrite((cs_num % 2) ? 0x80 : 0x00);   // bit7 = 1 para segundo do par, 0 para primeiro
}


//NUDAR ESTA FUNCAO... Náo existe o IDEC, o autor criou confusao... esta funcao é so apra o RA8876 apra modo compatibildaide com RA8875
/**
 * @brief IDEC Serial Interface Standard Mode 0 or 3
 *        
 * @verbatim                  
 * REG [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] IDEC Serial Interface
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, este bit [5-1] é não atribuido (NA).
 *       Mantem o modo intercompartibildaide sem gerar excecao.
 *       Se em RA8887/RA8876 a traca de pagina não existe pelo REG[46h], logo não fará efeito, passando para a execução
 *       dos registradores seguitnes [B7h] bit 4.
 *       Se em RA8889 [REG PAGE 1],[B7h] [bit4] não tem atribuição este bit é sempre 1.
 *       Se em RA8876/RA8877 Não existe troca de páginas 0 e 1 de registradores, acessados pelos registradores [46-4E].
 *
 * @result None
 */
void RA8889::IDEC_SPI_Select_StandardMode0orMode3(void)
{
  uint8_t temp;

  //Apenas para compatibilidade. Este bit nao existe no RA8889, 
  //será ignorado. No RA8876/RA8877 esta função de troca de pagina 
  //REG [46h-4eh] é reservado (sem funcao), mas o codigo abaixo para a 
  //escolha do bit 4 sim
  PageSwitch(ePageReg::Page1);                  //0x46, Troca para a Pagina 1 de registradores do RA8889, mas não existe no RA8876/RA8877
                                       
  //Acessar o registrador SFL_CTRL do RA8876/RA8877
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL) do RA8876/RA8877
  temp = _bus->DataRead();                       
  CLRB(temp,4);                                //Reset bit 4  
  _bus->DataWrite(temp);                         
									           
  PageSwitch(ePageReg::Page0);                 //Retorna para a Pagina 0 de registradores
}


//NUDAR ESTA FUNCAO... Náo existe o IDEC, o autor criou confusao... esta funcao é so apra o RA8876 apra modo compatibildaide com RA8875
/**
 * @brief IDEC Serial Interface Mode 0 and 3
 *        
 * @verbatim                  
 * [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *        bit [7] IDEC Serial Interface
 *                0: standard SPI mode 0 or mode 3 timing
 *                1: Follow RA8875 mode 0 & mode 3 timing
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 1, REG 0xB7 bit [5-1] é não atribuido (NA).
 *       Mantem o modo intercompartibildaide sem gerar excecao.
 *       Se em RA8887/RA8876 a troca de pagina não existe pelo REG[46h], logo não fará efeito, passando para a execução
 *       dos registradores seguitnes [B7h] bit 4.
 *       Se em RA8889 [REG PAGE 1],[B7h] [bit4] não tem atribuição este bit é sempre 1.
 *       Se em RA8876/RA8877 Não existe troca de páginas 0 e 1 de registradores, acessados pelos registradores [46-4E].
 *
 * @result None
 */
void RA8889::IDEC_RA8875_SPI_Select_Mode0andMode3(void)
{
  uint8_t temp;

  //Apenas para compatibilidade. Este bit nao existe no RA8889, 
  //será ignorado. No RA8876/RA8877 esta função de troca de pagina 
  //REG [46h-4eh] é reservado (sem funcao), mas o codigo abaixo para a 
  //escolha do bit 4 sim
  PageSwitch(ePageReg::Page1);                  //Troca para a Pagina 1 de registradores do RA8889, mas não existe no RA8876/RA8877

  //Acessar o registrador SFL_CTRL do RA8876/RA8877
  _bus->CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL) do RA8876/RA8877
  temp = _bus->DataRead();                       
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);                         
									           
  PageSwitch(ePageReg::Page0);                 //Retorna para a Pagina 0 de registradores
}


/**
 * @brief IDEC Select Serial Flash/ROM Address 24-bit
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 * bit [5] Serial Flash/ROM Address Mode
 *     0: 24 bits address mode
 *     1: 32 bits address mode
 *     If user wants to use 32 bits address mode, user must manually send EX4B command (B7h) to serial flash then set the bit to high.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::IDEC_SFI_Select_24bitAddress(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL1);                    //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  CLRB(temp,5);                                //Reset bit 5
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief IDEC Select Serial Flash/ROM Address 32-bit
 *        
 * @verbatim                  
 * PAGE 1 REG [0xb7] Serial flash AVI/JPG/BMP (IDEC_CTRL1)
 * bit [5] Serial Flash/ROM Address Mode
 *     0: 24 bits address mode
 *     1: 32 bits address mode
 *     If user wants to use 32 bits address mode, user must manually send EX4B command (B7h) to serial flash then set the bit to high.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void RA8889::IDEC_SFI_Select_32bitAddress(void)
{
  uint8_t temp;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CTRL1);                    //0xb7, page 1, Serial flash AVI/JPG/BMP (IDEC_CTRL1)
  temp = _bus->DataRead();
  SETB(temp,5);                                //Set bit 5
  _bus->DataWrite(temp);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xB8] SPI master Tx /Rx FIFO Data Register (SPIDR)
//
//================================================================================


/**
 * @brief SPI master Tx /Rx FIFO Get Data
 *        
 * @verbatim                  
 * REG [0xb8] SPI master Tx /Rx FIFO Data Register (SPIDR)
 *            bit [7-0] SPI master Tx /Rx FIFO Data Register
 *            
 *            After programming the core’s control register SPI 
 *            transfers can be initiated. A transfer is initiated by 
 *            writing to the Serial Peripheral Data Register [SPIDR]. 
 *            Writing to the Serial Peripheral Data Register is 
 *            actually writing to a 16 entries deep FIFO called the 
 *            Write FIFO. Each write access adds a data byte to the 
 *            Write FIFO. When the core is enabled – SS_ACTIVE is set 
 *            to 1 and the Write FIFO is not full, the core 
 *            automatically transfers the oldest data byte.
 *            
 *            Receiving data is done simultaneously with transmitting 
 *            data; whenever a data byte is transmitted a data byte is 
 *            received. For each byte that needs to be read from a 
 *            device, a dummy byte needs to be written to the Write 
 *            FIFO. This instructs the core to initiate an SPI 
 *            transfer, simultaneously transmitting the dummy byte and 
 *            receiving the desired data. Whenever a transfer is 
 *            finished, the received data byte is added to the Read 
 *            FIFO. The Read FIFO is the counterpart of the Write FIFO. 
 *            It is an independent 16 entries deep FIFO. The FIFO 
 *            contents can be read by reading from the Serial 
 *            Peripheral Data Register [SPIDR].                          
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
uint8_t RA8889::SPIM_TxRxFIFOData_Get(void)
{
  uint8_t temp;
  while (SPIM_RxFIFO_Empty() == 1);            //If it is not empty, execute it.
  _bus->CmdWrite(REG_SPIDR);                     //0xb8, SPI master Tx /Rx FIFO Data Register (SPIDR)
  temp = _bus->DataRead();                       
  // while(Rx_FIFO_full_flag());               //Required only when writing 16 records continuously
  return temp;
}


/**
 * @brief SPI master Tx /Rx FIFO Put Data
 *        
 * @verbatim                  
 * REG [0xb8] SPI master Tx /Rx FIFO Data Register (SPIDR)
 *            bit [7-0] SPI master Tx /Rx FIFO Data Register
 *            
 *            After programming the core’s control register SPI 
 *            transfers can be initiated. A transfer is initiated by 
 *            writing to the Serial Peripheral Data Register [SPIDR]. 
 *            Writing to the Serial Peripheral Data Register is 
 *            actually writing to a 16 entries deep FIFO called the 
 *            Write FIFO. Each write access adds a data byte to the 
 *            Write FIFO. When the core is enabled – SS_ACTIVE is set 
 *            to 1 and the Write FIFO is not full, the core 
 *            automatically transfers the oldest data byte.
 *            
 *            Receiving data is done simultaneously with transmitting 
 *            data; whenever a data byte is transmitted a data byte is 
 *            received. For each byte that needs to be read from a 
 *            device, a dummy byte needs to be written to the Write 
 *            FIFO. This instructs the core to initiate an SPI 
 *            transfer, simultaneously transmitting the dummy byte and 
 *            receiving the desired data. Whenever a transfer is 
 *            finished, the received data byte is added to the Read 
 *            FIFO. The Read FIFO is the counterpart of the Write FIFO. 
 *            It is an independent 16 entries deep FIFO. The FIFO 
 *            contents can be read by reading from the Serial 
 *            Peripheral Data Register [SPIDR].                          
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
uint8_t RA8889::SPIM_TxRxFIFOData_Put(uint8_t data)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIDR);                   //0xb8, SPI master Tx /Rx FIFO Data Register (SPIDR)
  _bus->DataWrite(data);
  while (SPIM_TxFIFO_Empty() == 0);
  temp = SPIM_TxRxFIFOData_Get();
  return temp;
}


//================================================================================
//
// [0xB9] SPI master Control Register (SPIMCR2)
//
//================================================================================


/**
 * @brief Control Slave Select drive on which xnsfcs0..xnsfcs3 Channel
 *        
 * @verbatim                  
 * Controle de acionamento do sinal de seleção do escravo (xnsfcs0..xnsfcs3)
 *
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [7,5] Control Slave Select drive on which xnsfcs
 *                      B7 and B5 = 00b: nSS drive on xnsfcs[0]
 *                      B7 and B5 = 01b: nSS drive on xnsfcs[1]
 *                      B7 and B5 = 10b: nSS drive on xnsfcs[2]
 *                      B7 and B5 = 11b: nSS drive on xnsfcs[3]
 * @endverbatim
 *
 * @param eNSS_Channel::XNSFCS0
 *        eNSS_Channel::XNSFCS1
 *        eNSS_Channel::XNSFCS2
 *        eNSS_Channel::XNSFCS3
 *
 * @note None
 *
 * @result None
 */
void RA8889::nSS_Select_Channel(eNSS_Channel channel)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  MSKB(temp,1U<<7|1U<<5,0);                    //Reset bits 7 and 5
  // aplica valor do canal (2 bits: b7:b5)
  if (static_cast<uint8_t>(channel) & 0x02) SETB(temp,7);  // bit1 → b7
  if (static_cast<uint8_t>(channel) & 0x01) SETB(temp,5);  // bit0 → b5
  _bus->DataWrite(temp);
}


/**
 * @brief Interrupt SPI Master Enable/Disable
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [6] SPI Master Interrupt enable
 *                    0b0: Disable interrupt.
 *                    0b1: Enable interrupt.
 *            *** If you disable SPIM interrupt flag, then RA8889 won’t 
 *            assert interrupt event to inform MPU but you still may 
 *            check interrupt event on SPIMSR.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Interrupt_SPIM_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  b ? SETB(temp,6) : CLRB(temp,6);            //Set/Reset bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Slave Select Singal Inactive (nSS Port)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [4] Slave Select signal active [SS_ACTIVE]
 *                    0b0: inactive (nSS port will goes high)
 *                    0b1: active (nSS port will goes low)
 *            While Slave Select signal be in inactive, FIFO will be 
 *            cleared and this function will stay in Idle state. Note: 
 *            Do not change CPOL/CPHA when Slave Select signal is 
 *            active.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::nSS_Inactive(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  CLRB(temp,4);                                //Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Slave Select Singal Inactive (nSS Port)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [4] Slave Select signal active [SS_ACTIVE]
 *                    0b0: inactive (nSS port will goes high)
 *                    0b1: active (nSS port will goes low)
 *            While Slave Select signal be in inactive, FIFO will be 
 *            cleared and this function will stay in Idle state. Note: 
 *            Do not change CPOL/CPHA when Slave Select signal is 
 *            active.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::nSS_Active(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief interrupt Enable/Disable for FIFO overflow error
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [3] Mask interrupt for FIFO overflow error [OVFIRQEN]
 *                    0b0: unmask
 *                    0b1: mask
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Interrupt_FIFOOverflow_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  b ? SETB(temp,3) : CLRB(temp,3);            //Set/Reset bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [2] Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
 *                    0b0: unmask
 *                    0b1: mask
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Interrupt_EMTIRQEN_Enable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  b ? SETB(temp,2) : CLRB(temp,2);             //Set/Reset bit 2
  _bus->DataWrite(temp);
}


/**
 * @brief Reset Clock Polarity (CPOL)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Reset_CPOL(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  CLRB(temp,1);                                //Reset bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief Set Clock Polarity (CPOL)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Set_CPOL(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  SETB(temp,1);                                //Set bit 1
  _bus->DataWrite(temp);
}


/**
 * @brief Reset Clock Phase (CPHA)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Reset_CPHA(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Set Clock Phase (CPHA)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Set_CPHA(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = _bus->DataRead();
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 0
// [0xBA] SPI master Status Register (SPIMSR)
//
//================================================================================


/**
 * @brief Tx FIFO empty flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [7] Tx FIFO empty flag
 *                    0: not empty
 *                    1: empty
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is empty 
 */
bool RA8889::SPIM_TxFIFO_Empty(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x80);              //Check bit 7
}


/**
 * @brief Tx FIFO full flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [6] Tx FIFO full flag
 *                    0: not full
 *                    1: full
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is full 
 */
bool RA8889::SPIM_TxFIFO_Full(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x40);              //Check bit 6
} 


/**
 * @brief Rx FIFO empty flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [5] Rx FIFO empty flag
 *                    0: not empty
 *                    1: empty
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is empty 
 */
bool RA8889::SPIM_RxFIFO_Empty(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x20);              //Check bit 5
} 


/**
 * @brief Rx FIFO full flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [4] Rx FIFO full flag
 *                    0: not full
 *                    1: full
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is full 
 */
bool RA8889::SPIM_RxFIFO_Full(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x10);              //Check bit 4
} 


/**
 * @brief Occur Overflow interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [3] Overflow interrupt flag
 *            Read: 
 *            0: No Overflow
 *            1: Overflow interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: occur overflow
 */
bool RA8889::Interrupt_Overflow_Flag(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x08);              //Occur Overflow Interrupt
}


/**
 * @brief Clear Overflow interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [3] Overflow interrupt flag
 *            Read: 
 *            0: No Overflow
 *            1: Overflow interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::Interrupt_ClearOverflow_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  temp = _bus->DataRead();
  SETB(temp,3);                                //Set bit 3
  _bus->DataWrite(temp);
}


/**
 * @brief Occur Tx FIFO empty /FSM idle interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 * bit [2] Tx FIFO empty /FSM idle interrupt flag
 * Read: 
 * 0: No interrupt flag
 * 1: interrupt flag
 * Write
 * 1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: occur Tx FIFO empty /FSM idle
 */
bool RA8889::EMTI_Flag(void)
{
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (_bus->DataRead() & 0x04);              //occur Tx FIFO empty /FSM idle interrupt flag
}


/**
 * @brief Clear Tx FIFO empty /FSM idle interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 * bit [2] Tx FIFO empty /FSM idle interrupt flag
 * Read: 
 * 0: No interrupt flag
 * 1: interrupt flag
 * Write
 * 1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::EMTI_Clear_Flag(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  temp = _bus->DataRead();
  SETB(temp,2);                                //Set bit 2
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 0
// [0xBB] SPI Clock period (SPI_DIVSOR)
//
//================================================================================


/**
 * @brief SPI Clock period
 *        
 * @verbatim                  
 * REG [0xbb] SPI Clock period (SPI_DIVSOR)
 * bit [7-0] SPI Clock period (default Fsck=3)
 *     According to system clock to set low & high period for SPI clock. 
 *     SPI Master:
 *       Fsck = Fcore / (divisor * 2)       //Provavelmente incorreto
 *       Fsck = Fcore / ((divisor + 1)* 2)  //Provavelmente correto
 *     Serial Flash:
 *       Fsck = Fcore / (divisor * 2)
 *     When SPI_DIVSOR = 0,
 *       Fsck = Fcore
 * @endverbatim
 *
 * @param divisor: valor do divisor para o ajuste de frequencia do periodo de clock do SPI
 *
 * @bug
 * No datasheet indica a equação SPI master de frequencia de clock SPI Fsck = Fcore / (divisor * 2)
 * que segundo alguns codigos produzidos pela propria RAIO, o correto é: Fsck = Fcore / ((divisor + 1) * 2)
 *
 * @note quando se fala em divisor é o resultado final do denominador (divdendo),
 *       no caso ((divisor + 1)* 2), mas o valor do divisor é 0,1,2,3,4... e dividendo seria 2,4,6,8,10...
 *
 * @result None
 */
void RA8889::SPI_Clock_Period(uint8_t divisor)
{
  _bus->CmdWrite(REG_SPI_DIVSOR);                //0xbb, SPI Clock period (SPI_DIVSOR)
  _bus->DataWrite(divisor);
} 


//================================================================================
//
// PAGE 1
// [0xBB] IDEC Clock divide (IDEC_CLKDIV)
//
//================================================================================


/**
 * @brief IDEC SPI Clock Divide
 *        
 * @verbatim                  
 * PAGE 1 REG [0xbb] IDEC Clock divide
 * bit [7-0] 
 *      2’b00: idec_clock = cclk
 *      2’b01: idec_clock = cclk/2
 *      2’b10: idec_clock = cclk/4
 *      2’b11: reserved
 *      
 *      Note:
 *      1.This register is used for setting the clock speed for idec_serial flash and idec block
 *      2.The clock must exceed 2 times of xosc (10Mhz),
 *        For example: cclk = 50Mhz,
 *        Idec_clk = 50/2 = 25>(2*10)Mhz, then valid
 *        Idec_clk = 50/4,= 12.5 < (2*10)Mhz, then invalid
 * @endverbatim
 *
 * @param spiclockdivide
 *
 * @note IDEC_clock must be more than 2 times OSC clock. IDEC_clock must be > 2 x OSC_clock.
 *
 * @result None
 */
void RA8889::IDEC_SPI_ClockDivide(uint8_t spiclockdivide)
{
  uint32_t temp = 0;
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_CLKDIV);               //0xbb, page 1, IDEC Clock divide (IDEC_CLKDIV)
  _bus->DataWrite(spiclockdivide);
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xBC] Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
// [0xBD] Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
// [0xBE] Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
// [0xBF] Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
//
//================================================================================


/**
 * @brief Configura o endereço inicial de origem de leitura da Flash Serial DMA
 *        Serial Flash I/F DMA Source Starting Address
 *        
 * @verbatim
 *
 * Esta função define o endereço inicial de onde o RA8889 irá ler os dados
 * da memória serial flash quando um DMA for iniciado. O endereço é de 32 bits,
 * dividido em quatro registradores consecutivos (DMA_SSTR0 ~ DMA_SSTR3).
 *
 * REG [0xbc] Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [7:0]
 *            This register sets the start address [7:0] of serial flash memory.
 *            Directly specify the start address of source image.
 *             
 * REG [0xbd] Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [15:8]
 *            This register sets the start address [15:8] of serial flash memory.
 *            Directly specify the start address of source image.
 *            
 * REG [0xbe] Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [23:16]
 *            This register sets the start address [23:16] of serial flash memory.
 *            Directly specify the start address of source image.
 * 
 * REG [0xbf] Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [31:24]
 *            This register sets the start address [31:24] of serial flash memory.
 *            Directly specify the start address of source image.
 * @endverbatim
 *
 * @param addr: 
 *
 * @note This bits index serial flash address [7:0][15:8][23:16][31:24]
 *
 * @result None
 */
void RA8889::SFI_DMA_SourceAddress(uint32_t addr)
{
//  _bus->CmdWrite(REG_DMA_SSTR0);      //0xbc, Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
//  _bus->DataWrite(addr);              //address [7:0]
//  _bus->CmdWrite(REG_DMA_SSTR1);      //0xbd, Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
//  _bus->DataWrite(addr >> 8);         //address [15:8]
//  _bus->CmdWrite(REG_DMA_SSTR2);      //0xbe, Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
//  _bus->DataWrite(addr >> 16);        //address [23:16]
//  _bus->CmdWrite(REG_DMA_SSTR3);      //0xbf, Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
//  _bus->DataWrite(addr >> 24);        //address [31:24]
  
  // Array com os registradores consecutivos
  const uint8_t regs[4] = {REG_DMA_SSTR0, REG_DMA_SSTR1, REG_DMA_SSTR2, REG_DMA_SSTR3};
  for (int i = 0; i < 4; i++) {
      _bus->CmdWrite(regs[i]);                   //envia cada 8 bits para os registradores de endereço
      _bus->DataWrite((addr >> (8 * i)) & 0xFF); // envia cada byte do endereço
  }
}


//================================================================================
//
// PAGE 1
// [0xBC] Serial flash AVI/JPG/BMP Source Starting Address 0 (IDEC_SADR0)
// [0xBD] Serial flash AVI/JPG/BMP Source Starting Address 1 (IDEC_SADR1)
// [0xBE] Serial flash AVI/JPG/BMP Source Starting Address 2 (IDEC_SADR2)
// [0xBF] Serial flash AVI/JPG/BMP Source Starting Address 3 (IDEC_SADR3)
//
//================================================================================


/**
 * @brief Define o endereço inicial da fonte de dados (AVI,JPG,BMP) IDEC no RA8889.
 *
 * @verbatim
 * PAGE 1 REG [0xbc] Serial flash AVI/JPG/BMP Source Starting Address 0 (IDEC_SADR0)
 * bit [7-0] Serial flash IDEC Source START ADDRESS [7:0]
 *           The register represents serial flash address [7:0]
 *           Direct point to the start address of source image in serial flash.
 *
 * PAGE 1 REG [0xbd] Serial flash AVI/JPG/BMP Source Starting Address 1 (IDEC_SADR1)
 * bit [7-0] Serial flash IDEC Source START ADDRESS [15:8]
 *           The register represents serial flash address [15:8]
 *           Direct point to the start address of source image in serial flash.
 *
 * PAGE 1 REG [0xbe] Serial flash AVI/JPG/BMP Source Starting Address 2 (IDEC_SADR2)
 * bit [7-0] Serial flash IDEC Source START ADDRESS [23:16]
 *           The register represents serial flash address [23:16]
 *           Direct point to the start address of source image in serial flash.
 *
 * PAGE 1 REG [0xbf] Serial flash AVI/JPG/BMP Source Starting Address 3 (IDEC_SADR3)
 * bit [7-0] Serial flash IDEC Source START ADDRESS [31:24]
 *           The register represents serial flash address [31:24]
 *           Direct point to the start address of source image in serial flash.
 *
 * Esta função seleciona a Page 1 dos registradores internos e configura
 * os registradores consecutivos IDEC_SADR0~3, que armazenam o endereço
 * de início (source start address) dos dados de imagem/vídeo localizados
 * na memória flash serial externa (AVI/JPG/BMP).
 *
 * O endereço de 32 bits é dividido em quatro bytes, gravados em ordem
 * crescente (little-endian), do registrador IDEC_SADR0 (byte menos significativo)
 * até IDEC_SADR3 (byte mais significativo).
 *
 * Após a configuração, a função retorna a seleção de página para Page 0.
 * @endverbatim
 *
 * @param addr Endereço de 32 bits que indica a posição inicial dos dados
 *             na memória serial flash.
 *
 * @note
 * - Página de registradores utilizada: Page 1.
 * - Registradores escritos: 0xBC–0xBF (IDEC_SADR0~3).
 * - O IDEC utiliza esse endereço como referência para iniciar a leitura
 *   dos dados gráficos compactados.
 */
void RA8889::IDEC_Source_StartAddress(uint32_t addr) 
{
  PageSwitch(ePageReg::Page1);
  // Array com os registradores consecutivos
  const uint8_t regs[4] = {REG_IDEC_SADR0, REG_IDEC_SADR1, REG_IDEC_SADR2, REG_IDEC_SADR3};
    for (int i = 0; i < 4; i++) {
        _bus->CmdWrite(regs[i]);                            //envia cada 8 bits para os registradores de endereço
        _bus->DataWrite((uint8_t)(addr >> (8 * i)) & 0xFF); // envia cada byte do endereço
    }
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xC0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
// [0xC1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
// [0xC2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
// [0xC3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
//
//================================================================================


/**
 * @brief Configura o endereço inicial de destino da Flash Serial DMA
 *        Serial Flash I/F DMA Destination Starting Address
 *        
 * @verbatim
 * REG [0xc0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [7:2] in SDRAM.
 *
 * REG [0xc1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [15:8] in SDRAM.
 *
 * REG [0xc2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [23:16] in SDRAM.
 *
 * REG [0xc3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [31:24] in SDRAM.
 * @endverbatim
 *
 * @param addr: 
 *
 * @note This bits index SDRAM address [7:0][15:8][23:16][31:24], bits [1:0] fix at 0.
 *
 * @result None
 */
void RA8889::SFI_DMA_DestinationAddress(uint32_t addr)
{
    //Array com os registradores consecutivos
    const uint8_t regs[4] = {REG_DMA_DX0, REG_DMA_DX1, REG_DMA_DY0, REG_DMA_DY1};
    for (int i = 0; i < 4; i++) {
        _bus->CmdWrite(regs[i]);                   //envia cada 8 bits para os registradores de endereço
        _bus->DataWrite((addr >> (8 * i)) & 0xFF); //envia cada byte do endereço
    }
}


/**
 * @brief  Configura o ponto superior esquerdo da janela de destino do DMA
 *
 * @verbatim
 * Define as coordenadas (X, Y) do canto superior esquerdo da área de destino
 * para onde os dados transferidos por DMA serão gravados no canvas do RA8889.
 * 
 * Em modo Block (REG 5Eh bit2 = 0), os registradores definem diretamente
 * as coordenadas X e Y do ponto inicial no canvas.
 * 
 * Em modo Linear (REG 5Eh bit2 = 1), os mesmos registradores são usados
 * como endereço base de destino na SDRAM.
 *
 * REG [0xc0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [7:2] in SDRAM.
 *
 * REG [0xc1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [15:8] in SDRAM.
 *
 * REG [0xc2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [23:16] in SDRAM.
 *
 * REG [0xc3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [31:24] in SDRAM.
 * @endverbatim
 *
 * @param Wx Coordenada X inicial (0 ~ 4095) da janela de destino
 * @param Hy Coordenada Y inicial (0 ~ 4095) da janela de destino
 *
 * @note Para uso no modo de Canvas (Block Mode), REG 5Eh (AW_COLOR) bit 1 = 0, Block Mode
 *
 * @result None
 */
void RA8889::SFI_DMA_DestinationUpperLeftCorner(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_DMA_DX0);                   //0xc0, DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
  _bus->DataWrite(Wx);                           //byte baixo de Wx [7:0]  
  _bus->CmdWrite(REG_DMA_DX1);                   //0xc1, DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
  _bus->DataWrite(Wx >> 8);                      //byte alto de Wx [12:8] 
									           
  _bus->CmdWrite(REG_DMA_DY0);                   //0xc2, DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
  _bus->DataWrite(Hy);                           //byte baixo de Hy [7:0]
  _bus->CmdWrite(REG_DMA_DY1);                   //0xc3, DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
  _bus->DataWrite(Hy >> 8);                      //byte alto de Hy [12:8]
}



//================================================================================
//
// PAGE 0
// [0xC5] SPI Master Bus Select (SPIMBS)
//
//================================================================================


/**
 * @brief  Select SPI Master Bus 0
 *
 * @verbatim
 * REG [0xc5] SPI Master Bus Select (SPIMBS)
 * bit [7] SPI master bus select
 *     0: Bus 0(xsck, xmosi, xmiso)
 *     1: Bus 1(xspi1_sck, xspi1_msio0, xspi1_msio1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::SPIM_Select_Bus0(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMBS);                  //0xc5, SPI Master Bus Select (SPIMBS)
  temp = _bus->DataRead();
  CLRB(temp,7);                               //bit 7=0
  _bus->DataWrite(temp);
}


/**
 * @brief  Select SPI Master Bus 1
 *
 * @verbatim
 * REG [0xc5] SPI Master Bus Select (SPIMBS)
 * bit [7] SPI master bus select
 *     0: Bus 0(xsck, xmosi, xmiso)
 *     1: Bus 1(xspi1_sck, xspi1_msio0, xspi1_msio1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
 void RA8889::SPIM_Select_Bus1(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMBS);                  //0xc5, SPI Master Bus Select (SPIMBS)
  temp = _bus->DataRead();
  SETB(temp,7);                              //bit 7=1
  _bus->DataWrite(temp);
}


/**
 * @brief  Configura o SPI Master para capturar dados na borda de subida do clock
 *
 * Essas funções permitem selecionar a borda de captura do registrador RX do SPI Master.
 * A escolha da borda correta é importante para sincronização de dados com dispositivos
 * conectados ao barramento SPI.
 *
 * @verbatim
 * REG [0xc5] SPI Master Bus Select (SPIMBS)
 * bit [5] SPI master rx register latch edge
 *     0: cclk rising edge
 *     1: cclk falling edge
 *
 * - SPIM_RxLatchEdge_Rising() → configura o SPI Master para capturar os 
 *   dados do registrador RX na borda de subida do clock.
 * - SPIM_RxLatchEdge_Falling() → configura o SPI Master para capturar os 
 *   dados do registrador RX na borda de descida do clock.
 *
 * Isso garante compatibilidade e sincronização com diferentes dispositivos 
 * SPI, que podem exigir captura em bordas específicas.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note Estas funções alteram apenas o bit 5 do registrador SPIMBS,
 *       preservando os demais bits.
 *
 * @result None
 */
void RA8889::SPIM_RxLatchEdge_Rising(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMBS);                  //0xc5, SPI Master Bus Select (SPIMBS)
  temp = _bus->DataRead();
  CLRB(temp,5);                              //bit 5=0
  _bus->DataWrite(temp);
}


/**
 * @brief  Configura o SPI Master para capturar dados na borda de descida do clock
 *
 * Essas funções permitem selecionar a borda de captura do registrador RX do SPI Master.
 * A escolha da borda correta é importante para sincronização de dados com dispositivos
 * conectados ao barramento SPI.
 *
 * - SPIM_RxLatchEdge_Rising() → configura o SPI Master para capturar os 
 *   dados do registrador RX na borda de subida do clock.
 * - SPIM_RxLatchEdge_Falling() → configura o SPI Master para capturar os 
 *   dados do registrador RX na borda de descida do clock.
 *
 * Isso garante compatibilidade e sincronização com diferentes dispositivos 
 * SPI, que podem exigir captura em bordas específicas.
 *
 * @verbatim
 * REG [0xc5] SPI Master Bus Select (SPIMBS)
 * bit [5] SPI master rx register latch edge
 *     0: cclk rising edge
 *     1: cclk falling edge
 * @endverbatim
 *
 * @param None
 *
 * @note Estas funções alteram apenas o bit 5 do registrador SPIMBS,
 *       preservando os demais bits.
 *
 * @result None
 */
void RA8889::SPIM_RxLatchEdge_Falling(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_SPIMBS);                  //0xc5, SPI Master Bus Select (SPIMBS)
  temp = _bus->DataRead();
  SETB(temp,5);                              //bit 5=1, B0010_0000
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0xC0] IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 0 (IDEC_DX0)
// [0xC1] IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 1 (IDEC_DX1)
// [0xC2] IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 0 (IDEC_DY0)
// [0xC3] IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 1 (IDEC_DY1)
//
//================================================================================


/**
 * @brief Define the IDEC destination window upper-left corner coordinates.
 * 
 * Esta função configura os registradores do RA8889 responsáveis por indicar
 * a posição inicial (X, Y) onde a imagem decodificada (JPG/BMP) será exibida
 * na área do canvas. As coordenadas fornecidas em 16 bits são divididas e
 * gravadas nos registradores IDEC_DX0~1 (X) e IDEC_DY0~1 (Y).
 *
 * @verbatim                  
 * PAGE 1 REG [0xC0] IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 0 (IDEC_DX0)
 * bit [7-0] Block Mode (Mode coordinate X-Y)
 *           This register defines IDEC Destination Window Upper-Left corner X-coordinates [7:0] on Canvas area.
 *
 * PAGE 1 REG [0xC1] IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 1 (IDEC_DX1)
 * bit [7-0] Block Mode (Mode coordinate X-Y)
 *           This register defines IDEC Destination Window Upper-Left corner X-coordinates [12:8] on Canvas area.
 *
 * PAGE 1 REG [0xC2] IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 0 (IDEC_DY0)
 * bit [7-0] Block Mode (Mode coordinate X-Y)
 *           This register defines IDEC Destination Window Upper-Left corner Y-coordinates [7:0] on Canvas area.
 *
 * PAGE 1 REG [0xC3] IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 1 (IDEC_DY1)
 * bit [7-0] Block Mode (Mode coordinate X-Y)
 *           This register defines IDEC Destination Window Upper-Left corner Y-coordinates [12:8] on Canvas area.
 *
 * @endverbatim
 *
 * @param Wx Coordenada X de 16 bits da posição superior-esquerda da janela de destino.
 * @param Hy Coordenada Y de 16 bits da posição superior-esquerda da janela de destino.
 *
 * @note
 * - Página de registradores utilizada: Page 1.
 * - Registradores escritos: 0xC0–0xC3 (IDEC_DX0~1, IDEC_DY0~1).
 * - Após a configuração, a página ativa retorna para Page 0.
 *
 * @result None
 */
void RA8889::IDEC_Destination_UpperLeftCorner(uint16_t Wx, uint16_t Hy)
{
  PageSwitch(ePageReg::Page1);
  
  _bus->CmdWrite(REG_IDEC_DX0);             //0xc0, page 1, IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 0 (IDEC_DX0)
  _bus->DataWrite(Wx);
  _bus->CmdWrite(REG_IDEC_DX1);             //0xc1, page 1, IDEC (JPG/BMP) Destination Window Upper-Left corner X-coordinates 1 (IDEC_DX1)
  _bus->DataWrite(Wx >> 8);
  
  _bus->CmdWrite(REG_IDEC_DY0);             //0xc2, page 1, IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 0 (IDEC_DY0)
  _bus->DataWrite(Hy);
  _bus->CmdWrite(REG_IDEC_DY1);             //0xc3, page 1, IDEC (JPG/BMP) Destination Window Upper-Left corner Y-coordinates 1 (IDEC_DY1)
  _bus->DataWrite(Hy >> 8);

  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 1
// [0xC5] IDEC AVI PIP controller (IDEC_PIP)
//
//================================================================================


/**
 * @brief Set AVI display buffer to PIP1 with shadow
 * 
 * Configures the IDEC AVI PIP controller to output the AVI display
 * buffer to PIP1 and enable shadow PIP.
 *
 * @verbatim                  
 * PAGE 1 REG [0xC5] IDEC AVI PIP controller (IDEC_PIP)
 * bit [1-0] 
 *     0b00: AVI display buffer use pip1 + shadow pip
 *     0b01: AVI display buffer use pip2 + shadow pip
 *     0b1X: AVI display buffer use pip1
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_AVI_Decoding_PIP1_Shadow(void)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_PIP);                  //0xc5, page 1, IDEC AVI PIP controller (IDEC_PIP)
  _bus->DataWrite(0x00);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief Set AVI display buffer to PIP2 with shadow
 * 
 * Configures the IDEC AVI PIP controller to output the AVI display
 * buffer to PIP2 and enable shadow PIP.
 *
 * @verbatim                  
 * PAGE 1 REG [0xC5] IDEC AVI PIP controller (IDEC_PIP)
 * bit [1-0] 
 *     0b00: AVI display buffer use pip1 + shadow pip
 *     0b01: AVI display buffer use pip2 + shadow pip
 *     0b1X: AVI display buffer use pip1
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_AVI_Decoding_PIP2_Shadow(void)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_PIP);                  //0xc5, page 1, IDEC AVI PIP controller (IDEC_PIP)
  _bus->DataWrite(0x01);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief Set AVI display buffer to PIP1 without shadow
 * 
 * Configures the IDEC AVI PIP controller to output the AVI display
 * buffer to PIP1 and disable shadow PIP.
 *
 * @verbatim                  
 * PAGE 1 REG [0xC5] IDEC AVI PIP controller (IDEC_PIP)
 * bit [1-0] 
 *     0b00: AVI display buffer use pip1 + shadow pip
 *     0b01: AVI display buffer use pip2 + shadow pip
 *     0b1X: AVI display buffer use pip1
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_AVI_Decoding_PIP1_NoShadow(void)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_PIP);                  //0xc5, page 1, IDEC AVI PIP controller (IDEC_PIP)
  _bus->DataWrite(0x02);
  PageSwitch(ePageReg::Page0);
}


/**
 * @brief Set AVI display buffer mode (generic)
 *
 * Configures the IDEC AVI PIP controller to select the AVI display buffer
 * output mode, using an enum to select PIP1/PIP2 and shadow/no-shadow.
 *
 * @verbatim
 * PAGE 1 REG [0xC5] IDEC AVI PIP controller (IDEC_PIP)
 * Bits [1:0]:
 *     0b00: AVI display buffer use PIP1 + shadow PIP
 *     0b01: AVI display buffer use PIP2 + shadow PIP
 *     0b1X: AVI display buffer use PIP1
 * @endverbatim
 *
 * @param mode Mode selection (PIP1/PIP2, Shadow/NoShadow)
 *
 * @note None
 *
 * @result None
 */
void RA8889::IDEC_AVI_SetMode(eAVIMode mode)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_PIP);                  //0xc5, page 1, IDEC AVI PIP controller (IDEC_PIP)
  _bus->DataWrite(static_cast<uint8_t>(mode));
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xC6] DMA Block Width 0 (DMAW_WTH0)
// [0xC7] DMA Block Width 1 (DMAW_WTH1)
// [0xC8] DMA Block Height 0 (DMAW_HIGH0)
// [0xC9] DMA Block Height 1 (DMAW_HIGH1)
//
//================================================================================


/**
 * @brief  
 *
 * @verbatim
 *
 * REG [0xc6] DMA Block Width 0 (DMAW_WTH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [7:0]
 *
 * REG [0xc7] DMA Block Width 1 (DMAW_WTH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [15:8]
 *
 * REG [0xc8] DMA Block Height 0 (DMAW_HIGH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [23:16]
 *
 * REG [0xc9] DMA Block Height 1 (DMAW_HIGH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note 
 *
 * @result Unit : Pixel
 *         When REG DMACR bit 1 = 0 (Linear Mode)
 *         DMA Transfer Number [7:0][15:8][23:16][31:24]
 *         
 *         When REG DMACR bit 1 = 1 (Block Mode)
 *         DMA Block Width [7:0][15:8]
 *         DMA Block HIGH[7:0][15:8]
 */
void RA8889::SFI_DMA_TransferNumber(uint32_t addr)
{
  _bus->CmdWrite(REG_DMAW_WTH0);                   //0xc6, DMA Block Width 0 (DMAW_WTH0)
  _bus->DataWrite(addr);                           
  _bus->CmdWrite(REG_DMAW_WTH1);                   //0xc7, DMA Block Width 1 (DMAW_WTH1)
  _bus->DataWrite(addr >> 8);                      
  _bus->CmdWrite(REG_DMAW_HIGH0);                  //0xc8, DMA Block Height 0 (DMAW_HIGH0)
  _bus->DataWrite(addr >> 16);                     
  _bus->CmdWrite(REG_DMAW_HIGH1);                  //0xc9, DMA Block Height 1 (DMAW_HIGH1)
  _bus->DataWrite(addr >> 24);                     
}


/**
 * @brief  
 *
 * @verbatim
 *
 * REG [0xc6] DMA Block Width 0 (DMAW_WTH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [7:0]
 *
 * REG [0xc7] DMA Block Width 1 (DMAW_WTH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [15:8]
 *
 * REG [0xc8] DMA Block Height 0 (DMAW_HIGH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [23:16]
 *
 * REG [0xc9] DMA Block Height 1 (DMAW_HIGH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note 
 *
 * @result Unit : Pixel
 *         When REG DMACR bit 1 = 0 (Linear Mode)
 *         DMA Transfer Number [7:0][15:8][23:16][31:24]
 *         
 *         When REG DMACR bit 1 = 1 (Block Mode)
 *         DMA Block Width [7:0][15:8]
 *         DMA Block HIGH[7:0][15:8]
 */
void RA8889::SFI_DMA_TransferWidthHeight(uint16_t Wx, uint16_t Hy)
{
  _bus->CmdWrite(REG_DMAW_WTH0);                 //0xc6, DMA Block Width 0 (DMAW_WTH0)
  _bus->DataWrite(Wx);                           //byte baixo de Wx [7:0]
  _bus->CmdWrite(REG_DMAW_WTH1);                 //0xc7, DMA Block Width 1 (DMAW_WTH1)
  _bus->DataWrite(Wx >> 8);                      //byte alto de Wx [12:8]
  
  _bus->CmdWrite(REG_DMAW_HIGH0);                //0xc8, DMA Block Height 0 (DMAW_HIGH0)
  _bus->DataWrite(Hy);                           //byte baixo de Hy [7:0]
  _bus->CmdWrite(REG_DMAW_HIGH1);                //0xc9, DMA Block Height 1 (DMAW_HIGH1)
  _bus->DataWrite(Hy >> 8);                      //byte alto de Hy [12:8]
}


//================================================================================
//
// PAGE 1
// [0xC6] IDEC (AVI/JPG/BMP) transfer number 0 (IDEC_TF0)
// [0xC7] IDEC (AVI/JPG/BMP) transfer number 1 (IDEC_TF1)
// [0xC8] IDEC (AVI/JPG/BMP) transfer number 2 (IDEC_TF2)
// [0xC9] IDEC (AVI/JPG/BMP) transfer number 3 (IDEC_TF3)
//
//================================================================================


/**
 * @brief Configura o número de transferência DMA de imagem do IDEC
 *
 * Esta função define o tamanho do bloco de dados (em bytes) que o 
 * IDEC (Image Display Engine Controller) do RA8889 irá transferir para 
 * imagens JPG/BMP. O valor de 32 bits é automaticamente dividido e 
 * armazenado nos registradores IDEC_TF0~IDEC_TF3. 
 * 
 * @verbatim
 * PAGE 1 REG [0xc6] IDEC (AVI/JPG/BMP) transfer number 0 (IDEC_TF0)
 * bit [7-0] Image DMA Transfer Number [7:0]
 *           The number in IDEC_TF[31:0] is the image size.
 *
 * PAGE 1 REG [0xc7] IDEC (AVI/JPG/BMP) transfer number 1 (IDEC_TF1)
 * bit [7-0] Image DMA Transfer Number [15:8]
 *           The number in IDEC_TF[31:0] is the image size.
 *
 * PAGE 1 REG [0xc8] IDEC (AVI/JPG/BMP) transfer number 2 (IDEC_TF2)
 * bit [7-0] Image DMA Transfer Number [23:16]
 *           The number in IDEC_TF[31:0] is the image size.
 *
 * PAGE 1 REG [0xc9] IDEC (AVI/JPG/BMP) transfer number 3 (IDEC_TF3)
 * bit [7-0] Image DMA Transfer Number [31:24]
 *           The number in IDEC_TF[31:0] is the image size.
 * @endverbatim
 *
 * @param num 32-bit total number of bytes to transfer for the image (JPG/BMP)
 *
 * @note O número em IDEC é o tamanho da imagem.
 *       A função lida automaticamente com a troca de páginas e escreve
 *       cada byte no registrador correspondente.
 *       Retorna para Page0 ao final. 
 */
void RA8889::IDEC_SetImageDMANumber(uint32_t num)
{
  PageSwitch(ePageReg::Page1);
  // Array com os registradores consecutivos
  const uint8_t regs[4] = {REG_IDEC_TF0, REG_IDEC_TF1, REG_IDEC_TF2, REG_IDEC_TF3};
  for (int i = 0; i < 4; i++) {
      _bus->CmdWrite(regs[i]);                   //envia cada 8 bits para os registradores de endereço
      _bus->DataWrite((num >> (8 * i)) & 0xFF); // envia cada byte do endereço
  }
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xCA] DMA Source Picture Width 0 (DMA_SWTH0)
// [0xCB] DMA Source Picture Width 0 (DMA_SWTH1)
//
//================================================================================


/**
 * @brief  DMA Source Picture Width
 *
 * @verbatim
 * REG [0xca] DMA Source Picture Width 0 (DMA_SWTH0)
 *            bit [7-0] DMA Source Picture Width [7:0]
 *            Unit: pixel
 *
 * REG [0xcb] DMA Source Picture Width 0 (DMA_SWTH1)
 *            bit [4-0] DMA Source Picture Width [12:8]
 * @endverbatim
 *
 * @param Wx
 *
 * @note DMA Source Picture Width [7:0][12:8]
 *
 * @result None
 */
void RA8889::SFI_DMA_SourceWidth(uint16_t Wx)
{
  _bus->CmdWrite(REG_DMA_SWTH0);       //0xca, DMA Source Picture Width 0 (DMA_SWTH0)
  _bus->DataWrite(Wx);
  _bus->CmdWrite(REG_DMA_SWTH1);       //0xcb, DMA Source Picture Width 0 (DMA_SWTH1)
  _bus->DataWrite(Wx >> 8);
}


//================================================================================
//
// [0xCC] Character Control Register 0 (CCR0)
//
//================================================================================


/**
 * @brief Select User-defined Font in Text Mode
 *
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_UseUserDefined(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set bit 7, Select user-defined Character
  CLRB(temp,6);                                //Reset bit 6, Select user-defined Character
  _bus->DataWrite(temp);                         //Set selection user-defined Character
}


/**
 * @brief Select Internal CGROM Font in Text Mode
 *        
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_UseInternalCGROM(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                //Reset bit 7, Select internal CGROM Character
  CLRB(temp,6);                                //Reset bit 6, Select internal CGROM Character
  _bus->DataWrite(temp);
}


/**
 * @brief Select External CGROM Font (Genitop serial flash) in Text Mode
 *
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_UseExternalCGROM(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                //Reset bit 7, Select external CGROM Character. (Genitop serial flash)
  SETB(temp,6);                                //Set bit 6, Select external CGROM Character. (Genitop serial flash)
  _bus->DataWrite(temp);
}


/**
 * @brief Character source selection
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [7-6] Character source selection
 *                            User-defined Font /CGROM Font Selection Bit in Text Mode
 *                            0b00: Select internal CGROM Character.
 *                            0b01: Select external CGROM Character. (Genitop serial flash)
 *                            0b10: Select user-defined Character.
 *                            0b11: NA
 * @endverbatim
 *
 * @param eFontSource::Internal : Select internal CGROM Character.
 *        eFontSource::External : Select external CGROM Character. (Genitop serial flash)
 *        eFontSource::User     : Select user-defined Character.
 *
 * @note Exemplo: Font_SetSource(eFontSource::ExternalCGROM); 
 */
void RA8889::Font_SetSource(eFontSource source)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Reset bits 7 e 6 de uma vez
  temp |= static_cast<uint8_t>(source);        //Converte enum para uint8_t
  _bus->DataWrite(temp);                         //Set Character source selection
}


/** OK
 * @brief Select Fonte Height 8x16 / 16x16
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void RA8889::Font_SetHeight_16(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,5);                                //Reset bit 5
  CLRB(temp,4);                                //Reset bit 4 
  _bus->DataWrite(temp);
}


/** OK
 * @brief Select Fonte Height 12x24 / 24x24 (Default)
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash (Intrnal 
 *                     CGROM) is decided by chosen character sets and need to 
 *                     configure GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void RA8889::Font_SetHeight_24(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,5);                                //Reset bit 5
  SETB(temp,4);                                //Set bit 4
  _bus->DataWrite(temp);
}


/** OK
 * @brief Select Fonte Height 16x32 / 32x32
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void RA8889::Font_SetHeight_32(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  SETB(temp,5);                               //Set bit 5
  CLRB(temp,4);                               //Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Select Fonte Height (8x16 / 16x16 / 12x24 / 24x24 / 16x32 / 32x32)
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param enum FontHeight height: Altura desejada da fonte
 *             FontHeight::H16  :  8x16 / 16x16
 *             FontHeight::H24  : 12x24 / 24x24
 *             FontHeight::H32  : 16x32 / 32x32
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */ 
void RA8889::Font_SetHeight(eFontHeight height)
{
  uint8_t temp;
  uint8_t bits = 0;
  
  // Mapear enum para bits [5:4] do CCR0
  switch(height)
  {
      case eFontHeight::H16: bits = 0x00;  break;
      case eFontHeight::H24: SETB(bits,0); break;
      case eFontHeight::H32: SETB(bits,1); break;
      default: 
          return; // nunca deve acontecer
  }
  
  _bus->CmdWrite(REG_CCR0);       //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();        //Lê valor atual
  temp &= ~(cSetb5 | cSetb4);   //Limpa bits 5:4
  temp |= bits;                 //Seta bits corretos
  _bus->DataWrite(temp);          //Escreve de volta
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-1
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [1-0] Character Selection for internal CGROM
 *                            When FNCR0 [7-6] 0b00, Internal CGROM 
 *                            supports character sets with the standard coding 
 *                            of ISO/IEC 8859-1,2,4,5, which supports English 
 *                            and most of European country languages
 *                            0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                            0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                            0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                            0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Select_Internal_CGROM_ISOIEC8859_1(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,1);                                //Reset bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-2
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Select_Internal_CGROM_ISOIEC8859_2(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  CLRB(temp,1);                                //Reset bit 1
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-4
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Select_Internal_CGROM_ISOIEC8859_4(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  SETB(temp,1);                                //Set bit 1
  CLRB(temp,0);                                //Reset bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-5
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Select_Internal_CGROM_ISOIEC8859_5(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  SETB(temp,1);                                //Set bit 1
  SETB(temp,0);                                //Set bit 0
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 * bit [1-0] Character Selection for internal CGROM
 *     When FNCR0 [7-6] 0b00, Internal CGROM 
 *     supports character sets with the standard coding 
 *     of ISO/IEC 8859-1,2,4,5, which supports English 
 *     and most of European country languages
 *     0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *     0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *     0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *     0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param iso ISO/IEC 8859 code to select (eInternalCharSet)
 *
 * @note None
 */
void RA8889::Select_Internal_CGROM_ISO8859(eInternalCharSet iso)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      // Seleciona o registrador CCR0
  temp = _bus->DataRead();                       // Lê valor atual
  temp &= ~(cSetb1 | cSetb0);                  // Limpa os bits B1:B0
  temp |= static_cast<uint8_t>(iso);           // Seta bits conforme enum
  _bus->DataWrite(temp);                         // Escreve no registrador
}


/**
 * @brief Set Text Parameter from CCR0 Register
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 * bit [7-6] Character source selection
 *     0b00: Select internal CGROM Character.
 *     0b01: Select external CGROM Character. (Genitop serial flash)
 *     0b10: Select user-defined Character.
 *     0b11: NA
 * bit [5-4] Character Height Setting for external CGROM (12x24) & user-defined Character(8x16,12x24,16x32)
 *     0b00 : 16; ex. 8x16 / 16x16 / variable character width x 16
 *     0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *     0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32 Note:
 *     1. User-defined character width is decided by character code; width for code < 8000h is 8/12/16 and width for code >=8000h is 16/24/32.
 *     2. The character width of Genitop’s serial flash is decided by chosen character sets and need to configure GT Font ROM registers (CEh, CFh).
 *     3. Internal CGROM supports size 12x24.
 * bit [1-0] Character Selection for internal CGROM
 *     When FNCR0 [7-6] 0b00, Internal CGROM 
 *     supports character sets with the standard coding 
 *     of ISO/IEC 8859-1,2,4,5, which supports English 
 *     and most of European country languages
 *     0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *     0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *     0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *     0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param sourceselect
 *        0: internal CGROM
 *        1: external CGROM
 *        2: user-define (user make a font_table
 * @param sizeselect
 *        0: 8*16/16*16
 *        1: 12*24/24*24
 *        2: 16*32/32*32
 * @param isoselect: ISO/IEC 8859 code to select (eInternalCharSet)
 *        0: iso8859-1
 *        1: iso8859-2
 *        2: iso8859-4
 *        3 : iso8859-5
 *
 * @note None
 */
void RA8889::SetTextParameter0(uint8_t sourceselect, uint8_t sizeselect, uint8_t isoselect)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = _bus->DataRead();                       
  temp |= (sourceselect << 6) & 0xC0;
  temp |= (sizeselect << 4) & 0x30;
  temp |= isoselect & 0x03;
  _bus->DataWrite(temp);                         
}


//================================================================================
//
// [0xCD] Character Control Register 1 (CCR1)
//
//================================================================================


/**
 * @brief Characer Full Alignment Set
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [7] Full Alignment Selection Bit
 *                    0b0 : Full alignment disable.
 *                    0b1 : Full alignment enable.
 *                    
 *                    When Full alignment is enabled, the character 
 *                    width is equal to half of the character height, 
 *                    width = height / 2, (the condition is character 
 *                    width is equal to or small than half of the 
 *                    character height), otherwise the character width 
 *                    is equal to character height.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_FullAlignmentEnable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                //Set full alignment
  _bus->DataWrite(temp);                         
}


/**
 * @brief Characer Full Alignment Set
 *        
 * @verbatim  
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [7] Full Alignment Selection Bit
 *                    0b0 : Full alignment disable.
 *                    0b1 : Full alignment enable.
 *                   
 *                    When Full alignment is enabled, the character 
 *                    width is equal to half of the character height, 
 *                    width = height / 2, (the condition is character 
 *                    width is equal to or small than half of the 
 *                    character height), otherwise the character width 
 *                    is equal to character height.
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_FullAlignmentDisable(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                //Disable full alignment
  _bus->DataWrite(temp);
}


/**
 * @brief Font Background Transparency
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [6] Chroma keying enable on Text input
 *                    0b0 : Character’s background displayed with specified color.
 *                    0b1 : Character’s background displayed with original canvas' background (transparency).
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_UseBackgroundTransparency(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  SETB(temp,6);                                //Set bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Font Background Color
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [6] Chroma keying enable on Text input
 *                    0b0 : Character’s background displayed with specified color.
 *                    0b1 : Character’s background displayed with original canvas’ background (transparency).
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_UseBackgroundColor(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  CLRB(temp,6);                                //Reset bit 6, Set original canva's color 
  _bus->DataWrite(temp);
}


/**
 * @brief Font Rotete 0 degree
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [4] Character Rotation
 *                    0b0 : Normal
 *                          Text direction from left to right then from top to bottom
 *                    0b1 : Counterclockwise 90 degree & horizontal flip
 *                          Text direction from top to bottom then from left to right
 *                          (it should accommodate with set VDIR as 1)
 *                    This attribute can be changed only when previous font write
 *                    finished (core_busy = 0)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_0degree(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  CLRB(temp,4);                                //Reset bit 4
  _bus->DataWrite(temp);
}


/**
 * @brief Font Rotete 90 degree
 *
 * @verbatim 
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [4] Character Rotation
 *                    0b0 : Normal
 *                          Text direction from left to right then from top to bottom
 *                    0b1 : Counterclockwise 90 degree & horizontal flip
 *                          Text direction from top to bottom then from left to right
 *                          (it should accommodate with set VDIR as 1)
 *                    This attribute can be changed only when previous font write
 *                    finished (core_busy = 0)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_90degree(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  SETB(temp,4);                                //Set bit 4 
  _bus->DataWrite(temp);
}


/**
 * @brief Character width enlargement factor
 *        
 * @verbatim  
 *        REG [CDh] Character Control Register 1 (CCR1)
 *                  bit [3-2] Character width enlargement factor
 *                          0b00 : factor x1
 *                          0b01 : factor x2
 *                          0b10 : factor x3
 *                          0b11 : factor x4
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_WidthEnlargFactor(eFontEnlargFactor factor)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();
  temp &= ~(cSetb3 | cSetb2);                  //Reset bits 3 e 2 de uma vez
  uint8_t f = static_cast<uint8_t>(factor);    //Converte para byte
  temp |= (f << 2);                            //Posiciona para o bit 3-2
  _bus->DataWrite(temp);
}


/**
 * @brief Character height enlargement factor
 *        
 * @verbatim  
 *        REG [CDh] Character Control Register 1 (CCR1)
 *                  bit [1-0] Character height enlargement factor
 *                          0b00 : factor x1
 *                          0b01 : factor x2
 *                          0b10 : factor x3
 *                          0b11 : factor x4
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void RA8889::Font_HeightEnlargFactor(eFontEnlargFactor factor)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();      
  temp &= ~(cSetb1 | cSetb0);                  //Reset bits 1 e 0 de uma vez
  temp |= static_cast<uint8_t>(factor);
  _bus->DataWrite(temp);
}


/**
 * @brief Set Text Parameter from CCR1 Register
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 * bit [7] Full Alignment Selection Bit
 *     0: Full alignment is disabled.
 *     1: Full alignment is enabled.
 *     When Full alignment is enabled, the character width is equal to half of the character height (the condition is character width is equal to or small than half of the character height), otherwise the character width is equal to character height.
 * bit [6] Chroma keying enable on Text input
 *     0: Character’s background displayed with specified color.
 *     1: Character’s background displayed with original canvas’ background.
 * bit [3-2] Character width enlargement factor
 *     00b: X1
 *     01b: X2
 *     10b: X3
 *     11b: X4
 * bit [1-0] Character height enlargement factor
 *     00b: X1
 *     01b: X2
 *     10b: X3
 *     11b: X4
 * @endverbatim
 *
 * @param align
 *        0: full alignment disable
 *        1: full alignment enable
 * @param chromakey
 *        0: text with chroma key disable
 *        1: text with chroma key enable
 * @param widthenlarge
 *        0: X1
 *        1: X2
 *        2: X3
 *        3: X4
 * @param heightenlarge
 *        0: X1
 *        1: X2
 *        2: X3
 *        3: X4
 *
 * @note None
 */
void RA8889::SetTextParameter1(uint8_t align, uint8_t chromakey, uint8_t widthenlarge, uint8_t heightenlarge)
{
  uint8_t temp;
  _bus->CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = _bus->DataRead();                       
  temp |= (align << 7) & 0x80;
  temp |= (chromakey << 6) & 0x40;
  temp |= (widthenlarge << 2) & 0x0c;
  temp |= heightenlarge & 0x03;
  _bus->DataWrite(temp);                         
}


//================================================================================
//
// PAGE 1
// [0xCA] IDEC (JPG/BMP) Destination memory start addr 0 (IDEC_DADR0)
// [0xCB] IDEC (JPG/BMP) Destination memory start addr 1 (IDEC_DADR1)
// [0xCC] IDEC (JPG/BMP) Destination memory start addr 2 (IDEC_DADR2)
// [0xCD] IDEC (JPG/BMP) Destination memory start addr 3 (IDEC_DADR3)
//
//================================================================================


/**
 * @brief Set IDEC SDRAM destination start address for JPG/BMP
 *
 * Configures the starting address in the IDEC SDRAM where the JPG or BMP
 * data will be written. The address is 32-bit and is split across four
 * consecutive registers.
 * 
 * @verbatim
 * PAGE 1 REG [0xca] IDEC (JPG/BMP) Destination memory start addr 0 (IDEC_DADR0)
 * bit [7-0] IDEC SDRAM Destination start address [7:0]
 *           Note: just only for JPG/BMP
 *
 * PAGE 1 REG [0xcb] IDEC (JPG/BMP) Destination memory start addr 1 (IDEC_DADR1)
 * bit [7-0] IDEC SDRAM Destination start address [15:8]
 *           Note: just only for JPG/BMP
 *
 * PAGE 1 REG [0xcc] IDEC (JPG/BMP) Destination memory start addr 2 (IDEC_DADR2)
 * bit [7-0] IDEC SDRAM Destination start address [23:16]
 *           Note: just only for JPG/BMP
 *
 * PAGE 1 REG [0xcd] IDEC (JPG/BMP) Destination memory start addr 3 (IDEC_DADR3)
 * bit [7-0] IDEC SDRAM Destination start address [31:24]
 *           Note: just only for JPG/BMP
 * @endverbatim
 *
 * @param addr 32-bit start address in SDRAM for JPG/BMP data 
 *
 * @note The function automatically handles page switching and writes
 *       each byte to the corresponding register.
 */
void RA8889::IDEC_Destination_StartAddress(uint32_t addr) 
{
  PageSwitch(ePageReg::Page1);
  // Array com os registradores consecutivos
  const uint8_t regs[4] = {REG_IDEC_DADR0, REG_IDEC_DADR1, REG_IDEC_DADR2, REG_IDEC_DADR3};
    for (int i = 0; i < 4; i++) {
        _bus->CmdWrite(regs[i]);                            //envia cada 8 bits para os registradores de endereço
        _bus->DataWrite((uint8_t)(addr >> (8 * i)) & 0xFF); // envia cada byte do endereço
    }
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// [0xCE] GT Character ROM Select (GTFNT_SEL)
//
//================================================================================


/**
 * @brief Font Selection for External CGROM Genitop's GT21L16T1W IC
 *        
 * @verbatim  
 * REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *           bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                     0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                     0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                     0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                     0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                     0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                     0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                     0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim  
 *                            
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT21L16T1W(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                     //Reset bit 7 
  CLRB(temp,6);                                     //Reset bit 6
  CLRB(temp,5);                                     //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L16U2W IC
 *
 * @verbatim       
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT30L16U2W(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                     //Reset bit 7
  CLRB(temp,6);                                     //Reset bit 6
  SETB(temp,5);                                     //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L24T3Y IC
 *        
 * @verbatim  
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT30L24T3Y(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                     //Reset bit 7
  SETB(temp,6);                                     //Set bit 6
  CLRB(temp,5);                                     //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L24M1Z IC
 *        
 * @verbatim
 * REG [0xce] GT Character ROM Select (GTFNT_SEL)
 * bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *     0b000 : Circuito Integrado External CGROM GT21L16T1W
 *     0b001 : Circuito Integrado External CGROM GT30L16U2W
 *     0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *     0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *     0b100 : Circuito Integrado External CGROM GT30L32S4W
 *     0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *     0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT30L24M1Z(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  CLRB(temp,7);                                     //Reset bit 7
  SETB(temp,6);                                     //Set bit 6
  SETB(temp,5);                                     //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L32S4W IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT30L32S4W(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                     //Set bit 7
  CLRB(temp,6);                                     //Reset bit 6
  CLRB(temp,5);                                     //Reset bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT20L24F6Y IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT20L24F6Y(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                     //Set bit 7
  CLRB(temp,6);                                     //Reset bit 6
  SETB(temp,5);                                     //Set bit 5
  _bus->DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT21L24S1W IC
 * 
 * @verbatim 
 * REG [CEh] GT Character ROM Select (GTFNT_SEL)
 * bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *     0b000 : Circuito Integrado External CGROM GT21L16T1W
 *     0b001 : Circuito Integrado External CGROM GT30L16U2W
 *     0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *     0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *     0b100 : Circuito Integrado External CGROM GT30L32S4W
 *     0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *     0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::GTFont_Select_GT21L24S1W(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = _bus->DataRead();                       
  SETB(temp,7);                                     //Set bit 7
  SETB(temp,6);                                     //Set bit 6
  CLRB(temp,5);                                     //Reset bit 5
  _bus->DataWrite(temp);
}


/** OK
 * @brief Font Genitop Character ROM Parameters
 * 
 * @verbatim 
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 * bit [7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *     0: Serial Flash/ROM 0 I/F is selected.
 *     1: Serial Flash/ROM 1 I/F is selected.
 *     Note: when page1 B7h bit 7 = 1 , then serial flash chip select 2,3 * bit [6] Serial Flash / ROM Access Mode
 * bit [6] Serial Flash / ROM Access Mode
 *     0b0: Font mode – for external CGROM
 *     0b1: DMA mode – for CGRAM, pattern, boot start image or OSD * 
 * bit [5] Serial Flash / ROM Access Mode
 *     0b0: 24 bits address mode
 *     0b1: 32 bits address mode
 * 
 *     If user wants to use 32 bits address mode, user 
 *     must manual send EX4B command (B7h) to serial 
 *     flash then set this bit to 1.
 * bit [3-0] Read Command code & behavior selection
 *     0b000x: 1x read command code – 03h. Normal read 
 *     speed. Single data input on xmiso. Without 
 *     dummy cycle between address and data.
 *     
 *     0b010x: 1x read command code – 0Bh. To some 
 *     serial flash provide faster read speed. Single 
 *     data input on xmiso. 8 dummy cycles inserted 
 *     between address and data.
 *     
 *     0b1x0x: 1x read command code – 1Bh. To some 
 *     serial flash provide fastest read speed. Single 
 *     data input on xmiso. 16 dummy cycles inserted 
 *     between address and data.
 *
 *     0bxx10: 2x read command code – 3Bh. Interleaved 
 *     data input on xmiso & xmosi. 8 dummy cycles 
 *     inserted between address and data phase. (dual 
 *     mode 0, reference Figure 16-7).
 *
 * REG [0xce] GT Character ROM Select (GTFNT_SEL)
 * bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *     0b000 : Integrate Circuit External CGROM GT21L16T1W
 *     0b001 : Integrate Circuit External CGROM GT30L16U2W
 *     0b010 : Integrate Circuit External CGROM GT30L24T3Y
 *     0b011 : Integrate Circuit External CGROM GT30L24M1Z
 *     0b100 : Integrate Circuit External CGROM GT30L32S4W
 *     0b101 : Integrate Circuit External CGROM GT20L24F6Y
 *     0b110 : Integrate Circuit External CGROM GT21L24S1W
 *
 * REG [0xbb] SPI Clock period (SPI_DIVSOR)
 * bit [7-0] SPI Clock period (default Fsck=3)
 * According to system clock to set low & high period for SPI clock. 
 * SPI Master:
 *   Fsck = Fcore / (divisor * 2)
 * Serial Flash:
 *   Fsck = Fcore / (divisor * 2)
 * When SPI_DIVSOR = 0,
 *   Fsck = Fcore
 * 
 * REG [CFh] GT Character ROM Control register (GTFNT_CR)
 * bit [7-3] Character sets
 *          FONT ROM Coding Setting
 *           For specific GT serial Font ROM, the coding method must be set for decoding.
 *           
 *           b. Two byte character code for following character sets:
 *              0b00000: GB2312
 *              0b00001: GB12345/GB18030
 *              0b00010: BIG5
 *              0b00011: UNICODE
 *              0b00100: ASCII
 *              0b00101: UNI-Japanese
 *              0b00110: JIS0208
 *              0b00111: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
 *
 *           a. Single byte character code for following character sets:
 *              0b01000: Korea
 *              0b10001: ISO-8859-1
 *              0b10010: ISO-8859-2
 *              0b10011: ISO-8859-3
 *              0b10100: ISO-8859-4
 *              0b10101: ISO-8859-5
 *              0b10110: ISO-8859-6
 *              0b10111: ISO-8859-7
 *              0b11000: ISO-8859-8
 *              0b11001: ISO-8859-9
 *              0b11010: ISO-8859-10
 *              0b11011: ISO-8859-11
 *              0b11100: ISO-8859-12
 *              0b11101: ISO-8859-13
 *              0b11110: ISO-8859-14
 *              0b11111: ISO-8859-15
 *
 * [bit 1-0] GT Character width setting
 *           00b: for fix width’s font sets. Its width is half of character height.
 *            Ex. ISO-8859, GB2312, GB12345/GB18030, BIG5,
 *            UNI-Japanese, JIS0208, Thai.
 *            Others: variable width for following character sets: ASCII, Latin,
 *            Greek, Cyrillic & Arabic.
 *            
 *            ASCII / Latin/Greek/ Cyrillic / Arabic
 *               
 *                     (ASCII)   (Latin/Greek/Cyrillic)      (Arabic)
 *            00b       Normal            Normal                NA
 *            01b       Arial         Variable Width     Presentation Forms-A
 *            10b       Roman               NA           Presentation Forms-B
 *            11b       Bold                NA                  NA
 * @endverbatim
 *
 * @param scs_select : Serial chip Select
 *        0: Serial Flash/ROM 0 I/F is selected
 *        1: Serial Flash/ROM 1 I/F is selected
 *
 * @param clk_div: Divisor de clock
 *
 * @param rom_select: Modelo Genitop Font Serial Flash ROM
 *        0: GT21L16T1W
 *        1: GT30L16U2W
 *        2: GT30L24T3Y
 *        3: GT30L24M1Z
 *        4: GT30L32S4W
 *        5: GT20L24F6Y
 *        6: GT21L24S1W
 *        
 * @param character_select
 *        
 * @param gt_width
 *
 * @note A escolha de rom_select vai depender do modelo da ROM soldado na placa de circuito do dsiplay.
 *       Pode apresnetar na placa dois chip's ROM que podems er escolhidos com scs_select.
 *       Use esta funcao com 
 */
void RA8889::GTFont_CharacterParameter(uint8_t scs_select, uint8_t clk_div, uint8_t rom_select, uint8_t character_select, uint8_t gt_width)
{ 
  uint8_t temp;
  
  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  scs_select &= 0x01;
  if(scs_select==0) _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT0|BIT_SERIAL_FLASH_FONT_MODE|BIT_SERIAL_FLASH_ADDR_24BIT|BIT_FOLLOW_RA8875_MODE|BIT_SPI_FAST_READ_8DUMMY);
  if(scs_select==1) _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT1|BIT_SERIAL_FLASH_FONT_MODE|BIT_SERIAL_FLASH_ADDR_24BIT|BIT_FOLLOW_RA8875_MODE|BIT_SPI_FAST_READ_8DUMMY);
  
  //0xbb, SPI Clock period (SPI_DIVSOR)
  _bus->RegisterWrite(REG_SPI_DIVSOR, clk_div);
  
  //0xce, Character Control Register 0 (GTFNT_SEL)
  _bus->RegisterWrite(REG_GTFNT_SEL, rom_select);
  
  //0xcf, GT Character ROM Control register (GTFNT_CR)
  _bus->RegisterWrite(REG_GTFNT_CR, (character_select & 0xf8) | (gt_width & 0x03));

}


//================================================================================
//
// [0xCF] GT Character ROM Control register (GTFNT_CR)
//
//================================================================================


/**
 * @brief Genitop's Character Set and Decoder
 *
 * @verbatim  
 * REG [CFh] GT Character ROM Control register (GTFNT_CR)
 * bit [7-3] Character sets
 *          FONT ROM Coding Setting
 *           For specific GT serial Font ROM, the coding method must be set for decoding.
 *           
 *           b. Two byte character code for following character sets:
 *              0b00000: GB2312
 *              0b00001: GB12345/GB18030
 *              0b00010: BIG5
 *              0b00011: UNICODE
 *              0b00100: ASCII
 *              0b00101: UNI-Japanese
 *              0b00110: JIS0208
 *              0b00111: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
 *
 *           a. Single byte character code for following character sets:
 *              0b01000: Korea
 *              0b10001: ISO-8859-1
 *              0b10010: ISO-8859-2
 *              0b10011: ISO-8859-3
 *              0b10100: ISO-8859-4
 *              0b10101: ISO-8859-5
 *              0b10110: ISO-8859-6
 *              0b10111: ISO-8859-7
 *              0b11000: ISO-8859-8
 *              0b11001: ISO-8859-9
 *              0b11010: ISO-8859-10
 *              0b11011: ISO-8859-11
 *              0b11100: ISO-8859-12
 *              0b11101: ISO-8859-13
 *              0b11110: ISO-8859-14
 *              0b11111: ISO-8859-15
 *
 * [bit 1-0] GT Character width setting
 *           00b: for fix width’s font sets. Its width is half of character height.
 *            Ex. ISO-8859, GB2312, GB12345/GB18030, BIG5,
 *            UNI-Japanese, JIS0208, Thai.
 *            Others: variable width for following character sets: ASCII, Latin,
 *            Greek, Cyrillic & Arabic.
 *            
 *            ASCII / Latin/Greek/ Cyrillic / Arabic
 *               
 *                     (ASCII)   (Latin/Greek/Cyrillic)      (Arabic)
 *            00b       Normal            Normal                NA
 *            01b       Arial         Variable Width     Presentation Forms-A
 *            10b       Roman               NA           Presentation Forms-B
 *            11b       Bold                NA                  NA
 * @endverbatim
 * @param temp
 *
 * @note None
 */
void RA8889::GTFont_SetDecoder(uint8_t temp)
{
  _bus->CmdWrite(REG_GTFNT_CR);                  //0xcf, GT Character ROM Control register (GTFNT_CR)
  _bus->DataWrite(temp);
}


//================================================================================
//
// PAGE 1
// [0xCE] IDEC (JPG/BMP) Destination Image Width 0 (IDEC_DWTH0)
// [0xCF] IDEC (JPG/BMP) Destination Image Width 1 (IDEC_DWTH1)
//
//================================================================================



/**
 * @brief Set IDEC SDRAM destination image width for JPG/BMP
 *
 * @verbatim  
 * PAGE 1 REG [0xce] IDEC (JPG/BMP) Destination Image Width 0 (IDEC_DWTH0)
 * bit [7-0] IDEC Destination Image Width [7:0]
 *           Note: just only for JPG/BMP
 *
 * PAGE 1 REG [0xcf] IDEC (JPG/BMP) Destination Image Width 1 (IDEC_DWTH1)
 * bit [7-0] IDEC Destination Image Width [12:8]
 *           Note: just only for JPG/BMP
 * @endverbatim
 *
 * @param Wx Destination image width in pixels (13-bit value)
 *
 * @note The function handles page switching and writes each byte
 *       to the corresponding register automatically.
 */
void RA8889::IDEC_Destination_ImageWidth(uint16_t Wx)
{
  PageSwitch(ePageReg::Page1);
  _bus->CmdWrite(REG_IDEC_DWTH0);                //0xce, page 1, IDEC (JPG/BMP) Destination Image Width 0 (IDEC_DWTH0)
  _bus->DataWrite((uint8_t)(Wx & 0xFF));
  _bus->CmdWrite(REG_IDEC_DWTH1);                //0xcf, page 1, IDEC (JPG/BMP) Destination Image Width 1 (IDEC_DWTH1)
  _bus->DataWrite((uint8_t)(Wx >> 8));
  PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 0
// [0xD0] Character Line gap Setting Register (FLDR)
//
//================================================================================


/**
 * @brief Character Line gap Setting
 *        
 *        REG [D0h] Character Line gap Setting Register (FLDR)
 *                  bit [4-0] Character Line gap Setting
 *                            Setting the character line distance when setting 
 *                            memory font write cursor auto move in active 
 *                            window. (Unit: pixel)
 *                            Color of gap will fill-in background color.
 *                            *** It won’t be enlarged by character enlargement function.
 *                            
 * @param temp
 *
 * @note None
 */
void RA8889::Font_LineDistance(uint8_t gap)
{
  _bus->CmdWrite(REG_FLDR);                      //0xd0, Character Line gap Setting Register (FLDR)
  _bus->DataWrite(gap);
}


//================================================================================
//
// [0xD1] Character to Character Space Setting Register (F2FSSR)
//
//================================================================================


/**
 * @brief Character Line gap Setting
 *        
 *        REG [D1h] Character to Character Space Setting Register (F2FSSR)
 *                  bit [5-0] Character to Character Space Setting
 *                            00h : 0 pixel
 *                            01h : 1 pixel
 *                            02h : 2 pixels
 *                            ...
 *                            3Fh : 63 pixels
 *                  Color of space will fill-in background color.
 *                  *** It won’t be enlarged by character enlargement function.
 *                            
 * @param pixels: pixels spaces bethween character
 *
 * @note None
 */
void RA8889::Font_toFontWidthSetting(uint8_t pixels)
{
  _bus->CmdWrite(REG_F2FSSR);          //0xd1,  Character to Character Space Setting Register (F2FSSR)
  _bus->DataWrite(pixels);
}


//================================================================================
//
// PAGE 0
// [0xD2] Foreground Color Register - Red (FGCR)
// [0xD3] Foreground Color Register - Green (FGCG)
// [0xD4] Foreground Color Register - Blue (FGCB)
//
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 * @verbatim
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note use o determinado numero de bits para compor o n umero de cores 256, 
 *       65K e 16.7M cores.
 */
void RA8889::ForegroundColorRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  _bus->CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  _bus->DataWrite(red);                          //Escreve o formato da cor vermelha 
  _bus->CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  _bus->DataWrite(green);                        //Escreve o formato da cor verde
  _bus->CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  _bus->DataWrite(blue);                         //Escreve o formato da cor azul
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB3:3:2) de 256 cores
 *        
 * @verbatim
 * Color depht 8bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R3G3B2 (3 bits para o vermelho, 
 *        3 bits para o verde e 2 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 *       
 */
void RA8889::ForegroundColor8bpp(uint8_t color)
{
  _bus->CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  _bus->DataWrite(color);                        //Vermelho so usa o bit de [7~5], o resto ignorado
  _bus->CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  _bus->DataWrite(color << 3);                   //Deslocar a posicao do verde para o bit [7~5], o resto ignorado
  _bus->CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  _bus->DataWrite(color << 6);                   //Deslocar a posicao do azul para o bit [7~6], o resto ignorado
}


/** OK
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB5:6:5) de 65k cores
 *
 * @verbatim        
 * Color depht 16bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R5G6B5 (5 bits para o vermelho, 
 *        6 bits para o verde e 5 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void RA8889::ForegroundColor16bpp(uint16_t color)
{
  _bus->CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  _bus->DataWrite(color >> 8);                   //Desloca os 5 bits do vermelho so usa o bit de [7~3], a sujeira ignorado
  _bus->CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  _bus->DataWrite(color >> 3);                   //Deslocar os 6 bits do verde para o bit [7~2], a sujeira ignorado
  _bus->CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  _bus->DataWrite(color << 3);                   //Deslocar os 5 bits do azul para o bit [7~3], a sujeira ignorado
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB8:8:8) de 16.7M cores
 *        
 * @verbatim
 * Color depht de 24bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R8G8B8 (8 bits para o vermelho, 
 *        8 bits para o verde e 8 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 *       
 */ 
void RA8889::ForegroundColor24bpp(uint32_t color)
{
  _bus->CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  _bus->DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  _bus->CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  _bus->DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  _bus->CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  _bus->DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
//
// PAGE 1
// [0xD3] AVI pause
//
//================================================================================


/**
 * @brief Pausar o video AVI
 *        
 * @verbatim
 * PAGE 1 REG [0xd3] AVI pause
 *            bit [0] Pause, the video will be paused when the bit is set
 *                    Write : 
 *                    1: enter pause / exit pause
 *                    Permuta cada vez que escreve 1 entra em pausa, escrevendo nomanete sai da pausa.
 *                    para saber em que modo se encontra leia o registrador antes.
 *                    Read: 
 *                    1 – AVI pause
 *                    0 – AVI display
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::AVI_Pause(void)
{
    PageSwitch(ePageReg::Page1);
    _bus->CmdWrite(REG_AVI_PAUSE);               //0xd3, page 1, AVI pause
    _bus->DataWrite(0x01);                       //Pause Video
    PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// PAGE 1
// [0xD4] AVI stop
//
//================================================================================

/**
 * @brief Parar  e encerrar o video AVI
 *        
 * @verbatim
 * PAGE 1 REG [0xd4] AVI stop
 *            bit [0] Stop, the video will be stopped and exited when the bit is set
 *            1: stop enable
 *            0: no operation
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::AVI_Stop(void)
{
    PageSwitch(ePageReg::Page1);
    _bus->CmdWrite(REG_AVI_STOP);                //0xd4, page 1, AVI stop
    _bus->DataWrite(0x01);                       //Stop Video
    PageSwitch(ePageReg::Page0);
}


//================================================================================
//
// [0xD5] Background Color Register - Red (BGCR)
// [0xD6] Background Color Register - Green (BGCG)
// [0xD7] Background Color Register - Blue (BGCB)
//
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 * @verbatim
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void RA8889::BackgroundColorRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  _bus->CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  _bus->DataWrite(red);                          //Escreve o formato da cor vermelha 
  _bus->CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  _bus->DataWrite(green);                        //Escreve o formato da cor verde
  _bus->CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  _bus->DataWrite(blue);                         //Escreve o formato da cor azul
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB3:3:2) de 256 cores
 *        
 * @verbatim
 * Color depht 8bpp
 * 
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R3G3B2 (3 bits para o vermelho, 
 *        3 bits para o verde e 2 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void RA8889::BackgroundColor8bpp(uint8_t color)
{
  _bus->CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  _bus->DataWrite(color);                        //Vermelho so usa o bit de [7~5], o resto ignorado
  _bus->CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  _bus->DataWrite(color << 3);                   //Deslocar a posicao do verde para o bit [7~5], o resto ignorado
  _bus->CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  _bus->DataWrite(color << 6);                   //Deslocar a posicao do azul para o bit [7~6], o resto ignorado
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB5:6:5) de 65k cores
 *        
 * @verbatim 
 * Color depht de 16bpp
 *
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R5G6B5 (5 bits para o vermelho, 
 *        6 bits para o verde e 5 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void RA8889::BackgroundColor16bpp(uint16_t color)
{
  _bus->CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  _bus->DataWrite(color >> 8);                   //Desloca os 5 bits do vermelho so usa o bit de [7~3], a sujeira ignorado
  _bus->CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  _bus->DataWrite(color >> 3);                   //Deslocar os 6 bits do verde para o bit [7~2], a sujeira ignorado 
  _bus->CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  _bus->DataWrite(color << 3);                   //Deslocar os 5 bits do azul para o bit [7~3], a sujeira ignorado
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB8:8:8) de 16.7M cores
 *        
 * @verbatim
 * Color depht 24bpp
 *
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R8G8B8 (8 bits para o vermelho, 
 *        8 bits para o verde e 8 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */ 
void RA8889::BackgroundColor24bpp(uint32_t color)
{
  _bus->CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  _bus->DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  _bus->CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  _bus->DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  _bus->CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  _bus->DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
//
// [0xDB] CGRAM Start Address 0 (CGRAM_STR0)
// [0xDC] CGRAM Start Address 1 (CGRAM_STR1)
// [0xDD] CGRAM Start Address 2 (CGRAM_STR2)
// [0xDE] CGRAM Start Address 3 (CGRAM_STR3)
//
//================================================================================


/**
 * @brief CGRAM Start Address [31:0]
 *        
 * @verbatim
 * REG [0xdb] CGRAM Start Address 0 (CGRAM_STR0)
 *            bit [7~0] CGRAM START ADDRESS [7:0]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xdc] CGRAM Start Address 1 (CGRAM_STR1)
 *            bit [7~0] CGRAM START ADDRESS [15:8]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xdd] CGRAM Start Address 2 (CGRAM_STR2)
 *            bit [7~0] CGRAM START ADDRESS [23:16]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xde] CGRAM Start Address 3 (CGRAM_STR3)
 *            bit [7-0] CGRAM START ADDRESS [31:24]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color and 
 *     Text/graphic mode setting, please make sure core_busy (fontwr_busy) 
 *     status bit is low.
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */ 
void RA8889::CGRAM_StartAddress(uint32_t addr)
{
  _bus->CmdWrite(REG_CGRAM_STR0);                //0xdb, CGRAM Start Address 0 (CGRAM_STR0)
  _bus->DataWrite(addr);                         
  _bus->CmdWrite(REG_CGRAM_STR1);                //0xdc, CGRAM Start Address 1 (CGRAM_STR1)
  _bus->DataWrite(addr >> 8);                    
  _bus->CmdWrite(REG_CGRAM_STR2);                //0xdd,  CGRAM Start Address 2 (CGRAM_STR2)
  _bus->DataWrite(addr >> 16);                   
  _bus->CmdWrite(REG_CGRAM_STR3);                //0xde, CGRAM Start Address 3 (CGRAM_STR3)
  _bus->DataWrite(addr >> 24);                   
}


//================================================================================
//
// [0xDF] Power Management register (PMU)
//
//================================================================================


/**
 * @brief Entrar em modo normal de operação de energia
 *        
 * @verbatim
 * REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */
void RA8889::Power_NormalMode(void)
{
  _bus->CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  _bus->DataWrite(0x00);                         //Limpa bits [7,1,0]
  IC_WaitReady();                              //Aguarda até está pronto
}


/**
 * @brief Entrar em modo standby de operação de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Power_SavingStandbyMode(void)
{
  _bus->CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  _bus->DataWrite(cSetb7 | cSetb0);              //bit 7 = 1 (power saving), bit [1-0] = 01 (standby)
}


/**
 * @brief Entrar em modo suspensão de operação de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Power_SavingSuspendMode(void)
{
  _bus->CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  _bus->DataWrite(0x82);                         //bit 7 = 1 (power saving), bit [1-0] = 10 (suspend)
}


/**
 * @brief Entrar em modo de dormência de operação economia de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void RA8889::Power_SavingSleepMode(void)
{
  _bus->CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  _bus->DataWrite(0x03);                         //bit [1-0] = 11 (sleep)
  _bus->CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  _bus->DataWrite(0x83);                         //bit 7 = 1 (power saving), bit [1-0] = 11 (sleep)
}


//================================================================================
//
// [0xE5] IIC Master Clock Pre-scale Register 0 (IICMCPR0)
// [0xE6] IIC Master Clock Pre-scale Register 1 (IICMCPR1)
// [0xE7] IIC Master Transmit Register (IICMTXR)
// [0xE8] IIC Master Receiver Register (IICMRXR)
//
//================================================================================


/**
 * @brief I2C Master Clock Pre-Scale
 *        
 * @verbatim
 * REG [0xe5] IIC Master Clock Pre-scale Register 0 (IICMCPR0)
 *            bit [7-0] IIC Master Clock Pre-scale [7:0]
 *                      XSCL = CCLK / (5*(Pre-scale + 2))
 * 
 * REG [0xe6] IIC Master Clock Pre-scale Register 1 (IICMCPR1)
 *            bit [7-0] IIC Master Clock Pre-scale [15:8]
 *                      XSCL = CCLK / (5*(Pre-scale + 2))
 * @endverbatim
 *
 * @param prescale: valor do pre-scale
 *
 * @note Only RA8889
 *       bits [15:8][7:0] XSCL = CCLK / (5*(Pre-scale + 2))
 */
void RA8889::I2CM_ClockPrescale(uint16_t prescale)
{
  _bus->CmdWrite(REG_IICMCPR0);                  //0xe5, IIC Master Clock Pre-scale Register 0 (IICMCPR0)
  _bus->DataWrite(prescale);                     //byte baixo de pre-scale
  _bus->CmdWrite(REG_IICMCPR1);                  //0xe6, IIC Master Clock Pre-scale Register 1 (IICMCPR1)
  _bus->DataWrite(prescale >> 8);                //byte alto de pre-scale
}


/**
 * @brief I2C Master Transmit Data
 *        
 * @verbatim
 * REG [0xe7] IIC Master Transmit Register (IICMTXR)
 *            bit [7-0] IIC Master Transmit [7:0]
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8889
 *       
 */
void RA8889::I2CM_TransmitData(uint8_t data)
{
  _bus->CmdWrite(REG_IICMTXR);       //0xe7, IIC Master Transmit Register (IICMTXR)
  _bus->DataWrite(data);
}


/**
 * @brief I2C Master Receive Data
 *        
 * @verbatim
 * REG [0xe8] IIC Master Receiver Register (IICMRXR)
 *            bit [7-0] IIC Master Receiver [7:0]
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8889
 *       
 */
uint8_t RA8889::I2CM_Receiver_Data(void)
{
  _bus->CmdWrite(REG_IICMRXR);                   //0xe8, IIC Master Receiver Register (IICMRXR)
  return _bus->DataRead();
}


/**
 * @brief Configura a frequência do clock I²C Mestre do RA8889.
 *
 * @param xscl_hz Frequência desejada do I²C (em Hz), ex: 100000 (100 kHz), 400000 (400 kHz).
 * @param coreclk_mhz Frequencia clock do core (sistema) em MHz
 * 
 * @note A fórmula usada é:
 *       PreScale = (CCLK / (5 * XSCL)) - 2
 *       XSCL = CCLK / (5 * (PreScale + 2))
 *
 *       Onde CCLK é o clock principal do RA8889.
 */
void RA8889::I2CM_SetFrequency(uint32_t xscl_hz, uint16_t coreclk_mhz)
{
  // Defina aqui o clock principal do RA8889 (em Hz).
  // Verifique no seu hardware/datasheet qual é o valor exato.
  const uint32_t CCLK = coreclk_mhz * 1000000UL;  //Converte MHz para Hz
  
  if (xscl_hz == 0) return; // Evita divisão por zero
  
  // Calcula PreScale conforme a fórmula
  uint32_t prescale = (CCLK / (5 * xscl_hz)) - 2;
  
  // Garante que cabe em 16 bits
  if (prescale > 0xFFFF) prescale = 0xFFFF;
  
  // Chama a função que grava nos registradores do RA8889
  I2CM_ClockPrescale((uint16_t)prescale);
}


//================================================================================
//
// [0xE9] IIIC Master Command Register (IICMCMDR)
//
//================================================================================


/**
 * @brief Write I²C Master with start.
 *
 * @verbatim
 * REG [0xe9] IIC Master Command Register (IICMCMDR)
 *     [bit7] START
 *            Generate (repeated) start condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit6] STOP
 *            Generate stop condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit5] READ
 *            READ(READ and WRITE can’t be used simultaneously)
 *            Read form slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit4] WRITE
 *            WRITE(READ and WRITE can’t be used simultaneously)
 *            Write to slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit3] ACKNOWLEDGE
 *            When as a I2C master receiver
 *            0 : Sent ACK.
 *            1 : Sent NACK.
 *            Note : This bit is always read as 0.
 *     [bit0] Noise Filter
 *            0 : Enable.
 *            1 : Disable.
 * @endverbatim
 * @param None
 * 
 * @note None
 */
void RA8889::I2CM_WriteWithStart(void)
{
  _bus->CmdWrite(REG_IICMCMDR);         //0xe9, IIC Master Command Register (IICMCMDR)
  uint8_t temp = 0x90;                //Set bit 7 and 4
  #ifdef Disable_I2CM_Noise_Filter
  CLRB(temp,0);                       //Reset bit 0
  #endif
  #ifdef Enable_I2CM_Noise_Filter
  SETB(temp,0);                       //Set bit 0
  #endif
  _bus->DataWrite(temp);
}


/**
 * @brief I²C Master stop.
 *
 * @verbatim
 * REG [0xe9] IIC Master Command Register (IICMCMDR)
 *     [bit7] START
 *            Generate (repeated) start condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit6] STOP
 *            Generate stop condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit5] READ
 *            READ(READ and WRITE can’t be used simultaneously)
 *            Read form slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit4] WRITE
 *            WRITE(READ and WRITE can’t be used simultaneously)
 *            Write to slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit3] ACKNOWLEDGE
 *            When as a I2C master receiver
 *            0 : Sent ACK.
 *            1 : Sent NACK.
 *            Note : This bit is always read as 0.
 *     [bit0] Noise Filter
 *            0 : Enable.
 *            1 : Disable.
 * @endverbatim
 * @param None
 * 
 * @note None
 */
void RA8889::I2CM_Stop(void)
{
  _bus->CmdWrite(REG_IICMCMDR);         //0xe9, IIC Master Command Register (IICMCMDR)
  uint8_t temp = 0x40;                //Set bit 6
  #ifdef Disable_I2CM_Noise_Filter
  CLRB(temp,0);                       //Reset bit 0
  #endif
  #ifdef Enable_I2CM_Noise_Filter
  SETB(temp,0);                       //Set bit 0
  #endif
  _bus->DataWrite(temp);
}


/**
 * @brief Read I²C Master with Acknolegment
 *
 * @verbatim
 * REG [0xe9] IIC Master Command Register (IICMCMDR)
 *     [bit7] START
 *            Generate (repeated) start condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit6] STOP
 *            Generate stop condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit5] READ
 *            READ(READ and WRITE can’t be used simultaneously)
 *            Read form slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit4] WRITE
 *            WRITE(READ and WRITE can’t be used simultaneously)
 *            Write to slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit3] ACKNOWLEDGE
 *            When as a I2C master receiver
 *            0 : Sent ACK.
 *            1 : Sent NACK.
 *            Note : This bit is always read as 0.
 *     [bit0] Noise Filter
 *            0 : Enable.
 *            1 : Disable.
 * @endverbatim
 * @param None
 * 
 * @note None
 */
void RA8889::I2CM_ReadWithAck(void)
{
  _bus->CmdWrite(REG_IICMCMDR);         //0xe9, IIC Master Command Register (IICMCMDR)
  uint8_t temp = 0x20;                //Set bit 5, Reset bit 3
  #ifdef Disable_I2CM_Noise_Filter
  CLRB(temp,0);                       //Reset bit 0
  #endif
  #ifdef Enable_I2CM_Noise_Filter
  SETB(temp,0);                       //Set bit 0
  #endif
  _bus->DataWrite(temp);
}


/** NOTA: deve haver problema esta setando o bit 6 de STOP?????? (O autor acho que errou)
 * @brief Read I²C Master with No Acknolegment
 *
 * @verbatim
 * REG [0xe9] IIC Master Command Register (IICMCMDR)
 *     [bit7] START
 *            Generate (repeated) start condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit6] STOP
 *            Generate stop condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit5] READ
 *            READ(READ and WRITE can’t be used simultaneously)
 *            Read form slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit4] WRITE
 *            WRITE(READ and WRITE can’t be used simultaneously)
 *            Write to slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit3] ACKNOWLEDGE
 *            When as a I2C master receiver
 *            0 : Sent ACK.
 *            1 : Sent NACK.
 *            Note : This bit is always read as 0.
 *     [bit0] Noise Filter
 *            0 : Enable.
 *            1 : Disable.
 * @endverbatim
 * @param None
 * 
 * @note None
 */
void RA8889::I2CM_ReadWithNack(void)
{
  _bus->CmdWrite(REG_IICMCMDR);         //0xe9, IIC Master Command Register (IICMCMDR)
  uint8_t temp = 0x60;                //Set bits 6-5, Set bit 3
  #ifdef Disable_I2CM_Noise_Filter
  CLRB(temp,0);                       //Reset bit 0
  #endif
  #ifdef Enable_I2CM_Noise_Filter
  SETB(temp,0);                       //Set bit 0
  #endif
  _bus->DataWrite(temp);
}


/** NOTA: deve haver problema esta setando o bit 6 de STOP?????? (O autor acho que errou)
 * @brief Write I²C Master
 *
 * @verbatim
 * REG [0xe9] IIC Master Command Register (IICMCMDR)
 *     [bit7] START
 *            Generate (repeated) start condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit6] STOP
 *            Generate stop condition and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit5] READ
 *            READ(READ and WRITE can’t be used simultaneously)
 *            Read form slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit4] WRITE
 *            WRITE(READ and WRITE can’t be used simultaneously)
 *            Write to slave and be cleared by hardware automatically
 *            Note : This bit is always read as 0.
 *     [bit3] ACKNOWLEDGE
 *            When as a I2C master receiver
 *            0 : Sent ACK.
 *            1 : Sent NACK.
 *            Note : This bit is always read as 0.
 *     [bit0] Noise Filter
 *            0 : Enable.
 *            1 : Disable.
 * @endverbatim
 * @param None
 * 
 * @note None
 */
void RA8889::I2CM_Write(void)
{
  _bus->CmdWrite(REG_IICMCMDR);         //0xe9, IIC Master Command Register (IICMCMDR)
  uint8_t temp = 0x10;                //Set bits 4
  #ifdef Disable_I2CM_Noise_Filter
  CLRB(temp,0);                       //Reset bit 0
  #endif
  #ifdef Enable_I2CM_Noise_Filter
  SETB(temp,0);                       //Set bit 0
  #endif
  _bus->DataWrite(temp);
}


//================================================================================
//
// [0xEA] IIC Master Status Register (IICMSTUR)
//
//================================================================================


/** 
 * @brief I²C Master check received ACK from Slave
 *
 * @verbatim
 * REG [0xea] IIC Master Status Register (IICMSTUR)
 *     [bit7] Received acknowledge from slave
 *            0 : Acknowledge received.
 *            1 : No Acknowledge received.
 * @endverbatim
 *
 * @param None
 *
 * @return true : Received acknowledge from slave
 *         false: Don't received acknowledge from slave
 *
 * @note None
 */
bool RA8889::I2CM_CheckSlaveACK(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_IICMCMDR);         //0xea, IIC Master Status Register (IICMSTUR)
  temp = _bus->DataRead();
  return (temp & 0x80);               //check bit 7
}


/** 
 * @brief I²C Master Bus Busy
 *
 * @verbatim
 * REG [0xea] IIC Master Status Register (IICMSTUR)
 *     [bit6] IIC Bus is Busy
 *            0 : Idle. ‘0’ after STOP signal detected
 *            1 : Busy. ‘1’ after START signal detected
 * @endverbatim
 *
 * @param None
 *
 * @return true : busy
 *         false: Idle
 *
 * @note None
 */
bool RA8889::I2CM_BusBusy(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_IICMCMDR);         //0xea, IIC Master Status Register (IICMSTUR)
  temp = _bus->DataRead();
  return (temp & 0x40);               //Check bit 6
}


/** 
 * @brief I²C Master Chack Data Transfer in progress
 *
 * @verbatim
 * REG [0xea] IIC Master Status Register (IICMSTUR)
 *     [bit1] Transfer in progress
 *            0 : when transfer complete
 *            1 : when transferring data
 
 * @endverbatim
 *
 * @param None
 *
 * @return 1: in progress
 *         0: completed
 *
 * @note None
 */
uint8_t RA8889::I2CM_TransmitProgress(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_IICMCMDR);                  //0xea, IIC Master Status Register (IICMSTUR)
  temp = _bus->DataRead();
  return (temp & 0x02) ? 1 : 0;
}


/** 
 * @brief I²C Master Arbitration Lost
 *
 * @verbatim
 * REG [0xea] IIC Master Status Register (IICMSTUR)
 *     [bit0] Arbitration lost
 *            This bit is set 1 when the core lost arbitration. Arbitration is lost when:
 *            a STOP signal is detected but is not requested. At this time,
 *            the master will drive SDA to high, but other master will drive SDA to low.
 * @endverbatim
 *
 * @param None
 *
 * @return 1: in progress
 *         0: completed
 *
 * @note None
 */
uint8_t RA8889::I2CM_Arbitration(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_IICMCMDR);                  //0xea, IIC Master Status Register (IICMSTUR)
  temp = _bus->DataRead();
  CLRB(temp,0);                                //limpa o bit de arbitracao e...
  return temp;                                 //..e mostra qual foi perdido 
}


//================================================================================
// 
// [0xF0] GPIO-A direction (GPIOAD)
// [0xF1] GPIO-A (GPIOA)
// 
//================================================================================


/**
 * @brief  General Purpose I/O A (GPIOAD) direction control
 *        
 * @verbatim
 * REG [0xf0] GPIO-A direction (GPIOAD)
 *            bit [7-0] General Purpose I/O, Port A
 *                      GPIO-A_dir[7:0] : General Purpose I/O direction control.
 *                      0: Output
 *                      1: Input
 * @endverbatim
 *
 * @param dir:
 *        IN: indica a GPIO A como entrada
 *        OUT: indica a GPIO A como saida
 *
 * @note None
 */
void RA8889::GPIOA_InOut(uint8_t dir)
{
  _bus->CmdWrite(REG_GPIOAD);                    //0xf0, GPIO-A direction (GPIOAD)
   _bus->DataWrite(dir);
}


/**
 * @brief  Write General Purpose I/O A (GPIO A)
 *        
 * @verbatim
 * REG [0xf1] GPIO-A (GPIOA)
 *           bit [7-0] General Purpose I/O, Port A
 *                     Only available in parallel 8-bits MPU I/F & serial MPU I/F
 *                     For Write, Port A’s General Purpose Output
 *                     GPO-A[7:0] : Port A’s General Purpose Output, share with DB[15:8]
 *                     For Read, Port A’s General Purpose Input
 *                     GPI-A[7:0] : Port A’s General Purpose Input, share with DB[15:8]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
void RA8889::GPIOA_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOA);                     //0xf1, GPIO-A (GPIOA)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O A (GPIO A)
 *        
 * @verbatim
 * REG [0xf1] GPIO-A (GPIOA)
 *            bit [7-0] General Purpose I/O, Port A
 *                      Only available in parallel 8-bits MPU I/F & serial MPU I/F
 *                      For Write, Port A’s General Purpose Output
 *                      GPO-A[7:0] : Port A’s General Purpose Output, share with DB[15:8]
 *                      For Read, Port A’s General Purpose Input
 *                      GPI-A[7:0] : Port A’s General Purpose Input, share with DB[15:8] 
 * @endverbatim
 *
 * @param value
 *
 * @note  GPI-A[7:0] : General Purpose Input, share with DB[15:8]
 *        GPO-A[7:0] : General Purpose Output, share with DB[15:8]
 */
uint8_t RA8889::GPIOA_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOA);                     //0xf1, GPIO-A (GPIOA)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xF2] GPIO-B (GPIOB)
// 
//================================================================================


/**
 * @brief  Write General Purpose I/O B (GPIO B)
 *        
 * @verbatim
 * REG [0xf2] GPIO-B (GPIOB)
 *            bit [4] For Write. Port B’s General Purpose Output
 *                    The output data pin share with KOUT[0]
 *                    For Read, Port B’s General Purpose Input
 *                    The input data pin share with KIN[0]
 *            bit [3-0] For Read, Port B’s General Purpose Input
 *                    This bit not writable. Only valid on serial host interface.
 *                    {XA0, XnWR, XnRD, XnCS}
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
void RA8889::GPIOB_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOB);                     //0xf2, GPIO-B (GPIOB)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O B (GPIO B)
 *        
 * @verbatim
 * REG [0xf2] GPIO-B (GPIOB)
 *            bit [4] For Write. Port B’s General Purpose Output
 *                    The output data pin share with KOUT[0]
 *                    For Read, Port B’s General Purpose Input
 *                    The input data pin share with KIN[0]
 *            bit [3-0] For Read, Port B’s General Purpose Input
 *                    This bit not writable. Only valid on serial host interface.
 *                    {XA0, XnWR, XnRD, XnCS}
 * @endverbatim
 *
 * @param value
 *
 * @note Bit[7:0] are share with {XKIN[3:0], XA0, XnWR, XnRD, XnCS}
         Bit[3:0] are only available in serial MPU I/F, otherwise fix at 0.
 */
uint8_t RA8889::GPIOB_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOB);                     //0xf2, GPIO-B (GPIOB)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xF3] GPIO-C direction (GPIOCD)
// [0xF4] GPIO-C (GPIOC)
// 
//================================================================================


/**
 * @brief  General Purpose I/O C (GPIOCD) direction control
 *        
 * @verbatim
 * REG [0xf3] GPIO-C direction (GPIOCD)
 *            bit [7-0] General Purpose I/O, Port C
 *            GPIO-C_dir[7:0] : General Purpose I/O direction control.
 *            0: Output
 *            1: Input
 * @endverbatim
 *
 * @param dir:
 *        IN: indica a GPIO C como entrada
 *        OUT: indica a GPIO C como saida
 *
 * @note None
 */
void RA8889::GPIOC_InOut(uint8_t dir)
{
  _bus->CmdWrite(REG_GPIOCD);                    //0xf3, GPIO-C direction (GPIOCD)
  _bus->DataWrite(dir);
}



/**
 * @brief  Write General Purpose I/O C (GPIO C)
 *        
 * @verbatim
 * REG [0xf4] GPIO-C (GPIOC)
 *            bit [7-0] General Purpose I/O, Port C
 *            GPIO-C[7] & GPIO_C[4:0] : General Purpose Input / Output
 *            share with {XPWM0, XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCK}
 *            GPIO function valid only when relative function disabled. (ex. PWM, SPI master disabled).
 *            *** GPIO_C[6:5] are not available.
 * @endverbatim
 *
 * @param value
 *
 * @note  GPIO-C[7] : XPWM0,
 *        GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
 */
void RA8889::GPIOC_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOC);                     //0xf4, GPIO-C (GPIOC)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O C (GPIO C)
 *        
 * @verbatim
 * REG [0xf4] GPIO-C (GPIOC)
 *            bit [7-0] General Purpose I/O, Port C
 *            GPIO-C[7] & GPIO_C[4:0] : General Purpose Input / Output
 *            share with {XPWM0, XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCK}
 *            GPIO function valid only when relative function disabled. (ex. PWM, SPI master disabled).
 *            *** GPIO_C[6:5] are not available.
 * @endverbatim
 *
 * @param value
 *
 * @note GPIO-C[7] : XPWM0,
 *       GPIO_C[4:0] : XnSFCS1, XnSFCS0, XMISO, XMOSI, XSCLK.
 */
uint8_t RA8889::GPIOC_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOC);                     //0xf4, GPIO-C (GPIOC)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xF5] GPIO-D direction (GPIODD)
// [0xF6] GPIO-D (GPIOD)
// 
//================================================================================


/**
 * @brief  General Purpose I/O D (GPIODD) direction control
 *        
 * @verbatim
 * REG [0xf5] GPIO-D direction (GPIODD)
 *            bit [7-0] General Purpose I/O, Port D
 *            GPIO-D_dir[7:0] : General Purpose I/O direction control.
 *            0: Output
 *            1: Input
 * @endverbatim
 *
 * @param dir:
 *        IN: indica a GPIO D como entrada
 *        OUT: indica a GPIO D como saida
 *
 * @note None
 */
void RA8889::GPIOD_InOut(uint8_t dir)
{
  _bus->CmdWrite(REG_GPIODD);                    //0xf5, GPIO-D direction (GPIODD)
  _bus->DataWrite(dir);
}


/**
 * @brief  Write General Purpose I/O D (GPIO D)
 *        
 * @verbatim
 * REG [0xf6] GPIO-D (GPIOD)
 *            bit [7-0] General Purpose I/O, Port D
 *            GPIO-D[7:0] : General Purpose Input/Output
 *            share with PDAT[18, 2, 17, 16, 9, 8, 1, 0]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
void RA8889::GPIOD_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOD);                     //0xf6, GPIO-D (GPIOD)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O D (GPIO D)
 *        
 * @verbatim
 * REG [0xf6] GPIO-D (GPIOD)
 *            bit [7-0] General Purpose I/O, Port D
 *            GPIO-D[7:0] : General Purpose Input/Output
 *            share with PDAT[18, 2, 17, 16, 9, 8, 1, 0]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
uint8_t RA8889::GPIOD_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOD);                     //0xf6, GPIO-D (GPIOD)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xF7] GPIO-E direction (GPIOED)
// [0xF8] GPIO-E (GPIOE)
// 
//================================================================================


/**
 * @brief  General Purpose I/O E (GPIOED) direction control
 *        
 * @verbatim
 * REG [0xf7] GPIO-E direction (GPIOED)
 *            bit [7-0] General Purpose I/O, Port E
 *            GPIO-E_dir[7:0] : General Purpose I/O direction control.
 *            0: Output
 *            1: Input
 * @endverbatim
 *
 * @param dir:
 *        IN: indica a GPIO E como entrada
 *        OUT: indica a GPIO E como saida
 *
 * @note None
 */
void RA8889::GPIOE_InOut(uint8_t dir)
{
  _bus->CmdWrite(REG_GPIOED);                    //0xf7, GPIO-E direction (GPIOED)
  _bus->DataWrite(dir);
}


/**
 * @brief  Write General Purpose I/O E (GPIO E)
 *        
 * @verbatim
 * REG [0xf8] GPIO-E (GPIOE)
 *            bit [7-0] General Purpose I/O, Port E
 *            GPIO-E[7:0] : General Purpose Input/Output.
 *            share with XPDAT[12, 11, 10, 7, 6, 5, 4, 3]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
void RA8889::GPIOE_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOE);                     //0xf8, GPIO-E (GPIOE)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O E (GPIO E)
 *        
 * @verbatim
 * REG [0xf8] GPIO-E (GPIOE)
 *            bit [7-0] General Purpose I/O, Port E
 *            GPIO-E[7:0] : General Purpose Input/Output.
 *            share with XPDAT[12, 11, 10, 7, 6, 5, 4, 3]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
uint8_t RA8889::GPIOE_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOE);                     //0xf8, GPIO-E (GPIOE)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xF9] GPIO-F direction (GPIOFD)
// [0xFA] GPIO-F (GPIOF)
// 
//================================================================================


/**
 * @brief  General Purpose I/O F (GPIO F) direction control
 *        
 * @verbatim
 * REG [0xf9] GPIO-F direction (GPIOFD)
 *           bit [7-0] General Purpose I/O, Port F
 *           GPIO-F_dir[7:0] : General Purpose I/O direction control.
 *           0: Output
 *           1: Input
 * @endverbatim
 *
 * @param dir:
 *        IN: indica a GPIO F como entrada
 *        OUT: indica a GPIO F como saida
 *
 * @note None
 */
void RA8889::GPIOF_InOut(uint8_t dir)
{
  _bus->CmdWrite(REG_GPIOFD);                    //0xf9, GPIO-F direction (GPIOFD)
  _bus->DataWrite(dir);
}


/**
 * @brief  Write General Purpose I/O F (GPIO F)
 *        
 * @verbatim
 * REG [0xfA] GPIO-F (GPIOF)
 *            bit [7-0] General Purpose I/O, Port F
 *            GPIO-F[7:0] : General Purpose Input/Output.
 *            share with XPDAT[23, 22, 21, 20, 19, 15, 14, 13]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
void RA8889::GPIOF_Write(uint8_t value)
{
  _bus->CmdWrite(REG_GPIOF);                     //0xfa, GPIO-F (GPIOF)
  _bus->DataWrite(value);
}


/**
 * @brief  Read General Purpose I/O F (GPIO F)
 *        
 * @verbatim
 * REG [0xfA] GPIO-F (GPIOF)
 *            bit [7-0] General Purpose I/O, Port F
 *            GPIO-F[7:0] : General Purpose Input/Output.
 *            share with XPDAT[23, 22, 21, 20, 19, 15, 14, 13]
 * @endverbatim
 *
 * @param value
 *
 * @note None
 */
uint8_t RA8889::GPIOF_Read(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_GPIOF);                     //0xfa, GPIO-F (GPIOF)
  temp = _bus->DataRead();
  return temp;
}


//================================================================================
// 
// [0xFB] Key-Scan Control Register 1 (KSCR1)
// 
//================================================================================


/**
 * @brief Long Key Enable/Disble
 *        
 * @verbatim
 * REG [0xfb] Key-Scan Control Register 1 (KSCR1)
 *            bit [6] Long Key Enable Bit
 *                    1: Enable. Long key period is set by KSCR2 bit4-2.
 *                    0: Disable.
 * @endverbatim
 *
 * @param 
 *
 * @note This key pad controller supports 5x5 keys
 * 
 */
void RA8889::KeyScan_LongKeyEnable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSCR1);                     //0xfb, Key-Scan Control Register 1 (KSCR1)
  temp = _bus->DataRead();
  b ? SETB(temp,6) : CLRB(temp,6);             //Set bit 6
  _bus->DataWrite(temp);
}


/**
 * @brief Key Scan Frequency
 *        
 * @verbatim
 * REG [0xfb] Key-Scan Control Register 1 (KSCR1)
 *            bit [3] Repeatable Key enable
 *                    0: Disable Repeatable Key
 *                    1: Enable Repeatable Key
 *                    If the key is always pressed, the controller will periodically issue
 *                    key interrupt in every short key de-bounce time (long key
 *                    disable) or long key recognition time (long key enable) after user
 *                    clear interrupt flag.
 *
 *            bit [2-0] Row Scan Time
 *                      Period of Key scan controller to scan one row.
 *                      T_keyclk = (1 / F_sysclk) * 2048
 *                     
 *                      000: ROW_SCAN_Time = T_keyclk
 *                      001: ROW_SCAN_Time = T_keyclk * 2
 *                      010: ROW_SCAN_Time = T_keyclk * 4
 *                      011: ROW_SCAN_Time = T_keyclk * 8
 *                      100: ROW_SCAN_Time = T_keyclk * 16
 *                      101: ROW_SCAN_Time = T_keyclk * 32
 *                      110: ROW_SCAN_Time = T_keyclk * 64
 *                      111: ROW_SCAN_Time = T_keyclk * 128
 *                      This key pad controller supports 5x5 keys. Total Key pad
 *                      scan time = Row Scan Time * 5
 * @endverbatim
 *
 * @param 
 *
 * @note This key pad controller supports 5x5 keys
 *       Repeatable Key is disabled
 * 
 */
void RA8889::KeyScan_Freguency(uint8_t setx)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSCR1);     //0xfb, Key-Scan Control Register 1 (KSCR1)
  temp = _bus->DataRead();       //
  temp &= 0xf0;                //Reset bits 3 to 0
  temp |= (setx & 0x07);       //Row scan time adjust only
  _bus->DataWrite(temp);
}


//================================================================================
// 
// [0xFC] Key-Scan Controller Register 2 (KSCR2)
// 
//================================================================================


/**
 * @brief Key-Scan Wakeup Function Enable/Disable
 *        
 * @verbatim
 * REG [0xfc] Key-Scan Controller Register 2 (KSCR2)
 *            bit [7] Key-Scan Wakeup Function Enable Bit
 *                    0: Key-Scan Wakeup function is disabled.
 *                    1: Key-Scan Wakeup function is enabled.
 * @endverbatim
 *
 * @param 
 *
 * @note 
 * 
 */
void RA8889::KeyScan_WakeupFunctionEnable(bool b)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSCR2);            //0xfc, Key-Scan Controller Register 2 (KSCR2)
  temp = _bus->DataRead();
  b ? SETB(temp,7) : CLRB(temp,7);
  _bus->DataWrite(temp);
}


/**
 * @brief Long Key Timing Adjustment
 *        Adjusts the long key press timing in the Key-Scan Controller.
 *
 *        This function writes to the Key-Scan Controller Register 2 (KSCR2, address 0xFC) 
 *        to configure the timing for detecting long key presses. Only bits [4:2] of the 
 *        provided parameter are used, and these are OR'ed with the current register value 
 *        to preserve other control bits.
 *
 * @verbatim
 * REG [0xfc] Key-Scan Controller Register 2 (KSCR2)
 *            Bits [4:2] Long key timing adjustment
 *                       It determines long key recognition time since short key was
 *                       recognized. Value from 0 to 7.
 *
 *                       LongKey_Recognition_Time = RowScanTime * 5 * (LongKeyREcognitionFactor + 1) * 1024
 * @endverbatim
 *
 * @param setx Value containing the desired timing configuration. 
 *             Only bits [4:2] are considered; other bits are ignored.
 *
 * @note This function performs a read-modify-write cycle, meaning it updates 
 *       only the long key timing bits while preserving the other register fields.
 */
void RA8889::KeyScan_LongKeyTimingAdjust(uint8_t setx)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSCR2);                //0xfc, Key-Scan Controller Register 2 (KSCR2)
  temp = _bus->DataRead();
  temp |= setx & 0x1c;                    //Mask bit 4-2 only
  _bus->DataWrite(temp);
}


/**
 * @brief Reads the number of simultaneously pressed keys detected by the Key-Scan Controller.
 *
 *        This function accesses the Key-Scan Controller Register 2 (KSCR2, address 0xFC) 
 *        and extracts bits [1:0], which represent the number of keys currently pressed. 
 *        The value returned can range from 0 to 3, according to the hardware specification.
 *
 * @verbatim
 * REG [0xfc] Key-Scan Controller Register 2 (KSCR2)
 *            bit [1:0] → Key hit count (0 to 3)
 *            0: No key is pressed
 *            1: One key is pressed, REG[FDh] for the key code.
 *            2: Two keys are pressed, REG[FEh] for the 2nd key code.
 *            3: Three keys are pressed, REG[FFh] for the 3rd key code.
 *            It will auto return to 0 if w/o any keys are pressed for a debounce
 *            time. 
 * @endverbatim
 *
 * @param None
 *
 * @return uint8_t 
 *         - 0 → No key pressed
 *         - 1 → One key pressed
 *         - 2 → Two keys pressed
 *         - 3 → Three or more keys pressed (depending on HW support)
 *
 * @note This function only returns the number of simultaneous key hits, 
 *       not their individual positions or scan codes.
 * 
 */
uint8_t RA8889::KeyScan_KeyHits(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSCR2);              //0xfc, Key-Scan Controller Register 2 (KSCR2)
  temp = _bus->DataRead();                //Read key touch number
  temp = temp & 0x03;                   //Check how many keys are pressed
  return temp;
}


//================================================================================
// 
// [0xFD] Key-Scan Data Register (KSDR0)
// [0xFE] Key-Scan Data Register (KSDR1)
// [0xFF] Key-Scan Data Register (KSDR1)
//
//================================================================================


/**
 * @brief Reads the first key strobe data from the Key-Scan Controller.
 *
 * This function accesses Key-Scan Data Register 0 (KSDR0, 0xFD) to retrieve 
 * the key code of the first detected key. If no key is pressed, the register 
 * automatically returns 0xFF after the debounce time.
 *
 * @verbatim
 * REG [0xfd] Key-Scan Data Register (KSDR0)
 *            bit [7-0] Key Strobe Data 0
 *                      The corresponding key code 0 that is pressed.
 *                      It will auto return to FFh if w/o any keys are 
 *                      pressed for a debounce time.
 * @endverbatim
 *
 * @param None
 *
 * @return uint8_t
 *         - 0x00–0xFE → Key code of the pressed key
 *         - 0xFF      → No key pressed
 *
 * @note Only returns the first key. Additional keys may be read from KSDR1 or KSDR2.
 * 
 */
uint8_t RA8889::KeyScan_ReadKeyStrobeData0(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSDR0);                     //0xfd, Key-Scan Data Register (KSDR0)
  temp = _bus->DataRead();
  return temp;
}


/**
 * @brief Reads the second key strobe data from the Key-Scan Controller.
 *
 * This function accesses Key-Scan Data Register 1 (KSDR1, 0xFE) to retrieve 
 * the key code of the second detected key. Returns 0xFF if no key is pressed 
 * (after debounce).
 *
 * @verbatim
 * REG [0xfe] Key-Scan Data Register (KSDR1)
 *            bit [7-0] Key Strobe Data 1
 *                      The corresponding key code 1 that is pressed.
 *                      It will auto return to FFh if w/o any keys are 
 *                      pressed for a debounce time.
 * @endverbatim
 *
 * @param None
 *
 * @return uint8_t
 *         - 0x00–0xFE → Key code of the pressed key
 *         - 0xFF      → No key pressed
 *
 * @note None
 * 
 */
uint8_t RA8889::KeyScan_ReadKeyStrobeData1(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSDR1);                     //0xfe, Key-Scan Data Register (KSDR1)
  temp = _bus->DataRead();
  return temp;
}


/**
 * @brief Reads the third key strobe data from the Key-Scan Controller.
 * 
 * This function accesses Key-Scan Data Register 2 (KSDR2, 0xFF) to retrieve 
 * the key code of the third detected key. Returns 0xFF if no key is pressed 
 * (after debounce).
 *
 * @verbatim
 * REG [0xff] Key-Scan Data Register (KSDR2)
 *            bit [7-0] Key Strobe Data 2
 *            The corresponding key code 2 that is pressed.
 *            It will auto return to FFh if w/o any keys are 
 *            pressed for a debounce time.
 * @endverbatim
 *
 * @param None
 *
 * @return uint8_t
 *         - 0x00–0xFE → Key code of the pressed key
 *         - 0xFF      → No key pressed
 *
 * @note 
 * 
 */
uint8_t RA8889::KeyScan_ReadKeyStrobeData2(void)
{
  uint8_t temp;
  _bus->CmdWrite(REG_KSDR2);                     //0xff, Key-Scan Data Register (KSDR2)
  temp = _bus->DataRead();
  return temp;
}


/**
 * @brief Reads key strobe data from the Key-Scan Controller (KSDR0, KSDR1, KSDR2).
 *
 * This function retrieves the key code of a pressed key from the specified
 * Key-Scan Data Register. If no key is pressed in the selected register,
 * the function returns 0xFF after the debounce time.
 *
 * @verbatim
 * KSDR registers:
 *   REG [0xFD] KSDR0 → Key Strobe Data 0
 *   REG [0xFE] KSDR1 → Key Strobe Data 1
 *   REG [0xFF] KSDR2 → Key Strobe Data 2
 * 
 * Each register contains bits [7:0] representing the key code.
 * Returns 0xFF if no key is pressed after debounce.
 *
 * Notes:
 * Table 20-3 : Key Code Mapping Table (Normal Key)
 *       Kin0 Kin1 Kin2 Kin3 Kin4
 * Kout0 00h  01h  02h  03h  04h
 * Kout1 10h  11h  12h  13h  14h
 * Kout2 20h  21h  22h  23h  24h
 * Kout3 30h  31h  32h  33h  34h
 * Kout4 40h  41h  42h  43h  44h
 *
 * Table 20-4 : Key Code Mapping Table (Long Key)
 *       Kin0 Kin1 Kin2 Kin3 Kin4
 * Kout0 80h  81h  82h  83h  84h
 * Kout1 90h  91h  92h  93h  94h
 * Kout2 A0h  A1h  A2h  A3h  A4h
 * Kout3 B0h  B1h  B2h  B3h  B4h
 * Kout4 C0h  C1h  C2h  C3h  C4h
 * @endverbatim
 *
 * @param index Key strobe register index (0 for KSDR0, 1 for KSDR1, 2 for KSDR2)
 *
 * @return uint8_t
 *         - 0x00–0xFE → Key code of the pressed key
 *         - 0xFF      → No key pressed
 *
 * @note Index values outside 0–2 are invalid; the function will return 0xFF in that case.
 */
uint8_t RA8889::KeyScan_ReadKeyStrobeData(uint8_t index)
{
    switch(index)
    {
        case 0: _bus->CmdWrite(REG_KSDR0); break;
        case 1: _bus->CmdWrite(REG_KSDR1); break;
        case 2: _bus->CmdWrite(REG_KSDR2); break;
        default: return 0xFF;  // Invalid index
    }
    return _bus->DataRead();
}



//================================================================================
//
// API
// Funcoes de Escrita Direta de Dados para Memoria de Video
//
//================================================================================


//Escrita de Memoria de Display com MPU 8 bits / color depth 8bpp
void RA8889::MPU8_8bpp_MemoryWrite(
                                         uint16_t x,                    // x of coordinate
                                         uint16_t y,                    // y of coordinate
                                         uint16_t w,                    // width
                                         uint16_t h,                    // height
                                         const uint8_t *data            // 8bit data
                                        )
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  //ActiveWindow_XY(x, y);
  //ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);                          //Posicao inicial de escrita da memoria
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {                    //Varredura da linha
    for (j = 0; j < w; j++) {                  //Varredura da coluna
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apontado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();                      //Aguarde ate que a FIFO esteja vazia
}


//Escrita de Memoria de Display com MPU 8 bits / color depth 16bpp
void RA8889::MPU8_16bpp_MemoryWrite(
                                          uint16_t x,                    // x of coordinate
                                          uint16_t y,                    // y of coordinate
                                          uint16_t w ,                   // width
                                          uint16_t h ,                   // height
                                          const uint8_t *data            // 8bit data
                                         )
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  //ActiveWindow_XY(x, y);
  //ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);                          //Posicao inicial de escrita da memoria
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();
}


//Escrita de Memoria de Display com MPU 8 bits / color depth 24bpp
void RA8889::MPU8_24bpp_MemoryWrite(
                                          uint16_t x,                    // x of coordinate
                                          uint16_t y,                    // y of coordinate
                                          uint16_t w ,                   // width
                                          uint16_t h ,                   // height
                                          const uint8_t *data            // 8bit data
                                         )
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  //ActiveWindow_XY(x, y);
  //ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);                          //Posicao inicial de escrita da memoria
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();
}

//fazendo esta fucnao....
//Escrita de Memoria de Display com MPU 16 bits / color depth 16bpp
void RA8889::MPU16_16bpp_MemoryWrite(
                                           uint16_t x,                    // x of coordinate
                                           uint16_t y,                    // y of coordinate
                                           uint16_t w ,                   // width
                                           uint16_t h ,                   // height
                                           const uint16_t *data           // 16bit data
                                          )
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  //ActiveWindow_XY(x, y);
  //ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {                    //y
    for (j = 0; j < w; j++) {                  //x
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apontado no array de dados
      data++;                                  //incrementa o potneiro para a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();
}


//Escrita de Memoria de Display com MPU 16 bits / color depth 24bpp modo 1
void RA8889::MPU16_24bpp_Mode1_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data)
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  ActiveWindow_XY(x, y);
  ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w/2; j++) {
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      data++;                                  //incrementa o potneiro apra a poxima posição
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      data++;                                  //incrementa o potneiro apra a poxima posição
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      data++;                                  //incrementa o potneiro apra a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();
}


//Escrita de Memoria de Display com MPU 16 bits / color depth 24bpp modo 2
void RA8889::MPU16_24bpp_Mode2_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data)
{
  uint16_t i;                                  //posicao da linha
  uint16_t j;                                  //posicao da coluna
  if (!IsGraphicMode()) GraphicMode();         //Se no modo texto, muda para o modo grafico
  ActiveWindow_XY(x, y);
  ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
      Wait_WriteFIFO_NotFull();                //Aguarde ate que a FIFO nao esteja mais cheia
      _bus->DataWrite(*data);                    //Trasfere o byte apotnado no array de dados
      data++;                                  //incrementa o potneiro apra a poxima posição
    }
  }
  Wait_WriteFIFO_Empty();
}


//NAO ESTA CONCLUIDA....
//Esta funcao é a sitense das funcoes acima, funcionando de forma automatica, de acordo coma escolha inicial da
//inicializacao do sistema com MPU e color depth escolhido
void RA8889::MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint8_t *data)
{

#ifdef MCU_8bit_COLORDEPTH_8bpp
  MPU8_8bpp_MemoryWrite(x, y, w , h , data); 
#endif

#ifdef MCU_8bit_COLORDEPTH_16bpp
  MPU8_16bpp_MemoryWrite(x, y, w , h , data; 
#endif

#ifdef MCU_8bit_COLORDEPTH_24bpp
  MPU8_24bpp_MemoryWrite(x, y, w , h , data); 
#endif

#ifdef MCU_16bit_COLORDEPTH_8bpp_Mode2
  //
#endif

#ifdef MCU_16bit_COLORDEPTH_16bpp
  MPU16_16bpp_MemoryWrite(x, y, w, h, reinterpret_cast<const uint16_t*>(data));
#endif

#ifdef MCU_16bit_COLORDEPTH_24bpp_Mode1
  MPU16_24bpp_Mode1_MemoryWrite(x, y, w , h , reinterpret_cast<const uint16_t*>(data)); 
#endif

#ifdef MCU_16bit_COLORDEPTH_8bpp_Mode1

#endif

#ifdef MCU_16bit_COLORDEPTH_24bpp_Mode2
  MPU16_24bpp_Mode2_MemoryWrite(x, y, w , h , reinterpret_cast<const uint16_t*>(data));
#endif

}


//================================================================================
//
// API
// Funcoes de DMA
//
//================================================================================



/** IMPLEMENTACAO FUTURA
 * @brief Uso de DMA em funções que podem se utilziar deste recurso
 *
 * @param bool b indica a necessidade de uso de DMA 
 * 
 * @code
 * setWindow(area->x1, area->y1, w, h);
 * useDMA(true);
 * WritePixels(px_map, num_pixels, true);
 * useDMA(false); 
 * @endcode
 * 
 * @note None
 */
void RA8889::useDMA(bool b)
{
  _usedma = b;
}



/**
 * @brief 
 *
 * @param uint8_t scs        -> scs: Select SPI : SCS0/SCS1
 * @param uint8_t clk        -> Clk: SPI Clock = System Clock /{(Clk+1)*2}
 * @param uint16_t x1        -> X of DMA Coordinate
 * @param uint16_t y1        -> Y of DMA Coordinate
 * @param uint16_t Wx        -> DMA Block width
 * @param uint16_t Hy        -> DMA Block height
 * @param uint16_t picwidth  -> DMA Picture width
 * @param uint32_t addr      -> DMA Source Start address
 * 
 * @note None
 */
void RA8889::DMA_24bit_Block(uint8_t scs,         // scs: Select SPI : SCS0/SCS1
                                   uint8_t Clk,         // Clk: SPI Clock = System Clock /{(Clk+1)*2}
                                   uint16_t x1,         // X1: X of DMA Coordinate
                                   uint16_t y1,         // Y1: Y of DMA Coordinate
                                   uint16_t Wx,         // X_W: DMA data width
                                   uint16_t Hy,         // Y_H: DMA data height
                                   uint16_t picWidth,   // P_W: Picture's width
                                   uint32_t addr        //  DMA Source Start address
                                  )
{
  SFlashSPI_Enable(true);

  if(scs == 0)  SFI_Select_ROM0();                       // Select SPI0
  if(scs == 1)  SFI_Select_ROM1();                       // Select SPI1

  Select_SFI_DualData_3Bh();                             //Dual mode 0
  Memory_XYMode();                     
  SFI_Select_DMAMode();                                  // Select SPI DMA mode
  SPI_Clock_Period(Clk);                                 // Select SPI clock

  GotoPixel_XY(x1, y1);                                  // Setting the location of memory in the graphic mode
  SFI_DMA_DestinationUpperLeftCorner(x1, y1);            // DMA destination(SDRAM address)
  SFI_DMA_TransferWidthHeight(Wx, Hy);                   // Setting Block data: width&height
  SFI_DMA_SourceWidth(picWidth);                         // Setting the width of the source data
  SFI_DMA_SourceAddress(addr);                           // Setting the FLASH address of the source data

  SFI_DMA_Start();                                       //
  SFI_DMA_WaitReady();                                   //
}


/**
 * @brief 
 *
 * @param uint8_t clk        -> Clk : SPI Clock = System Clock /{(Clk)*2}, SPI CLK recommend <=90MHz
 * @param uint16_t x1        -> X of DMA Coordinate
 * @param uint16_t y1        -> Y of DMA Coordinate
 * @param uint16_t Wx        -> DMA Block width
 * @param uint16_t Hy        -> DMA Block height
 * @param uint16_t picwidth  -> DMA Picture width
 * @param uint32_t addr      -> DMA Source Start address
 * 
 * @note None
 */
void RA8889::DMA_24bit(
                              uint8_t clk,       // Clk : SPI Clock = System Clock /{(Clk)*2}, SPI CLK recommend <=90MHz
                              uint16_t x1,       // X of DMA Coordinate
                              uint16_t y1,       // Y of DMA Coordinate
                              uint16_t Wx,       // DMA Block width
                              uint16_t Hy,       // DMA Block height
                              uint16_t picwidth, // DMA Picture width
                              uint32_t addr      // DMA Source Start address
                            )
{
  SPIM_ClockDivided_1();                       // SPI Clock = System Clock /{(Clk)*2}
  SPIM_RxLatchEdge_Falling();                  // SPIM latch

  SFI_Select_DMAMode();                        // Set Serial Flash DMA Mode
  SPI_Clock_Period(clk);
  
  // DMA

  GotoPixel_XY(x1, y1);                        // set Memory coordinate in Graphic Mode
  SFI_DMA_DestinationUpperLeftCorner(x1, y1);  // DMA Destination position(x,y)
  SFI_DMA_TransferWidthHeight(Wx, Hy);         // Set DMA Block (Height , Width)
  SFI_DMA_SourceWidth(picwidth);               // Set DMA Source Picture Width
  SFI_DMA_SourceAddress(addr);                 // Set Serial Flash DMA Source Starting Address

  SFI_DMA_Start();                             // Start DMA
  SFI_DMA_WaitReady();                         // DMA Busy Check
}


/**
 * @brief 
 *
 * @param uint8_t clk        -> Clk : SPI Clock = System Clock /{(Clk+1)*2}
 * @param uint16_t x1        -> X of DMA Coordinate
 * @param uint16_t y1        -> Y of DMA Coordinate
 * @param uint16_t Wx        -> DMA Block width
 * @param uint16_t Hy        -> DMA Block height
 * @param uint16_t picwidth  -> DMA Picture width
 * @param uint32_t addr      -> DMA Source Start address
 * 
 * @note None
 */
void RA8889::DMA_32bit(uint8_t clk,       // Clk : SPI Clock = System Clock /{(Clk+1)*2}
                             uint16_t x1,       // X of DMA Coordinate
                             uint16_t y1,       // Y of DMA Coordinate
                             uint16_t Wx,       // DMA Block width
                             uint16_t Hy,       // DMA Block height
                             uint16_t picwidth, // DMA Picture width
                             uint32_t addr      // DMA Source Start address
                            )
{

  SFI_Select_DMAMode();                          // Set Serial Flash DMA Mode
  SPI_Clock_Period(clk);

  SFI_Select_32bitAddress();                     // Set Serial Flash/ROM 32bits Address

  //DMA

  GotoPixel_XY(x1, y1);                          // set Memory coordinate in Graphic Mode
  SFI_DMA_DestinationUpperLeftCorner(x1, y1);    // DMA Destination position(x,y)
  SFI_DMA_TransferWidthHeight(Wx, Hy);           // Set DMA Block (Height , Width)
  SFI_DMA_SourceWidth(picwidth);                 // Set DMA Source Picture Width
  SFI_DMA_SourceAddress(addr);                   // Set Serial Flash DMA Source Starting Address

  SFI_DMA_Start();                               // Start DMA
  SFI_DMA_WaitReady();                           // DMA Busy Check
  SFI_Select_24bitAddress();
}


/**
 * @brief 
 *
 * @param uint8_t bus_select
 *                0 : select bus0
 *                1 : select bus1
 * @param uint8_t scs_select
 *                0 : select SFI0 for bus0
 *                1 : select SFI1 for bus0
 *                2 : select SFI2 for bus1
 *                3 : select SFI3 for bus1
 * 
 * @note None
 */
void RA8889::Switch_24bitsTo32bits(uint8_t bus, uint8_t scs)
{
  SFlashSPI_Enable(true);
  SPIM_RxLatchEdge_Falling();
  SPIM_ClockDivided_1();
  SPI_Clock_Period(2);

  if (bus == 0) SPIM_Select_Bus0();
  if (bus == 1) SPIM_Select_Bus1();

  if (scs == 0) { nSS_Select_Channel(eNSS_Channel::XNSFCS0); }
  if (scs == 1) { nSS_Select_Channel(eNSS_Channel::XNSFCS1); }
  if (scs == 2) { nSS_Select_Channel(eNSS_Channel::XNSFCS2); }
  if (scs == 3) { nSS_Select_Channel(eNSS_Channel::XNSFCS3); }
    
    //**************************//data are read on the clock's rising edge(low!鱤igh transition)
    //**************************//and data are changed on a falling edge (high!鱨ow clock transition)
    Reset_CPOL();
    // Set_CPOL();
    Reset_CPHA();
    // Set_CPHA();
    
    //Enter 4-byte mode

    nSS_Active();                              // nSS port will goes low
    SPIM_TxRxFIFOData_Put(0xb7);

    delay(1);                                  // Delay 100uS
    nSS_Inactive();                            // nSS port will goes high
}


/**
 * @brief 
 *
 * @param uint8_t bus_select
 *                0 : select bus0
 *                1 : select bus1
 * @param uint8_t scs_select
 *                0 : select SFI0 for bus0
 *                1 : select SFI1 for bus0
 *                2 : select SFI2 for bus1
 *                3 : select SFI3 for bus1
 * @param uint8_t clk_div
 * @param uint16_t x0
 * @param uint16_t y0
 * @param uint16_t width
 * @param uint16_t height
 * @param uint16_t picture_width
 * @param uint32_t addr
 * 
 * @note None
 */
void RA8889::DMA_24bitAddressBlockMode(uint8_t bus_select,
                                             uint8_t scs_select,
                                             uint8_t clk_div,
                                             uint16_t x0,
                                             uint16_t y0,
                                             uint16_t width,
                                             uint16_t height,
                                             uint16_t picture_width,
                                             uint32_t addr
                                            )
{
  if(bus_select==0) Font_DMA_Select_Bus0();
  if(bus_select==1) Font_DMA_Select_Bus1();
   
  if(scs_select==0) {
    SFI_SelectROM_CS0();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT0 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==1) {
    SFI_SelectROM_CS1();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT1 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==2) {
    SFI_SelectROM_CS2();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT2 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==3) {
    SFI_SelectROM_CS3();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT3 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  }
       
  _bus->RegisterWrite(REG_SPI_DIVSOR,clk_div);//bbh  
  
  _bus->RegisterWrite(REG_DMA_DX0,x0);//c0h
  _bus->RegisterWrite(REG_DMA_DX1,x0>>8);//c1h
  _bus->RegisterWrite(REG_DMA_DY0,y0);//c2h
  _bus->RegisterWrite(REG_DMA_DY1,y0>>8);//c3h 
  _bus->RegisterWrite(REG_DMAW_WTH0,width);//c6h
  _bus->RegisterWrite(REG_DMAW_WTH1,width>>8);//c7h
  _bus->RegisterWrite(REG_DMAW_HIGH0,height);//c8h
  _bus->RegisterWrite(REG_DMAW_HIGH1,height>>8);//c9h 
  _bus->RegisterWrite(REG_DMA_SWTH0,picture_width);//cah
  _bus->RegisterWrite(REG_DMA_SWTH1,picture_width>>8);//cbh 
  _bus->RegisterWrite(REG_DMA_SSTR0,addr);//bch
  _bus->RegisterWrite(REG_DMA_SSTR1,addr>>8);//bdh
  _bus->RegisterWrite(REG_DMA_SSTR2,addr>>16);//beh
  _bus->RegisterWrite(REG_DMA_SSTR3,addr>>24);//bfh 
  
  _bus->RegisterWrite(REG_DMA_CTRL, BIT_DMA_START);//b6h 
  CoreTask_WaitReady(); 
 }


/**
 * @brief 
 *
 * @param uint8_t bus_select
 *                0 : select bus0
 *                1 : select bus1
 * @param uint8_t scs_select
 *                0 : select SFI0 for bus0
 *                1 : select SFI1 for bus0
 *                2 : select SFI2 for bus1
 *                3 : select SFI3 for bus1
 * @param uint8_t clk_div
 * @param uint16_t x0
 * @param uint16_t y0
 * @param uint16_t width
 * @param uint16_t height
 * @param uint16_t picture_width
 * @param uint32_t addr
 * 
 * @note None
 */
 void RA8889::DMA_32bitAddressBlockMode(uint8_t bus_select,
                                              uint8_t scs_select,
                                              uint8_t clk_div,
                                              uint16_t x0,
                                              uint16_t y0,
                                              uint16_t width,
                                              uint16_t height,
                                              uint16_t picture_width,
                                              uint32_t addr
                                             )
{

  if(bus_select==0) Font_DMA_Select_Bus0();
  if(bus_select==1) Font_DMA_Select_Bus1();
   
  if(scs_select==0) {
    SFI_SelectROM_CS0();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT0 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==1) {
    SFI_SelectROM_CS1();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT1 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==2) {
    SFI_SelectROM_CS2();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT2 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==3) {
    SFI_SelectROM_CS3();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT3 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  }   
  
  _bus->RegisterWrite(REG_SPI_DIVSOR,clk_div);//bbh 
  
  _bus->RegisterWrite(REG_DMA_DX0,x0);//c0h
  _bus->RegisterWrite(REG_DMA_DX1,x0>>8);//c1h
  _bus->RegisterWrite(REG_DMA_DY0,y0);//c2h
  _bus->RegisterWrite(REG_DMA_DY1,y0>>8);//c3h 
  _bus->RegisterWrite(REG_DMAW_WTH0,width);//c6h
  _bus->RegisterWrite(REG_DMAW_WTH1,width>>8);//c7h
  _bus->RegisterWrite(REG_DMAW_HIGH0,height);//c8h
  _bus->RegisterWrite(REG_DMAW_HIGH1,height>>8);//c9h 
  _bus->RegisterWrite(REG_DMA_SWTH0,picture_width);//cah
  _bus->RegisterWrite(REG_DMA_SWTH1,picture_width>>8);//cbh 
  _bus->RegisterWrite(REG_DMA_SSTR0,addr);//bch
  _bus->RegisterWrite(REG_DMA_SSTR1,addr>>8);//bdh
  _bus->RegisterWrite(REG_DMA_SSTR2,addr>>16);//beh
  _bus->RegisterWrite(REG_DMA_SSTR3,addr>>24);//bfh  
  
  _bus->RegisterWrite(REG_DMA_CTRL, BIT_DMA_START);//b6h 
  CoreTask_WaitReady(); 
 }
 

/**
 * @brief 
 *
 * @param uint8_t bus_select
 *                0 : select bus0
 *                1 : select bus1
 * @param uint8_t scs_select
 *                0 : select SFI0 for bus0
 *                1 : select SFI1 for bus0
 *                2 : select SFI2 for bus1
 *                3 : select SFI3 for bus1
 * @param uint8_t clk_div
 * @param uint32_t source_addr
 * @param uint32_t des_address
 * @param uint32_t number
 * 
 * @note None
 */
void RA8889::DMA_24bitAddressLinearMode(uint8_t bus_select,
                                              uint8_t scs_select,
                                              uint8_t clk_div,
                                              uint32_t source_addr,
                                              uint32_t des_address,
                                              uint32_t number
                                             )
{
  
  uint8_t temp = 0;

   //switch canvas to 8bpp format and canvas linear mode
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp |= BIT_CANVAS_LINEAR_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_8BPP;
  _bus->DataWrite(temp);

  if(bus_select==0) Font_DMA_Select_Bus0();
  if(bus_select==1) Font_DMA_Select_Bus1();
   
  if(scs_select==0) { 
    SFI_SelectROM_CS0();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT0 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==1) {
    SFI_SelectROM_CS1(); 
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT1 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==2) {
    SFI_SelectROM_CS2();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT2 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==3) {
    SFI_SelectROM_CS3();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT3 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_24BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  }
    
  _bus->RegisterWrite(REG_SPI_DIVSOR, clk_div);//bbh  
  
  _bus->RegisterWrite(REG_DMA_DX0, des_address);//c0h
  _bus->RegisterWrite(REG_DMA_DX1, des_address>>8);//c1h
  _bus->RegisterWrite(REG_DMA_DY0, des_address>>16);//c2h
  _bus->RegisterWrite(REG_DMA_DY1, des_address>>24);//c3h 

  _bus->RegisterWrite(REG_DMAW_WTH0, number);//c6
  _bus->RegisterWrite(REG_DMAW_WTH1, number>>8);//c7
  _bus->RegisterWrite(REG_DMAW_HIGH0, number>>16);//c8
  _bus->RegisterWrite(REG_DMAW_HIGH1, number>>24);//c9

  _bus->RegisterWrite(REG_DMA_SSTR0, source_addr);//bch
  _bus->RegisterWrite(REG_DMA_SSTR1, source_addr>>8);//bdh
  _bus->RegisterWrite(REG_DMA_SSTR2, source_addr>>16);//beh
  _bus->RegisterWrite(REG_DMA_SSTR3, source_addr>>24);//bfh 
  
  _bus->RegisterWrite(REG_DMA_CTRL, BIT_DMA_START);//b6h 
  CoreTask_WaitReady(); 

#if defined(COLOR_DEPTH_16)
  
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = 0;
  temp |= BIT_CANVAS_BLOCK_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_16BPP;
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = 0;
  temp |= BIT_S0_COLOR_DEPTH_16BPP;
  temp |= BIT_S1_COLOR_DEPTH_16BPP;
  temp |= BIT_DESTINATION_COLOR_DEPTH_16BPP;
  _bus->DataWrite(temp);

#elif defined(COLOR_DEPTH_24)
  
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = 0;
  temp |= BIT_CANVAS_BLOCK_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = 0;
  temp |= BIT_S0_COLOR_DEPTH_24BPP;
  temp |= BIT_S1_COLOR_DEPTH_24BPP;
  temp |= BIT_DESTINATION_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);

#endif

 }


 /**
 * @brief 
 *
 * @param uint8_t bus_select
 *                0 : select bus0
 *                1 : select bus1
 * @param uint8_t scs_select
 *                0 : select SFI0 for bus0
 *                1 : select SFI1 for bus0
 *                2 : select SFI2 for bus1
 *                3 : select SFI3 for bus1
 * @param uint8_t clk_div
 * @param uint32_t source_addr
 * @param uint32_t des_address
 * @param uint32_t number
 * 
 * @note None
 */
 void RA8889::DMA_32bitAddressLinearMode(uint8_t bus_select,
                                               uint8_t scs_select,
                                               uint8_t clk_div,
                                               uint32_t source_addr,
                                               uint32_t des_address,
                                               uint32_t number
                                              )
{
  uint8_t temp = 0;

   //switch canvas to 8bpp format and canvas linear mode
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp |= BIT_CANVAS_LINEAR_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_8BPP;
  _bus->DataWrite(temp);

  if(bus_select==0) Font_DMA_Select_Bus0();
  if(bus_select==1) Font_DMA_Select_Bus1();
   
  if(scs_select==0) { 
    SFI_SelectROM_CS0();  
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT0 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==1) {
    SFI_SelectROM_CS1();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT1 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==2) {
    SFI_SelectROM_CS2();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT2 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  } else if(scs_select==3) {
    SFI_SelectROM_CS3();
    _bus->RegisterWrite(REG_SFL_CTRL, BIT_SERIAL_FLASH_SELECT3 | BIT_SERIAL_FLASH_DMA_MODE | BIT_SERIAL_FLASH_ADDR_32BIT | BIT_FOLLOW_RA8875_MODE | BIT_SPI_FAST_READ_8DUMMY);
  }
     
  _bus->RegisterWrite(REG_SPI_DIVSOR,clk_div);//bbh 
  
  _bus->RegisterWrite(REG_DMA_DX0, des_address);//c0h
  _bus->RegisterWrite(REG_DMA_DX1, des_address>>8);//c1h
  _bus->RegisterWrite(REG_DMA_DY0, des_address>>16);//c2h
  _bus->RegisterWrite(REG_DMA_DY1, des_address>>24);//c3h 

  _bus->RegisterWrite(REG_DMAW_WTH0, number);//c6
  _bus->RegisterWrite(REG_DMAW_WTH1, number>>8);//c7
  _bus->RegisterWrite(REG_DMAW_HIGH0, number>>16);//c8
  _bus->RegisterWrite(REG_DMAW_HIGH1, number>>24);//c9

  _bus->RegisterWrite(REG_DMA_SSTR0, source_addr);//bch
  _bus->RegisterWrite(REG_DMA_SSTR1, source_addr>>8);//bdh
  _bus->RegisterWrite(REG_DMA_SSTR2, source_addr>>16);//beh
  _bus->RegisterWrite(REG_DMA_SSTR3, source_addr>>24);//bfh 
  
  _bus->RegisterWrite(REG_DMA_CTRL, BIT_DMA_START);//b6h 
  CoreTask_WaitReady(); 

#if defined(COLOR_DEPTH_16)
  
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = 0;
  temp |= BIT_CANVAS_BLOCK_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_16BPP;
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = 0;
  temp |= BIT_S0_COLOR_DEPTH_16BPP;
  temp |= BIT_S1_COLOR_DEPTH_16BPP;
  temp |= BIT_DESTINATION_COLOR_DEPTH_16BPP;
  _bus->DataWrite(temp);

#elif defined(COLOR_DEPTH_24)
  
  _bus->CmdWrite(REG_AW_COLOR);                  //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = 0;
  temp |= BIT_CANVAS_BLOCK_MODE;
  temp |= BIT_CANVAS_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);

  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = 0;
  temp |= BIT_S0_COLOR_DEPTH_24BPP;
  temp |= BIT_S1_COLOR_DEPTH_24BPP;
  temp |= BIT_DESTINATION_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);

#endif
   
 }


//================================================================================
//
// API
// Funcoes PIP - Picture-in-Picture
//
//================================================================================


void RA8889::PIP(bool On_Off,                   // 0 : disable PIP, 1 : enable PIP, 2 : To maintain the original state
                       uint8_t Pselect,               // 1 : use PIP1 , 2 : use PIP2
                       uint32_t Paddr,                // start address of PIP
                       uint16_t Px,                   // coordinate X of PIP Window, It must be divided by 4.
                       uint16_t Py,                   // coordinate Y of PIP Window, It must be divided by 4.
                       uint32_t imagewidth,           // Image Width of PIP (recommend = canvas image width)
                       uint16_t Dx,                   // coordinate X of Display Window
                       uint16_t Dy,                   // coordinate Y of Display Window
                       uint16_t Dwidth,               // width of PIP and Display Window, It must be divided by 4.
                       uint16_t Dheight               // height of PIP and Display Window , It must be divided by 4.
                      )
{
  if (Pselect == 1) { PIP_Select_Parameter(ePIPSelect::PIP1); }
  if (Pselect == 2) { PIP_Select_Parameter(ePIPSelect::PIP2); }
  PIP_Display_StartXY(Dx, Dy);
  PIP_Image_StartAddress(Paddr);
  PIP_Image_Width(imagewidth);
  PIP_WindowImage_StartXY(Px, Py);
  PIP_Window_WidthHeight(Dwidth, Dheight);
  if (Pselect == 1) { PIP1_Enable(On_Off); };
  if (Pselect == 2) { PIP2_Enable(On_Off); };
}


//================================================================================
//
// API
// Funcoes PWM
//
//================================================================================


/** nao surtiu efeito aparente
 * @brief 
 *
 * @param bool on_off                 -> true ON pwm, false OFF pwm
 * @param eDividerClock clock_divided -> divided PWM clock
 *                      eDividerClock::X1 = 1/1
 *                      eDividerClock::X2 = 1/2
 *                      eDividerClock::X4 = 1/4
 *                      eDividerClock::X8 = 1/8                             
 * @param uint8_t prescalar           -> Prescaler : only 1~256
 * @param uint16_t clock_per_period   -> Clock per Period (Count_Buffer) : set PWM output period time
 * @param uint16_t duty               -> ducty cicle (Compare_Buffer) : set PWM output high level time(Duty cycle)
 * 
 * @note 
 * Such as the following formula :
 *   PWM CLK = (Core CLK / Prescalar ) /2^ divided clock
 *   PWM output period = (Count Buffer + 1) x PWM CLK time
 *   PWM output high level time = (Compare Buffer + 1) x PWM CLK time
 */    
void RA8889::PWM0(bool on_off,                  // true ON pwm, false OFF pwm
                        eDividerClock clock_divided,  // divided PWM clock
                        uint8_t prescaler,            // Prescaler : only 1~256
                        uint16_t clock_per_period,    // clock per period (Count_Buffer) : set PWM output period time
                        uint16_t duty                 // duty (compare buffer) : set PWM output high level time (Duty cycle)
                        )
{
  //0x84, PWM Prescaler Register (PSCLR)

  PWM0_Select();                                
  PWM_Prescaler(prescaler);

  //0x85, PWM clock Mux Register (PMUXR)

  PWM0_ClockDividedBy(clock_divided);
  
  //0x8a, Timer 0 count buffer register [TCNTB0L]
  //0x8b, Timer 0 count buffer register [TCNTB0H]
  PWM0_ClocksPerPeriod(clock_per_period);      //Medidor superior
  
  //0x88, Timer 0 compare buffer register [TCMPB0L]
   //0x89, Timer 0 compare buffer register [TCMPB0H]
  PWM0_Duty(duty);                             //DUTY

  //0x86, PWM Configuration Register (PCFGR) 
  on_off ? PWM0_StartTimer() : PWM0_StopTimer();
}


/**
 * @brief 
 *
 * @param bool on_off                 -> true ON pwm, false OFF pwm
 * @param eDividerClock clock_divided -> divided PWM clock
 *                      eDividerClock::X1 = 1/1
 *                      eDividerClock::X2 = 1/2
 *                      eDividerClock::X4 = 1/4
 *                      eDividerClock::X8 = 1/8                             
 * @param uint8_t prescalar           -> Prescaler : only 1~256
 * @param uint16_t clock_per_period   -> Clock per Period (Count_Buffer) : set PWM output period time
 * @param uint16_t duty               -> ducty cicle (Compare_Buffer) : set PWM output high level time(Duty cycle)
 * 
 * @note 
 * Such as the following formula :
 *   PWM CLK = (Core CLK / Prescalar ) /2^ divided clock
 *   PWM output period = (Count Buffer + 1) x PWM CLK time
 *   PWM output high level time = (Compare Buffer + 1) x PWM CLK time
 */
void RA8889::PWM1(bool on_off,                        // true ON pwm, false OFF pwm
                        eDividerClock clock_divided,  // divided PWM clock
                        uint8_t prescaler,            // Prescaler : only 1~256
                        uint16_t clock_per_period,    // clock per period (Count_Buffer) : set PWM output period time
                        uint16_t duty                 // duty (compare buffer) : set PWM output high level time (Duty cycle)
                        )
{
  //0x84, PWM Prescaler Register (PSCLR)

  PWM1_Select();
  PWM_Prescaler(prescaler);

  //0x85, PWM clock Mux Register (PMUXR)

  PWM1_ClockDividedBy(clock_divided);

  //0x8e, Timer 1 count buffer register [TCNTB1L]
  //0x8f, Timer 1 count buffer register [TCNTB1H]
  PWM1_ClocksPerPeriod(clock_per_period);      //Medidor superior

  //0x8c, Timer 1 compare buffer register [TCMPB1L]
  //0x8d, Timer 1 compare buffer register [TCMPB1H]
  PWM1_Duty(duty);                             //DUTY

  //0x86, PWM Configuration Register (PCFGR) 
  on_off ? PWM1_StartTimer() : PWM1_StopTimer();
}


/** 
 * @brief  Seta os pino da luz de fundo
 *
 * @verbatim
 * Para uso desta função necessário configuração fisica de hardware
 * do módulo de display ER-TFT070-2-6105. Os jumper precisam ser 
 * configurados: J27 curto, J28 aberto
 * 
 * Uso com Sheild ER-AS-5517
 * -------------------------
 * O módulo shield ER-AS-5517 para ser utilizado em Arduino UNO/Mega/Duo 
 * não apresenta uma ligação de controle num dos pinos do arduino ao 
 * pino 14 do módulo de display ER-TFT070-2-6105. Ao invés disso, o pino 14 
 * do módulo de display é conectado ao pino 14 do shield e este ligado ao Vcc 
 * de 3,3V. Desta forma, o display permanece sempre ligado, sem possibilidade
 * de desligar a luz de fundo do display.  
 *
 * Sem uso do Shield
 * -----------------
 * Quando é utilziado um adaptador cabo flat para barra de pinos entre o MCU e 
 * o módulo de display o controle de ligar/desligar a luz de fundo já se torna 
 * possível.  Isso é feito utilziando um dos pinos do MCU disponível conectado 
 * diretamente ao pino 14 (BL CONBTROL) do módulo de display ER-TFT070-2-6105.  
 * No display ER-TFT070-2-6105 baseado no controlador da RAIO RA8889 o 
 * controle de backlight precisa ser feito pelo usuário.
 * 
 * Para outros módulos de display, por exemplo o ER-TFTM070-5 baseado no RA8875,
 * o seu pino 14 é mantido geralmetne solto, sem conexão com nada.
 * 
 * Importante: O controle de ligar e desligar a luz de fundo só é possível se 
 * o controlador MCU pode fornecer uma carga suficiente para acionar a luz de 
 * fundo, mesmo que seus pinos de saida digital opere em 3,3V. Desta forma, o
 * display não irá aparecer a imagem. APra o caso de não cosneguir cotnrole 
 * ligar/desligar a luz de fundo, conecte o pino 14 com CON3 diretamente na 
 * alimentação de 3,3V. Esta função não terá utilidade.
 * 
 * @endverbatim
 * @param pin: Número do pino do Host que será conectado ao modulo de display
 * 
 * @note None
 */
void RA8889::setBacklight(uint8_t pin)
{
  if (pin == 0) return;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);                      //por padrão é desligado
  _pin_backlight = pin;
}


/** 
 * @brief  Liga/desliga a luz de fundo
 *
 * @verbatim
 * Para uso desta função necessário configuração fisica de hardware
 * do módulo de display ER-TFT070-2-6105. Os jumper precisam ser 
 * configurados: J27 curto, J28 aberto
 * 
 * Liga e desliga a luz de fundo pelos pinos digitais previamente configurados
 * como saída. Utilzie a função setBacklight() para configurar o pino de saída 
 * digital conectado ao módulo de display, por exmeplo Pino 14 do módulo.
 * 
 * @endverbatim
 * 
 * @param on: true, liga a luz de fundo
 * 
 * @note  
 * O controle de ligar e desligar a luz de fundo só é possível se 
 * o controlador MCU pode fornecer uma carga suficiente para acionar a luz de 
 * fundo, mesmo que seus pinos de saida digital opere em 3,3V. Desta forma, o
 * display não irá aparecer a imagem. APra o caso de não cosneguir cotnrole 
 * ligar/desligar a luz de fundo, conecte o pino 14 com CON3 diretamente na 
 * alimentação de 3,3V. Esta função não terá utilidade.
 */
void RA8889::BacklightOn(bool on)
{
  if (_pin_backlight == 0) return;
  on ? digitalWrite(_pin_backlight, HIGH) : digitalWrite(_pin_backlight, LOW);
  DEBUG_PRINTD("Status Backlight", digitalRead(_pin_backlight), true, 500, true);         //Debug
}


/** 
 * @brief  Turn Backlight On/Off
 *
 * @verbatim
 * Para uso desta função necessário configuração fisica de hardware
 * do módulo de display ER-TFT070-2-6105. Os jumper precisam ser 
 * configurados: J27 aberto, J28 curto

 * @param bool on -> true backlight ON
 * 
 * @note  
 * O controle de ligar e desligar e intensidade da luz de fundo só é possível 
 * se o módulo de display for configurado fisicamente em seus jumper J27 e J28
 * para que o controle interno PWM seja direcionado ao pino 14 do CON3.
 */
void RA8889::Backlight(bool on) {
  if (on) {
	  _bus->RegisterWrite(REG_PMUXR, 
	                BIT_PWM1_TIMER_DIV1 | BIT_PWM0_TIMER_DIV1 | BIT_XPWM1_OUTPUT_ERROR_FLAG | 
					BIT_XPWM0_OUTPUT_PWM_TIMER0);
	  _bus->RegisterWrite(REG_PCFGR, 
	                BIT_PWM1_INVERTER_OFF | BIT_PWM1_AUTO_RELOAD | BIT_PWM1_STOP | 
	                BIT_PWM0_DEAD_ZONE_ENABLE | BIT_PWM0_INVERTER_OFF | BIT_PWM0_AUTO_RELOAD | BIT_PWM0_START);
      PWM0_SetCompareBuffer(0xffff); //Duty Cycles
  
  } else {
	  _bus->RegisterWrite(REG_PCFGR, 
	                BIT_PWM1_INVERTER_OFF | BIT_PWM1_AUTO_RELOAD | BIT_PWM1_STOP | 
	                BIT_PWM0_DEAD_ZONE_ENABLE | BIT_PWM0_INVERTER_OFF | BIT_PWM0_AUTO_RELOAD | BIT_PWM0_STOP);
  }
}


//================================================================================
//
// API
// Funcoes AVI
//
//================================================================================


/**
 * @brief 
 *
 * @param on_off: false: turn off AVI window, true :turn on AVI window
 * 
 * @note None
 */
void RA8889::AVI_Window(bool on_off)
{
  PIP1_Enable(on_off);
}


//================================================================================
//
// API
// Funções de Desenho
// 
//================================================================================


/**
 * @brief Retorna o numero de cores definidos do display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 * 
 * @return uint8_t color depth 8/16/24
 *
 */
uint8_t RA8889::getColorDepth(void) { return _bpp; }


/**
 * @brief Converte uma cor RGB565 (16 bpp) para uma paleta reduzida de 8 bpp (R3G3B2)
 *
 * @verbatim
 * Esta função reduz a profundidade de cor de 16 bits para 8 bits, mantendo
 * a fidelidade visual usando os bits mais significativos (MSBs) de cada canal.
 *
 * Estrutura do resultado (8 bits):
 * - Bits 7..5: R (3 bits)
 * - Bits 4..2: G (3 bits)
 * - Bits 1..0: B (2 bits)
 *
 * Extração dos MSBs:
 * - R: bits 15..11 (5 bits) -> pego os 3 MSBs
 * - G: bits 10..5  (6 bits) -> pego os 3 MSBs
 * - B: bits 4..0   (5 bits) -> pego os 2 MSBs
 * @endverbatim
 * 
 * @param color Cor de entrada no formato RGB565 (16 bits).
 * 
 * @return uint8_t Cor convertida em 8 bits (R3G3B2).
 *
 */
uint8_t RA8889::Color16To8bpp(uint16_t color) 
{
  // Extrai os bits mais significativos de cada canal
  uint8_t r = (color >> 13) & 0x07; // 3 MSBs de R
  uint8_t g = (color >> 8)  & 0x07; // 3 MSBs de G
  uint8_t b = (color >> 3)  & 0x03; // 2 MSBs de B
  // Junta na ordem R3G3B2
  return (r << 5) | (g << 2) | b;
}


/**
 * @brief Converte uma cor em paleta reduzida de 8 bpp para RGB565 (16 bpp).
 *
 * @verbatim
 * Conversão de 8 bpp (R3G3B2) para 16 bpp (RGB565).
 * - R (3 bits) -> 5 bits
 * - G (3 bits) -> 6 bits
 * - B (2 bits) -> 5 bits
 * 
 * Como a expansão é de menos bits para mais bits, os bits mais significativos
 * são replicados para manter a fidelidade visual aproximada.
 * @endverbatim
 * 
 * @param color8 Cor de entrada em 8 bits (formato reduzido: R3 G3 B2).
 * 
 * @return uint16_t Cor convertida em RGB565 (16 bits).
 *
 */
uint16_t RA8889::Color8To16bpp(uint8_t color8) 
{
    // Extrai os campos (R3 G3 B2)
    uint8_t r3 = (color8 >> 5) & 0x07;
    uint8_t g3 = (color8 >> 2) & 0x07;
    uint8_t b2 =  color8       & 0x03;

    // Expande para 5/6/5 bits
    uint8_t r5 = (r3 << 2) | (r3 >> 1);   // 3 -> 5 bits
    uint8_t g6 = (g3 << 3) | (g3 >> 0);   // 3 -> 6 bits
    uint8_t b5 = (b2 << 3) | (b2 << 1) | (b2 >> 1); // 2 -> 5 bits

    return (r5 << 11) | (g6 << 5) | b5;
}


/**
 * @brief Converte uma cor no formato RGB888 (24 bpp) para RGB565 (16 bpp).
 *
 * @verbatim
 * Conversão de 24 bpp (8-8-8) para 16 bpp (5-6-5).
 * - R (8 bits) -> 5 bits
 * - G (8 bits) -> 6 bits
 * - B (8 bits) -> 5 bits
 * @endverbatim
 * 
 * @param color24 Cor de entrada no formato RGB888 (0xRRGGBB).
 * 
 * @return uint16_t Cor convertida em RGB565 (16 bits).
 */
uint16_t RA8889::Color24To16bpp(uint32_t color24)
{
    uint8_t r = (color24 >> 16) & 0xFF;
    uint8_t g = (color24 >> 8)  & 0xFF;
    uint8_t b =  color24        & 0xFF;

    uint16_t r5 = (r >> 3) & 0x1F;
    uint16_t g6 = (g >> 2) & 0x3F;
    uint16_t b5 = (b >> 3) & 0x1F;

    return (r5 << 11) | (g6 << 5) | b5;
}


/**
 * @brief Converte uma cor no formato RGB565 (16 bpp) para RGB888 (24 bpp).
 *
 * @verbatim
 * Conversão de 16 bpp (5-6-5) para 24 bpp (8-8-8).
 * Os bits menos significativos são preenchidos
 * para manter uma boa aproximação visual:
 * - R (5 bits) -> 8 bits (replicando bits mais significativos)
 * - G (6 bits) -> 8 bits
 * - B (5 bits) -> 8 bits
 * @endverbatim
 * 
 * @param color16 Cor de entrada no formato RGB565 (16 bits).
 * 
 * @return uint32_t Cor convertida em RGB888 (0xRRGGBB).
 */
uint32_t RA8889::Color16To24bpp(uint16_t color16)
{
    uint8_t r5 = (color16 >> 11) & 0x1F;
    uint8_t g6 = (color16 >> 5)  & 0x3F;
    uint8_t b5 =  color16        & 0x1F;

    uint8_t r = (r5 << 3) | (r5 >> 2);
    uint8_t g = (g6 << 2) | (g6 >> 4);
    uint8_t b = (b5 << 3) | (b5 >> 2);

    return (r << 16) | (g << 8) | b;
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul
 *        
 * A profundidade de cor será a que foi definida pelo display em suas 
 * configurações
 * 
 * Color depht 8bpp/16bpp/24bpp
 *
 * @param color: entrada de dados no formato RGB
 *
 * @note None
 *       
 */
void RA8889::ForegroundColor(uint32_t color)
{
#if defined(COLOR_DEPTH_8)
  ForegroundColor8bpp(color & 0xff);
#elif defined(COLOR_DEPTH_16)
  ForegroundColor16bpp(color & 0xffff);
#elif defined(COLOR_DEPTH_24)
  ForegroundColor24bpp(color & 0xffffff);
#else
	#error "COLOR_DEPTH não definido corretamente"
#endif
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul
 *        
 * A profundidade de cor será a que foi definida pelo display em suas 
 * configurações
 * 
 * Color depht 8bpp/16bpp/24bpp
 *
 * @param color: entrada de dados no formato RGB
 *
 * @note None
 *       
 */
void RA8889::BackgroundColor(uint32_t color)
{
#if defined(COLOR_DEPTH_8)
  BackgroundColor8bpp(color & 0xff);
#elif defined(COLOR_DEPTH_16)
  BackgroundColor16bpp(color & 0xffff);
#elif defined(COLOR_DEPTH_24)
  BackgroundColor24bpp(color & 0xffffff);
#else
	#error "COLOR_DEPTH não definido corretamente"
#endif
}


/**
 * @brief Preenche o display com determinada cor
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param color Cor de fundo
 *
 * @note None
 */
void RA8889::FillScreen(uint32_t color)
{
  uint8_t mode = IsGraphicMode();   //Veja ese esta em modo grafico
  if (!mode) GraphicMode();         //Muda para modo grafico
  DrawSquare(0, 0, _displaywidth-1, _displayheight-1, color, true);
  if (!mode) {TextMode();}          //Restaura o modo anterior
}


//ajusta posicao do cursos
void RA8889::setPosCursor(uint16_t x, uint16_t y)
{
  GotoText_XY(x, y);
  _cursor_x = x;                               //Update global cursor position y variables
  _cursor_y = y;                               //Update global cursor position y variables
}


//limpa a atual ativa tela para o ultimo padrão de cores e posiciona o cursos no inicio da tela
void RA8889::ClearScreen()
{
  uint8_t mode = IsGraphicMode();              //Veja ese esta em modo grafico
  if (!mode) GraphicMode();                    //Muda para modo grafico
  DrawSquare(0, 0, _displaywidth-1, _displayheight-1, _bgcolor, true);
  TextColor(_text_fgcolor, _text_bgcolor);
  setPosCursor(_cursor_x,_cursor_y);
  if (!mode) {TextMode();}                     //Restaura o modo anterior
}


/**
 * @brief Seta a posicao do Pixel no display
 *        
 * @verbatim
 * @endverbatim
 *
 * @param (x,y):   Posicao coordenada na tela
 *
 * @note None
 */
void RA8889::SetPixelPosXY(uint16_t x, uint16_t y)
{
  GotoText_XY(x,y);
}


/**
 * @brief Seta a posicao do Pixel no display
 *        
 * @verbatim
 * @endverbatim
 *
 * @param (x,y):   Posicao coordenada na tela
 *
 * @note None
 */
void RA8889::SetPixelPos(pospixel_t pos)
{
  GotoText_XY(pos.x, pos.y);
}


/**
 * @brief Pega a posicao atual X do Pixel no display
 *        
 * @verbatim
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
uint16_t RA8889::GetPixelPosX()
{
  uint16_t pos;
  _bus->CmdWrite(REG_F_CURX0);                   //0x63, Text Write X-coordinates Register 0 (F_CURX0)
  pos = _bus->DataRead();                        //Text Write X-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURX1);                   //0x64, Text Write X-coordinates Register 1 (F_CURX1)
  pos |= (_bus->DataRead() >> 8);                //Text Write X-coordinate [12:8]
  return pos;
}


/**
 * @brief Pega a posicao atual Y do Pixel no display
 *        
 * @verbatim
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
uint16_t RA8889::GetPixelPosY()
{
  uint16_t pos;
  _bus->CmdWrite(REG_F_CURY0);                   //0x65, Text Write Y-coordinates Register 0 (F_CURY0)
  pos = _bus->DataRead();                        //Text Write Y-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURY1);                   //0x66, Text Write Y-coordinates Register 1 (F_CURY1)
  pos |= _bus->DataRead() >> 8;                  //Text Write Y-coordinate [12:8]
  return pos;
}


/**
 * @brief Pega a posicao atual X,Y do Pixel no display
 *        
 * @verbatim
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
pospixel_t RA8889::GetPixelPosXY()
{
  pospixel_t pos;
  _bus->CmdWrite(REG_F_CURX0);                   //0x63, Text Write X-coordinates Register 0 (F_CURX0)
  pos.x = _bus->DataRead();                      //Text Write X-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURX1);                   //0x64, Text Write X-coordinates Register 1 (F_CURX1)
  pos.x |= (_bus->DataRead() >> 8);              //Text Write X-coordinate [12:8]
    
  _bus->CmdWrite(REG_F_CURY0);                   //0x65, Text Write Y-coordinates Register 0 (F_CURY0)
  pos.y = _bus->DataRead();                      //Text Write Y-coordinate [7:0]
  _bus->CmdWrite(REG_F_CURY1);                   //0x66, Text Write Y-coordinates Register 1 (F_CURY1)
  pos.y |= _bus->DataRead() >> 8;                //Text Write Y-coordinate [12:8]
  
  return pos;
}


//page: número da página para setar
void RA8889::SetPage(uint8_t page)
{
  CanvasImage_StartAddr( LayerStartAddr(page) );
  CanvasImage_Width(_displaywidth);
  ActiveWindow_XY(0,0);
  ActiveWindow_WidhtHeight(_displaywidth, _displayheight);
}


//exibe uma determianda pagina
//page: número da página
void RA8889::ShowPage(uint8_t page)
{
#if defined(COLOR_DEPTH_8)
  Select_MainWindow_8bpp();
#elif defined(COLOR_DEPTH_16)
  Select_MainWindow_16bpp();
#elif defined(COLOR_DEPTH_24)
  Select_MainWindow_24bpp();
#else
	#error "COLOR_DEPTH não definido corretamente"
#endif

  MainImage_StartAddress( LayerStartAddr(page) );
  MainImage_Width(_displaywidth);
  MainWindow_StartXY(0, 0);

  CanvasImage_StartAddr( LayerStartAddr(page) );
  CanvasImage_Width(_displaywidth);
  ActiveWindow_XY(0, 0);
  ActiveWindow_WidhtHeight(_displaywidth, _displayheight);
}


//limpa a tela com a cor desejada
void RA8889::ClearCurrentPage(uint32_t color)
{
  ForegroundColor(color);                      //High level, Foreground color
  Point1_XY(0, 0);
  Point2_XY(_displaywidth-1, _displayheight-1);
  SquareMode_Start(true);
  //CoreTask_WaitReady();
}


/** Não testado
 * @brief Desenha um simples Pixel
 *        
 * @verbatim
 * Envia os bytes de cor de acordo com a configuracao anterior dos registador 
 * de formato de cor. Se for configurado registrador é 8bpp, é enviado 1 byte, 
 * 16bpp enviado 2 bytes... pelo registardor MRWDP que é um portão de entrada 
 * de dados arpa amemoria, que so passa 1 byte por vez.
 * O mecanismo itenrno do RA8889 sabe que precisa receber 1,2 ou 3 bytes para 
 * compeltar a ouperacao. Precisa setar o tipo de color depth antes. 
 * Geralemtne isso é definido na inicializacao do sistema o registrador é 
 * REG[10h] Main/PIP Window Control Register (MPWCTR) bit 3-2.
 *
 * Importante que o registrador 0x03 bit[1-0] Memory port Read/Write Destination Selection
 * esteja ajustado para 0b00 indicando Buffer de imagem (SDRAM) para dados de imagem, padrão e caracteres do usuário.
 * Nota: sempre que for alterado este bit, para casos especiais, será retornado para valor padrão 0b00. Desta forma esta funcionalidade não rpecisa ser sempre alterada apra situações comuns de escrita de pixel no display.
 * @endverbatim
 *
 * @param (x,y):   Posicao coordenada na tela
 *        color:   cor do pixel
 *                 Se color depth for 8bpp, será truncado os bits de 31-8
 *                 Se color depth for 16bpp, será truncado os bits de 31-16
 *                 Se color depth for 24bpp, será truncado os bits de 31-24
 *
 * @note 
 * O autor escreveu:
 *   this API does not support the case that MCU=16bit, 24bpp and mode 1
 *       
 * Eu escrevi:
 *   Talvez agora tenha suporte para "MCU=16bit, 24bpp and mode 1" pois o 
 *   envio é realziado em 3 bytes para o display colocando o CS em LOW 
 *   (bloqueia barramento SPI) e quando terminar o envio de 3 bytes coloca o 
 *   CS em HIGH (liberando o barramento SPI). Desta forma se cotnrola os 
 *   dados que envia pela SPI. Isso é realizado usando a funcao 
 *   SPI_DataWrite24bpp()
 */
void RA8889::PutPixel(uint16_t x,      // x of coordinate
                            uint16_t y,      // y of coordinate
                            uint32_t color   //formato 8bpp:R3G3B2, 16bpp:R5G6B5 ou 24bpp:R8G8B8
                           )
{
  GotoPixel_XY(x, y);                          //Posiciona o pixel na tela
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  Wait_WriteFIFO_NotFull();                    //Espera que a FIFO não esteja cheia de algum outro processamento anterior
  
  #if defined(COLOR_DEPTH_8)
    _bus->DataWrite8(color & 0xff);
  #elif defined(COLOR_DEPTH_16)
    _bus->DataWrite16(color & 0xffff);
  #elif defined(COLOR_DEPTH_24)
    _bus->DataWrite24(color & 0xffffff);
  #else
	#error "COLOR_DEPTH não definido corretamente"
  #endif
  
  #if USE_XNWAIT
    Wait_WriteFIFO_NotFull();                  //Espera no final do pixel ou bloco
  #endif
}


/**
 * @brief Pega a cor do pixel na determinada posição da tela da atual window
 *        
 * @param (x,y):   Posicao coordenada na tela
 *        
 * @return cor do pixel de acordo com o formato do color depth
 * 
 * @note A cor do pixel vai depender do Color Depth escolhido
 *       8bpp:  RGB332
 *       16bpp: RGB565
 *       24bpp: RGB888
 */
uint32_t RA8889::getPixel(uint16_t x, uint16_t y)
{
  uint32_t color = 0;
  GotoPixel_XY(x, y);                          //Posiciona o pixel na tela
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  _bus->DataRead();	                             //dummy read is required somehow

  Wait_WriteFIFO_NotFull();                    //Espera que a FIFO não esteja cheia de algum outro processamento anterior

  #if defined(COLOR_DEPTH_8)
    color = _bus->DataRead();
  #elif defined(COLOR_DEPTH_16)
    color = _bus->DataRead();                   //low byte color
    color |= _bus->DataRead() << 8;             //high byte color
  #elif defined(COLOR_DEPTH_24)
    color = _bus->DataRead();                   //low byte color
    color |= _bus->DataRead() << 8;             //middle byte color
    color |= _bus->DataRead() << 16;            //high byte color
  #else
	  #error "COLOR_DEPTH não definido corretamente"
  #endif

  return color;
}


//Nao testado
//transfere dados contido no buffer em ponto inicial da tela linearmente
//use uma area de janela para trasnferencia (o LVGL usa esta tecnica, tasnfere lienarmente os dados preenchendo esta area de janela apssada pelo evento flush_cb()
//use para calular num_pixels = width * height * (color_depth / 8)
void RA8889::PushBlock(uint16_t x, 
                             uint16_t y,
                             uint16_t num_pixels,      //numero total de pixels no buffer
                             const void* color_buffer  // ponteiro genérico
                          )
{
    GotoPixel_XY(x, y);           // posição inicial
    _bus->CmdWrite(REG_MRWDP);      // 0x04, Memory Data Read/Write Port

#if defined(COLOR_DEPTH_8)
    const uint8_t* p = static_cast<const uint8_t*>(color_buffer);
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        _bus->DataWrite8(p[i]);
    }

#elif defined(COLOR_DEPTH_16)
    const uint16_t* p = static_cast<const uint16_t*>(color_buffer);
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        _bus->DataWrite16(p[i]);
    }

#elif defined(COLOR_DEPTH_24)
    const uint8_t* p = static_cast<const uint8_t*>(color_buffer); // 3 bytes por pixel
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        // envia 3 bytes consecutivos
        _bus->DataWrite24(p[0] | (p[1]<<8) | (p[2]<<16));
        p += 3;
    }

#else
    #error "COLOR_DEPTH não definido corretamente"
#endif

#if USE_XNWAIT
    Wait_WriteFIFO_NotFull();
#endif
}


//Para uso com area Window definida onde a primeira posicao 0,0
//Esta funcao muito usada com o LVGL para transferir volume de dados para dentro de uma janela
//onde flush_fb ira calcular as dimensoes e passado para esta funcao transferir os pixels para o display.
//use com auto_increment = true, raramente se usa com false
/**
 * @brief Envia um bloco linear de pixels para o RA8889 dentro da janela ativa.
 *
 * @verbatim
 * Esta função envia os pixels contidos no buffer 'color_buffer' diretamente para o display.
 * 
 * Mesmo que o buffer seja linear (uma sequência contínua de pixels), não haverá distorção
 * ou esticamento da imagem, porque:
 * 1. Antes da escrita, deve-se definir a janela ativa com setWindow(x, y, width, height).
 * 2. O RA8889 incrementa automaticamente o cursor interno à medida que recebe pixels.
 * 3. Ao final de cada linha da janela, o cursor automaticamente passa para o início da próxima linha.
 *
 * Dessa forma, o buffer linear é interpretado como um bloco 2D dentro da janela, preservando
 * a forma correta da imagem.
 * @endverbatim
 *
 * @param color_buffer Ponteiro para o buffer de cores (8/16/24 bits, dependendo do COLOR_DEPTH).
 * @param num_pixels Número total de pixels a serem enviados.
 * @param auto_increment true = avança automaticamente o cursor interno do RA8889.
 */
void RA8889::WritePixels(const void* color_buffer,
                               uint32_t num_pixels,
                               bool auto_increment  // true = avança cursor, false = mantém posição (cursosr interno do display)
                              ) 
{
    GotoPixel_XY(0, 0);           // posição inicial

    _bus->CmdWrite(REG_MRWDP);      // Memory Data Read/Write Port

#if defined(COLOR_DEPTH_8)
    const uint8_t* p = static_cast<const uint8_t*>(color_buffer);
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        _bus->DataWrite8(p[i]);

        // mantém o cursor no mesmo pixel
        // precisa resetar para 0,0 ou posição inicial
        if(!auto_increment) GotoPixel_XY(0, 0);
    }

#elif defined(COLOR_DEPTH_16)
    const uint16_t* p = static_cast<const uint16_t*>(color_buffer);
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        _bus->DataWrite16(p[i]);
        if(!auto_increment) GotoPixel_XY(0, 0);
    }

#elif defined(COLOR_DEPTH_24)
    const uint8_t* p = static_cast<const uint8_t*>(color_buffer); 
    for(uint32_t i = 0; i < num_pixels; i++) {
        Wait_WriteFIFO_NotFull();
        _bus->DataWrite24(p[0] | (p[1]<<8) | (p[2]<<16));
        if(!auto_increment) GotoPixel_XY(0, 0);
        p += 3;
    }
#else
    #error "COLOR_DEPTH não definido corretamente"
#endif
}


/**
 * @brief Define a região ativa (janela) para operações gráficas no RA8889.
 *
 * @verbatim
 * A função configura a janela ativa do display RA8889, determinando a área 
 * retangular (X, Y, largura e altura) onde os pixels subsequentes serão escritos.
 * 
 * Internamente:
 *  - Chama ActiveWindow_XY(x, y) para definir o canto superior esquerdo.
 *  - Chama ActiveWindow_WidhtHeight(width, height) para definir largura e altura.
 *
 * Após a configuração da janela, operações de escrita de pixels (ex: WritePixels) 
 * afetam apenas essa região. É amplamente utilizada em conjunto com bibliotecas 
 * gráficas como LVGL para atualização parcial da tela.
 * @endverbatim
 *
 * @param x Coordenada X inicial (canto superior esquerdo da janela) (inicia valor 0...).
 * @param y Coordenada Y inicial (canto superior esquerdo da janela) (inicia valor 0...).
 * @param width Largura da janela em pixels (inicia valor 1...).
 * @param height Altura da janela em pixels (inicia valor 1...).
 *
 * @note 
 * - Certifique-se de que (x + width) e (y + height) não ultrapassem os limites 
 *   máximos suportados pelo painel (8188 para X, 8191 para Y).
 * - A janela definida permanece ativa até que outra seja configurada.
 */
void RA8889::setWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  ActiveWindow_XY(x, y);
  ActiveWindow_WidhtHeight(width, height); 
}


/**
 * @brief Desenha um Simples Pixel
 *        
 * Use o formato de cores RGB de acordo com o Color Depth escolhido
 * 
 * @param (x,y):   Posicao coordenada na tela
 *        color:   cor do pixel
 *
 * @note A cor do pixel vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{  
  PutPixel(x, y, color);
}


//testado em colordepth 16
//nao testado em colordepth 24
//nao testado em colordepth 8
/** 
 * @brief Draw a series of pixels
 *        
 * Use o formato de cores RGB de acordo com o Color Depth escolhido
 * 
 * @param (x,y):   Posicao coordenada na tela
 *        data:    an array of 8/16/24bit colors (pixels)
 *        num_pixels:   how many pixels
 * @note A cor do pixel vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawPixels(uint16_t x, uint16_t y, uint32_t num_pixels, uint16_t *data)
{  
  GotoPixel_XY(x, y);
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  Wait_WriteFIFO_NotFull();                    //Espera que a FIFO não esteja cheia de algum outro processamento anterior

  #ifdef COLOR_DEPTH_16
    for (uint32_t i = 0; i < num_pixels; i++) {                    //y
      _bus->DataWrite16((uint16_t)*data);
      data++;                                       //incrementa o potneiro 2 bytes para a poxima posição 
    }
  #endif
  
  #ifdef COLOR_DEPTH_24
    uint32_t color = 0;
    uint8_t *data8 = (uint8_t*) data;
    for (uint32_t i = 0; i < num_pixels; i++) {                    //y
      // lê 3 bytes consecutivos e monta uint32_t (24 bits)
      uint32_t tmpcolor  = ((uint32_t)data8[0])       // byte 0 → bits 0-7
                         | (((uint32_t)data8[1]) << 8)  // byte 1 → bits 8-15
                         | (((uint32_t)data8[2]) << 16); // byte 2 → bits 16-23
      
      _bus->DataWrite24(tmpcolor);

      data8+= 3;                              //incrementa o potneiro 3 bytes para a poxima posição
    }
  #endif
  Wait_WriteFIFO_Empty();
}


/**
 * @brief Desenha uma linha
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1)  :   primeiro ponto de coordenada na tela
 *        (x2,y2)  :   segundo ponto de coordenada na tela
 *        forecolor:   cor da linha
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void RA8889::DrawLine(uint16_t x1,
                            uint16_t y1,
                            uint16_t x2,
                            uint16_t y2,
                            uint32_t color
                           )
{
  if ((x1 == x2 && y1 == y2)) {
    DrawPixel(x1, y1, color);
    return;
  }
  ForegroundColor(color);                      //High level, Foreground color
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  LineMode_Start();
  CoreTask_WaitReady();
}


/**
 * @brief Desenha um retangulo/quadrado
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        forecolor: cor de frente de preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void RA8889::DrawSquare(uint16_t x1,
                              uint16_t y1,
                              uint16_t x2,
                              uint16_t y2,
                              uint32_t forecolor,
                              bool bfill
                             )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  SquareMode_Start(bfill);
  //CoreTask_WaitReady();
}


/**
 * @brief Desenha um triangulo
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        (x3,y3):   terceiro ponto de coordenada na tela
 *        forecolor: cor da linha/preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawTriangle(uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2,
                                uint16_t x3,
                                uint16_t y3,
                                uint32_t forecolor,
                                bool bfill
                               )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Point3_XY(x3, y3);
  TriangleMode_Start(bfill);
  //CoreTask_WaitReady();  
}


/**
 * @brief Desenha um circulo
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central do circulo
 *        R:         raio do circulo 
 *        forecolor: cor de frente de preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void RA8889::DrawCircle (uint16_t x1,
                               uint16_t y1,
                               uint16_t R,
                               uint32_t forecolor,
                               bool bfill
                              )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1,y1);
  Radius_RxRy(R, R);
  CircleMode_Start(bfill);
  //CoreTask_WaitReady();  
}


/**
 * @brief Desenha uma elipse
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da elispse
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawEllipse (uint16_t x1,
                                uint16_t y1,
                                uint16_t Rx,
                                uint16_t Ry,
                                uint32_t forecolor,
                                bool bfill
                               )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  EllipseMode_Start(bfill);
  //CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura esquerda/superior
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void RA8889::DrawCurveLeftUp(uint16_t x1,
                                   uint16_t y1,
                                   uint16_t Rx,
                                   uint16_t Ry,
                                   uint32_t forecolor,
                                   bool bfill
                                  )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftUpMode_Start(bfill);
  //CoreTask_WaitReady();  
}


/**
 * @brief Desenha uma curva com curvatura direita/inferior
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void RA8889::DrawCurveRightDown(uint16_t x1,
                                      uint16_t y1,
                                      uint16_t Rx,
                                      uint16_t Ry,
                                      uint32_t forecolor,
                                      bool bfill
                                     )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightDownMode_Start(bfill);
  //CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura direita/superior
 *
 * @verbatim
 *        Color depht de 16bpp
 * @endverbatim
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawCurveRightUp(uint16_t x1,
                                    uint16_t y1,
                                    uint16_t Rx,
                                    uint16_t Ry,
                                    uint32_t forecolor,
                                    bool bfill
                                   )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightUpMode_Start(bfill);
  //CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura esquerda/inferior
 *        
 * @verbatim
 *        Color depht de 16bpp
 * @endverbatim
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawCurveLeftDown(uint16_t x1,
                                     uint16_t y1,
                                     uint16_t Rx,
                                     uint16_t Ry,
                                     uint32_t forecolor,
                                     bool bfill
                                    )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftDownMode_Start(bfill); 
  //CoreTask_WaitReady();
}


/**
 * @brief Desenha um quadrado com arestas arredondadas
 *        
 * @verbatim
 *        Color depht de 16bpp
 * @endverbatim
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha/preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void RA8889::DrawCircleSquare(uint16_t x1,
                                    uint16_t y1,
                                    uint16_t x2,
                                    uint16_t y2,
                                    uint16_t Rx,
                                    uint16_t Ry,
                                    uint32_t forecolor,
                                    bool bfill
                                   )
{
  ForegroundColor(forecolor);                  //High level, Foreground color
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Radius_RxRy(Rx, Ry);
  CircleSquareMode_Start(bfill);
  //CoreTask_WaitReady();
}


/** Não Testado
 * @brief Mostra uma figura
 *        
 * @verbatim
 * Color depht de 16bpp (2 bytes por pixel)
 * Uso apenas com buffer criado por PROGGMEM
 *
 * Deve se lenvar em consideração a profundicade de cores 8/16/24bpp na matriz da figura
 * O tamanho da matriz com uma figura de 80 x 80 profundidade de cores de 16bpp será:
 * Size: Color Depth 16: 80 * 80 * (16 / 8) = 12.800 bytes
 *       Color Depth 24: 80 * 80 * (24 / 8) = 19.200 bytes
 * @endverbatim
 *
 * @param size:   tamanho de bytes da figura da matriz com color depth
 *        *datap  ponteiro para dados no PROGMEM
 *       
 * @note A cor do pixel vai depender do Color Depth definido como padrão para o dsiplay
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *       Para uso com outro formato de matriz de cores de figura, deve converter para o padrão 
 *       definido e suportado pelas configurações do display.
 * Exemplo: const uint8_t pic_80x80[] PROGMEM ={0X19,0X88,0X09,0X48,0X11,0XD2,0X01,...}
 *          ShowPicture(80*80*(16/2), pic_80x80);
 *
 */
void RA8889::ShowPicturePgm(uint32_t size, const uint8_t *datap)
{   
  uint32_t i;
  _bus->CmdWrite(REG_MRWDP);                            //0x04, Memory Data Read/Write Port (MRWDP)
#if defined(COLOR_DEPTH_8)
  while(i < size)
  {
    _bus->DataWrite(pgm_read_byte(&datap[i++]));
    _bus->DataWrite(pgm_read_byte(&datap[i++]));
    Wait_WriteFIFO_NotFull();
  }
#elif defined(COLOR_DEPTH_16)
  for(i=0; i < size; i+=2) {                          //total_bytes = tamanho da image * (byte_per_pixel/8)
	//declare Arduino.h, para usar a funcao pgm_read_byte()
    _bus->DataWrite( pgm_read_byte(&datap[i+1]) );      //Envia cada byte declarados em PROGMEM byte posterior
    _bus->DataWrite(pgm_read_byte(&datap[i]));          //Envia cada byte declarados em PROGMEM byte anterior
    Wait_WriteFIFO_NotFull();
  }
#elif defined(COLOR_DEPTH_24)
  for(i=0; i < size; i+=3) {                          //total_bytes = tamanho da image * (byte_per_pixel/8)
	//declare Arduino.h, para usar a funcao pgm_read_byte()
    _bus->DataWrite( pgm_read_byte(&datap[i+2]) );      //Envia cada byte declarados em PROGMEM byte posterior
    _bus->DataWrite( pgm_read_byte(&datap[i+1]) );      //Envia cada byte declarados em PROGMEM byte posterior
    _bus->DataWrite( pgm_read_byte(&datap[i])   );      //Envia cada byte declarados em PROGMEM byte anterior
    Wait_WriteFIFO_NotFull();
  }
#else
	#error "COLOR_DEPTH não definido corretamente"
#endif
}


/** Não Testado
 * @brief Draw Picture
 *        
 * @verbatim
 * Color depht de 16bpp (2 bytes por pixel)
 * Uso apenas por buffer criado com PROGMEM
 * @endverbatim
 *
 * @param (Wx,Hy):         coordenada na tela
 *        (width, height): dimensão da figura
 *        *datap           Ponteiro para dados no PROGMEM
 *
 * @note Exemplo: const uint8_t pic_80x80[] PROGMEM ={0X19,0X88,0X09,0X48,0X11,0XD2,0X01,...}
 *                DrawPicture(0, 0, 80, 80, pic_80x80);
 *
 */
void RA8889::DrawPicturePgm(uint16_t Wx, uint16_t Hy, uint16_t width, uint16_t height, const uint8_t *datap)
{
  ActiveWindow_XY(Wx, Hy);
  ActiveWindow_WidhtHeight(width, height);
  GotoPixel_XY(Wx, Hy);

  uint32_t size = width * height * (_bpp / 8);      //valor de _bpp é definido no sistema de acordo com o colordepth escolhido apra o display
  ShowPicturePgm(size, datap);

  ActiveWindow_XY(0,0);
  ActiveWindow_WidhtHeight(_displaywidth, _displayheight);
}




/** Não Testado
 * @brief Mostra uma figura
 *        
 * @verbatim
 * Deve se lenvar em consideração a profundicade de cores 8/16/24bpp na matriz da figura
 * O tamanho da matriz com uma figura de 80 x 80 profundidade de cores de 16bpp será:
 * Size: Color Depth 16: 80 * 80 * (16 / 8) = 12.800 bytes
 *       Color Depth 24: 80 * 80 * (24 / 8) = 19.200 bytes
 * @endverbatim
 *
 * @param numpixels:  numero de pixels total no buffer
 *        *datap:     ponteiro para dados no buffer
 *        pictureBpp: Color Depth do pixel no buffer
 * @note A cor do pixel vai depender do Color Depth do pixel no buffer
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 */
void RA8889::ShowPicture(eColorDepthBPP pictureBpp, uint32_t numpixels, const uint8_t *datap)
{   
  uint32_t i;
  _bus->CmdWrite(REG_MRWDP);                            //0x04, Memory Data Read/Write Port (MRWDP)
  if (pictureBpp == eColorDepthBPP::bpp8) {
    while(i < numpixels)
    {
      _bus->DataWrite(datap[i++]);
      _bus->DataWrite(datap[i++]);
      Wait_WriteFIFO_NotFull();
    }
  } else if (pictureBpp == eColorDepthBPP::bpp16) {
    for(i=0; i < numpixels; i+=2) {                     //total_bytes = tamanho da image * (byte_per_pixel/8)
      _bus->DataWrite(datap[i+1]);                        //Parte baixa no byte seguinte
      _bus->DataWrite(datap[i]);                          //Parte alta no atual byte 
      Wait_WriteFIFO_NotFull();
    }
  } else if (pictureBpp == eColorDepthBPP::bpp24) {
    for(i=0; i < numpixels; i+=3) {                     //total_bytes = tamanho da image * (byte_per_pixel/8)
      _bus->DataWrite(datap[i+2]);                        //Parte bits baixos [7:0] 
      _bus->DataWrite(datap[i+1]);                        //Parte bits central [15:8]  
      _bus->DataWrite(datap[i]);                          //Parte bits altos [23:16]
      Wait_WriteFIFO_NotFull();
    }
  }

}


//Não testado
void RA8889::DrawBitmap(uint8_t *pixels, eColorDepthBPP pictureBpp, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  ActiveWindow_XY(x, y);
  ActiveWindow_WidhtHeight(w, h);
  GotoPixel_XY(x, y);

  Memory_ColorDepth_BlockMode(pictureBpp);
  ShowPicture(pictureBpp, w*h, pixels);

  ActiveWindow_XY(0,0);
  ActiveWindow_WidhtHeight(_displaywidth, _displayheight);
}


//================================================================================
//
// API
// Funções Font
// 
//================================================================================


/**
 * @brief Inicializa a fonte primaria (interna) do display
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 */
void RA8889::Font_Init(void)
{
  _fntparam_source_select = eFontSource::InternalCGROM; 
  Font_SetSource(_fntparam_source_select);

  //Nota: CGROM Internal, suporta apenas format font 12x24 (default)
  _fntparam_size_select = eFontHeight::H24;
  _fntparam_height = static_cast<uint8_t>(_fntparam_size_select);
  Font_SetHeight_24();

  _fntparam_full_align = false;
  Font_FullAlignmentDisable();

  _fntparam_chroma_key = false;           //use backgraound color
  Font_UseBackgroundColor();

  _fntparam_width_enlarge = eFontEnlargFactor::X1;
  Font_WidthEnlargFactor(_fntparam_width_enlarge);

  _fntparam_height_enlarge = eFontEnlargFactor::X1;
  Font_HeightEnlargFactor(_fntparam_height_enlarge);

  //se setoru itnernal em FontParameters::eFontSource::
  _fntparam_intern_charset_select = eInternalCharSet::ISO8859_1;
  Select_Internal_CGROM_ISOIEC8859_1();
  
  Font_0degree();
}


/**
 * @brief Seta cor do texto
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param fgcolor
 * @param bgcolor
 *
 * @note None
 *
 */
void RA8889::TextColor(uint32_t fgcolor, uint32_t bgcolor)
{ 
  CoreTask_WaitReady();
//  uint8_t mode = IsGraphicMode();              //Veja ese esta em modo grafico
//  if (mode) TextMode();                        //Modo texto
  uint8_t mode = TextMode();                   //Modo texto  
  ForegroundColor(fgcolor);                    //High level, Foreground color
  BackgroundColor(bgcolor);                    //High level, Background color
  _text_bgcolor = bgcolor;
  _text_fgcolor = fgcolor;
  //if (mode) {GraphicMode();}                   //Restaura o modo anterior
  if (!mode) {GraphicMode();}                   //Restaura se estava em modo grafico
}


/** OK
 * @brief Envia texto para porta de entrada da memória do display
 *        
 * @verbatim
 * Para o uso desta funcao necessário esta no modo texto do display
 * apos terminar passar apora o modo grafico quando convier.
 * precisa estar no modo texto antes de impreimir caracter no display
 * Color depht de 16bpp (2 bytes por pixel)
 * @endverbatim
 *
 * @param *str Ponteiro de string terminado com zero
 *
 * @note None
 *
 */
void RA8889::ShowText(char *str)
{  
  _bus->CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  while(*str != '\0') {                        //Até final de string
    _bus->DataWrite(*str);                       //envia de um em um caracter
    Wait_WriteFIFO_NotFull();                  //
    ++str;                                     //proximo caracter para imprimir no display
  }
  CoreTask_WaitReady();                        //Precisa estar aqui!! Senão display bagunça
}


/**
 * @brief Imprime texto na tela
 *        
 * @verbatim
 * Precisa usar antes as funçções:
 *   Font_UseInternalCGROM() / Font_UseExternalCGROM()
 *   Font_SetHeight_16() / Font_SetHeight_24() / Font_SetHeight_32() 
 * @endverbatim
 *
 * @param *str      Ponteiro de string terminado com zero
 * @param (x,y)     coordenadas em pixel
 * @param foregcolor cor de frente do texto
 * @param backgcolor cor de fundo do texto
 * 
 * @bug
 * Se a primeira fonte for grande e imimir ela novamente na tela, fica com residuos por baixo.
 * Acho que precisa criar um window para cada string sendo impressa no display. PAra isso deve levar em consideração a altura da fonte, largura da fonte, espaçamento e e largura da string total a ser impressa na tela
 *
 * @note support ra8889 internal font and external string font code write from data pointer
 *
 */
void RA8889::Text(uint16_t x, uint16_t y, char *str, uint32_t foregcolor, uint32_t backgcolor)
{
  uint8_t mode = TextMode();                   //Modo texto 
  ForegroundColor(foregcolor);                 //High level, Foreground color
  BackgroundColor(backgcolor);                 //High level, Background color
  GotoText_XY(x, y);                           //posiciona o texto
  ShowText(str);                               //Envia caracterres para o portão de entrada da memoria
  if (!mode) GraphicMode();                    //Restaura o modo anterior se for grafico
}


//Não testado
//Escolhe a Origem da Fonte. Somente após a execução deste método que as configuracoes de fontes entrarão em vigor
//ou permite a troca do uso de fontes na ROM, Interno ou contruidos por matriz de pixel do usuário.
//Se é a primeira vez que esta configurando os aprametros de fonte o ideal é excutar este método após a configuração.
//Se as fotnes já estão configuradas da ROM, Interno ou usuário, então este método é utilziado apra permutar entre as diversas origens de fontes.
//eFontOrigin::User Fonte criado pelo usuario
//eFontOrigin::External Fonte fonte de origem do CGROM
//eFontOrigin::Internal Fonte de origem do RA8889 padrão
void RA8889::setFontSource(eFontSource source)
{
  if (_fntparam_source_select == source) return;
  if (source == eFontSource::InternalCGROM) Font_UseInternalCGROM();
  if (source == eFontSource::ExternalCGROM) Font_UseExternalCGROM();
  if (source == eFontSource::UserDefined)   Font_UseUserDefined();
  _fntparam_source_select = source;
}


/** Em construção
 * @brief Seta as configurações para fonte de usuário
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param enable                : opcao que ao excutar esta funcao ja muda e ativa a fonte externa da Flash ROM sem necessidade de uso de setFontUser(). Por padrão vem desabilitado
 * @param FontUserParam param   : parâmetros
 *
 * @todo None
 *
 * @note None
 *
 */
void RA8889::setFontUser(FontUserParam param, bool enable)
{
//compeltar... codigo aqui faltando

  //Character Control Register 0 (CCR0) [0xcc]
    
  if ((_fntparam_source_select != eFontSource::UserDefined) && enable){
    setFontSource(eFontSource::UserDefined);
  }
}


/** OK
 * @brief Seta as configurações para fonte externa da Flash ROM
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param enable                  : opcao que ao excutar esta funcao ja muda e ativa a fonte externa da Flash ROM sem necessidade de uso de setFontUser(). Por padrão vem desabilitado
 * @param FontExternalParam param : parâmetros
 *        width_set:   
 *           eExternalCharWidthSet::Fixed              BIT_GT_FIXED_WIDTH                0x00 Fixed width
 *           eExternalCharWidthSet::VariableArial      BIT_GT_VARIABLE_WIDTH_ARIAL       0x01 Variable width for Arial
 *           eExternalCharWidthSet::VariableFixedRoman BIT_GT_VARIABLE_FIXED_WIDTH_ROMAN 0x02 Variable and fixed width for Roman
 *           eExternalCharWidthSet::Bold               BIT_GT_BOLD                       0x03 Bold
 *
 * @todo None
 *
 * @note Essa função se utiliza das configurações realizadas para fonte 
 *       interna setFontInternal(). A fonte por padrão do sistema vem 
 *       configurada como tamanho 8x16, ISOIEC8859-1, com chromakey 
 *       desabilitado.
 */
void RA8889::setFontExternal(FontExternalParam param, bool enable)
{
  
  GTFont_CharacterParameter(param.scs_select,
                            BIT_SPI_DIV4, 
                            GTSERIAL_CGROM,
                            static_cast<uint8_t>(param.charset_select),
                            static_cast<uint8_t>(param.gt_width)                   
                           );
	
  //Character Control Register 0 (CCR0) [0xcc]
    
  if ((_fntparam_source_select != eFontSource::ExternalCGROM) && enable){
    setFontSource(eFontSource::ExternalCGROM);
  }

}


/** OK
 * @brief Seta as configurações para fonte CGROM interna do RA8889
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param enable                  :  opcao que ao excutar esta funcao ja muda e ativa a fonte inrna sem necessidade de uso de setFontUser(). Por padrão vem desabilitado
 * @param FontInternalParam param : parâmetros
 *
 * @todo None
 *
 * @note A fonte por padrão do sistema vem configurada como tamanho 12x24, 
 *       ISOIEC8859-1, com chromakey desabilitado. Não suporta outros valores
 *       de height de fonte interna.
 */
void RA8889::setFontInternal(FontInternalParam param, bool enable)
{
  uint8_t temp = 0;

  //Character Control Register 0 (CCR0) [0xcc]

  if (_fntparam_intern_charset_select != param.charset_select) {
    if (param.charset_select == eInternalCharSet::ISO8859_1) Select_Internal_CGROM_ISOIEC8859_1();
    if (param.charset_select == eInternalCharSet::ISO8859_2) Select_Internal_CGROM_ISOIEC8859_2();
    if (param.charset_select == eInternalCharSet::ISO8859_4) Select_Internal_CGROM_ISOIEC8859_4();
    if (param.charset_select == eInternalCharSet::ISO8859_5) Select_Internal_CGROM_ISOIEC8859_5();
    _fntparam_intern_charset_select = param.charset_select;
 }

  //Nota: CGROM Internal, suporta apenas format font 12x24 (default)
  _fntparam_size_select = eFontHeight::H24;
  _fntparam_height = static_cast<uint8_t>(_fntparam_size_select);
  Font_SetHeight_24();

  //Character Control Register 1 (CCR1) [0xcd]

  if ( _fntparam_full_align != param.full_align) {
   if (param.full_align) {Font_FullAlignmentEnable(); } else {  Font_FullAlignmentDisable(); }
    _fntparam_full_align = param.full_align;
  }

  if (_fntparam_chroma_key != param.chroma_key)  {
    if (_fntparam_chroma_key) { Font_UseBackgroundTransparency(); } else { Font_UseBackgroundColor(); }
    _fntparam_chroma_key =  param.chroma_key;
  }

  if (_fntparam_width_enlarge != param.width_enlarge)  {
    Font_WidthEnlargFactor(_fntparam_width_enlarge);
    _fntparam_width_enlarge = param.width_enlarge;
  }

  if (_fntparam_height_enlarge != param.height_enlarge)  {
    Font_HeightEnlargFactor(_fntparam_height_enlarge);
    _fntparam_height_enlarge = param.height_enlarge;
  }

  //Character Control Register 0 (CCR0) [0xcc]
    
  if ((_fntparam_source_select != eFontSource::InternalCGROM) && enable){
    setFontSource(eFontSource::InternalCGROM);
  }

}


/**
 * @brief Imprime texto na tela
 *        
 * @verbatim
 * Precisa usar antes as funções:
 *   Font_UseInternalCGROM() / Font_UseExternalCGROM()
 *   Font_SetHeight_16() / Font_SetHeight_24() / Font_SetHeight_32() 
 *   Definir foreground color e background color
 * @endverbatim
 *
 * @param *str      Ponteiro de string terminado com zero
 * @param (x,y)     coordenadas em pixel
 * 
 * @note support ra8889 internal font and external string font code write from data pointer
 *
 */
void RA8889::PutString(uint16_t x, uint16_t y, char *str)
{
  uint8_t mode = TextMode();       //Modo texto 
  GotoText_XY(x, y);               //posiciona o texto
  ShowText(str);                   //Envia caracterres ASCII, 1 byte, para o portão de entrada da memoria
  if (!mode) {GraphicMode();}      //Restaura o modo anterior se for grafico
}


//nao testado
//imprime caractrer ASCII/UNICODE a partir da Genitop ROM Flash ROM #0
//na atual janela ativa
//*str Unicode Font String which you want print on LCD (L"string" in keil c is Unicode string)
//     example: PutUnicodeString(0,0,clAquaGreen,clDenimDarkBlue,L"穝λ郡λカじ刁8腹6加ぇ5");
//return
//  true: fonte está presente na Genitop

bool RA8889::PutUnicodeString(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, char *str)
{
  bool res = false;

  //Olhe para o primeiro caracter e verificas o mapa de bits de presença de fonte
  //ASCII imprimiveis 32 até 127
  //ASCII extendido, padrao ISO 8859-1 de 128 até 255
  if((*str)>=0x0020 && (*str)<0x0080) {
    res = CheckFontExternal(BIT_ASCII);
  } else {
	res = CheckFontExternal(BIT_UNICODE);
  }
  if (!res) return false;
  
  SFI_Select_FontMode();
  Font_SetSource(eFontSource::ExternalCGROM);
  SPI_Clock_Period(_display_spi_clk_divider);
  
  SFI_Select_24bitAddress();
  SFI_Select_WaveformMode3();
  Select_SFI_SingleData_03h();
  
  //valor _font_rom_scs = 0 condigurado no sistema
  if (_fnt_rom_scs == 0) SFI_SelectROM_CS0();
  if (_fnt_rom_scs == 1) SFI_SelectROM_CS1();
  if (_fnt_rom_scs == 2) SFI_SelectROM_CS2();  
  if (_fnt_rom_scs == 3) SFI_SelectROM_CS3();

  //valor _font_dma_bus = 0 condigurado no sistema
  if (_fnt_dma_bus == 0) Font_DMA_Select_Bus0();
  if (_fnt_dma_bus == 1) Font_DMA_Select_Bus1();

  uint8_t mode = TextMode();                   //Modo texto
  
  ForegroundColor(fgcolor);                    //High level, Foreground color
  BackgroundColor(bgcolor);                    //High level, Background color

  while(*str != '\0')
  {
    if((*str)>=0x0020 && (*str)<0x0080) {
	   /* ASCII Code*/
       GTFont_SetDecoder(BIT_ASCII | BIT_GT_VARIABLE_WIDTH_ARIAL); 
       _bus->CmdWrite(REG_MRWDP);                //Memory Data Read/Write Port (MRWDP) 
       _bus->DataWrite(*str);
       CoreTask_WaitReady();
    } else {
       /* Unicode */
  	   GTFont_SetDecoder(BIT_UNICODE);         //
       _bus->CmdWrite(REG_MRWDP);                //Memory Data Read/Write Port (MRWDP) 
       _bus->DataWrite((*str)>>8); 
       _bus->DataWrite(*str);
       CoreTask_WaitReady();
    }
  	++str;
  }  

  if (!mode) GraphicMode();                    //Restaura o modo grafico
  return true;
}


/**
 * @brief Char of the dimension 8x12
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param code           codigo do caracter
 *
 * @note None
 *
 */
void RA8889::PutChar8x12(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code)
{ 
  uint16_t i = 0;
  uint16_t j = 0;
  uint8_t tmpchar = 0;

  for (i=0; i<12; i++) {

    tmpchar = ascii_table_8x12[((code-0x20)*12)+i];    //minus 32 offset, because this table from ascii table "space"

    for (j=0; j<8; j++) {
      if ( (tmpchar >> 7-j) & 0x01 == 0x01 ) { 
        #if defined (COLOR_DEPTH_16)
          PutPixel(x + j, y + i, fgcolor);
        #elif defined (COLOR_DEPTH_24)
          PutPixel(x + j, y + i, fgcolor);
        #endif
      } else {   
        if(!bgtransparent) {
          #if defined (COLOR_DEPTH_16)
            PutPixel(x + j, y + i, bgcolor);
          #elif defined (COLOR_DEPTH_24)
	          PutPixel(x + j, y + i, bgcolor);
          #endif
        }
      } 
    }

  }
}


/**
 * @brief String with char 8x12 dimension
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param *ptr           Ponteiro para string com null terminado caracter
 *
 * @code
 * PutString8x12(0, 0, 0xFFFF, 0x0000, true, "!\"#$%&'()*+,-./012345678");
 * @endcode
 *
 * @note None
 *
 */
void RA8889::PutString8x12(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr)
{
  uint16_t i = 0;

  //screen width = 800,  800/8 = 100 
  //if string more then 100 fonts, no show
  while ((*ptr != 0) & (i < 100))
  {
    PutChar8x12(x, y, fgcolor, bgcolor, bgtransparent, *ptr);
    x += 8;
    ptr++;
    i++;  
  }
}


/**
 * @brief Char of the dimension 16x24
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param code           codigo do caracter
 *
 * @note None
 *
 */
void RA8889::PutChar16x24(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code)
{ 

  uint16_t i = 0;
  uint16_t j = 0;
  uint8_t tmpchar = 0;
  uint32_t array_addr = 0;

  for (i=0; i<24; i++) {
    
	//minus 32 offset, because this table from ascii table "space"  
    array_addr = ((code-0x20)*2*24)+(i*2); 
    tmpchar = ascii_table_16x24[array_addr]<<8 | ascii_table_16x24[array_addr+1];
	
    for (j=0; j<16; j++) {
      if ( (tmpchar >> 15-j) & 0x01 == 0x01) { 
        #if defined(COLOR_DEPTH_16)
          PutPixel(x + j, y + i, fgcolor);
        #elif defined(COLOR_DEPTH_24)
          PutPixel(x + j, y + i, fgcolor);
        #endif
      } else {
        if(!bgtransparent) {
          #if defined(COLOR_DEPTH_16)
            PutPixel(x + j, y + i, bgcolor);
          #elif defined(COLOR_DEPTH_24)
            PutPixel(x + j, y + i, bgcolor);
          #endif
        }
      }
    }
    
  }
}


/**
 * @brief String with char 16x24 dimension
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param *ptr           Ponteiro para string com null terminado caracter
 *
 * @code
 * PutString16x24(0, 0, clRed, clBlack, true, "!\"#$%&'()*+,-./012345678");
 * @endcode
 *
 * @note None
 *
 */
void RA8889::PutString16x24(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr)
{
  uint16_t i = 0;
  
  //screen width = 800,  800/16 = 50 
  //if string more then 50 fonts, no show
  while ((*ptr != 0) & (i < 50))
  {
    PutChar16x24(x, y, fgcolor, bgcolor, bgtransparent, *ptr);
    x += 16;
    ptr++;
    i++;  
  }
}


/**
 * @brief Char of the dimension 32x48
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param code           codigo do caracter
 *
 * @note None
 *
 */
void RA8889::PutChar32x48(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code)
{ 

  uint16_t i = 0;
  uint16_t j = 0;
  uint8_t tmpchar = 0;
  uint32_t array_addr = 0;

  for (i=0; i<48; i++) {
    
    //minus 32 offset, because this table from ascii table "space"  
    array_addr = ((code-0x20)*4*48)+(i*4); 
    tmpchar = ascii_table_32x48[array_addr]<<24 | ascii_table_32x48[array_addr+1]<<16 | ascii_table_32x48[array_addr+2]<<8 | ascii_table_32x48[array_addr+3];
    
    for (j=0; j<32; j++) {
    
      if ( (tmpchar >> (31-j)) & 0x01 == 0x01) {  
        #if defined(COLOR_DEPTH_16)
          PutPixel(x + j, y + i, fgcolor);
        #elif defined(COLOR_DEPTH_24)
          PutPixel(x + j, y + i, fgcolor);
        #endif
      } else {
        if(!bgtransparent) {
          #if defined(COLOR_DEPTH_16)
            PutPixel(x + j, y + i, bgcolor);
          #elif defined(COLOR_DEPTH_24)
            PutPixel(x + j, y + i, bgcolor);
          #endif
        }
      }
    } 
    
  }
}


/**
 * @brief String with char 32x48 dimension
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param (x,y)          coordenadas em pixel da tela
 * @param fgcolor        cor de frente do texto
 * @param bgcolor        cor de fundo do texto
 * @param bg_transparent cor transparente com o fundo
 * @param *ptr           Ponteiro para string com null terminado caracter
 *
 * @code
 * PutString32x48(0, 0, clRed, clBlack, true, "!\"#$%&'()*+,-./012345678");
 * @endcode
 *
 * @note None
 *
 */
void RA8889::PutString32x48(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr)
{
  uint16_t i = 0;
  
  //screen width = 800,  800/32 = 25 
  //if string more then 25 fonts, no show
  while ((*ptr != 0) & (i < 25)) {
    PutChar32x48(x, y, fgcolor, bgcolor, bgtransparent, *ptr);
    x += 32;
    ptr++;
    i++;
  }
}


/**
 * @brief Put double precision value format base on sprintf()
 *        
 * @verbatim
 * Tamanho máximo de 32 caracteres o buffer interno
 *
 *  Value Float:    
 *    Arduino Floats have only 6-7 decimal digits of precision. That means the
 *    total number of digits, not the number to the right of the decimal point. 
 *    Unlike other platforms, where you can get more precision by using a 
 *    double (e.g. up to 15 digits), on the Arduino, double is the same size as 
 *    float.
 *    vaule:     (3.4E-38) ~ (3.4E38)
 *    len:       1~11 minimum output length
 *    precision: right side of point numbers 1~4 
 *    joker:
 *      n: Alinhar à direita
 *      -: Alinhar à esquerda
 *      +: Exibir um sinal
 *      (espaço): Exibir um espaço quando nenhum sinal for exibido
 *      #: Forçar ponto decimal
 *      0: Preencher o número inicial (esquerdo) com zeros em vez de espaços.
 *    Example:
 *      "%3.1f"
 *      "%-1.4f"
 *      "%02.3f"
 *      "%12.3e" de +0.000123 retrona "  -1.230e-04"
 *       %e → sem largura (sem espaço)
 *       %+e → sempre mostra sinal
 *       %0Xe → com zero padding, ex: %010.3e
 * Logo, para -0.000123:
 * - %12.3e (alinhamento à direita) → " -1.230e-04" (2 espaços antes)
 *  - %-12.3e (alinhamento à esquerda) → "-1.230e-04 " (2 espaços depois)
 *       
 * @endverbatim
 *
 * @param fmt : formato de string para exibição - Mascara de Formatação (Identico a mascara de sprintf() 
 *
 * @note None
 *
 */
void RA8889::PutFloat(uint16_t x, uint16_t y, double value, const char *fmt)
{
  char buffer[32];

  #if defined(ARDUINO_ARCH_AVR)
    formatDoubleAdvanced(buffer, value, fmt);
  #elif defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ESP32) || defined(ESP8266)
    // Arduino Due (SAM), Zero (SAMD), ESP32, ESP8266: têm suporte a float no sprintf
    sprintf(buffer, fmt, value);  // 2 casas decimais
  #else
    sprintf(buffer, fmt, value);
  #endif  

  PutString(x, y, buffer);
}


/**
 * @brief Put decimal value format base on sprintf()
 *        
 * @verbatim
 * Tamanho máximo de 12 caracteres o buffer interno
 * 
 * Value Decimal: 
 *   value: -2147483648(-2^31) ~ 2147483647(2^31-1)
 *   len:   1~11 minimum output length
 *   Joker:
 *     n: Direita
 *     -: Esquerda
 *     +: Exibe um sinal
 *     (espaço): Exibe um espaço quando nenhum sinal é exibido
 *     0: Preenche a esquerda com 0s em vez de espaços.
 *   Example:
 *     "%1d"
 *     "%-5d"
 *     "%-5d"
 *     "%02d"
 * @endverbatim
 *
 * @param fmt : formato de string para exibição - Mascara de Formatação (Identico a mascara de sprintf() 
 *
 * @note None
 *
 */
void RA8889::PutDecimal(uint16_t x, uint16_t y, uint32_t value, const char *fmt)
{
  char buffer[12];
  sprintf(buffer ,fmt, value);
  PutString(x, y, buffer);
}
 

/**
 * @brief Put hexadecimal value format base on sprintf()
 *        
 * @verbatim
 * Tamanho máximo de 12 caracteres o buffer interno
 * 
 * Value Hexadecimal:
 *   Value: 0x00000000 ~ 0xffffffff
 *   len:   1~11 minimum output length
 *   joker:
 *     n: Alinhado à direita, preenchido com espaços
 *     #: Força a saída a começar com 0x.
 *     0: Preenchido com zeros no início (lado esquerdo) em vez de espaços.
 *     x: Força a saída a começar com 0x, preenchido com zeros.
 *   Example:
 *     "%4x" 
 *     "%08x"
 *     "%#6x"
 *     "%#08x"
 *     "%X"      hexadecimal em caixa alta
 * @endverbatim
 *
 * @param fmt : formato de string para exibição - Mascara de Formatação (Identico a mascara de sprintf() 
 *
 * @note None
 *
 */
void RA8889::PutHexa(uint16_t x, uint16_t y, uint32_t value, const char *fmt)
{
  char buffer[12];
  sprintf(buffer ,fmt, value);
  PutString(x, y, buffer);
}


//================================================================================
//
// API
// Funções BTE
// 
//================================================================================


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s0_addr
 * @param uint16_t s0_image_width
 * @param uint16_t s0_x
 * @param uint16_t s0_y
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t copy_width
 * @param uint16_t copy_height
 *
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MemoryCopy(uint32_t s0_addr,
                                  uint16_t s0_image_width,
                                  uint16_t s0_x,
                                  uint16_t s0_y,
                                  uint32_t des_addr,
                                  uint16_t des_image_width, 
                                  uint16_t des_x,
                                  uint16_t des_y,
                                  uint16_t copy_width,
                                  uint16_t copy_height
                                 )
{
  uint8_t temp = 0;
  BTE_S0_MemoryStartAddress(s0_addr);
  BTE_S0_ImageWidth(s0_image_width);
  BTE_S0_WindowStart_XY(s0_x,s0_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  
  BTE_WindowSize(copy_width,copy_height); 
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_ROP_CODE_12;              //BTE ROP Code Bit[3:0] or Color expansion starting bit, S0
  temp |= BIT_BTE_MEMORY_COPY_WITH_ROP;     //Memory Copy with ROP, S0 or S1 comes from memory.
  _bus->DataWrite(temp);
  
  BTE_Enable(true);
  
  CoreTask_WaitReady();
} 


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s0_addr
 * @param uint16_t s0_image_width
 * @param uint16_t s0_x
 * @param uint16_t s0_y
 * @param uint32_t s1_addr
 * @param uint16_t s1_image_width
 * @param uint16_t s1_x
 * @param uint16_t s1_y 
 * @param uint32_t des_addr
 * @param uint16_t des_image_width,
 * @param uint16_t des_x
 * @param uint16_t des_y
 * @param uint16_t copy_width
 * @param uint16_t copy_height
 * @param uint8_t rop_code
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 *
 */
void RA8889::BTE_MemoryCopyWithROP(uint32_t s0_addr,
                                         uint16_t s0_image_width,
                                         uint16_t s0_x,
                                         uint16_t s0_y,
                                         uint32_t s1_addr,
                                         uint16_t s1_image_width,
                                         uint16_t s1_x,
                                         uint16_t s1_y,
                                         uint32_t des_addr,
                                         uint16_t des_image_width,
                                         uint16_t des_x,
                                         uint16_t des_y,
                                         uint16_t copy_width,
                                         uint16_t copy_height,
                                         uint8_t rop_code
                                        )
{
  uint8_t temp = 0;
  BTE_S0_MemoryStartAddress(s0_addr);
  BTE_S0_ImageWidth(s0_image_width);
  BTE_S0_WindowStart_XY(s0_x,s0_y);
  BTE_S1_MemoryStartAddress(s1_addr);
  BTE_S1_ImageWidth(s1_image_width);
  BTE_S1_WindowStart_XY(s1_x,s1_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(copy_width,copy_height);
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp &= 0x0f;                                //Clear bits 7-4
  temp |= (rop_code & 0x0f) << 4;              //BTE ROP Code
  temp |= BIT_BTE_MEMORY_COPY_WITH_ROP;        //Memory Copy with ROP, S0 or S1 comes from memory. 
  _bus->DataWrite(temp);

  BTE_Enable(true);
  
  CoreTask_WaitReady();
} 


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s0_addr
 * @param uint16_t s0_image_width
 * @param uint16_t s0_x
 * @param uint16_t s0_y
 * @param uint32_t des_addr
 * @param uint16_t des_image_width,
 * @param uint16_t des_x
 * @param uint16_t des_y
 * @param uint16_t copy_width
 * @param uint16_t copy_height
 * @param uint32_t chromakey_color
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MemoryCopyWithChromaKey(uint32_t s0_addr,
                                               uint16_t s0_image_width,
                                               uint16_t s0_x,
                                               uint16_t s0_y,
                                               uint32_t des_addr,
                                               uint16_t des_image_width, 
                                               uint16_t des_x,
                                               uint16_t des_y,
                                               uint16_t copy_width,
                                               uint16_t copy_height, 
                                               uint32_t chromakey_color
                                              )
{
  uint8_t temp = 0;
  BTE_S0_MemoryStartAddress(s0_addr);
  BTE_S0_ImageWidth(s0_image_width);
  BTE_S0_WindowStart_XY(s0_x,s0_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(copy_width,copy_height);
  
  BackgroundColor(chromakey_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_MEMORY_COPY_WITH_CHROMA;
  _bus->DataWrite(temp);
  
  BTE_Enable(true);
  
  CoreTask_WaitReady();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s1_addr
 * @param uint16_t s1_image_width
 * @param uint16_t s1_x
 * @param uint16_t s1_y
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height
 * @param uint8_t rop_code
 * @param const uint8_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MPUWriteWithROP(uint32_t s1_addr,
                                       uint16_t s1_image_width,
                                       uint16_t s1_x,
                                       uint16_t s1_y,
                                       uint32_t des_addr,
                                       uint16_t des_image_width,
                                       uint16_t des_x,
                                       uint16_t des_y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t rop_code,
                                       const uint8_t *data
                                      )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_S1_MemoryStartAddress(s1_addr);
  BTE_S1_ImageWidth(s1_image_width);
  BTE_S1_WindowStart_XY(s1_x,s1_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp &= 0x0f;                                //Clear bits 7-4
  temp |= (rop_code & 0x0f) << 4;              //BTE ROP Code
  temp |= BIT_BTE_MPU_WRITE_WITH_ROP;          //Memory Copy with ROP, S0 or S1 comes from memory. 
  _bus->DataWrite(temp);
  
  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
#if defined(COLOR_DEPTH_16)
  for(i=0;i< height;i++) {	
    for(j=0;j< (width*2);j++) {
      Wait_WriteFIFO_NotFull();
      _bus->DataWrite(*data);
      data++;
    }
  }
#elif defined(COLOR_DEPTH_24)
  for(i=0;i< height;i++) {  
    for(j=0;j< (width);j++) {
      Wait_WriteFIFO_NotFull();                //adaptado por mim, wiethaus
      lcdDataWrite(*data);
      data++;
      lcdDataWrite(*data);
      data++;
      lcdDataWrite(*data);
      data++;
    }
  }
#endif 

  Wait_WriteFIFO_Empty();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s1_addr
 * @param uint16_t s1_image_width
 * @param uint16_t s1_x
 * @param uint16_t s1_y
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint8_t rop_code
 * @param const uint16_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MPUWriteWithROP(uint32_t s1_addr,
                                       uint16_t s1_image_width,
                                       uint16_t s1_x,
                                       uint16_t s1_y,
                                       uint32_t des_addr,
                                       uint16_t des_image_width,
                                       uint16_t des_x,
                                       uint16_t des_y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t rop_code,
                                       const uint16_t *data
                                      )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_S1_MemoryStartAddress(s1_addr);
  BTE_S1_ImageWidth(s1_image_width);
  BTE_S1_WindowStart_XY(s1_x,s1_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);

  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp &= 0x0f;                                //Clear bits 7-4
  temp |= (rop_code & 0x0f) << 4;              //BTE ROP Code
  temp |= BIT_BTE_MPU_WRITE_WITH_ROP;          //MPU Write with ROP
  _bus->DataWrite(temp);
  
  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
  for(j=0;j<height;j++) {
    for(i=0;i<width;i++) {
      Wait_WriteFIFO_NotFull();                //if high speed mcu and without Xnwait check
      _bus->DataWrite16(*data);
      data++;
    }
  } 
  Wait_WriteFIFO_Empty();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s1_addr
 * @param uint16_t s1_image_width
 * @param uint16_t s1_x
 * @param uint16_t s1_y
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height
 * @param uint8_t rop_code
 * 
 * @code
 * None
 * @endcode
 *
 * @note write data after setting
 */
void RA8889::BTE_MPUWriteWithROP(uint32_t s1_addr,
                                       uint16_t s1_image_width,
                                       uint16_t s1_x,
                                       uint16_t s1_y,
                                       uint32_t des_addr,
                                       uint16_t des_image_width,
                                       uint16_t des_x,
                                       uint16_t des_y,
                                       uint16_t width,
                                       uint16_t height,
                                       uint8_t rop_code
                                      )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_S1_MemoryStartAddress(s1_addr);
  BTE_S1_ImageWidth(s1_image_width);
  BTE_S1_WindowStart_XY(s1_x,s1_y);
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x, des_y);
  BTE_WindowSize(width,height);
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp &= 0x0f;                                //Clear bits 7-4
  temp |= (rop_code & 0x0f) << 4;              //BTE ROP Code
  temp |= BIT_BTE_MPU_WRITE_WITH_ROP;          //MPU Write with ROP
  _bus->DataWrite(temp);
    
  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t chromakey_color
 * @param const uint8_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                             uint16_t des_image_width, 
                                             uint16_t des_x,
                                             uint16_t des_y,
                                             uint16_t width,
                                             uint16_t height,
                                             uint32_t chromakey_color,
                                             const uint8_t *data
                                            )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);

  BackgroundColor(chromakey_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_MPU_WRITE_WITH_CHROMA;
  _bus->DataWrite(temp);

  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
#if defined(COLOR_DEPTH_16)
  for(i=0;i< height;i++) {
    for(j=0;j< (width*2);j++) {
      Wait_WriteFIFO_NotFull();
      _bus->DataWrite(*data);
      data++;
    }
  }
#elif defined(COLOR_DEPTH_24)
  for(i=0; i< height; i++) {  
    for(j=0; j<(width); j++) {
     Wait_WriteFIFO_NotFull();                 //implementado por mim, wiethaus
     lcdDataWrite(*data);
     data++;
     lcdDataWrite(*data);
     data++;
     lcdDataWrite(*data);
     data++;
    }
  }
#endif 
  
  Wait_WriteFIFO_Empty();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t chromakey_color
 * @param const uint16_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                             uint16_t des_image_width, 
                                             uint16_t des_x,
                                             uint16_t des_y,
                                             uint16_t width,
                                             uint16_t height,
                                             uint32_t chromakey_color,
                                             const uint16_t *data
                                            )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);

  BackgroundColor(chromakey_color);           //High level, Background color

  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_MPU_WRITE_WITH_CHROMA;
  _bus->DataWrite(temp);

  BTE_Enable(true);

  _bus->CmdWrite(REG_MRWDP);
  for(j=0;j<height;j++) {
    for(i=0;i<width;i++) {
      Wait_WriteFIFO_NotFull();//if high speed mcu and without Xnwait check
      _bus->DataWrite16(*data);
      data++;
    }
  } 
  Wait_WriteFIFO_Empty();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t chromakey_color
 * 
 * @code
 * None
 * @endcode
 *
 * @note write data after setting
 */
void RA8889::BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                             uint16_t des_image_width, 
                                             uint16_t des_x,
                                             uint16_t des_y,
                                             uint16_t width,
                                             uint16_t height,
                                             uint32_t chromakey_color
                                            )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);
  
  BackgroundColor(chromakey_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_MPU_WRITE_WITH_CHROMA;
  _bus->DataWrite(temp);

  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t foreground_color
 * @param uint32_t background_color
 * @param const uint8_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note None
 */
void RA8889::BTE_MPUWriteColorExpansion(uint32_t des_addr,
                                              uint16_t des_image_width, 
                                              uint16_t des_x,
                                              uint16_t des_y,
                                              uint16_t width,
                                              uint16_t height,
                                              uint32_t foreground_color,
                                              uint32_t background_color,
                                              const uint8_t *data
                                             )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);
  
  ForegroundColor(foreground_color);           //High level, Foreground color
  BackgroundColor(background_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_ROP_BUS_WIDTH8;
  temp |= BIT_BTE_ROP_CODE_7;      
  _bus->DataWrite(temp);

  BTE_Enable(true);
    
  _bus->CmdWrite(REG_MRWDP);
  for(i=0;i< height;i++) {	
   for(j=0;j< (width/8);j++) {
     Wait_WriteFIFO_NotFull();
     _bus->DataWrite(*data);
     data++;
   }
  }
  Wait_WriteFIFO_Empty();
  CoreTask_WaitReady();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t foreground_color
 * @param uint32_t background_color
 * 
 * @code
 * None
 * @endcode
 *
 * @note write data after setting
 */
void RA8889::BTE_MPUWriteColorExpansion(uint32_t des_addr,
                                              uint16_t des_image_width,
                                              uint16_t des_x,
                                              uint16_t des_y,
                                              uint16_t width,
                                              uint16_t height,
                                              uint32_t foreground_color,
                                              uint32_t background_color
                                             )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;

  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height); 

  ForegroundColor(foreground_color);           //High level, Foreground color
  BackgroundColor(background_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_ROP_BUS_WIDTH8;
  temp |= BIT_BTE_MPU_WRITE_COLOR_EXPANSION;
  _bus->DataWrite(temp);

  BTE_Enable(true);
  
  _bus->CmdWrite(REG_MRWDP);
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t foreground_color
 * @param uint32_t background_color
 * @param const uint8_t *data
 * 
 * @code
 * None
 * @endcode
 *
 * @note background_color do not set the same as foreground_color
 */
void RA8889::BTE_MPUWriteColorExpansionWithChromaKey(uint32_t des_addr,
                                                           uint16_t des_image_width, 
                                                           uint16_t des_x,
                                                           uint16_t des_y,
                                                           uint16_t width,
                                                           uint16_t height,
                                                           uint32_t foreground_color,
                                                           uint32_t background_color,
                                                           const uint8_t *data
                                                          )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);

  ForegroundColor(foreground_color);           //High level, Foreground color
  BackgroundColor(background_color);           //High level, Background color
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_ROP_BUS_WIDTH8;
  temp |= BIT_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA;
  _bus->DataWrite(temp);
  
  BTE_Enable(true);
    
  _bus->CmdWrite(REG_MRWDP);
  for(i=0;i< height;i++) {
   for(j=0;j< (width/8);j++) {
     Wait_WriteFIFO_NotFull();
     _bus->DataWrite(*data);
     data++;
   }
  }
  Wait_WriteFIFO_Empty();
  CoreTask_WaitReady();
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t width
 * @param uint16_t height,
 * @param uint32_t foreground_color
 * @param uint32_t background_color
 * 
 * @code
 * None
 * @endcode
 *
 * @note background_color do not set the same as foreground_color
 *       write data after setting
 */
void RA8889::BTE_MPUWriteColorExpansionWithChromaKey(uint32_t des_addr,
                                                           uint16_t des_image_width,
                                                           uint16_t des_x,
                                                           uint16_t des_y,
                                                           uint16_t width,
                                                           uint16_t height,
                                                           uint32_t foreground_color,
                                                           uint32_t background_color
                                                          )
{
  uint16_t i;
  uint16_t j;
  uint8_t temp = 0;
  
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  BTE_WindowSize(width,height);
  
  ForegroundColor(foreground_color);           //High level, Foreground color
  BackgroundColor(background_color);           //High level, Background color

  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_ROP_BUS_WIDTH8;
  temp |= BIT_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA;
  _bus->DataWrite(temp);

  BTE_Enable(true);

  _bus->CmdWrite(REG_MRWDP);
}


/**
 * @brief 
 *        
 * @verbatim
 * None
 * @endverbatim
 *
 * @param uint32_t s1_addr
 * @param uint16_t s1_image_width
 * @param uint32_t des_addr
 * @param uint16_t des_image_width
 * @param uint16_t des_x
 * @param uint16_t des_y 
 * @param uint16_t copy_width
 * @param uint16_t copy_height
 * 
 * @code
 * None
 * @endcode
 *
 * @note background_color do not set the same as foreground_color
 *       write data after setting
 */
void RA8889::BTE_MemoryCopyWith_ARGB8888(uint32_t s1_addr,
                                               uint16_t s1_image_width,
                                               uint32_t des_addr,
                                               uint16_t des_image_width,
                                               uint16_t des_x,
                                               uint16_t des_y,
                                               uint16_t copy_width,
                                               uint16_t copy_height
                                              )
{
  uint8_t temp = 0;
  
  _bus->CmdWrite(REG_BTE_CTRL1);                 //0x91, BTE Function Control Register1 (BTE_CTRL1)
  temp |= BIT_BTE_MEMORY_COPY_WITH_OPACITY;
  _bus->DataWrite(temp);
  
  temp = 0;
 
  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp |= BIT_S0_COLOR_DEPTH_24BPP;
  temp |= BIT_S1_32BIT_ARGB_ALPHA_BLENDING;
  temp |= BIT_DESTINATION_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);
  
  BTE_S1_MemoryStartAddress(s1_addr);
  BTE_S1_ImageWidth(s1_image_width);
  BTE_S1_WindowStart_XY(0,0);

  BTE_S0_MemoryStartAddress(des_addr);
  BTE_S0_ImageWidth(des_image_width);
  BTE_S0_WindowStart_XY(des_x,des_y);
   
  BTE_Destination_MemoryStartAddress(des_addr);
  BTE_Destination_ImageWidth(des_image_width);
  BTE_Destination_WindowStart_XY(des_x,des_y);
  
  BTE_WindowSize(copy_width,copy_height);

  BTE_Enable(true);
  
  CoreTask_WaitReady();

  temp = 0;

  _bus->CmdWrite(REG_BTE_COLR);                  //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp |= BIT_S0_COLOR_DEPTH_24BPP;
  temp |= BIT_S1_COLOR_DEPTH_24BPP;
  temp |= BIT_DESTINATION_COLOR_DEPTH_24BPP;
  _bus->DataWrite(temp);
}
