/*
*********************************************************************************************************
*
*	ģ������ : PSAM CARDģ��
*	�ļ����� : app_psam_card.c
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/
#include "app.h"

typedef enum
{
	CESHI = 0x10
}PSAM_CARD_CMD_NAME;

/* PSAM_CARD���� */
//PSAM_CARD_RST
#define RCC_PSAM_CARD_RST 						(RCC_APB2Periph_GPIOB)
#define GPIO_PSAM_CARD_RST_PORT 			(GPIOB)
#define GPIO_PSAM_CARD_RST_PIN 				(GPIO_Pin_11)

#define PSAM_CARD_RST_ON()	do{	\
	GPIO_ResetBits(GPIO_PSAM_CARD_RST_PORT, GPIO_PSAM_CARD_RST_PIN); \
}while(0)

#define PSAM_CARD_RST_OFF() do{	\
	GPIO_SetBits(GPIO_PSAM_CARD_RST_PORT, GPIO_PSAM_CARD_RST_PIN); \
}while(0)

//PSAM_CARD����ͨ�ţ�USART3_TX��USART3_CK
#define RCC_PSAM_CARD_INTERFACE_GPIO							(RCC_APB2Periph_GPIOB)
#define RCC_PSAM_CARD_INTERFACE_AFIO							(RCC_APB2Periph_AFIO)
#define RCC_PSAM_CARD_INTERFACE_USART 						(RCC_APB1Periph_USART3)
#define GPIO_PSAM_CARD_INTERFACE_PORT 						(GPIOB)
#define GPIO_PSAM_CARD_INTERFACE_USART_TX_PIN			(GPIO_Pin_10)
#define GPIO_PSAM_CARD_INTERFACE_USART_CK_PIN			(GPIO_Pin_12)

#define PSAM_CARD_TX_HIGH() do{	\
	GPIO_SetBits(GPIO_PSAM_CARD_INTERFACE_PORT, GPIO_PSAM_CARD_INTERFACE_USART_TX_PIN); \
}while(0)
#define PSAM_CARD_TX_LOW() do{	\
	GPIO_ResetBits(GPIO_PSAM_CARD_INTERFACE_PORT, GPIO_PSAM_CARD_INTERFACE_USART_TX_PIN); \
}while(0)

#define PSAM_CARD_CK_HIGH() do{	\
	GPIO_SetBits(GPIO_PSAM_CARD_INTERFACE_PORT, GPIO_PSAM_CARD_INTERFACE_USART_CK_PIN); \
}while(0)
#define PSAM_CARD_CK_LOW() do{	\
	GPIO_ResetBits(GPIO_PSAM_CARD_INTERFACE_PORT, GPIO_PSAM_CARD_INTERFACE_USART_CK_PIN); \
}while(0)

/*
*********************************************************************************************************
*	�� �� ��: psam_card_rst
*	����˵��: PSAM����λ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void psam_card_rst(void)
{
  PSAM_CARD_RST_ON();
  PSAM_CARD_TX_HIGH();
	bsp_DelayUS(400);
	PSAM_CARD_RST_OFF();	
}
/*
*********************************************************************************************************
*	�� �� ��: psam_card_rcv
*	����˵��: PSAM����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void psam_card_rcv(void)
{
	uint8_t temp_data;
	uint8_t databuf[100];

	static uint8_t rxd_timeout;
	static uint16_t rxd_num;

	if (comGetChar(COM3, &temp_data) == 0)
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
*	����˵��:
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ceshi(uint8_t * databuf, uint8_t length)
{
	comSendBuf(COM3, (uint8_t *)databuf, length);
	Usart1_Txd_Tempdata[0] = 0x00;
	Usart1_Txd_Tempdata[1] = 0x01;
	Usart1_Txd_Tempdata[2] = PSAM_CARD;
	USART1_Tx_Chars(Usart1_Txd_Tempdata, 4);
}

/*
*********************************************************************************************************
*	�� �� ��: psam_card_Init
*	����˵��: PSAM CARDƬ��س�ʼ��������1.�������ź�IO��ʼ��  2.USART��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void psam_card_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_InitTypeDef USART_InitStructure;
  
	/* PSAM_CARD_USARTͨ������ */
	RCC_APB2PeriphClockCmd(RCC_PSAM_CARD_INTERFACE_GPIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_PSAM_CARD_INTERFACE_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_PSAM_CARD_INTERFACE_USART, ENABLE);
	
	/* PSAM_CARD_RST */
	RCC_APB2PeriphClockCmd(RCC_PSAM_CARD_RST, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_PSAM_CARD_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PSAM_CARD_RST_PORT, &GPIO_InitStructure);
	
  /* Configure USART CK pin as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_PSAM_CARD_INTERFACE_USART_CK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PSAM_CARD_INTERFACE_PORT, &GPIO_InitStructure);
  
  /* Configure USART Tx pin as alternate function open-drain */
  GPIO_InitStructure.GPIO_Pin = GPIO_PSAM_CARD_INTERFACE_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PSAM_CARD_INTERFACE_PORT, &GPIO_InitStructure);

  /* SC_USART Clock set to 4.5MHz (PCLK1 = 36 MHZ / 8) */
  USART_SetPrescaler(USART3, 0x04);
  /* SC_USART Guard Time set to 2 Bit */
  USART_SetGuardTime(USART3, 0x02);

  USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
  USART_ClockInit(USART3, &USART_ClockInitStructure);

  USART_InitStructure.USART_BaudRate = 12097;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART3, &USART_InitStructure); 

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */
  USART_SmartCardNACKCmd(USART3, ENABLE);
  USART_SmartCardCmd(USART3, ENABLE);
  
	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */  

	psam_card_rst();
}

/*
*********************************************************************************************************
*	�� �� ��: psam_card_CMD_DEAL
*	����˵��: ����PSAM CARD�������
*	��    �Σ�databuf	�����ڽ��յ������ݰ������ֽڵ�ַ
						length	: ���ݰ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void psam_card_CMD_DEAL(uint8_t *databuf, uint16_t length)
{
	switch (*databuf)
	{
		case CESHI:
			//��������
			if(*(databuf + 1) == 0x10)
			{
				psam_card_rst();
				Usart1_Txd_Tempdata[0] = 0x00;
				Usart1_Txd_Tempdata[1] = 0x01;
				Usart1_Txd_Tempdata[2] = PSAM_CARD;
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
				Usart1_Txd_Tempdata[2] = PSAM_CARD | 0x80;
				Usart1_Txd_Tempdata[3] = 0x10;
				USART1_Tx_Chars(Usart1_Txd_Tempdata, 4);
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: psam_card_SERVER_TASK
*	����˵��: PSAM CARD��ط�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void psam_card_SERVER_TASK(void)
{
	static uint32_t sys_time, time_ms = 0;
	
	DISABLE_INT();
	sys_time = Sys_Time;
	ENABLE_INT();
	
	if (time_ms != sys_time)
	{
		time_ms = sys_time;
		//psam_card_rcv();
	}
}
