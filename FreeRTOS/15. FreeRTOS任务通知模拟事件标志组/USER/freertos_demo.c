/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ��ֲʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽����F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "usart.h"
#include "led.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1            /* �������ȼ� */
#define START_STK_SIZE 128           /* �����ջ��С */
TaskHandle_t StartTask_Handler;      /* ������ */
void start_task(void *pvParameters); /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO 2            /* �������ȼ� */
#define TASK1_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task1Task_Handler; /* ������ */
void task1(void *pvParameters); /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO 3            /* �������ȼ� */
#define TASK2_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
void task2(void *pvParameters); /* ������ */
/******************************************************************************************************/
#define EVENTBIT_0 0X01               // �¼���־λ
#define EVENTBIT_1 0X02

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t)start_task,          /* ������ */
                (const char *)"start_task",          /* �������� */
                (uint16_t)START_STK_SIZE,            /* �����ջ��С */
                (void *)NULL,                        /* ������������Ĳ��� */
                (UBaseType_t)START_TASK_PRIO,        /* �������ȼ� */
                (TaskHandle_t *)&StartTask_Handler); /* ������ */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();                    /* �����ٽ��� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t)task2,
                (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIO,
                (TaskHandle_t *)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

// ����1����������ֵ֪ͨ
void task1(void *pvParameters)
{
    uint8_t serial_buf;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // ��ȡ���ڽ��յ�������
        USART_RX_BUF[0] = 0;
        if (serial_buf == 0x01)
        {
             printf("��bit0λ��1\r\n");
            //��������ֵ֪ͨ������2����bit0��1�� ����3Ϊ����֪ͨ��ʽ������ָ��bit��
            xTaskNotify(Task2Task_Handler, EVENTBIT_0, eSetBits);
        }
        else if (serial_buf == 0x02)
        {
             printf("��bit1λ��1\r\n");
            //��������ֵ֪ͨ������2����bit1��1�� ����3Ϊ����֪ͨ��ʽ������ָ��bit��
            xTaskNotify(Task2Task_Handler, EVENTBIT_1, eSetBits);
        }
        vTaskDelay(10); /* ��ʱ1000ticks */
    }
}

// ����2����������ֵ֪ͨ
void task2(void *pvParameters)
{
    uint32_t notify_val = 0;
    uint32_t event_bit = 0;
    while (1)
    {
        //����1����ȥ֮ǰ�Ƿ����ԭ�ȵ�ֵ��0Ϊ�������
        //����2���˳�֮��������е�λ
        xTaskNotifyWait( 0, 0xFFFFFFFF, &notify_val, portMAX_DELAY );
        if(notify_val & EVENTBIT_0)  //���bit0λ�Ѿ�����1��
        {
            event_bit |= EVENTBIT_0;
        }
        if(notify_val & EVENTBIT_1)  //���bit1λҲ����1��
        {
            event_bit |= EVENTBIT_1;
        }
        if(event_bit == (EVENTBIT_0 | EVENTBIT_1))  //���bit0��bit1������1��
        {
            printf("����֪ͨģ���¼���־����ճɹ�����\r\n");
            event_bit = 0;  //����
        }
    }
}
