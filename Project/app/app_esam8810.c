/*
*********************************************************************************************************
*
*	ģ������ : ESAM����оƬģ��
*	�ļ����� : app_esam8810.c
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/
#include "app.h"

//ESAM����оƬ�������б�
typedef enum
{
	CESHI = 0x10
}ESAM8810_CMD_NAME;

/* ESAM����оƬ���� */
//ESAM8810_RST
#define RCC_ESAM8810_RST 				(RCC_APB2Periph_GPIOA)
#define GPIO_ESAM8810_RST_PORT 			(GPIOA)
#define GPIO_ESAM8810_RST_PIN 			(GPIO_Pin_3)

#define ESAM8810_RST_ON()	do{	\
	GPIO_ResetBits(GPIO_ESAM8810_RST_PORT, GPIO_ESAM8810_RST_PIN); \
}while(0)

#define ESAM8810_RST_OFF() do{	\
	GPIO_SetBits(GPIO_ESAM8810_RST_PORT, GPIO_ESAM8810_RST_PIN); \
}while(0)

//ESAM8810����ͨ�ţ�USART2_TX��USART2_CK
#define RCC_ESAM8810_INTERFACE_GPIO					(RCC_APB2Periph_GPIOA)
#define RCC_ESAM8810_INTERFACE_AFIO					(RCC_APB2Periph_AFIO)
#define RCC_ESAM8810_INTERFACE_USART 				(RCC_APB1Periph_USART2)
#define GPIO_ESAM8810_INTERFACE_PORT 				(GPIOA)
#define GPIO_ESAM8810_INTERFACE_USART_TX_PIN		(GPIO_Pin_2)
#define GPIO_ESAM8810_INTERFACE_USART_CK_PIN		(GPIO_Pin_4)

#define ESAM8810_TX_HIGH()	do{	\
	GPIO_SetBits(GPIO_ESAM8810_RST_PORT, GPIO_ESAM8810_INTERFACE_USART_TX_PIN); \
}while(0)

#define ESAM8810_TX_LOW() do{	\
	GPIO_ResetBits(GPIO_ESAM8810_RST_PORT, GPIO_ESAM8810_INTERFACE_USART_TX_PIN); \
}while(0)

/*
*********************************************************************************************************
*	�� �� ��: esam8810_rst
*	����˵��: ESAM8810����оƬ��λ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void esam8810_rst(void)
{
  ESAM8810_RST_ON();
  ESAM8810_TX_HIGH();
  bsp_DelayUS(400);
  ESAM8810_RST_OFF();
}

/*
*********************************************************************************************************
*	�� �� ��: esam8810_rcv
*	����˵��: ESAM8810��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void esam8810_rcv(void)
{
	uint8_t temp_data;
	uint8_t databuf[100];

	static uint8_t rxd_timeout;
	static uint16_t rxd_num;

	if (comGetChar(COM2, &temp_data) == 0)
	{
		if (rxd_timeout)
		{
			rxd_timeout--;
		}
		else
		{
			if(rxd_num)
			{
				comSendBuf(COM1, (uint8_t *)databuf, rxd_num);
				rxd_num = 0;
			}
		}
	}
	else
	{
		rxd_timeout = 50;
		databuf[rxd_num ++] = temp_data;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ceshi
*	����˵��: ����ESAM8810����оƬ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ceshi(uint8_t * databuf, uint8_t length)
{
	comSendBuf(COM2, (uint8_t *)databuf, length);

	Usart1_Txd_Tempdata[0] = 0x00;
	Usart1_Txd_Tempdata[1] = 0x01;
	Usart1_Txd_Tempdata[2] = ESAM8810;
	USART1_Tx_Chars(Usart1_Txd_Tempdata, 3);
}

/*
*********************************************************************************************************
*	�� �� ��: esam8810_Init
*	����˵��: ESAM8810����оƬ��س�ʼ��������1.�������ź�IO��ʼ��  2.USART��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void esam8810_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ESAM8810_USARTͨ������ */
	RCC_APB2PeriphClockCmd(RCC_ESAM8810_INTERFACE_GPIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_ESAM8810_INTERFACE_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_ESAM8810_INTERFACE_USART, ENABLE);	
	
	/* ESAM8810_RST */
	RCC_APB2PeriphClockCmd(RCC_ESAM8810_RST, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_ESAM8810_RST_PIN;
	GPIO_Init(GPIO_ESAM8810_RST_PORT, &GPIO_InitStructure);

	/* Configure USART CK pin as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_ESAM8810_INTERFACE_USART_CK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_ESAM8810_INTERFACE_PORT, &GPIO_InitStructure);

	/* Configure USART Tx pin as alternate function open-drain */
	GPIO_InitStructure.GPIO_Pin = GPIO_ESAM8810_INTERFACE_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_ESAM8810_INTERFACE_PORT, &GPIO_InitStructure);

	/* SC_USART Clock set to 4.5MHz (PCLK1 = 36 MHZ / 8) */
	USART_SetPrescaler(USART2, 0x04);
	/* SC_USART Guard Time set to 2 Bit */
	USART_SetGuardTime(USART2, 0x2);

	USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(USART2, &USART_ClockInitStructure);

	USART_InitStructure.USART_BaudRate = 12097;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */
	USART_SmartCardNACKCmd(USART2, ENABLE);
	USART_SmartCardCmd(USART2, ENABLE);

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */  

	esam8810_rst();
}

/*
*********************************************************************************************************
*	�� �� ��: esam8810_CMD_DEAL
*	����˵��: �����ͷ�������
*	��    �Σ�databuf	�����ڽ��յ������ݰ������ֽڵ�ַ
						length	: ���ݰ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void esam8810_CMD_DEAL(uint8_t *databuf, uint16_t length)
{
	switch (*databuf)
	{
		case CESHI:
			//��������
			if(*(databuf + 1) == 0x10)
			{
				esam8810_rst();
				Usart1_Txd_Tempdata[0] = 0x00;
				Usart1_Txd_Tempdata[1] = 0x01;
				Usart1_Txd_Tempdata[2] = ESAM8810;
				USART1_Tx_Chars(Usart1_Txd_Tempdata, 3);
			}
			else if(*(databuf + 1) == 0x11)
			{
				ceshi(databuf + 3, *(databuf + 2));
			}
			break;
		default :
			//����֧�ָ�����
			Usart1_Txd_Tempdata[0] = 0x00;
			Usart1_Txd_Tempdata[1] = 0x02;
			Usart1_Txd_Tempdata[2] = ESAM8810 | 0x80;
			Usart1_Txd_Tempdata[3] = 0x21;
			USART1_Tx_Chars(Usart1_Txd_Tempdata, 4);
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: esam8810_SERVER_TASK
*	����˵��: ��ͷ��ط�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void esam8810_SERVER_TASK(void)
{
  static uint32_t sys_time, time_ms = 0;
	
	DISABLE_INT();
	sys_time = Sys_Time;
	ENABLE_INT();
	
	if (time_ms != sys_time)
	{
		time_ms = sys_time;
		esam8810_rcv();
	}
}
