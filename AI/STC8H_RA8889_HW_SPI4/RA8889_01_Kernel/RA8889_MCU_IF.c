#include "RA8889_MCU_IF.h"
#include "stc8h.h"       //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�


/*********************************************************/
/*  SPI-4 ͨ��Э�飺Ӳ����ʽ                             */
/*********************************************************/

/* SPI-4 ��ʼ�� */
void SPI4_Init()
{
	SPCTL |=  (1 << 7); //���� SS ���Ź��ܣ�ʹ�� MSTR ȷ���������������Ǵӻ�
	SPCTL |=  (1 << 6); //ʹ�� SPI ����
	SPCTL &= ~(1 << 5); //�ȷ���/�������ݵĸ�λ�� MSB��
	SPCTL |=  (1 << 4); //��������ģʽ
	SPCTL |=  (1 << 3); //SCLK ����ʱΪ�ߵ�ƽ��SCLK ��ǰʱ����Ϊ�½��أ���ʱ����Ϊ������
	SPCTL |=  (1 << 2); //������ SCLK ��ǰʱ������������ʱ���ز���
	SPCTL = (SPCTL & ~3) | 2;   //SPI ʱ��Ƶ��ѡ��, 0: 4T, 1: 8T,  2: 16T,  3: 2T  ��ʱ��Ƶ���ȵ������������������Լ���ϵͳ������
	
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO��ѡ��P2. 0��P1.2/P5.4 P1.3 P1.4 P1.5  ||  1��P2.2 P2.3 P2.4 P2.5  ||  2��P5.4 P4.0 P4.1 P4.3  ||  3��P3.5 P3.4 P3.3 P3.2

	SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־	
}

/* SPI-4 �������� */
void SPI4_SendByte(unsigned char dat)
{
	SPDAT = dat;
	while((SPSTAT & SPIF) == 0) ;
	SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
}

/* SPI-4 �������� */
unsigned char SPI4_ReceiveByte()
{
	SPDAT = 0xff;
	while((SPSTAT & SPIF) == 0) ;
	SPSTAT = SPIF + WCOL;   //��0 SPIF��WCOL��־
	return (SPDAT);
}



/* LCD API: д������ */
void LCD_CmdWrite(unsigned char cmd)
{
	SPI_SS=0;
	SPI4_SendByte(0x00);
	SPI4_SendByte(cmd);
	SPI_SS=1;
}

/* LCD API: д������ */
void LCD_DataWrite(unsigned char Data)
{
	SPI_SS=0;
	SPI4_SendByte(0x80);
	SPI4_SendByte(Data);
	SPI_SS=1;
}

/* LCD API: ��ȡ���� */
unsigned char LCD_DataRead(void)
{
	unsigned char Data;
	
	SPI_SS=0;
	SPI4_SendByte(0xc0);
	Data = SPI4_ReceiveByte();
	SPI_SS=1;
	return Data;
}

/* LCD API: ��ȡоƬ״̬ */
unsigned char LCD_StatusRead(void)
{
	unsigned char Data;
	
	SPI_SS=0;
	SPI4_SendByte(0x40);
	Data = SPI4_ReceiveByte();
	SPI_SS=1;
	return Data;
}
