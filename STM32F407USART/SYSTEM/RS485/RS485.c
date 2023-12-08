#include "RS485.h"


//TX引脚是USART外设控制的输出脚，所以选用复用推挽输出
//RX引脚是USART外设数据输入脚，所以要选择输入模式，输入模式不分什么普通输入还是复用输入
//一根线只能有一个输出，但可以有多个输入，所以输入脚，外设和GPIO都可以同时使用

//选择串口USART6  TX_PC6复用推挽输出  RX_PC7上拉输入或者浮空输入
void RS485_Init(void)
{   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;    

	//配置 TX_PC6复用推挽输出	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//配置 RX_PC7上拉输入	
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
        
    //USART6   NVIC配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //子优先级
    NVIC_Init(&NVIC_InitStructure);
    
    //开启接受数据中断
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

    //使能串口
    USART_Cmd(USART6, ENABLE);
}


//modbus串口发送一个字节数据
void Modbus_Send_Byte(uint8_t Modbus_byte)
{
    USART_SendData(USART6,Modbus_byte);
    
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
    
	USART_ClearFlag(USART6, USART_FLAG_TC); 
}

//modbus串口中断服务程序
void USART6_IRQHandler(void)                
{
    uint8_t st,Res;
    
	st = USART_GetITStatus(USART6, USART_IT_RXNE);
    
	if(st == SET)//接收中断
	{
		Res =USART_ReceiveData(USART6);	//读取接收到的数据
	    USART_SendData(USART1, Res);//接受到数据之后返回给串口1
        
	    if( modbus.reflag==1)  //有数据包正在处理
	    {
		   return ;
	    }		
	    modbus.rcbuf[modbus.recount++] = Res;
		modbus.timout = 0;
		if(modbus.recount == 1)  //已经收到了第二个字符数据
		{
            modbus.timrun = 1;  //开启modbus定时器计时
        }
	  }	
} 

