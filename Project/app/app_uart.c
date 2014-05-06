/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : app_uart.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "app.h"

FRAME_STATE uart_FRAME_STATE = frame_sof;


static uint16_t Rxdtime = 0;
static uint16_t USART1_RXD_CON = 0;
static uint8_t Usart1_Rxd_Tempdata[USART1_DATA_MAX_LEN];


const uint8_t error0_data[]={0x55, 0x00, 0x03, 0x80, 0x00, 0xAA};

void USART1_Tx_Chars(u8 *puchMsg,u16 num)
{
	u16 i;
	comSendChar(COM1, frame_SOF);
	for(i=0;i<num;i++)
	{
		if((*(puchMsg + i) == frame_SOF)||(*(puchMsg + i) == frame_EOF)||(*(puchMsg + i) == frame_MARK))
		{
			comSendChar(COM1, frame_MARK);
		}
		comSendChar(COM1, *(puchMsg + i));
	}
	comSendChar(COM1, frame_EOF);
}

uint8_t uart_SERVER_TASK(void)
{
	uint8_t temp_data;
	if(comGetChar(COM1, &temp_data) == 0)
	{
		if(Rxdtime > 0)
		{
			Rxdtime--;
		}
		else
		{
			uart_FRAME_STATE = frame_sof;
		}
	}
	else
	{
		if((Rxdtime == 0)&&(uart_FRAME_STATE == frame_sof)&&(temp_data == frame_SOF))
		{
			USART1_RXD_CON = 0;
			uart_FRAME_STATE = frame_normal;
		}
		else if(uart_FRAME_STATE == frame_normal)
		{
			if(temp_data == frame_MARK)
			{
				uart_FRAME_STATE = frame_mark;
			}
			else if(temp_data == frame_SOF)
			{
				//�������ݰ�
				uart_FRAME_STATE = frame_sof;//��ʼ��״̬��
			}
			else if(temp_data == frame_EOF)
			{
				uart_FRAME_STATE = frame_eof;
				/*  */
				comSendBuf(COM1, Usart1_Rxd_Tempdata, USART1_RXD_CON);
			}
			else
			{
				Usart1_Rxd_Tempdata[USART1_RXD_CON] = temp_data;
				USART1_RXD_CON++;
			}
		}
		else if(uart_FRAME_STATE == frame_mark)
		{
			if((temp_data == frame_MARK) || (temp_data == frame_SOF) || (temp_data == frame_EOF))
			{
				Usart1_Rxd_Tempdata[USART1_RXD_CON] = temp_data;
				USART1_RXD_CON++;
				uart_FRAME_STATE = frame_normal;
			}
			else
			{
				//��������ݰ�
				uart_FRAME_STATE = frame_sof;//��ʼ��״̬��
			}
		}
		Rxdtime = frame_TIME_OUT;
	}
	return 0;
}
