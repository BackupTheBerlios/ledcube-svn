

/*
 * File:   discovery.h
 * Author: peterharrison
 *
 * Created on 01 March 2012, 07:14
 * Adds macros for the LEDs on the STM32F4Discovery board
 * These are very wordy here but serve to remind that the
 * LEDs might well be on different ports and pins. This way, it will
 * always work out if they are properly described here.
 */

#ifndef DISCOVERY_H
#define DISCOVERY_H

#ifdef  __cplusplus
extern "C" {
#endif

  /*
   * These are the LEDs on the Discovery board
   */

#define GREEN_LED_PIN GPIO_Pin_12
#define GREEN_LED_PORT  GPIOD
#define GREEN_LED_PORT_CLOCK RCC_AHB1Periph_GPIOD

#define ORANGE_LED_PIN GPIO_Pin_13
#define ORANGE_LED_PORT  GPIOD
#define ORANGE_LED_PORT_CLOCK RCC_AHB1Periph_GPIOD

#define RED_LED_PIN GPIO_Pin_14
#define RED_LED_PORT  GPIOD
#define RED_LED_PORT_CLOCK RCC_AHB1Periph_GPIOD

#define BLUE_LED_PIN GPIO_Pin_15
#define BLUE_LED_PORT  GPIOD
#define BLUE_LED_PORT_CLOCK RCC_AHB1Periph_GPIOD

#define TEST_PIN_PIN GPIO_Pin_10
#define TEST_PIN_PORT  GPIOD
#define TEST_PIN_PORT_CLOCK RCC_AHB1Periph_GPIOD

#define GREEN_LED     GREEN_LED_PORT,GREEN_LED_PIN
#define ORANGE_LED    ORANGE_LED_PORT,ORANGE_LED_PIN
#define RED_LED       RED_LED_PORT,RED_LED_PIN
#define BLUE_LED      BLUE_LED_PORT,BLUE_LED_PIN
#define TEST_PIN      TEST_PIN_PORT,TEST_PIN_PIN

#define GREEN_ON()      GPIO_SetBits(GREEN_LED);
#define GREEN_OFF()     GPIO_ResetBits(GREEN_LED);
#define GREEN_TOGGLE()  GPIO_ToggleBits(GREEN_LED);

#define ORANGE_ON()      GPIO_SetBits(ORANGE_LED);
#define ORANGE_OFF()     GPIO_ResetBits(ORANGE_LED);
#define ORANGE_TOGGLE()  GPIO_ToggleBits(ORANGE_LED);

#define RED_ON()      GPIO_SetBits(RED_LED);
#define RED_OFF()     GPIO_ResetBits(RED_LED);
#define RED_TOGGLE()  GPIO_ToggleBits(RED_LED);

#define BLUE_ON()      GPIO_SetBits(BLUE_LED);
#define BLUE_OFF()     GPIO_ResetBits(BLUE_LED);
#define BLUE_TOGGLE()  GPIO_ToggleBits(BLUE_LED);

#define TEST_ON()      GPIO_SetBits(TEST_PIN);
#define TEST_OFF()     GPIO_ResetBits(TEST_PIN);
#define TEST_TOGGLE()  GPIO_ToggleBits(TEST_PIN);

  /*
   * Using a single SPI port for the output devices
   */
#define SPI SPI2
#define SPI_PORT                  SPI2
#define SPI_PORT_CLOCK            RCC_APB1Periph_SPI2
#define SPI_PORT_CLOCK_INIT       RCC_APB1PeriphClockCmd

#define SPI_SCK_PIN              GPIO_Pin_13
#define SPI_SCK_GPIO_PORT        GPIOB
#define SPI_SCK_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define SPI_SCK_SOURCE           GPIO_PinSource13
#define SPI_SCK_AF               GPIO_AF_SPI2

#define SPI_MOSI_PIN             GPIO_Pin_15
#define SPI_MOSI_GPIO_PORT       GPIOB
#define SPI_MOSI_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define SPI_MOSI_SOURCE          GPIO_PinSource15
#define SPI_MOSI_AF              GPIO_AF_SPI2

#define SPI_MISO_PIN             GPIO_Pin_14
#define SPI_MISO_GPIO_PORT       GPIOB
#define SPI_MISO_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define SPI_MISO_SOURCE          GPIO_PinSource14
#define SPI_MISO_AF              GPIO_AF_SPI2

    //
  /* Definition for DMAx resources **********************************************/
#define SPI_PORT_DR_ADDRESS                SPI_PORT->DR

#define SPI_PORT_DMA                       DMA1
#define SPI_PORT_DMAx_CLK                  RCC_AHB1Periph_DMA1

#define SPI_PORT_TX_DMA_CHANNEL            DMA_Channel_0
#define SPI_PORT_TX_DMA_STREAM             DMA1_Stream4
//#define SPI_PORT_TX_DMA_FLAG_FEIF          DMA_FLAG_FEIF4
//#define SPI_PORT_TX_DMA_FLAG_DMEIF         DMA_FLAG_DMEIF4
//#define SPI_PORT_TX_DMA_FLAG_TEIF          DMA_FLAG_TEIF4
//#define SPI_PORT_TX_DMA_FLAG_HTIF          DMA_FLAG_HTIF4
//#define SPI_PORT_TX_DMA_FLAG_TCIF          DMA_FLAG_TCIF4

#define SPI_PORT_DMA_TX_IRQn               DMA1_Stream4_IRQn
#define SPI_PORT_DMA_TX_IRQHandler         DMA1_Stream4_IRQHandler

#ifdef  __cplusplus
}
#endif

#endif  /* DISCOVERY_H */

