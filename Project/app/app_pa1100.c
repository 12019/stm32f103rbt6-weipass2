/*
*********************************************************************************************************
*
*	ģ������ : ��ͷģ��
*	�ļ����� : app_PA1100.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "app.h"

//PA1100��ͷ����оƬ��Ӧ������
typedef enum
{
	CESHI = 0x10
}PA1100_CMD_NAME;

/* ��ͷ����оƬ���� */
//����ͨ�ţ�SPI_MOSI��SPI_SCK
#define RCC_PA1100_INTERFACE_GPIO					(RCC_APB2Periph_GPIOA)
#define GPIO_PA1100_INTERFACE_PORT 				(GPIOA)
#define GPIO_PA1100_INTERFACE_SPI_SCK			(GPIO_Pin_5)
#define GPIO_PA1100_INTERFACE_SPI_MOSI		(GPIO_Pin_7)

#define PA1100_MOSI_HIGH()	do{	\
	GPIO_SetBits(GPIO_PA1100_INTERFACE_PORT, GPIO_PA1100_INTERFACE_SPI_MOSI); \
	bsp_DelayUS(10);\
}while(0)

#define PA1100_MOSI_LOW() do{	\
	GPIO_ResetBits(GPIO_PA1100_INTERFACE_PORT, GPIO_PA1100_INTERFACE_SPI_MOSI); \
	bsp_DelayUS(10);\
}while(0)

#define PA1100_SCK_HIGH()	do{	\
	GPIO_SetBits(GPIO_PA1100_INTERFACE_PORT, GPIO_PA1100_INTERFACE_SPI_SCK); \
	bsp_DelayUS(10);\
}while(0)

#define PA1100_SCK_LOW() do{	\
	GPIO_ResetBits(GPIO_PA1100_INTERFACE_PORT, GPIO_PA1100_INTERFACE_SPI_SCK); \
	bsp_DelayUS(10);\
}while(0)

/*
*********************************************************************************************************
*	�� �� ��: is_detect_down
*	����˵��: ��ͷ����оƬ�������ż��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint8_t is_detect_down(void) 
{
	if ((GPIO_PA1100_INTERFACE_PORT->IDR & GPIO_PA1100_INTERFACE_SPI_MOSI) == RESET) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: set_mosi_out_pp
*	����˵��: ��ͷ����оƬ������������Ϊ�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void set_mosi_out_pp(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PA1100_INTERFACE_SPI_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIO_PA1100_INTERFACE_PORT, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: set_mosi_ipu
*	����˵��: ��ͷ����оƬ������������Ϊ��������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void set_mosi_ipu(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PA1100_INTERFACE_SPI_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIO_PA1100_INTERFACE_PORT, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: spi_read_byte
*	����˵��: ��ͷ����оƬ��ȡһ�ֽ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint8_t spi_read_byte(void)
{
	uint8_t i;
	uint8_t temp_data = 0;
	for(i = 0; i < 8; i ++)
	{
		if(is_detect_down())
		{
		}
		else
		{
			 temp_data |= (1 << 7 - i);
		}
		PA1100_SCK_HIGH();
		PA1100_SCK_LOW();
	}
	return temp_data;
}

/*
*********************************************************************************************************
*	�� �� ��: pa1100_rst
*	����˵��: ��ͷ����оƬ��λ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void pa1100_rst(void)
{
	//������������Ч ���� 011	
	set_mosi_out_pp();
	
	PA1100_SCK_HIGH();
	PA1100_MOSI_HIGH();
	
	
	PA1100_MOSI_LOW();
	
	PA1100_SCK_LOW();
	PA1100_SCK_HIGH();
	PA1100_SCK_LOW();
	
	
	PA1100_MOSI_HIGH();
	
	PA1100_SCK_HIGH();
	PA1100_SCK_LOW();
	PA1100_SCK_HIGH();
	PA1100_SCK_LOW();
	
	set_mosi_ipu();
}

/*
*********************************************************************************************************
*	�� �� ��: pa1100_read_data
*	����˵��: ��ͷ����оƬ���ݶ�ȡ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void pa1100_read_data(uint8_t *databuf)
{
	uint16_t i;
	for(i = 0; i < 270; i ++)
	{
		*(databuf + i) = spi_read_byte();
	}
	pa1100_rst();
}

/*
*********************************************************************************************************
*	�� �� ��: pa1100_Init
*	����˵��: ��ͷоƬ������ʼ��������1.�������ź�IO��ʼ��  2.SPI��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void pa1100_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* PA1100-SPIͨ������ */
	RCC_APB2PeriphClockCmd(RCC_PA1100_INTERFACE_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_PA1100_INTERFACE_SPI_SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIO_PA1100_INTERFACE_PORT, &GPIO_InitStructure);

	pa1100_rst();
}

