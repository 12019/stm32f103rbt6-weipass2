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

typedef enum
{
	frame_idle = 0, 
	frame_sof, 
	frame_normal,
	frame_mark,
	frame_eof
}FRAME_STATE;

extern FRAME_STATE uart_FRAME_STATE;

void uart_SERVER_TASK(void);

#endif


