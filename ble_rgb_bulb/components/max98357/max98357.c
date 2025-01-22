#include "max98357.h"
#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "global_functions.h"

/***************
* 
* LRC | IO26  | Left Right Clock
* BCLK| IO27  | Bit Clock Input
* DIN | IO25  | Digital Data input
* GAIN| GND   | Gain Select. If connected with VDD, Gain is 6dB, If GND, Gain 12dB.
* SD  |  -    | Shutdown. To activate, pull it low.
* GND |   GND | Supply Ground
* VIN |   5V  | Voltage Input
*
***************/
#define EXAMPLE_STD_BCLK_IO1        GPIO_NUM_27      //ESP's I2S bit clock io number    I2S_BCLK
#define EXAMPLE_STD_WS_IO1          GPIO_NUM_26      //ESP's I2S word select io number  I2S_LRC
#define EXAMPLE_STD_DOUT_IO1        GPIO_NUM_25      //ESP's I2S data out io number     I2S_DOUT
#define EXAMPLE_STD_DIN_IO1         GPIO_NUM_NC      //I2S data in io number

#define EXAMPLE_BUFF_SIZE           1024
#define SAMPLE_RATE                 44100
#define VOLUME                      6       //Volume Range: 3 - 10

static i2s_chan_handle_t tx_chan;        // I2S tx channel handler

static const char *tag = "MAX98357:";
//_binary_n2_pcm_start
//_binary_n2_pcm_end
extern const uint8_t pcm_start1[] asm("_binary_n1_pcm_start");
extern const uint8_t pcm_end1[]   asm("_binary_n1_pcm_end");

extern const uint8_t pcm_start2[] asm("_binary_n2_pcm_start");
extern const uint8_t pcm_end2[]   asm("_binary_n2_pcm_end");

extern const uint8_t pcm_start3[] asm("_binary_n3_pcm_start");
extern const uint8_t pcm_end3[]   asm("_binary_n3_pcm_end");

extern const uint8_t pcm_start4[] asm("_binary_n4_pcm_start");
extern const uint8_t pcm_end4[]   asm("_binary_n4_pcm_end");

void i2s_write_audio(void)
{


    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);
    uint16_t *buffer2 = calloc(1, EXAMPLE_BUFF_SIZE * 2);

    size_t w_bytes = 0;
    uint32_t offset = 0;

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            ESP_LOGI(tag, "Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end1-pcm_start1)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = pcm_start1[offset]<<3; 
        }
        //printf("size %d\noffset %lu\n", pcm_end1-pcm_start1,offset);

    }

    free(buffer);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    w_bytes = 0;
    offset = 0;

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while(1){

        /* Write i2s data from buffer2 */
        if (i2s_channel_write(tx_chan, buffer2, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            ESP_LOGI(tag, "Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end2-pcm_start2)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer2[i] = pcm_start2[offset]<<3;
        }
        //printf("size %d\noffset %lu\n", pcm_end2-pcm_start2,offset);

    }

    free(buffer2);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));

}

void notify_1(void)
{
    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);
    
    size_t w_bytes = 0;
    uint32_t offset = 0;
    
    max98357a_init();

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            //printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end1-pcm_start1)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = pcm_start1[offset]<<VOLUME; 
        }
        //printf("size %d\noffset %lu\n", pcm_end1-pcm_start1,offset);

    }

    free(buffer);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
    i2s_del_channel(tx_chan);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void notify_2(void)
{
    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);
    
    size_t w_bytes = 0;
    uint32_t offset = 0;

    max98357a_init();

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            //printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end2-pcm_start2)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = (pcm_start2[offset]<<VOLUME);
        }
        //printf("size %d\noffset %lu\n", pcm_end2-pcm_start2,offset);

    }

    free(buffer);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
    i2s_del_channel(tx_chan);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void notify_3(void)
{
    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);
    
    size_t w_bytes = 0;
    uint32_t offset = 0;

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            //printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end3-pcm_start3)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = pcm_start3[offset]<<VOLUME; 
        }
        //printf("size %d\noffset %lu\n", pcm_end3-pcm_start3,offset);

    }

    free(buffer);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void notify_4(void)
{
    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);
    
    size_t w_bytes = 0;
    uint32_t offset = 0;

    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            //printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            ESP_LOGI(tag, "Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end4-pcm_start4)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = pcm_start4[offset]<<VOLUME; 
        }
        //printf("size %d\noffset %lu\n", pcm_end4-pcm_start4,offset);

    }

    free(buffer);
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void max98357a_init(void) 
{
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));


    i2s_std_config_t tx_std_cfg = {
            .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                        I2S_SLOT_MODE_MONO),

            .gpio_cfg = {
                    .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
                    .bclk = EXAMPLE_STD_BCLK_IO1,
                    .ws   = EXAMPLE_STD_WS_IO1,
                    .dout = EXAMPLE_STD_DOUT_IO1,
                    .din  = EXAMPLE_STD_DIN_IO1,
                    .invert_flags = {
                            .mclk_inv = false,
                            .bclk_inv = false,
                            .ws_inv   = false,
                    },
            },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &tx_std_cfg));
}


void audio_play(void)
{
    max98357a_init();
    i2s_write_audio();
}
