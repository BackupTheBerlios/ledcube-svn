/*
 * File:   HCMS3907.h
 * Author: peterharrison
 *
 * Created on 10 March 2012, 22:42
 */

#ifndef HCMS3907_H
#define	HCMS3907_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "hardwareSPI.h"
#include "font5x7.h"

  extern char HCMS3907_buffer[];
  extern uint8_t HCMS3907_refresh_count; //tells the DMA updater to refresh the display

  void HCMS3907_init(void);
  void HCMS3907_setMethod(SPI_CommunicationMethodType method);
  SPI_CommunicationMethodType HCMS3907_getMethod(SPI_CommunicationMethodType method);
  void HCMS3907_cmd(char c);
  void HCMS3907_data(char c);
  void HCMS3907_putBuffer(char *buf, uint8_t len);
  void HCMS3907_putBufferPolled(char *buf, uint8_t len);
  void HCMS3907_putBufferDMA(char *buf, uint8_t len);
  void HCMS3907_putchar(char c);
  void HCMS3907_puts(const char *s);
  void HCMS3907_on(void);
  void HCMS3907_off(void);
  void HCMS3907_toggle(void);
  void HCMS3907_update(void);
  void HCMS3907_blink(uint16_t period);
  void HCMS3907_setBrightness(uint8_t brightness);
  void HCMS3907_cls(void);

#ifdef	__cplusplus
}
#endif

#endif	/* HCMS3907_H */

