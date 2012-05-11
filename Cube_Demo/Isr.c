#include "Isr.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "Hardware.h"
#include "Cube.h"

/// TIM8 Update ISR : Begin cycle
void TIM8_UP_TIM13_IRQHandler()
{
	static unsigned int layer = 0;

	if (TIM8->SR & TIM_SR_UIF)
	{
		// Begin cycle: Latch off, layer X on, blank off, send layer X+1	
		Latch_Off();
		Blank_Off();

		Layer_On(layer);
		layer = (layer + 1) % DIMENSION;
		//layer = (layer - 1 + DIMENSION) % DIMENSION;
		Cube_Send(layer);
	}

	// reset the status register
	TIM8->SR = 0x0;
}

/// TIM8 CaptureCompare : Handle blank and latch
void TIM8_CC_IRQHandler(void)
{
	// CC1 : Blank ON
	if (TIM8->SR & TIM_SR_CC1IF)
	{
		Blank_On();	
		Layer_On(DIMENSION+1);
	}

	// CC2 : Latch ON
	if (TIM8->SR & TIM_SR_CC2IF)
	{
		Latch_On();
	}

	// reset the status register
	TIM8->SR = 0x0;
}


/// SysTick
void SysTick_Handler(void)
{
	static unsigned int time = 0;
	time++;

	// Every X ms, advance
	if((time % 20) == 0)
	{
		// Tick for Cube
		Cube_Tick_1();
		//Cube_Tick_2();
	}
}
