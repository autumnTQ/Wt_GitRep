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
 *	函 数 名: main
 *	入口参数: 无
 *	返 回 值: 无
 *	函数功能: 运行主程序,使用串口发送数据
 *	说    明: 使用USART1与上位机通信
 ****************************************************************************************************/

// 十进制转换为十六进制
int DectoHex(int32_t dec, unsigned char *hex, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        hex[i] = (dec % 256) & 0xFF;
        dec /= 256;
    }

    return 0;
}

unsigned char Set_HoldingJawOpen[] = {0xEB, 0x91, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00};  // 设置夹爪张开
unsigned char Set_HoldingJawClose[] = {0xEB, 0x91, 0x30, 0x06, 0x01, 0x00, 0x00, 0x00}; // 设置夹爪闭合
unsigned char Set_HoldingJawInit[] = {0xEB, 0x91, 0x30, 0x08, 0x00, 0x00, 0x00, 0x00};  // 设置夹爪上电后初始化动作

uint8_t j = 0;
uint8_t h = 0;

uint32_t val = 1000000; // 测试强转的实验，结果:舍弃高位保留低位   同样数据保存是小端先存  1000000 = 0xF4240  存储为->40 42 0F
uint8_t val1 = 0;

int main(void)
{
    Delay_Init();        // 延时函数初始化
    LED_Init();          // LED初始化
    KEY_Init();          // 按键IO口初始化
    SPI_LCD_Init();      // LCD初始化
    PWM_Init();          // PWM初始化
    LCD_Test_Clear();    // 清屏测试 显示固定文本
    Serial_USART();      // 串口初始化USART1（115200）（只用来打印测试数据）
    RS485_Init();        // 初始化RS485控制引脚USART6
    Modbus_TIME3_Init(); // 定时器初始化TIM3   1ms中断一次
    Modbus_Init();       // MODBUS初始化--本机做作为从机设备地址，本机要匹配的从机地址
    uart_init3(9600);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级

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
