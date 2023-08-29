/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/rmt_tx.h"
//#include "led_strip_encoder.h"
#include "led_control.h"
#include "max98357.h"
#include "ble_lib.h"

static const char *tag = "BLE_BULB";

#define QUEUE_LENGTH   10
#define ITEM_SIZE      sizeof(uint8_t)

static QueueHandle_t bleWriteQueue;

volatile bool interruptTriggered = false;

uint8_t gdata = 0;

static int pdata = 0;       //For getting data from BLE Service Inrrpts
int get_blewrt_data(void);

int get_blewrt_data()
{
    uint8_t wdata = ble_data_send();

    if(wdata != pdata)
    {
        interruptTriggered = true;
        pdata = wdata;
        return wdata;
    }
    else 
    {
        interruptTriggered = false;
        return 0;
    }
}

static void led_task(void *arg)
{
    //int i = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (1/*i<20*/) {
        /*
            blink_strip(led_color_code, time to blink for(ms) , delay time between blinks(ms))
            color code:  0:White 1:Purple 2:Blue 3:Green 4:Yellow 5:Orange 6:Red
        */
        //blink_strip(5, 0, 10000);
        //blink_strip_single();
        //led_strip_tranz();
        //led_rainbow();
        //vTaskDelay(10 / portTICK_PERIOD_MS);
        //i++;
        //int w_data = gdata;
        switch(gdata)
        {
            case 1 : 
                    led_strip_single(0, 0, 255);
                    break;
            
            case 2 : 
                    led_strip_single(255, 0, 0);
                    break;
            
            case 3 : 
                    led_strip_single(0, 255, 0);
                    notify_2();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    break;
            
            case 4 :
                    led_strip_tranz();
                    break;
            
            default:
                    led_off();
                    break;
        }
    }
    // blink_strip(1, 5000, 25);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // blink_strip(4, 5000, 50);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // blink_strip(6, 5000, 100);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // blink_strip(0, 5000, 150);
    // vTaskDelay(100 / portTICK_PERIOD_MS);

    vTaskDelete(NULL);
}

static void ble_data_task(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    while (1) {

        uint8_t wdata = get_blewrt_data();
        
        if(interruptTriggered)
        {
            if(xQueueSendFromISR(bleWriteQueue, &wdata, &xHigherPriorityTaskWoken) == pdTRUE) 
            {
                // Data sent successfully
                ESP_LOGI(tag,"Data send to Queue. %d \n", wdata);
                gdata = wdata;
                // if(wdata == 1)
                // {
                //     vTaskDelay(10 / portTICK_PERIOD_MS);
                //     notify_2();
                // }
            }
            else if(uxQueueMessagesWaitingFromISR(bleWriteQueue) >= QUEUE_LENGTH)
            {
                ESP_LOGE(tag,"Queue Full!!!");
                // Reset the queue
                xQueueReset(bleWriteQueue);
                ESP_LOGI(tag,"Queue RESET");
                xQueueSendFromISR(bleWriteQueue, &wdata, &xHigherPriorityTaskWoken);
                gdata = wdata;
                ESP_LOGI(tag,"Data send to Queue. %d \n", wdata);
            }
            
            interruptTriggered = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        // if(xQueuePeek(bleWriteQueue, &gdata, pdMS_TO_TICKS(500)) == pdTRUE) {
        //     //Process the received data
        //     //... (your processing logic)
        //     //printf("\nQ out: %d\n", wdata);
        //     ESP_LOGI(tag,"Data Read in Queue: %d", wdata);
        // }
        //ESP_LOGI(tag,"Data Read in Global: %d", gdata);
    }
    //vTaskDelete(NULL);
}

// static void audio_task(void *arg)
// {
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//     ESP_LOGI(tag,"Audio Task Starting.\n");

//     while(1)
//     {
//         int w_data = gdata;
//     //     switch(gdata)
//     //     {
//     //         case 1 : 
//     //                 notify_1();
//     //                 ESP_LOGI(tag,"Audio 1 Played.");
//     //                 vTaskDelay(100 / portTICK_PERIOD_MS);
//     //                 break;
            
//     //         case 2 : 
//     //                 notify_2();
//     //                 ESP_LOGI(tag,"Audio 2 Played.");
//     //                 vTaskDelay(100 / portTICK_PERIOD_MS);
//     //                 break;
            
//     //         // case 3 : 
//     //         //         notify_3();
//     //         //         vTaskDelay(100 / portTICK_PERIOD_MS);
//     //         //         break;
            
//     //         // case 4 :
//     //         //         notify_4();
//     //         //         vTaskDelay(100 / portTICK_PERIOD_MS);
//     //         //         break;
            
//     //         default:
//     //                 //ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
//     //                 break; 
//     //     }
//         if(w_data == 2)
//         {
//             notify_1();
//             vTaskDelay(50 / portTICK_PERIOD_MS);
//         }
//         //notify_1();
//         vTaskDelay(100 / portTICK_PERIOD_MS);
//     }

//     vTaskDelete(NULL);
// }

void app_main(void)
{
    bleWriteQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    
    if (bleWriteQueue == NULL) {
        // Queue creation failed, handle the error.
        ESP_LOGE(tag,"Queue NOT Created!");
    }

    ble_start();        //BLE Module Initiated
    max98357a_init();   //I2S Audio Module Initiated
    led_setup();        //LED GPIO Module Initiated
    /***
        Keep the Task Priority arranged properly. 
        Avoid using too long delays.
        Use Hard and Soft Interrupts as much as possible.
        LEDs flicker even when "led_strip_single()" func is used. Try different approach.
    ***/
    xTaskCreate(ble_data_task, "ble_data_task", 2048*2, NULL, configMAX_PRIORITIES-1, NULL);
    
    xTaskCreate(led_task,       "led_task",     2048*2, NULL, configMAX_PRIORITIES, NULL);

    //xTaskCreate(audio_task,     "audio_task",   2048*2, NULL, configMAX_PRIORITIES, NULL);
    
}
