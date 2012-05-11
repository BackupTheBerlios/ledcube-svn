#ifndef ISR_H
#define ISR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"



// TIM8 Update ISR : Begin cycle
void TIM8_UP_TIM13_IRQHandler();

// TIM8 CaptureCompare : Handle blank and latch
void TIM8_CC_IRQHandler();

// SysTick
void SysTick_Handler();



#endif //ISR_H
