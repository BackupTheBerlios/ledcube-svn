
/*
 * File:   HCMS3907.c
 * Author: peterharrison
 *
 * Created on 10 March 2012, 22:42
 *
 * The HCMS-29xx series are high performance, easy to use dot matrix displays
 * driven by on-board CMOS ICs. Each display can be directly interfaced with
 * a microprocessor, thus eliminating the need for cumbersome interface
 * components. The serial IC interface allows higher character count
 * information displays with a minimum of data lines. A variety of colours,
 * font heights, and character counts gives designers a wide range of product
 * choices for their specific applications and the easy to read 5 x 7 pixel
 * format allows the display of uppercase, lower case, Katakana, and custom
 * user- defined characters. These displays are stackable in the x- and y-
 * directions, making them ideal for high character count displays.
 *
 * The HCMS3907 is a 3.3V green device with four characters.
 *
 * It is a write-only peripheral driven over SPI. As bytes are clocked in, the
 * right-most column of LEDs lights up with the most significant 7 bits and
 * all the columns shift left one place. Data in the shift registers is
 * transferred to the display when CS goes high so that the entire display
 * can be updated in one go.
 *
 * A register select line allows commands to be sent to set brightness and such.
 *
 * See the full product information at
 *
 * http://www.avagotech.com/pages/en/led_displays/smart_alphanumeric_displays/serial_interface/
 *
 *
 */

#include "stm32f4xx.h"
#include "discovery-hardware.h"
#include "HCMS3907.h"


// This buffer holds raw data for the display
// normally, text is rendered into it and only the first 20 bytes
// (for a four character display) are sent out - generally by DMA.
// Since the display is latched, it only need to get sent once.
#define HCMS3907_BUFFER_LENGTH 32
char HCMS3907_buffer[HCMS3907_BUFFER_LENGTH];
// DMA transfers can be made from anywhere so a pointer is used. It normally
// ends up pointing to the buffer though.
static char *HCMS3907_SrcAddress;
static SPI_CommunicationMethodType HCMS3907_CommunicationMethod;
uint8_t HCMS3907_refresh_count; //tells the DMA updater to refresh the display
static int16_t HCMS3907_blinkTime;
static int16_t HCMS3907_blinkDelay;
static uint8_t HCMS3907_brightness = 7;
static uint8_t HCMS3907_enabled = 1;

SPI_CommunicationMethodType HCMS3907_getMethod(SPI_CommunicationMethodType method) {
  return HCMS3907_CommunicationMethod;
}

void HCMS3907_setMethod(SPI_CommunicationMethodType method) {
  assert_param(IS_SPI_XFER_METHOD(method));
  HCMS3907_refresh_count = 0;
  HCMS3907_CommunicationMethod = method;
}

/*
 * Send a single byte of data to the display over SPI
 * Best used for commands or the updating of a single column since
 * the display latches data to the LEDs when the Chip Enable goes high
 * Polling is used always for this.
 * TODO: check that there is not a DMA transfer in progress
 */
void HCMS3907_putbyte(char c) {
  HCMS3907_SELECT();
  // make sure the transmit buffer is free
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI_PORT, c);
  // we are not reading data so be sure that the character goes to the shift register
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  // and then be sure it has been sent over the wire
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
  HCMS3907_DESELECT();
}

/*
 * Dumps a series of bytes straight out to the display.
 * Either DMA or polling will be used depending how the method
 * flag is set.
 * Note that the DMA handler will deselect the display at the end of the transfer
 */
void HCMS3907_putBuffer(char *buf, uint8_t len) {
  if (HCMS3907_CommunicationMethod = DMA) {
    HCMS3907_putBufferPolled(buf, len);
  } else {
    HCMS3907_putBufferDMA(buf, len);
  }
}

void HCMS3907_putBufferPolled(char *buf, uint8_t len) {
  HCMS3907_DATA();
  HCMS3907_SELECT();
  while (len) {
    // always make sure the transmit buffer is free
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI_PORT, *buf++);
    len--;
  }
  // be sure the last byte is sent to the shift register
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  // and then wait until it goes over the wire
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
  HCMS3907_DESELECT();

}

void HCMS3907_putBufferDMA(char *buf, uint8_t len) {
  // DMA transfer - simply setting the length
  // triggers a transfer on the next systick
  HCMS3907_SrcAddress = buf;
  HCMS3907_refresh_count = len;
}

/*
 * send a command byte to the display
 */
void HCMS3907_cmd(char c) {
  HCMS3907_COMMAND();
  HCMS3907_putbyte(c);
  HCMS3907_DATA();
}

/*
 * send a data byte to the display. Leave the chip in data mode.
 * Note that DMA assumes that data is being sent.
 *
 */
