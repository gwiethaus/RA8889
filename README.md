# RA8889

Desenvolvimento e Adaptação de Driver para uso de display com controladora RA8889 com tela de toque capacitivo baseado nos exemplos da BuyDisplay e RAIO.

# Notas

Os driver para o uso da tela está preparado através do ajuste de configuração da SDRAM, PLL, ROM, Flash SPI e Fontes basicas. Permite a configuração do tipo de profundidde de cores de operação do display em 8 bits (RGB332), 16 (RGB565) ou 24 bits (RGB888). Possui tambem módulo de tela de toque capacitivo controlado pelo circuito FocalTech da série FT5xx6.

O microcontrolador de Display RA8889 possui recurso de manter a última tela antes do Power Off. No inicio parecia um Bug de que o Reset de Harware e de software não estavam ocorrendo adequadamente.
Após alguns testes, destacou que de inicio após as configuraões do display usando o método Begin() manter o display desligado. Preencher a tela com algum padrão de fundo ou apresentação e então lgiar o dsiplay pelo comando DisplayOn(). Esse recurso de aramzenanmento da ultima imagem de tela faz sentido quando precisa desligar a tela e religar ela sem a eprda de suas informações em memória.

Todos os métodos e propriedades tem como padrão o cabeçalho explicativo no formato Doxygen para gerar documentação.

# Todo

- Transferencia de DMA
- Fontes de Usuário
- Melhorias das fotnes de ROM
- Melhoria das funções IDEC
- Configuração Paralela
- Configuração I2C
- Melhoria do Driver FocalTech FT5xx6
