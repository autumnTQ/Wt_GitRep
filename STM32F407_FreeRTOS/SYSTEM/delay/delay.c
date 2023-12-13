#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��OS,����������ͷ�ļ�����ucosΪ��������.
#if SYSTEM_SUPPORT_OS

#include "FreeRTOS.h"
#include "task.h"

#endif
//////////////////////////////////////////////////////////////////////////////////  
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���(֧��OS)
//����delay_us,delay_ms
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1 20140803 
//1,delay_us,��Ӳ�������0�ж�,�����������0,��ֱ���˳�. 
//2,�޸�ucosii��,delay_ms����,����OSLockNesting���ж�,�ڽ����жϺ�,Ҳ����׼ȷ��ʱ.
//V1.2 20150411  
//�޸�OS֧�ַ�ʽ,��֧������OS(������UCOSII��UCOSIII,����������OS������֧��)
//���:delay_osrunning/delay_ostickspersec/delay_osintnesting�����궨��
//���:delay_osschedlock/delay_osschedunlock/delay_ostimedly��������
//V1.3 20150521
//����UCOSIII֧��ʱ��2��bug��
//delay_tickspersec��Ϊ��delay_ostickspersec
//delay_intnesting��Ϊ��delay_osintnesting
////////////////////////////////////////////////////////////////////////////////// 

static u8  fac_us=0;							//us��ʱ������	

extern void xPortSysTickHandler(void);

//systick�жϷ�����,ʹ��OSʱ�õ�
void SysTick_Handler(void)
{	
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }	
}
			   
//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��Ƶ��
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	u32 reload;
#endif
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	reload=SYSCLK;						//ÿ���ӵļ������� ��λΪM	   
	reload*=1000000/configTICK_RATE_HZ;	//����delay_ostickspersec�趨���ʱ��
											//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 					//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 	//����SYSTICK    
#endif
}								    

#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
//��ʱnus
//nus:Ҫ��ʱ��us��.	
//nus:0~204522252(���ֵ��2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						//��Ҫ�Ľ����� 
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}  
//��ʱnms
//nms:Ҫ��ʱ��ms��
//nms:0~65535
void delay_ms(u16 nms)
{	
    uint32_t i;
    
    for (i=0; i<nms; i++)
    {
        delay_us(1000);
    }
}
#else  //����ucosʱ
//��ʱnus
//nusΪҪ��ʱ��us��.	
//ע��:nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��168M������,nms<=798ms 
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
} 
//��ʱnms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;						//������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
											//���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
	u16 remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
#endif
			 



































