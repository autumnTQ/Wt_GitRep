#ifndef __PWM_H
#define __PWM_H

#include "stm32f4xx.h"                  // Device header

void PWM_Init(void);                      //ʹ��TIM2ͨ��2���PWM��
void PWM_SetCompare2(uint16_t compare);   //�޸�ռ�ձ�   �޸ıȽϼĴ���CCR��ֵ 
   



#endif 