void HCMS3907_data(char c) {
  HCMS3907_DATA();
  HCMS3907_putbyte(c);
}

/*
 * Send five more columns to the display with data from a
 * single character. Whenever new data arrives in the display, it clocks
 * old data out the end so the display shifts left;
 */
void HCMS3907_putchar(char c) {
  int i;
  char *fontData;
  char *buf;
  buf = HCMS3907_buffer;
  fontData = (char *) Font5x7 + (c % 128) * 5;
  HCMS3907_putBufferPolled(fontData, 5);
}

/*
 * A string of more or less arbitrary length is converted to a bitmap in the
 * buffer and sent to the display. The number of bytes sent is
 * limited to the size of the buffer so cascaded displays will work. If the
 * string is longer than the display size, the last n characters will be
 * displayed, where n is the display width.
 */
void HCMS3907_puts(const char *s) {
  int i;
  const char *fontData;
  char *buf;
  uint16_t numBytes = 0;
  buf = HCMS3907_buffer;
  while ((*s) && (numBytes < (uint16_t)sizeof (HCMS3907_buffer) - 5)) {
    fontData = Font5x7 + (*s % 128) * 5;
    for (i = 0; i < 5; i++) {
      *buf++ = *fontData++;
    }
    numBytes += 5;
    s++;
  }
  HCMS3907_putBuffer(HCMS3907_buffer, numBytes);
}

/*
 * blanks the display, turning off the LEDs. The display data is retained
 */
void HCMS3907_off(void) {
  HCMS3907_enabled = 0;
  HCMS3907_cmd(0);
}

/*
 * Restores the display by turning on the LEDs
 */
void HCMS3907_on(void) {
  HCMS3907_enabled = 1;
  HCMS3907_cmd(HCMS3907_brightness | 0b01000000);
}

/*
 * Toggles display state between on and off
 */
void HCMS3907_toggle(void) {
  if (HCMS3907_enabled) {
    HCMS3907_off();
  } else {
    HCMS3907_on();
  }
}

/*
 * Set the blink period in milliseconds. The HCMS3907_update() function, called from
 * the system timer decrements the counter and toggles the display every time
 * the counter reaches zero. Thus the display is made to blink.
 * A period of zero stops the blinking and leaves the display on all the time.
 * Small numbers for the period give more rapid blinking.
 */
void HCMS3907_blink(uint16_t period) {
  HCMS3907_blinkTime = period;
}

/*
 * A control register in the display can be used to set the brightness by
 * pulse-width-modulating the LEDs. Brightness values are in the range 0-15
 */
void HCMS3907_setBrightness(uint8_t brightness) {
  if (brightness < 0) {
    brightness = 0;
  }
  if (brightness > 15) {
    brightness = 15;
  }
  HCMS3907_brightness = brightness;
  HCMS3907_on();
}

/*
 * This function needs to be called regularly. The system timer will do the
 * job very nicely. A 1kHz rate means that the counter values are in ms.
 * Two tasks are performed. First, the blink timer is tested and the blink
 * behaviour determined.
 * Second, the number of bytes in the DMA transfer buffer is tested and, if
 * non-zero, a DMA transfer of that number of bytes is initiated.
 * On completion, an interrupt is triggered which will wait until the
 * last byte goes from the SPI hardware and then deselect the display.
 * Once sent, the display latches all the data and need not be updated again
 * unless there is a change.
 */
void HCMS3907_update(void) {
  static int sz = 1;
  if (HCMS3907_blinkTime) {
    HCMS3907_blinkDelay--;
    if (HCMS3907_blinkDelay <= 0) {
      HCMS3907_toggle();
      HCMS3907_blinkDelay = HCMS3907_blinkTime;
    }
  }
  if (HCMS3907_refresh_count) {
    HCMS3907_SELECT();
    HCMS3907_putBuffer(HCMS3907_SrcAddress, HCMS3907_refresh_count);
    HCMS3907_refresh_count = 0;
  }
}

/*
 * Clearing the display is conveniently done by sending 20 null values
 * to it. The entire display buffer is emptied as well and DMA used
 * to transfer the data to avoid blocking.
 */
void HCMS3907_cls(void) {
  uint16_t i;
  for (i = 0; i < sizeof (HCMS3907_buffer); i++) {
    HCMS3907_buffer[i] = 0;
  }
  HCMS3907_putBufferPolled(HCMS3907_buffer, 20);
}


// wait as long as the DMA attached to the display is in the state given

int HCMS3907_WaitWhileDMA(FunctionalState state) {
  while (DMA_GetCmdStatus(SPI_PORT_TX_DMA_STREAM) == state) {
  }
}

   
