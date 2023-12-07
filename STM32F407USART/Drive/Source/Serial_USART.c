#include "Serial_USART.h"



//串口初始化 使用USART1 配置波特率为115200
void Serial_USART()   //USART TX__PA9   RX__PA10
{
    GPIO_InitTypeDef GPIO_InitStructure;      //TX(PA9)引脚配置为推挽输出模式
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //复用
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //有上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        // RX(PA10)引脚配置为上拉输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    //配置串口USART1  波特率为115200  8字节 1个停止位 无奇偶校验  
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
	
	//配置NVIC中断

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //子优先级3
    NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    //打开串口接收数据中断
    USART_Cmd(USART1, ENABLE);                       //使能USART1
}



//uint8_t是8位的数据，uint16_t是16位的数据
//当uint8_t的8位数据传递给uint16_t的16位数据时，会自动强制类型转换

//串口发送一个字节的数据函数（一个字节是8位数据，定义的参数变量是一个8位的）
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data)//每次只能发送8位的数据
{
	//调用固件库函数
	USART_SendData(pUSARTx,data);//往串口中写入数据
	
	//发送完数据是检测TXE这个位是否置1，发送数据寄存器空了，表明已经把数据传递到数据移位寄存器了
	//检测TXE这个位也需要一个固件库函数
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
	//如果这个位一直为0的话就一直等待，只有当被设为SET后才会跳出这个循环（表示一个字节发送出去了）

}



//有时候传感器数据可能是16位的，怎么发送？发送两个字节
//发送两个字节的数据就是十六位的
//半字表示16位，两个字节，，参数2是十六位的数据

//发送两个字节数据函数
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data)
{
	//发送十六位数据要分为两次来发送，先定义两个变量
	uint8_t temp_h,temp_l;//定义8位的变量（分别存储高8位和低8位）

	//首先取出高8位
	temp_h=(data&0xff00)>>8;//低八位先与0相&，低8位变为0再右移8位（0xff00共16位二进制）
	//再取出低8位
	temp_l=data&0xff;//取出低8位数据
	//16位的数据这样子就放到了两个变量里面（共16位）
	
	//调用固件库函数
	USART_SendData(pUSARTx,temp_h);//先发送高8位
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//等待数据发送完毕

	USART_SendData(pUSARTx,temp_l);//再发送低8位
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//等待数据发送完毕

}


//有时候数据很多想要发送数组：数组数据是8位的 
//发送8位数据的数组--需要写一个循环来调用发送一个字节的函数即可
//数组的话传进来的就是一个指针了

//发送一个数组数据
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num)
{
	//每次想要发送多少数据，通过形参num传进来，num定义的是8位的，那么函数最多发送255个
	int i;
	for(i=0;i<num;i++)
	{
		//调用发送一个字节函数发送数据（下面两种写法都可以）
		//Usart_SendByte(USART1,*array++);
		Usart_SendByte(USART1,array[i]);//每次只能发送8位数据
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//等待发送完毕
}
//判断发送一个字节的数据标志位：USART_FLAG_TXE
//判断发送一连串字节的数据标志位：USART_FLAG_TC



//发送字符串
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str)//指定串口，要发送的字符串内容
{
	uint8_t i=0;
	//使用do-while循环，do的时候已经开始发送了
	do{
		//需要调用发送一个字节函数
		Usart_SendByte(USART1,*(str+i));//发送一次之后指针地址后移一个
		i++;
	}while(*(str+i)!='\0');//最后结尾不等于'\0'为真，继续发送
	//如果='\0'表示发送完毕
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//等待发送完毕
}



/**********************************************************************/
//getchar()等价于scanf()函数
//如果使用getchar函数也需要重新定义

////重定向c库函数scanf到串口，重写后可以使用scanf和getchar函数
int fgetc(FILE *f)
{
	//等待串口输入数据
	/* 有了这个等待就不需要在中断中进行了 */
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	return (int)USART_ReceiveData(USART1);
}
//如果在主函数中使用getchar()需要把下面的中断设置代码注释掉，否则会冲突


//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    
    USART_SendData(USART1,(uint8_t)ch);  
    
	return ch;
}

////中断服务函数（串口助手发送什么数据，就会返回什么数据）
//void USART1_IRQHandler() 
//{
//	uint8_t ucTemp;
//	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET)
//	{
//		ucTemp=USART_ReceiveData(USART1);
//		USART_SendData(USART1, ucTemp);
//	}
//}


uint8_t table_data[9];//这是提前定义一个数组存放接收到的数据
uint8_t table_cp[9];//这是额外定义一个数组，将接收到的数据复制到这里面
uint16_t count=0;//接收数据计数

////使用自定义协议接收十六进制数据

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//		u8 Res,i;
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//		{
//			Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
//			
//			if(count==0)//如果是接收的第一个数据
//			{
//				table_data[count]=Res;//将第一个数据存到数据中第一元素
//				if(table_data[0]==0x2c)//判断接收的第一个数据是不是十六进制0X2C
//				  count++;//如果第一个数据是0X2C则表示正确计数+1
//			}
//			else if(count==1)//第一个数据接收正确的情况下，判断第二个数据
//			{
//				if(Res==0xe4)//如果刚接收的数据是0xE4则表示数据正确
//				{
//					table_data[count]=Res;//将数据储存到数组第二个元素位置
//					count++;//接收数据计数+1
//				}
//				else//如果第二个字符不是0XE4则计数清零，重新接收
//					count=0;
//			}
//			else if(count==2&&Res==0)//如果前两个数据正确，接收的第三个数据是0，则清零计数，重新接收数据
//			{
//				count=0;
//			}
//			else if(count>1&&count<9)//这是可以接收数据的范围，只要count在数据可接收数据范围内即可进行存入数据
//			{
//				table_data[count]=Res;
//				count++;
//			}
//			else if(count>=9)//如果接收数据超过数组大小，则清零重新接收
//			{
//				count=0;
//			}		
//   } 
//	 
//		memset(table_cp, 0, sizeof(table_data));//在使用数组table_cp时清空
//		for(i=0;i<9;i++)//把接收到的数据复制到table_cp数组中
//		{
//			 table_cp[i]= table_data[i];
//		
//	}
//} 

//初始化IO 串口3  用于给夹爪发送指令  夹爪波特率默认9600
//bound:波特率   PB10_TX   PB11_RX
void uart_init3(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;   //GPIOB10与GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	     //速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);                    //初始化PB10，PB11

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;                                         //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                              //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                                 //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                    //收发模式
	USART_Init(USART3, &USART_InitStructure);                                           //初始化串口2
	
	USART_Cmd(USART3, ENABLE);  //使能串口3 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;           //串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;     //抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		    //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                            //根据指定的参数初始化VIC寄存器、
}

