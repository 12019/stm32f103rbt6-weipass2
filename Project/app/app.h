/*
*********************************************************************************************************
*
*	ģ������ : APPģ��
*	�ļ����� : app.h
*	��    �� : V1.0
*	˵    �� :
*
*********************************************************************************************************
*/
#ifndef _ASP_H_
#define _ASP_H_

#include "bsp.h"

#include "app_uart.h"
#include "app_printer.h"

/* ����ϵͳ״̬ */
typedef enum
{
	SYS_POWER_ON = 0,

}WORK_MODE;

extern WORK_MODE Sys_Work_Mode;

void App_Init(void);
void App_Task(void);
#endif
