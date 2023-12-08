#ifndef __MODBUS_TIMER_H
#define __MODBUS_TIMER_H	 
#include "stm32f4xx.h"


//我们需要识别的是一个报文
//报头，报尾
//按照包与包之间的时间间隔来来判断没有接收到新的数据，我们认为数据接收完成

extern uint8_t sec_flag;
extern int time;
void Modbus_TIME3_Init(void);
		 				    
#endif