/*
*********************************************************************************************************
*	�� �� ��: ceshi
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ceshi(void)
{
	uint8_t temp_data;
	uint8_t databuf[300];
	
	temp_data = is_detect_down();

	if(temp_data)
	{//test �ɹ�
		PA1100_SCK_HIGH();
		PA1100_SCK_LOW();
		PA1100_SCK_HIGH();
		PA1100_SCK_LOW();
		
		memset(databuf, 0, 300);
		pa1100_read_data(databuf);
		//comSendBuf(COM1, databuf+2, 88*3);
		Usart1_Txd_Tempdata[0] = 1;
		Usart1_Txd_Tempdata[1] = 43;
		Usart1_Txd_Tempdata[2] = PA1100;
		memcpy((char *)(Usart1_Txd_Tempdata + 3), (const char *)databuf, 300);
		USART1_Tx_Chars(Usart1_Txd_Tempdata, 303);
	}
	else
	{
		Usart1_Txd_Tempdata[0] = 0x00;
		Usart1_Txd_Tempdata[1] = 0x02;
		Usart1_Txd_Tempdata[2] = PA1100 | 0x80;
		Usart1_Txd_Tempdata[3] = 0x30;
		USART1_Tx_Chars(Usart1_Txd_Tempdata, 4);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: pa1100_CMD_DEAL
*	����˵��: �����ͷ�������
*	��    �Σ�databuf	�����ڽ��յ������ݰ������ֽڵ�ַ
						length	: ���ݰ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void pa1100_CMD_DEAL(uint8_t *databuf, uint16_t length)
{
	switch (*databuf)
	{
		case CESHI:
			//��������
			if(*(databuf + 1) == 0x00)
			{
				ceshi();
			}
			else if(*(databuf + 1) == 0x01)
			{
				pa1100_rst();
				Usart1_Txd_Tempdata[0] = 0x00;
				Usart1_Txd_Tempdata[1] = 0x01;
				Usart1_Txd_Tempdata[2] = PA1100;
				USART1_Tx_Chars(Usart1_Txd_Tempdata, 3);
			}
			break;
		default :
			//����֧�ָ�����
			Usart1_Txd_Tempdata[0] = 0x00;
			Usart1_Txd_Tempdata[1] = 0x02;
			Usart1_Txd_Tempdata[2] = PA1100 | 0x80;
			Usart1_Txd_Tempdata[3] = 0x10;
			USART1_Tx_Chars(Usart1_Txd_Tempdata, 4);
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: pa1100_SERVER_TASK
*	����˵��: ��ͷ��ط�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void pa1100_SERVER_TASK(void)
{
  static uint32_t sys_time, time_ms = 0, time_add_up = 0;
  const uint32_t work_time_max = 2;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	DISABLE_INT();
	sys_time = Sys_Time;
	ENABLE_INT();
	
	if (time_ms != sys_time)
	{
		if (time_ms < sys_time)
		{
			offset_time_ticks = sys_time - time_ms;
			time_ms = sys_time;
			if (((uint64_t)offset_time_ticks  + (uint64_t)time_add_up) > UINT_LEAST32_MAX)
			{
				timeout_flag = 1;
				time_add_up = 0;
			}
			else if ((offset_time_ticks  + time_add_up) > work_time_max)
			{
				timeout_flag = 1;
				time_add_up = 0;
			}
			else
			{
				time_add_up += offset_time_ticks;
			}
		}
		else
		{
			offset_time_ticks = UINT_LEAST32_MAX + sys_time - time_ms;
			time_ms = sys_time;
			if (((uint64_t)offset_time_ticks  + (uint64_t)time_add_up) > UINT_LEAST32_MAX)
			{
				timeout_flag = 1;
				time_add_up = 0;
			}
			else if ((offset_time_ticks  + time_add_up) > work_time_max)
			{
				timeout_flag = 1;
				time_add_up = 0;
			}
			else
			{
				time_add_up += offset_time_ticks;
			}
		}
		if (timeout_flag == 1)
		{
			timeout_flag = 0;
			
		}
	}
}
