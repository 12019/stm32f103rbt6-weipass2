/*
*********************************************************************************************************
*
*	ģ������ : ��ӡ��ģ��
*	�ļ����� : app_printer.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __APP_PRINTER_H
#define __APP_PRINTER_H

void printer_Init(void);
void TIM2_ISR(void);
void printer_CMD_DEAL(uint8_t *databuf, uint16_t length);
void printer_SERVER_TASK(void);

#endif


