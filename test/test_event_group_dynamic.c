/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-24     yangjie      the first version
 * 2020-10-17     Meco Man     translate to English comment
 */

/*
 * Demo: event group
 *
 * This demo demonstrates dynamically creating an event group and using it to send events between two tasks.
 *
 */

#include <FreeRTOS.h>
#include <event_groups.h>
#include <task.h>

#define TASK_PRIORITY         8
#define BIT_0                 ( 1 << 0 )
#define BIT_4                 ( 1 << 4 )

/* mutex handler */
static EventGroupHandle_t xEventGroup = NULL;
static TaskHandle_t TaskHandle1 = NULL;
static TaskHandle_t TaskHandle2 = NULL;

static void vTask1Code(void *pvParameters)
{
    while (1)
    {
        EventBits_t uxBits;
        // Wait for either bit 0 or bit 4, clear bit before return
        uxBits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_4, pdTRUE, pdFALSE, portMAX_DELAY);
        if ((uxBits & BIT_0) != 0)
        {
            rt_kprintf("Task 1 received bit 0\n");
        }
        if ((uxBits & BIT_4) != 0)
        {
            rt_kprintf("Task 1 received bit 4\n");
        }
    }
}

static void vTask2Code(void * pvParameters)
{
    while (1)
    {
        rt_kprintf("Task 2 send bit 0\n");
        xEventGroupSetBits(xEventGroup, BIT_0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        rt_kprintf("Task 2 send bit 4\n");
        xEventGroupSetBits(xEventGroup, BIT_4);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int event_group_dynamic(void)
{
    xEventGroup = xEventGroupCreate();
    xEventGroupDelete(xEventGroup);
    xEventGroup = xEventGroupCreate();
    if (xEventGroup == NULL)
    {
        rt_kprintf("create event group failed.\n");
        return -1;
    }
    xTaskCreate( vTask1Code, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY - 1, &TaskHandle1 );
    if (TaskHandle1 == NULL)
    {
        rt_kprintf("Create task 1 failed\n");
        return -1;
    }
    xTaskCreate( vTask2Code, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, &TaskHandle2 );
    if (TaskHandle2 == NULL)
    {
        rt_kprintf("Create task 2 failed\n");
        return -1;
    }

    return 0;
}

MSH_CMD_EXPORT(event_group_dynamic, event group sample);
