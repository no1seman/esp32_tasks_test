#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "main.h"

#define BLINK_GPIO 5
#define BLINK_GPIO_BIT_MASK (1ULL << BLINK_GPIO)
#define BLINK_PERIOD (1000)

char *TAG = "tasks_test";

static TaskHandle_t blink_task;
static uint32_t counter;

static void blink_task_function(void *pvParameter)
{
    while (1)
    {
        counter++;
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(BLINK_PERIOD / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(BLINK_PERIOD / portTICK_PERIOD_MS);
    }

    gpio_set_level(BLINK_GPIO, 0);
    ESP_LOGI(TAG, "Blink task stopped");
    vTaskDelete(NULL);
}

esp_err_t blink_task_start(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BLINK_GPIO_BIT_MASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    //BaseType_t ret = xTaskCreatePinnedToCore(blink_task_function, "BLINK", 2048, NULL, 5, &blink_task, 1);
    BaseType_t ret = xTaskCreate(blink_task_function, "BLINK", 2048, NULL, 5, &blink_task);

    if (ret == pdPASS)
    {
        ESP_LOGI(TAG, "Blink task started");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "Blink task start failed! Error: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }
}

esp_err_t blink_task_stop(void)
{
    vTaskDelete(blink_task);
    gpio_set_level(BLINK_GPIO, 0);
    ESP_LOGI(TAG, "Blink task stopped");
    return ESP_OK;
}

void print_tasks(void)
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime = 0;
    UBaseType_t task_stack_size, task_stack_used;
    task_control_block_t *TCB = NULL;
    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL)
    {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        for (x = 0; x < uxArraySize; x++)
        {
            TCB = (task_control_block_t *)pxTaskStatusArray[x].xHandle;
            task_stack_size = (UBaseType_t)(TCB->pxEndOfStack) - (UBaseType_t)(TCB->pxStack) + 4;
            task_stack_used = (UBaseType_t)(TCB->pxEndOfStack) - (UBaseType_t)(TCB->pxTopOfStack) + 4;
            ESP_LOGI(TAG, "Task name: '%s', Stack size: %u, Stack used: %u, HWM: %u",
                     pxTaskStatusArray[x].pcTaskName,
                     task_stack_size,
                     task_stack_used,
                     pxTaskStatusArray[x].usStackHighWaterMark);
        }

        if (pxTaskStatusArray)
        {
            vPortFree(pxTaskStatusArray);
            pxTaskStatusArray = NULL;
        }
    }
    ESP_LOGI(TAG,"");
}

void app_main()
{
    blink_task_start();

    while (1)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        print_tasks();
    }
}
