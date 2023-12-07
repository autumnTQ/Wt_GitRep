#include "modbus_timer.h"
#include "led.h"
#include "stm32f4xx.h"                  // Device header
#include "RS485.h"
#include "modbus.h"


uint8_t sec_flag=0;//sec_flag=1ʱ��������

//int Host_Sendtime=0;
void Modbus_TIME3_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //��TIM3��ʱ��
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;      //ARR �����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler = 840 - 1;   //PSC     �趨Ƶ��500HZ
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
            
	//ʹ�ܶ�ʱ���ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//Ҫʹ�ܶ�ʱ���ĸ����ж�
	
	//�ж����ȼ�����
	NVIC_InitTypeDef NVIC_InitStruct;//�жϷ���ṹ��
	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//ʹ�ܶ�ʱ��
	 TIM_Cmd(TIM3, ENABLE);
}


// Modbus ��ʱ���жϺ��� 1ms�ж�һ��
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
        
		if(modbus.timrun != 0)//����ʱ�䣡=0����
		 {
              modbus.timout++;
              if(modbus.timout >=8)
              {
                  modbus.timrun = 0;
                  modbus.reflag = 1;//�����������
              }			
		 }
		 modbus.Host_Sendtime++;//��������һ֡���ʱ�����
		 if(modbus.Host_Sendtime>1000)//���뷢����һ֡����1s��
			{
				//1sʱ�䵽
				modbus.Host_time_flag=1;//�������ݱ�־λ��1
				
			}
	}
}
