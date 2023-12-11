#include "RS485.h"


//TX������USART������Ƶ�����ţ�����ѡ�ø����������
//RX������USART������������ţ�����Ҫѡ������ģʽ������ģʽ����ʲô��ͨ���뻹�Ǹ�������
//һ����ֻ����һ��������������ж�����룬��������ţ������GPIO������ͬʱʹ��

//ѡ�񴮿�USART6  TX_PC6�����������  RX_PC7����������߸�������
void RS485_Init(void)
{   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;    

	//���� TX_PC6�����������	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//���� RX_PC7��������	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength  = USART_WordLength_8b;
    USART_Init(USART6, &USART_InitStructure);
        
    //USART6   NVIC����
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //�����ȼ�
    NVIC_Init(&NVIC_InitStructure);
    
    //�������������ж�
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

    //ʹ�ܴ���
    USART_Cmd(USART6, ENABLE);
}


//modbus���ڷ���һ���ֽ�����
void Modbus_Send_Byte(uint8_t Modbus_byte)
{
    USART_SendData(USART6,Modbus_byte);
    
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
    
	USART_ClearFlag(USART6, USART_FLAG_TC); 
}

//modbus�����жϷ������
void USART6_IRQHandler(void)                
{
    uint8_t st,Res;
    
	st = USART_GetITStatus(USART6, USART_IT_RXNE);
    
	if(st == SET)//�����ж�
	{
		Res =USART_ReceiveData(USART6);	//��ȡ���յ�������
	    USART_SendData(USART1, Res);//���ܵ�����֮�󷵻ظ�����1
        
	    if( modbus.reflag==1)  //�����ݰ����ڴ���
	    {
		   return ;
	    }		
	    modbus.rcbuf[modbus.recount++] = Res;
		modbus.timout = 0;
		if(modbus.recount == 1)  //�Ѿ��յ��˵ڶ����ַ�����
		{
            modbus.timrun = 1;  //����modbus��ʱ����ʱ
        }
	  }	
} 

