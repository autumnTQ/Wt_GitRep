#include "Serial_USART.h"



//���ڳ�ʼ�� ʹ��USART1 ���ò�����Ϊ115200
void Serial_USART()   //USART TX__PA9   RX__PA10
{
    GPIO_InitTypeDef GPIO_InitStructure;      //TX(PA9)��������Ϊ�������ģʽ
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //����
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        // RX(PA10)��������Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    //���ô���USART1  ������Ϊ115200  8�ֽ� 1��ֹͣλ ����żУ��  
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
	
	//����NVIC�ж�

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3
    NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    //�򿪴��ڽ��������ж�
    USART_Cmd(USART1, ENABLE);                       //ʹ��USART1
}



//uint8_t��8λ�����ݣ�uint16_t��16λ������
//��uint8_t��8λ���ݴ��ݸ�uint16_t��16λ����ʱ�����Զ�ǿ������ת��

//���ڷ���һ���ֽڵ����ݺ�����һ���ֽ���8λ���ݣ�����Ĳ���������һ��8λ�ģ�
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data)//ÿ��ֻ�ܷ���8λ������
{
	//���ù̼��⺯��
	USART_SendData(pUSARTx,data);//��������д������
	
	//�����������Ǽ��TXE���λ�Ƿ���1���������ݼĴ������ˣ������Ѿ������ݴ��ݵ�������λ�Ĵ�����
	//���TXE���λҲ��Ҫһ���̼��⺯��
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
	//������λһֱΪ0�Ļ���һֱ�ȴ���ֻ�е�����ΪSET��Ż��������ѭ������ʾһ���ֽڷ��ͳ�ȥ�ˣ�

}



//��ʱ�򴫸������ݿ�����16λ�ģ���ô���ͣ����������ֽ�
//���������ֽڵ����ݾ���ʮ��λ��
//���ֱ�ʾ16λ�������ֽڣ�������2��ʮ��λ������

//���������ֽ����ݺ���
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data)
{
	//����ʮ��λ����Ҫ��Ϊ���������ͣ��ȶ�����������
	uint8_t temp_h,temp_l;//����8λ�ı������ֱ�洢��8λ�͵�8λ��

	//����ȡ����8λ
	temp_h=(data&0xff00)>>8;//�Ͱ�λ����0��&����8λ��Ϊ0������8λ��0xff00��16λ�����ƣ�
	//��ȡ����8λ
	temp_l=data&0xff;//ȡ����8λ����
	//16λ�����������Ӿͷŵ��������������棨��16λ��
	
	//���ù̼��⺯��
	USART_SendData(pUSARTx,temp_h);//�ȷ��͸�8λ
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//�ȴ����ݷ������

	USART_SendData(pUSARTx,temp_l);//�ٷ��͵�8λ
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//�ȴ����ݷ������

}


//��ʱ�����ݺܶ���Ҫ�������飺����������8λ�� 
//����8λ���ݵ�����--��Ҫдһ��ѭ�������÷���һ���ֽڵĺ�������
//����Ļ��������ľ���һ��ָ����

//����һ����������
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num)
{
	//ÿ����Ҫ���Ͷ������ݣ�ͨ���β�num��������num�������8λ�ģ���ô������෢��255��
	int i;
	for(i=0;i<num;i++)
	{
		//���÷���һ���ֽں����������ݣ���������д�������ԣ�
		//Usart_SendByte(USART1,*array++);
		Usart_SendByte(USART1,array[i]);//ÿ��ֻ�ܷ���8λ����
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//�ȴ��������
}
//�жϷ���һ���ֽڵ����ݱ�־λ��USART_FLAG_TXE
//�жϷ���һ�����ֽڵ����ݱ�־λ��USART_FLAG_TC



//�����ַ���
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str)//ָ�����ڣ�Ҫ���͵��ַ�������
{
	uint8_t i=0;
	//ʹ��do-whileѭ����do��ʱ���Ѿ���ʼ������
	do{
		//��Ҫ���÷���һ���ֽں���
		Usart_SendByte(USART1,*(str+i));//����һ��֮��ָ���ַ����һ��
		i++;
	}while(*(str+i)!='\0');//����β������'\0'Ϊ�棬��������
	//���='\0'��ʾ�������
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//�ȴ��������
}



/**********************************************************************/
//getchar()�ȼ���scanf()����
//���ʹ��getchar����Ҳ��Ҫ���¶���

////�ض���c�⺯��scanf�����ڣ���д�����ʹ��scanf��getchar����
int fgetc(FILE *f)
{
	//�ȴ�������������
	/* ��������ȴ��Ͳ���Ҫ���ж��н����� */
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	return (int)USART_ReceiveData(USART1);
}
//�������������ʹ��getchar()��Ҫ��������ж����ô���ע�͵���������ͻ


//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    
    USART_SendData(USART1,(uint8_t)ch);  
    
	return ch;
}

////�жϷ��������������ַ���ʲô���ݣ��ͻ᷵��ʲô���ݣ�
//void USART1_IRQHandler() 
//{
//	uint8_t ucTemp;
//	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET)
//	{
//		ucTemp=USART_ReceiveData(USART1);
//		USART_SendData(USART1, ucTemp);
//	}
//}


uint8_t table_data[9];//������ǰ����һ�������Ž��յ�������
uint8_t table_cp[9];//���Ƕ��ⶨ��һ�����飬�����յ������ݸ��Ƶ�������
uint16_t count=0;//�������ݼ���

////ʹ���Զ���Э�����ʮ����������

//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//		u8 Res,i;
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//		{
//			Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//			
//			if(count==0)//����ǽ��յĵ�һ������
//			{
//				table_data[count]=Res;//����һ�����ݴ浽�����е�һԪ��
//				if(table_data[0]==0x2c)//�жϽ��յĵ�һ�������ǲ���ʮ������0X2C
//				  count++;//�����һ��������0X2C���ʾ��ȷ����+1
//			}
//			else if(count==1)//��һ�����ݽ�����ȷ������£��жϵڶ�������
//			{
//				if(Res==0xe4)//����ս��յ�������0xE4���ʾ������ȷ
//				{
//					table_data[count]=Res;//�����ݴ��浽����ڶ���Ԫ��λ��
//					count++;//�������ݼ���+1
//				}
//				else//����ڶ����ַ�����0XE4��������㣬���½���
//					count=0;
//			}
//			else if(count==2&&Res==0)//���ǰ����������ȷ�����յĵ�����������0����������������½�������
//			{
//				count=0;
//			}
//			else if(count>1&&count<9)//���ǿ��Խ������ݵķ�Χ��ֻҪcount�����ݿɽ������ݷ�Χ�ڼ��ɽ��д�������
//			{
//				table_data[count]=Res;
//				count++;
//			}
//			else if(count>=9)//����������ݳ��������С�����������½���
//			{
//				count=0;
//			}		
//   } 
//	 
//		memset(table_cp, 0, sizeof(table_data));//��ʹ������table_cpʱ���
//		for(i=0;i<9;i++)//�ѽ��յ������ݸ��Ƶ�table_cp������
//		{
//			 table_cp[i]= table_data[i];
//		
//	}
//} 

//��ʼ��IO ����3  ���ڸ���צ����ָ��  ��צ������Ĭ��9600
//bound:������   PB10_TX   PB11_RX
void uart_init3(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;   //GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	     //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);                    //��ʼ��PB10��PB11

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;                                         //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                              //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                                 //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                    //�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure);                                           //��ʼ������2
	
	USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;           //����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;     //��ռ���ȼ�7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		    //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                            //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

