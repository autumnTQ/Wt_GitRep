#ifndef __SERIAL_USART_H
#define __SERIAL_USART_H
#include "stm32f4xx.h"                  // Device header
#include <stdio.h>


extern uint8_t table_data[9];//������ǰ����һ�������Ž��յ�������
extern uint8_t table_cp[9];//���Ƕ��ⶨ��һ�����飬�����յ������ݸ��Ƶ�������
extern uint16_t count;//�������ݼ���

//���ڳ�ʼ������
void Serial_USART(void);   //USART TX__PA9   RX__PA10

//���ڷ���һ���ֽڵ����ݺ�����һ���ֽ���8λ���ݣ�����Ĳ���������һ��8λ�ģ�
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data);

//���������ֽ����ݺ���
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data);

//����һ����������
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);

//�����ַ���
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str);//ָ�����ڣ�Ҫ���͵��ַ�������
void uart_init3(u32 bound);

#endif 


