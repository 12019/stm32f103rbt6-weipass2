/*
*********************************************************************************************************
*
*	ģ������ : ����ģ��
*	�ļ����� : app_uart.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __APP_UART_H
#define __APP_UART_H


#define frame_TIME_OUT 	50		//����֡�ֽڼ����ʱʱ��
#define frame_SOF				0x55	//����֡��ʼ�ֽ�
#define frame_EOF				0xAA	//����֡�����ֽ�
#define frame_MARK			0xFF	//����ת���־
#define USART1_DATA_MAX_LEN			1000


typedef enum
{
	frame_sof = 0, 
	frame_normal,
	frame_mark,
	frame_eof
}FRAME_STATE;//����������ݰ�ʱ��״̬��

extern FRAME_STATE uart_FRAME_STATE;

uint8_t uart_SERVER_TASK(void);

#endif


