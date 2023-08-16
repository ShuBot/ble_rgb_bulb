
#include "led_strip_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

void blink_strip_single(void);
void blink_strip(uint32_t c, uint32_t time, uint32_t delay_ms);
void led_select_multiple(uint32_t x, uint32_t y, uint32_t z, uint32_t r, uint32_t g, uint32_t b);
void led_select_single(uint32_t x, uint32_t r, uint32_t g, uint32_t b);
void led_rainbow();
void led_strip_tranz();
void led_off(void);
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t brightness, uint32_t *r, uint32_t *g, uint32_t *b);
void led_setup(void);

#ifdef __cplusplus
}
#endif
