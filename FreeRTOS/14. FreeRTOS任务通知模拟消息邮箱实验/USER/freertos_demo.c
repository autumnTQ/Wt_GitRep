/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 3            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */
/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();                    /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t)task2,
                (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIO,
                (TaskHandle_t *)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

// 任务1，发送任务通知值
void task1(void *pvParameters)
{
    uint8_t serial_buf;

    while (1)
    {
        serial_buf = USART_RX_BUF[0]; // 接收串口数据
        USART_RX_BUF[0] = 0;
        //判断接收到的数据，并且任务2创建成功                       
        if ((serial_buf == 0x01) && (Task2Task_Handler!= NULL))  
        {
            printf("发送任务通知模拟消息邮箱发送，发送的消息为:%d\r\n", serial_buf);
            //用于往指定任务发送任务通知，通知方式可以自由指定，并且不获取发送任务通知前任务通知的通知值
            xTaskNotify(Task2Task_Handler, serial_buf, eSetValueWithOverwrite);
        }
        vTaskDelay(10); /* 延时10ticks */
    }
}

// 任务2，接收任务通知值
void task2(void *pvParameters)
{
    uint32_t noyify_val = 0;
    while (1)
    {   //用于等待任务通知通知值中的指定比特位被置一， 此函数可以在等待前和成功等待
        //到任务通知通知值中的指定比特位被置一后清零指定比特位，并且还能获取等待超时后任务通知的通知值。
        xTaskNotifyWait(0, 0xFFFFFFF, &noyify_val, portMAX_DELAY);
        switch(noyify_val)
        {
            case 0x01:
            {
                printf("接收到的通知值为：%d\r\n", noyify_val);
                break;
            }
            default: break;
        }
    }
}
