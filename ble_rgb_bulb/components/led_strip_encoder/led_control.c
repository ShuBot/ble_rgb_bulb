#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include "led_control.h"


#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      15

#define LED_NUMBERS         24
#define CHASE_SPEED_MS      50
#define LED_BRIGHTNESS      50

uint32_t red    = 0;
uint32_t green  = 0;
uint32_t blue   = 0;

static const char *TAG = "RGB_BULB";

//led_strip_pixels["led number in strip" * 3 + N] = 0-255;
//N = 0 for green, 1 for red and 2 for blue
static uint8_t led_strip_pixels[LED_NUMBERS * 3];

#define RED     1
#define GREEN   0
#define BLUE    2
//Create RMT TX Channel
rmt_channel_handle_t led_chan = NULL;
rmt_tx_channel_config_t tx_chan_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
    .gpio_num = RMT_LED_STRIP_GPIO_NUM,
    .mem_block_symbols = 64, // increase the block size can make the LED less flickering
    .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
    .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
};

//Install LED Strip Encoder
rmt_encoder_handle_t led_encoder = NULL;
led_strip_encoder_config_t encoder_config = {
    .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
};

rmt_transmit_config_t tx_config = {
    .loop_count = 0, // no transfer loop
};

void blink_strip_single(void)
{
    /*  Blinking the entire LED Strip as a Single LED.
    */
    uint32_t brightness = LED_BRIGHTNESS;

    red = 250;  blue = 0; green = 0;

    red   = (red * brightness) / 255;
    green = (green * brightness) / 255;
    blue  = (blue * brightness) / 255;
    
    for(int j = 0; j < LED_NUMBERS; j++)
    {
        led_strip_pixels[j * 3 + RED]   = red;
        led_strip_pixels[j * 3 + GREEN] = green;
        led_strip_pixels[j * 3 + BLUE]  = blue;
        ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        vTaskDelay(pdMS_TO_TICKS(1));
        //vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    }
    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    led_off();
    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    // memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
    // ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));

}
void blink_strip(uint32_t c, uint32_t time, uint32_t delay_ms)
{
    TickType_t elapsed_ticks, new_ticks;
    uint32_t elapsed_ms, run_time, new_ms = 0;
    uint32_t brightness = LED_BRIGHTNESS;

    elapsed_ticks = xTaskGetTickCount();
    elapsed_ms = pdTICKS_TO_MS(elapsed_ticks);
    run_time = elapsed_ms + time;
    printf("Run Time: %ld\n", run_time);

    while(run_time > new_ms)
    {
        for(int i = 0; (i < 7); i++)
        {
            for(int j = 0; j < LED_NUMBERS; j++)
            {
                switch(c)
                {
                    case 0:             //White[] = 255, 255, 255 
                        red     = 255;
                        green   = 255;
                        blue    = 255;
                        break;

                   case 1:             //Purple[] = 150, 0, 210
                        red     = 150;
                        green   = 0;
                        blue    = 210;
                        break;

                    case 2:             //Blue[]   = 0, 0, 255
                        red     = 0;
                        green   = 0;
                        blue    = 255;
                        break;

                    case 3:             //Green[]  = 0, 255, 0
                        red     = 0;
                        green   = 255;
                        blue    = 0;
                        break;

                    case 4:             //Yellow[] = 255, 255, 0
                        red     = 255;
                        green   = 255;
                        blue    = 0;
                        break;

                    case 5:             //Orange[] = 255, 120, 0
                        red     = 255;
                        green   = 110;
                        blue    = 0;
                        break;

                    case 6:             //Red[]    = 255, 0, 0 
                        red     = 255;
                        green   = 0;
                        blue    = 0;
                        break;

                    default:            //White[]   = 255, 255, 255
                        red     = 255;
                        green   = 255;
                        blue    = 255;
                        break;
                }
                red   = (red * brightness) / 255;
                green = (green * brightness) / 255;
                blue  = (blue * brightness) / 255;
                
                led_strip_pixels[j * 3 + RED]   = red;
                led_strip_pixels[j * 3 + GREEN] = green;
                led_strip_pixels[j * 3 + BLUE]  = blue;

                ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            //memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
            
            //ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
            //vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
        led_off();
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        new_ticks = xTaskGetTickCount();
        new_ms = pdTICKS_TO_MS(new_ticks);
        printf("New Time: %ld\n", new_ms);
        //printf("Run Time: %ld\n", run_time);
    }
    led_off();
    //memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
    //vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void led_select_multiple(uint32_t x, uint32_t y, uint32_t z, uint32_t r, uint32_t g, uint32_t b)
{
    red     = (r * LED_BRIGHTNESS) / 255;   //Red LED Brightness
    green   = (g * LED_BRIGHTNESS) / 255;   //Green LED Brightness
    blue    = (b * LED_BRIGHTNESS) / 255;   //Blue LED Brightness

    led_strip_pixels[x * 3 + RED]   = red;
    led_strip_pixels[y * 3 + GREEN] = green;
    led_strip_pixels[z * 3 + BLUE]  = blue;

    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));   
}

void led_select_single(uint32_t x, uint32_t r, uint32_t g, uint32_t b)
{
        //x = 5;     //LED Number in the strip.
        red     = (r * LED_BRIGHTNESS) / 100;   //Red LED Brightness
        green   = (g * LED_BRIGHTNESS) / 100;   //Green LED Brightness
        blue    = (b * LED_BRIGHTNESS) / 100;   //Blue LED Brightness

        led_strip_pixels[x * 3 + RED]  = red;
        led_strip_pixels[x * 3 + GREEN] = green;
        led_strip_pixels[x * 3 + BLUE] = blue;

        ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
        memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
        ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
}

void led_rainbow()
{
    /*
    This fun. needs min 7 LEDs, as of right now.
    Color[] = R G B
    */
    for(int i = 0; i < LED_NUMBERS; i++)
    {
        uint32_t cnt = (LED_NUMBERS/7);

        for (int j = i; j < LED_NUMBERS; j += cnt)
        {
            uint32_t r = (j * cnt) / (cnt * cnt);
            uint32_t brightness = LED_BRIGHTNESS;
            
            switch(r)
            {
                case 0:             //Violet[] = 150, 0, 210
                    red     = 150;
                    green   = 0;
                    blue    = 210;
                    break;

               case 1:             //Indigo[] = 75, 0, 130
                    red     = 75;
                    green   = 0;
                    blue    = 130;
                    break;

                case 2:             //Blue[]   = 0, 0, 255
                    red     = 0;
                    green   = 0;
                    blue    = 255;
                    break;

                case 3:             //Green[]  = 0, 255, 0
                    red     = 0;
                    green   = 255;
                    blue    = 0;
                    break;

                case 4:             //Yellow[] = 255, 255, 0
                    red     = 255;
                    green   = 255;
                    blue    = 0;
                    break;

                case 5:             //Orange[] = 255, 120, 0
                    red     = 255;
                    green   = 110;
                    blue    = 0;
                    break;

                case 6:             //Red[]    = 255, 0, 0 
                    red     = 255;
                    green   = 0;
                    blue    = 0;
                    break;

                default:            //White[]   = 255, 255, 255
                    red     = 255;
                    green   = 255;
                    blue    = 255;
                    break;
            }
            
            red   = (red * brightness) / 255;
            green = (green * brightness) / 255;
            blue  = (blue * brightness) / 255;
            
            led_strip_pixels[j * 3 + RED]   = red;
            led_strip_pixels[j * 3 + GREEN] = green;
            led_strip_pixels[j * 3 + BLUE]  = blue;
        }
        ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
        // memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
        // ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        // vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    }
}

void led_strip_tranz()
{
    /*
    Color[] = R G B
    */
    uint32_t brightness = LED_BRIGHTNESS;
    for(int i = 0; i < 7; i++)
    {
        for(int j = 0; j < LED_NUMBERS; j++)
        {
            switch(i)
            {
                case 0:             //Violet[] = 150, 0, 210
                    red     = 150;
                    green   = 0;
                    blue    = 210;
                    break;

               case 1:             //Indigo[] = 75, 0, 130
                    red     = 75;
                    green   = 0;
                    blue    = 130;
                    break;

                case 2:             //Blue[]   = 0, 0, 255
                    red     = 0;
                    green   = 0;
                    blue    = 255;
                    break;

                case 3:             //Green[]  = 0, 255, 0
                    red     = 0;
                    green   = 255;
                    blue    = 0;
                    break;

                case 4:             //Yellow[] = 255, 255, 0
                    red     = 255;
                    green   = 255;
                    blue    = 0;
                    break;

                case 5:             //Orange[] = 255, 120, 0
                    red     = 255;
                    green   = 110;
                    blue    = 0;
                    break;

                case 6:             //Red[]    = 255, 0, 0 
                    red     = 255;
                    green   = 0;
                    blue    = 0;
                    break;

                default:            //White[]   = 255, 255, 255
                    red     = 255;
                    green   = 255;
                    blue    = 255;
                    break;
            }
            red   = (red * brightness) / 255;
            green = (green * brightness) / 255;
            blue  = (blue * brightness) / 255;
            
            led_strip_pixels[j * 3 + RED]   = red;
            led_strip_pixels[j * 3 + GREEN] = green;
            led_strip_pixels[j * 3 + BLUE]  = blue;

            ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
            vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
            // memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
            // ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
            // vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
        }
    }
}
void led_off(void)
{
        for(int i = 0; i < LED_NUMBERS; i++)
        {
            led_strip_pixels[i * 3 + 0] = 0;
            led_strip_pixels[i * 3 + 1] = 0;
            led_strip_pixels[i * 3 + 2] = 0;   

            ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
}

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t brightness, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }

    // Adjust the RGB values based on brightness
    *r = (*r * brightness) / 100;
    *g = (*g * brightness) / 100;
    *b = (*b * brightness) / 100;
}

void led_setup(void)
{
     //uint16_t hue = 0;
    //uint16_t start_rgb = 0;

    ESP_LOGI(TAG, "Create RMT TX channel");
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_LOGI(TAG, "Install led strip encoder");
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));

    ESP_LOGI(TAG, "Start LED rainbow chase");
    
    // while (1) {
    //     for (int i = 0; i < 3; i++) {
    //         for (int j = i; j < LED_NUMBERS; j += 3) {
    //             // Build RGB pixels
    //             //hue = j * 360 / LED_NUMBERS + start_rgb;
    //             //led_strip_hsv2rgb(hue, 100, 100, LED_BRIGHTNESS, &red, &green, &blue);
    //             led_strip_pixels[j * 3 + 0] = const_green;  //green;
    //             //led_strip_pixels[j * 3 + 1] = blue;
    //             //led_strip_pixels[j * 3 + 2] = red;
    //         }
    //         // Flush RGB values to LEDs
    //          ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
    //          vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    //          //memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
    //          //ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
    //          //vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    //     }
    //     start_rgb += 60;
    // }

    //while(1){
        //led_off();
        //led_select_single(5, 150, 0, 150);  //(LED No, R, G, B)
        //led_select_multiple(0, 7, 15, 150, 150, 150); //(LED1, LED2, LED3, R, G, B)
        //led_rainbow();
        //led_strip_tranz();
        //ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        //vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
        //memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
        //ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
        //vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    //}
}