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
    //int i = 0;
    // while (1/*i<20*/) {
    //     /*
    //         blink_strip(led_color_code, time to blink for(ms) , delay time between blinks(ms))
    //         color code:  0:White 1:Purple 2:Blue 3:Green 4:Yellow 5:Orange 6:Red
    //     */
    //     //blink_strip(1, 5000, 100);
    //     //blink_strip_single();
    //     //led_strip_tranz();
    //     //led_rainbow();
    //     //vTaskDelay(10 / portTICK_PERIOD_MS);
    //     //i++;
    // }
    blink_strip(1, 5000, 25);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    blink_strip(4, 5000, 50);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    blink_strip(6, 5000, 100);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    blink_strip(0, 5000, 150);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    vTaskDelete(NULL);
}
void app_main(void)
{
    led_setup();
    xTaskCreate(led_task, "led_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
