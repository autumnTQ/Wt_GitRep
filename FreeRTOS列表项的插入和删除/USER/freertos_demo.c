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
#include "list.h"

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

/******************************************************************************************************/

List_t         TestList;     //��������б�
ListItem_t     ListItem1;    //��������б���1
ListItem_t     ListItem2;    //��������б���2
ListItem_t     ListItem3;    //��������б���3

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
        printf("task1����!!!\r\n");
        vTaskDelay(1000);                                               /* ��ʱ1000ticks */
    }
}

/**
 * @brief       task2 ʵ���б�������ɾ��
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{   
    
    vListInitialise(&TestList);        //��ʼ���б�
    vListInitialiseItem(&ListItem1);   //��ʼ���б���
    vListInitialiseItem(&ListItem2);   //��ʼ���б���
    vListInitialiseItem(&ListItem3);   //��ʼ���б���
    
    ListItem1.xItemValue = 40;         //�����б����ֵ
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;
    
    /* �ڶ�������ӡ�б�������б���ĵ�ַ */
    printf("/**************�ڶ�������ӡ�б���б���ĵ�ַ**************/\r\n");
    printf("��Ŀ\t\t\t��ַ\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("/**************************����***************************/\r\n");
    
    /* ���������б���1�����б� */
    printf("/r/n/*****************���������б���1�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */    //��������б�
                (ListItem_t*)&ListItem1);       /* �б��� */
                
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));          //�б������һ���б���ָ�����һ���б���
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));                         //�б���1ָ�����һ���б���
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));  //�б������һ���б���ָ�����һ��
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));                 //�б���1ָ�����һ��
    printf("/**************************����***************************/\r\n");
    while(1)
    {
        printf("task2����!!!\r\n");
        vTaskDelay(1000);                                               /* ��ʱ1000ticks */
    }
}






