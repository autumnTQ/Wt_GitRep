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
#include "malloc.h"	   
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

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
#define TASK1_PRIO      1                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      1                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/******************************************************************************************************/

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
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

/**
 * @brief       task1
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *pvParameters)
{    
    while(1)
    {
//        printf("task1����!!!\r\n");
        vTaskDelay(1000);                                               /* ��ʱ1000ticks */
    }
}


char task_buff[500];

/**
 * @brief       task2
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{
    UBaseType_t task_num = 0;
    TaskStatus_t * status_array = NULL;    //���ڻ�ȡ��������״̬������
    TaskStatus_t * status_array2 = NULL;   //���ڻ�ȡ��������״̬������
    uint8_t i = 0;
    
    task_num = uxTaskGetNumberOfTasks();  // ��ȡ���������
    status_array = mymalloc(SRAMIN, sizeof(TaskStatus_t)* task_num);
    task_num = uxTaskGetSystemState( (TaskStatus_t*)status_array, /* ����״̬��Ϣ buffer */
                                     (UBaseType_t)task_num,       /* buffer ��С */
                                     (uint32_t*)NULL);            /* ����ȡ��������ʱ����Ϣ */
    printf("������\t\t ���ȼ�\t\t ������\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%ld\t\t%ld\t\t\t%d\r\n",
                                status_array[i].pcTaskName,
                                status_array[i].uxCurrentPriority,
                                status_array[i].xTaskNumber,
                                status_array[i].usStackHighWaterMark);
    }
    
    status_array2 = mymalloc(SRAMIN, sizeof(TaskStatus_t));
    vTaskGetInfo( Task2Task_Handler, status_array2, pdTRUE, eInvalid);
    
    printf("��������%s\r\n", status_array2->pcTaskName);
    printf("�������ȼ���%ld\r\n", status_array2->uxCurrentPriority);
    printf("�����ţ�%ld\r\n", status_array2->xTaskNumber);
    printf("����״̬��%d\r\n", status_array2->eCurrentState);
    
    myfree(SRAMIN, status_array);  
    myfree(SRAMIN, status_array2);
    
    vTaskList(task_buff); //�ԡ���񡱵���ʽ��ȡϵͳ���������Ϣ
    printf("%s\r\n", task_buff);
    

    while(1)
    {
        vTaskDelay(1000);     /* ��ʱ1000ticks */
    }
}
