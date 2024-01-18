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
#define TASK2_PRIO      3                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */
/******************************************************************************************************/

QueueSetHandle_t  queueset_handle;   //���м����
QueueHandle_t     queue_handle;    //���о��
QueueHandle_t     semphr_handle;     //��ֵ�ź������
/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    queueset_handle = xQueueCreateSet( 2 );   // �������м�
    if(queueset_handle != NULL)
    {
        printf("���м�queueset_handle�����ɹ�\r\n");
    }    
    queue_handle = xQueueCreate( 1, sizeof(uint8_t) );   // ��������
    semphr_handle = xSemaphoreCreateBinary();            // ������ֵ�ź���
    
    xQueueAddToSet( queue_handle, queueset_handle );
    xQueueAddToSet( semphr_handle, queueset_handle );
    
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
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

// ����1��ʵ�ֶ��з����Լ��ź����ͷ�
void task1(void *pvParameters)
{
    uint8_t serial_buf;    
    BaseType_t err = 0;
       
    while(1)
    {
        serial_buf = USART_RX_BUF[0];    //��ȡ���ڽ��յ�
        USART_RX_BUF[0] = 0;
        if(serial_buf == 0x11)
        {
            err = xQueueSend( queue_handle, &serial_buf, portMAX_DELAY );  //�������д������
            if(err == pdPASS)
            {
                printf("��queue_handleд�����ݳɹ�\r\n");
            }
        }
        else if(serial_buf == 0x22)
        {
            err = xSemaphoreGive( semphr_handle );      //�ͷŶ�ֵ�ź���
            if(err == pdPASS)
            {
                printf("�ͷ��ź����ɹ�\r\n");
            }
        }
        vTaskDelay(10);    /* ��ʱ1000ticks */
    }
}

// ����2����ȡ���м�����Ϣ
void task2(void *pvParameters)
{
    uint8_t serial_buf;
    QueueSetMemberHandle_t member_handle;
    
    while(1)
    {
        //�������л�ȡ���м�������Ч��Ϣ�Ķ���
        member_handle = xQueueSelectFromSet( queueset_handle, portMAX_DELAY);
        if(member_handle == queue_handle)
        {
            xQueueReceive( queue_handle, &serial_buf, portMAX_DELAY);
            printf("��ȡ���Ķ�������Ϊ��%d\r\n", serial_buf);
        }
        else if(member_handle == semphr_handle)
        {
            xSemaphoreTake( semphr_handle, portMAX_DELAY);
            printf("��ȡ�ź����ɹ�\r\n");
        }
        
    }
}


