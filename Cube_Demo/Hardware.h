#ifndef HARDWARE_H
#define HARDWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*  Definitionen der Pins an die die Cube angeschlossen wird...

    DC_PRG = B0
    VPRG = B1
    BLANK = B11
    XLAT = B12
    SCLK = B13
    MISO = B14
    MOSI = B15

    CH0 = D8
    CH1 = D9
    CH2 = D10
    CH3 = D11
    CH4 = D12
    CH5 = D13
    CH6 = D14
    CH7 = D15

    GSCLK = E9

*/


void TIM1_Config();

void TIM8_Config();

void USART_Configuration();

void IO_Init();

void Interrupts_Init();

void SysTick_Init();

void Latch_On();

void Latch_Off();

void Blank_On();

void Blank_Off();

void DCPRG_On();

void DCPRG_Off();

void VPRG_On();

void VPRG_Off();

void Layer_On(int layer);

#endif //HARDWARE_H
