
/*
 * File:   hardwareSPI.h
 * Author: peterharrison *
 * Created on 10 March 2012, 17:48
 */

#ifndef HARDWARESPI_H
#define	HARDWARESPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "discovery-hardware.h"

  typedef enum 
  {
    POLLING = 0, DMA = !POLLING
  } SPI_CommunicationMethodType;

#define IS_SPI_XFER_METHOD(METHOD) (((METHOD) == POLLING) || ((METHOD) == DMA))

  void SPI_PORT_DMA_TX_IRQHandler();
  extern uint16_t spiConfigured;
  void HardwareSPI_init(void);
  void spiPutByte(uint8_t data);
  void spiPutWord(uint16_t data);
  void spiPutBufferPolled(char * buffer, uint16_t length);
  void spiPutBufferDMA(char * buffer, uint16_t length);

#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARESPI_H */

