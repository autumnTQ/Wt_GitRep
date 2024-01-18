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

TimerHandle_t timer1_handle;   //���ζ�ʱ�����
TimerHandle_t timer2_handle;   //���ڶ�ʱ�����
void timer1_callback( TimerHandle_t pxTimer );  //������ζ�ʱ���ص�����
void timer2_callback( TimerHandle_t pxTimer );  //������ڶ�ʱ���ص�����
/******************************************************************************************************/

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
    
    //�������������ʱ�� 
    timer1_handle =  xTimerCreate( "timer1",
                                    1000,
                                    pdFALSE,    //���ζ�ʱ��
                                    (void *)1,
                                    timer1_callback );
    //�������������ʱ�� 
   timer2_handle =  xTimerCreate( "timer2",
                                    1000,      //1000ϵͳʱ�ӽ��� 1s
                                    pdTRUE,    //���ڶ�ʱ��
                                    (void *)1,
                                    timer2_callback );
    /* ��������1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

// ����1������ɨ�貢�ҿ��������ʱ��
void task1(void *pvParameters)
{
    uint8_t serial_buf;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // ��ȡ���ڽ��յ�������
        USART_RX_BUF[0] = 0;
        if (serial_buf == 0x01)   //������ʱ��
        {
            xTimerStart(timer1_handle, portMAX_DELAY);
            xTimerStart(timer2_handle, portMAX_DELAY);
        }
        else if (serial_buf == 0x02)  //�رն�ʱ��
        {
            xTimerStop(timer1_handle, portMAX_DELAY);
            xTimerStop(timer2_handle, portMAX_DELAY);
        }
        vTaskDelay(10); /* ��ʱ10ticks */
    }
}

//timer1�ĳ�ʱ�ص�����
void timer1_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer1�����д�����%d\r\n", ++timer);
}

//timer2�ĳ�ʱ�ص�����
void timer2_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer2�����д�����%d\r\n", ++timer);
}

