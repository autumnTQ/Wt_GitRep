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

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO      2                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      2                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO      2                   /* �������ȼ� */
#define TASK3_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task3Task_Handler;  /* ������ */
void task3(void *pvParameters);             /* ������ */
/******************************************************************************************************/

QueueHandle_t xiao_queue;    //С����
QueueHandle_t da_queue;    //�����

char buff[100] = {"�������Ķ���"};
/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    //���еĴ���
    xiao_queue = xQueueCreate(2, sizeof(uint8_t));
    if(xiao_queue != NULL)
    {
        printf("xiao_queue���д����ɹ�\r\n");
    }
    
    da_queue = xQueueCreate(1, sizeof(char *));
    if(da_queue != NULL)
    {
        printf("da_queue���д����ɹ�\r\n");
    }
    
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler();
}


/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�) 
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
     
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* ��������3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
                
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

// ����1task1 ʵ�����
void task1(void *pvParameters)
{
    uint8_t serial_buf;
    char *buf;
    BaseType_t err = 0;
    
    buf = buff;
    
    while(1)
    {
        serial_buf = USART_RX_BUF[0];    //��ȡ���ڽ��յ�
        USART_RX_BUF[0] = 0;
        if(serial_buf == 0x11)
        {
            err = xQueueSend( xiao_queue, &serial_buf, portMAX_DELAY );
            if(err != pdTRUE)
            {
                printf("xiao_queue���з���ʧ��\r\n");
            }
        }
        
        if(serial_buf == 0x22)
        {
            err = xQueueSend( da_queue, &buf, portMAX_DELAY);
            if(err != pdTRUE)
            {
                printf("da_queue���з���ʧ��\r\n");
            }
        }
       
        vTaskDelay(10);    /* ��ʱ1000ticks */
    }
}

// ����2task2 С���ݳ�����
void task2(void *pvParameters)
{
    uint8_t serial_buf;
    BaseType_t err;
    
    while(1)
    {
        err = xQueueReceive( xiao_queue, &serial_buf, portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("xiao_queue���ж�ȡʧ��\r\n");
        }
        else
        {
            printf("%c", serial_buf); 
        }
    }
}

// ����3task3  �����ݳ���
void task3(void *pvParameters)
{
    char * buf;
    BaseType_t err;
    
    while(1)
    {
        err = xQueueReceive( da_queue, &buf, portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("da_queue���ж�ȡʧ��\r\n");
        }
        else
        {
            printf("%s", buf);
        }
     }
}

