#ifndef H_BLEPRPH_
#define H_BLEPRPH_

#include "bleprph.h"

#ifdef __cplusplus
extern "C" {
#endif

//int ble_data_send(void);
void ble_start(void);
int ble_get_write_data(void);
int ble_send_write_data(int x);

#ifdef __cplusplus
}
#endif

#endif
