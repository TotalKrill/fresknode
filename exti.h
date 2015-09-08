#ifndef EXTI_H_

#define EXTI_H
#include "hal.h"

void extcb1(EXTDriver *extp, expchannel_t channel);
void start_thd(void);

#endif
