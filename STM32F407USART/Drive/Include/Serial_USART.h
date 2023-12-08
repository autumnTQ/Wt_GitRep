#ifndef __SERIAL_USART_H
#define __SERIAL_USART_H
#include "stm32f4xx.h"                  // Device header
#include <stdio.h>


extern uint8_t table_data[9];//这是提前定义一个数组存放接收到的数据
extern uint8_t table_cp[9];//这是额外定义一个数组，将接收到的数据复制到这里面
extern uint16_t count;//接收数据计数

//串口初始化函数
void Serial_USART(void);   //USART TX__PA9   RX__PA10

//串口发送一个字节的数据函数（一个字节是8位数据，定义的参数变量是一个8位的）
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data);

//发送两个字节数据函数
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data);

//发送一个数组数据
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);

//发送字符串
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str);//指定串口，要发送的字符串内容
void uart_init3(u32 bound);

#endif 


