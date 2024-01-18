#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "freertos_demo.h"
 
int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);            //��ʼ����ʱ����
	uart_init(115200);		    //��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED 
 	KEY_Init();					//������ʼ��
    
 	FSMC_SRAM_Init();			//��ʼ���ⲿSRAM  	
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
    
    freertos_demo();            //����FreeRTOS����
}


