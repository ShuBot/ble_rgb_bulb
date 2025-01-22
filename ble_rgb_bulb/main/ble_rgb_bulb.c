/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/rmt_tx.h"
//#include "led_strip_encoder.h"
#include "led_control.h"
#include "max98357.h"
#include "ble_lib.h"
#include "bleprph.h"
#include "global_functions.h"

static const char *tag = "BLE_BULB";

#define QUEUE_LENGTH   10
#define ITEM_SIZE      sizeof(uint8_t)

static QueueHandle_t bleWriteQueue;

SemaphoreHandle_t bleDataUpdate = NULL;

volatile bool interruptTriggered = false;

int gdata_test = 0;
int brightness_in = 0;  //LED brightness input from BLE

// static void led_task(void *arg)
// {
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//     while (1) {
//         /*
//             blink_strip(led_color_code, time to blink for(ms) , delay time between blinks(ms))
//             color code:  0:White 1:Purple 2:Blue 3:Green 4:Yellow 5:Orange 6:Red
//         */
//         //blink_strip(5, 0, 10000);
//         //blink_strip_single();
//         //led_strip_tranz();
//         //led_rainbow();
//         //vTaskDelay(10 / portTICK_PERIOD_MS);
//         //led_strip_single(0, 0, 255);  //Send RGB color data for LED to glow continuously.
//         //vTaskDelay(10 / portTICK_PERIOD_MS);
        
//         if (xSemaphoreTake(bleDataUpdate, portMAX_DELAY) == pdTRUE)
//         {
//             // Handle the event data received from ble_data_task
//             int lb = brightness_in;
//             int eventData = 0;
//             // Receive the event data from ble_data_task
//             if (xQueueReceive(bleWriteQueue, &eventData, portMAX_DELAY) == pdTRUE) {
//                 printf("led_task received event data: %d\n", eventData);
//             }

//             switch(eventData)
//             {
//                 case 0 :
//                         led_off();
//                         //blink_strip_single();
//                         vTaskDelay(10 / portTICK_PERIOD_MS);
//                         break;

//                 case 1 : 
//                         // blink_strip(2, 50000, 50);
//                         printf("BLUE LED ON! \n");
//                         led_strip_single(0, 0, 255, lb);
//                         vTaskDelay(10 / portTICK_PERIOD_MS);
//                         break;
                
//                 case 2 : 
//                         // blink_strip(6, 10000, 250);
//                         printf("REG LED ON! \n");
//                         led_strip_single(255, 0, 0, lb);
//                         vTaskDelay(10 / portTICK_PERIOD_MS);
//                         break;
                
//                 case 3 : 
//                         printf("GREEN LED ON! \n");
//                         led_strip_single(0, 255, 0, lb);
//                         vTaskDelay(10 / portTICK_PERIOD_MS);
//                         // notify_2();
//                         // vTaskDelay(10 / portTICK_PERIOD_MS);
//                         break;
                
//                 case 4 :
//                         printf("RAINBOW LED ON! \n");
//                         led_strip_tranz(lb);
//                         vTaskDelay(10 / portTICK_PERIOD_MS);
//                         break;
                
//                 default:
//                         break;
//             }
//         }
        
//     }
//     // blink_strip(1, 5000, 25);
//     // vTaskDelay(100 / portTICK_PERIOD_MS);
//     // blink_strip(4, 5000, 50);
//     // vTaskDelay(100 / portTICK_PERIOD_MS);
//     // blink_strip(6, 5000, 100);
//     // vTaskDelay(100 / portTICK_PERIOD_MS);
//     // blink_strip(0, 5000, 150);
//     // vTaskDelay(100 / portTICK_PERIOD_MS);

//     // vTaskDelete(NULL);
// }

static void ble_data_task(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    while (1) {

        uint8_t wdata = ble_get_write_data();    //function getting data from "ble_lib.c", from "gatt_srv.c"
        // if(gdata_test >= 10)
        //     printf("global variable working! \n");
        
        if((wdata != 255))    //interruptTriggered)
        {
            if(xQueueSendFromISR(bleWriteQueue, &wdata, &xHigherPriorityTaskWoken) == pdTRUE) 
            {
                // Data sent successfully
                ESP_LOGI(tag,"Data send to Queue. %d \n", wdata);
                xSemaphoreGive(bleDataUpdate);
                gdata_test = wdata;
            }
            else if(uxQueueMessagesWaitingFromISR(bleWriteQueue) >= QUEUE_LENGTH)
            {
                ESP_LOGE(tag,"Queue Full!!!");
                // Reset the queue
                xQueueReset(bleWriteQueue);
                ESP_LOGI(tag,"Queue RESET");
                xQueueSendFromISR(bleWriteQueue, &wdata, &xHigherPriorityTaskWoken);
                ESP_LOGI(tag,"Data send to Queue. %d \n", wdata);
            }
            
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    //vTaskDelete(NULL);
}

static void audio_task(void *arg)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ESP_LOGI(tag,"Audio Task Starting.\n");

    while(1)
    {
        int w_data = 0;
        // notify_1();
        // ESP_LOGI(tag,"Audio 1 Played.");
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // if (xSemaphoreTake(bleDataUpdate, portMAX_DELAY) == pdTRUE)
        // {
        //     if (xQueueReceive(bleWriteQueue, &w_data, portMAX_DELAY) == pdTRUE) {
        //         printf("audio_task received event data: %d\n", w_data);
        // }
        
        w_data = gdata_test;
        switch(w_data)
        {
            case 0 :
                    // led_off();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    break;
            
            case 1 : 
                    ESP_LOGI(tag,"Audio 1 Played.");
                    notify_1();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    break;
            
            case 2 : 
                    ESP_LOGI(tag,"Audio 2 Played.");
                    notify_2();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    break;
            
            case 3 : 
                    ESP_LOGI(tag,"Audio 1 Played.");
                    notify_1();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    break;
            
            case 4 :
                    ESP_LOGI(tag,"Audio 2 Played.");
                    notify_2();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    break;
            
            default:
                    //ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
                    break; 
        }
        // }
    }

    // vTaskDelete(NULL);
}

void app_main(void)
{
    bleDataUpdate = xSemaphoreCreateBinary();
    bleWriteQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    
    if (bleWriteQueue == NULL) {
        // Queue creation failed, handle the error.
        ESP_LOGE(tag,"Queue NOT Created!");
    }

    ble_start();        //BLE Module Initiated
    max98357a_init();   //I2S Audio Module Initiated
    // led_setup();        //LED GPIO Module Initiated

    /***
        Properly manage the Task Priorities.
        Avoid using too long delays.
        Use Hard and Soft Interrupts as much as possible.
    ***/
    xTaskCreate(ble_data_task, "ble_data_task", 2048*2, NULL, configMAX_PRIORITIES-2, NULL);
    
    // xTaskCreatePinnedToCore(led_task,   "led_task",     2048*2, NULL, configMAX_PRIORITIES-1, NULL, 1);

    xTaskCreate(audio_task, "audio_task",   2048*2, NULL, configMAX_PRIORITIES-1, NULL);
    
}
