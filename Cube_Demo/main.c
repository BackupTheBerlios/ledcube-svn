#include "stm32f4_discovery.h"
#include "Hardware.h"
#include "hardwareSPI.h"




int main(void)
{
	// Init all needed hardware
	IO_Init();
	VPRG_Off();
	DCPRG_Off();
	Layer_On(-1);
	TIM1_Config();
	TIM8_Config();
	HardwareSPI_init();
	Interrupts_Init();
	SysTick_Init();

	while (1)
	{
		;
	}
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
