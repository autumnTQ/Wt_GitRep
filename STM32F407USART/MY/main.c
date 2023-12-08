#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "LCD_Test.h"
#include "Serial_USART.h"
#include "PWM.h"
#include "RS485.h"
#include "modbus_timer.h"
#include "modbus.h"

/***************************************************************************************************
 *	�� �� ��: main
 *	��ڲ���: ��
 *	�� �� ֵ: ��
 *	��������: ����������,ʹ�ô��ڷ�������
 *	˵    ��: ʹ��USART1����λ��ͨ��
 ****************************************************************************************************/

// ʮ����ת��Ϊʮ������
int DectoHex(int32_t dec, unsigned char *hex, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        hex[i] = (dec % 256) & 0xFF;
        dec /= 256;
    }

    return 0;
}

unsigned char Set_HoldingJawOpen[] = {0xEB, 0x91, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00};  // ���ü�צ�ſ�
unsigned char Set_HoldingJawClose[] = {0xEB, 0x91, 0x30, 0x06, 0x01, 0x00, 0x00, 0x00}; // ���ü�צ�պ�
unsigned char Set_HoldingJawInit[] = {0xEB, 0x91, 0x30, 0x08, 0x00, 0x00, 0x00, 0x00};  // ���ü�צ�ϵ���ʼ������

uint8_t j = 0;
uint8_t h = 0;

uint32_t val = 1000000; // ����ǿת��ʵ�飬���:������λ������λ   ͬ�����ݱ�����С���ȴ�  1000000 = 0xF4240  �洢Ϊ->40 42 0F
uint8_t val1 = 0;

int main(void)
{
    Delay_Init();        // ��ʱ������ʼ��
    LED_Init();          // LED��ʼ��
    KEY_Init();          // ����IO�ڳ�ʼ��
    SPI_LCD_Init();      // LCD��ʼ��
    PWM_Init();          // PWM��ʼ��
    LCD_Test_Clear();    // �������� ��ʾ�̶��ı�
    Serial_USART();      // ���ڳ�ʼ��USART1��115200����ֻ������ӡ�������ݣ�
    RS485_Init();        // ��ʼ��RS485��������USART6
    Modbus_TIME3_Init(); // ��ʱ����ʼ��TIM3   1ms�ж�һ��
    Modbus_Init();       // MODBUS��ʼ��--��������Ϊ�ӻ��豸��ַ������Ҫƥ��Ĵӻ���ַ
    uart_init3(9600);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    val1 = (unsigned char)val;

    while (1)
    {
        if (KEY_Scan() == KEY_ON)
        {
            j++;
            if ((j == 1) && (h == 0))
            {
                for (int i = 0; i < 8; i++)
                {
                    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
                        ;
                    USART_SendData(USART3, Set_HoldingJawInit[i]);
                }
                h = 1;
            }
            if ((j == 2) && (h == 1))
            {
                for (int i = 0; i < 8; i++)
                {
                    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
                        ;
                    USART_SendData(USART3, Set_HoldingJawClose[i]);
                }
                h = 2;
            }
            if ((j == 3) && (h == 2))
            {
                for (int i = 0; i < 8; i++)
                {
                    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
                        ;
                    USART_SendData(USART3, Set_HoldingJawOpen[i]);
                }
                j = 0;
            }
        }
    }
}
