#include "modbus_timer.h"
#include "led.h"
#include "stm32f4xx.h"                  // Device header
#include "RS485.h"
#include "modbus.h"


uint8_t sec_flag=0;//sec_flag=1时发送数据

//int Host_Sendtime=0;
void Modbus_TIME3_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //打开TIM3的时钟
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;      //ARR 设置自动重装载寄存器周期的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 840 - 1;   //PSC     设定频率500HZ
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
            
	//使能定时器中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//要使能定时器的更新中断
	
	//中断优先级分组
	NVIC_InitTypeDef NVIC_InitStruct;//中断分组结构体
	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//使能定时器
	 TIM_Cmd(TIM3, ENABLE);
}


// Modbus 定时器中断函数 1ms中断一次
void TIM3_IRQHandler(void)   //TIM3中断
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        
		if(modbus.timrun != 0)//运行时间！=0表明
		 {
              modbus.timout++;
              if(modbus.timout >=8)
              {
                  modbus.timrun = 0;
                  modbus.reflag = 1;//接收数据完毕
              }			
		 }
		 modbus.Host_Sendtime++;//发送完上一帧后的时间计数
		 if(modbus.Host_Sendtime>1000)//距离发送上一帧数据1s了
			{
				//1s时间到
				modbus.Host_time_flag=1;//发送数据标志位置1
				
			}
	}
}
