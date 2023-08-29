#ifndef MAX98357_H
#define MAX98357_H

#include "driver/i2s_std.h"

#ifdef __cplusplus
extern "C" {
#endif

void i2s_write_audio(void);
void max98357a_init(void);
void audio_play(void);
void notify_1(void);
void notify_2(void);
void notify_3(void);
void notify_4(void);


#ifdef __cplusplus
}
#endif

#endif // MAX98357_H
