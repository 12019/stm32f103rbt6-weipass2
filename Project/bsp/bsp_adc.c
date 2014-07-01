/*
*********************************************************************************************************
*
*	ģ������ : ADCģ��
*	�ļ����� : bsp_adc.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "bsp.h"
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitAdc
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitAdc(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	
  //PC0 ��Ϊģ��ͨ����������    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                    // ADC1 �ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                         // ENABLE-ADC��ͨ��ɨ��, DISABLE-ADC��ͨ��ɨ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    // ENABLE--ADC����ת��ģʽ DISABLE--ADC����ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ����������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                // �������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                               // ����ת��1��ADͨ��ֵ
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 ͨ��10����ʱ������ */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);
 
  ADC_Cmd(ADC1, ENABLE); // ʹ�� ADC1
  
  ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ���
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
	
	ADC_StartCalibration(ADC1);		//��ʼָ��ADC1��У׼״̬
 
	while(ADC_GetCalibrationStatus(ADC1));		//��ȡָ��ADC1��У׼����,����״̬��ȴ�
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1������ת����������
}
/*
//ADC1_value = ADC_GetConversionValue(ADC1); 

�������� 75 ���϶� ��СֵΪ1.4K

�� adcֵ Ϊ 1.4/��1.4+30��*4095

// 183=
*/