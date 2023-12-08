#ifndef __RS485_H
#define __RS485_H
#include "stm32f4xx.h"                  // Device header
#include "modbus.h"
#include "modbus_timer.h"

void RS485_Init(void);
void Modbus_Send_Byte(uint8_t Modbus_byte);


#endif

