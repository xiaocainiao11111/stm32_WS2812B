#ifndef __ADC1_H
#define __ADC1_H
#include "stm32f10x.h"

void ADC1_Init(void);
uint16_t ADC1_GetValue(uint8_t Channel);

#endif
