/***
	*************************************************************************************************
	*	@version V1.0
	*  @date    2023-3-13
	*	@author  ���ͿƼ�	
	*	@brief   LED�ӿ����
   *************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨������STM32F407ZGT6���İ� ���ͺţ�FK407M2��
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
>>>>> �ļ�˵����
	*
	*	��ʼ��LED��IO�ڣ�����Ϊ����������ٶȵȼ�2M��
	*
	************************************************************************************************
***/


#include "led.h"  

/*************************************************************************************************
*	�� �� ��:	LED_Init
*
*	��������:	IO�ڳ�ʼ��
*	 
*************************************************************************************************/

void LED_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ��
	RCC_AHB1PeriphClockCmd ( LED1_CLK, ENABLE); 	//��ʼ��GPIOGʱ��	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   //���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //�ٶ�ѡ��
	
	//��ʼ�� LED1 ����
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;	 
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);	
	
	GPIO_SetBits(LED1_PORT,LED1_PIN);  //PC13����͵�ƽ
}




