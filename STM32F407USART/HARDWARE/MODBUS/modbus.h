#ifndef __MODBUS_h_
#define __MODBUS_h_
#include "stm32f4xx.h"                  // Device header
#include "modbus_crc.h"
#include "modbus_timer.h"
#include "Serial_USART.h"
#include "delay.h"
#include "RS485.h"

typedef struct 
{
	//��Ϊ�ӻ�ʱʹ��
    u8  myadd;        //���豸�ӻ���ַ
	u8  rcbuf[100];   //modbus���ܻ�����
	u8  timout;       //modbus���ݳ���ʱ��
	u8  recount;      //modbus�˿ڽ��յ������ݸ���
	u8  timrun;       //modbus��ʱ���Ƿ��ʱ��־
	u8  reflag;       //modbusһ֡���ݽ�����ɱ�־λ
	u8  sendbuf[100]; //modbus�ӷ��ͻ�����
	
	//��Ϊ������Ӳ���
	u8 Host_Txbuf[8];	//modbus��������	
	u8 slave_add;		//Ҫƥ��Ĵӻ��豸��ַ��������ʵ��ʱʹ�ã�
	u8 Host_send_flag;//�����豸����������ϱ�־λ
	int Host_Sendtime;//������һ֡���ݺ�ʱ�����
	u8 Host_time_flag;//����ʱ�䵽��־λ��=1��ʾ����������ʱ����
	u8 Host_End;//�������ݺ������
}MODBUS;


//typedef struct  
//{
//    u8 addr;//�ӻ���ַ
//    u8 start;//�Ĵ�����ʼ
//    u8 len;  //���յ�������͵ļĴ�����
//    u8 flag;
//    u8 buf[100];//�Ĵ�������

////}Host_slave; //�û�����

//extern Host_slave Modbus_TX;
//extern Host_slave Modbus_RX;


extern MODBUS modbus;
extern u16 Reg[];
void Modbus_Init(void);
void Modbus_Func3(void);//���Ĵ�������
void Modbus_Func6(void);//��1���Ĵ�����д������
void Modbus_Func16(void);//������Ĵ�����д������
void Modbus_Event(void);



//void Host_send03(void);
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void Host_RX(void);
//�������մӻ�����Ϣ���д���
void HOST_ModbusRX(void);

#endif

