/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
//#include "led_strip_encoder.h"
#include "led_control.h"

static void led_task(void *arg)
{
    while (1) {
        led_strip_tranz();
        //led_rainbow();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    led_setup();
    xTaskCreate(led_task, "led_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
