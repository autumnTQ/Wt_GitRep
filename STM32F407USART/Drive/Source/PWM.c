#include "PWM.h"


void PWM_Init()      //ʹ��TIM2ͨ��2���PWM��
{
    GPIO_InitTypeDef GPIO_InitStructure;    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //��TIM2��GPIOA��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          //��ʼ��GPIO_PA1  �����������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);  //TIM2ͨ��2����PA1
            
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 200 - 1;      //ARR �����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler = 840 - 1;   //PSC     �趨Ƶ��500HZ
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50;    //CCR
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
//   TIM_CtrlPWMOutputs(TIM1, ENABLE);   //�߼���ʱ��  
    TIM_Cmd(TIM2, ENABLE);
}


//�޸�ռ�ձ�   �޸ıȽϼĴ���CCR��ֵ 
void PWM_SetCompare2(uint16_t compare)  
{   
	TIM_SetCompare2(TIM2, compare);
}

