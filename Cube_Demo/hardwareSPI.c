#include "discovery-hardware.h"
#include "HardwareSPI.h"

void HardwareSPI_init(void) 
{
  DMA_InitTypeDef DMA_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // enable the SPI peripheral clock
  SPI_PORT_CLOCK_INIT(SPI_PORT_CLOCK, ENABLE);
  
  // enable the peripheral GPIO port clocks
  RCC_AHB1PeriphClockCmd(SPI_SCK_GPIO_CLK | SPI_MOSI_GPIO_CLK, ENABLE);
  
  // Connect SPI pins to AF5 - see section 3, Table 6 in the device data sheet
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, SPI_SCK_AF);
  GPIO_PinAFConfig(SPI_MISO_GPIO_PORT, SPI_MISO_SOURCE, SPI_MISO_AF);
  GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, SPI_MOSI_AF);
  
  // now configure the pins themselves
  // they are all going to be fast push-pull outputs
  // but the SPI pins use the alternate function
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
  GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  
  // now we can set up the SPI peripheral
  // Assume the target is write only and we look after the chip select ourselves
  // SPI clock rate will be system frequency/4/prescaler
  // so here we will go for 72/4/2 = 9 MHz
  SPI_I2S_DeInit(SPI_PORT);
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_Init(SPI_PORT, &SPI_InitStructure);

  // Enable the SPI port
  SPI_Cmd(SPI_PORT, ENABLE);
  // now set up the DMA
  // first enable the clock
  RCC_AHB1PeriphClockCmd(SPI_PORT_DMAx_CLK, ENABLE);
  // start with a blank DMA configuration just to be sure
  DMA_DeInit(SPI_PORT_TX_DMA_STREAM);
  /*
   * Check if the DMA Stream is disabled before enabling it.
   * Note that this step is useful when the same Stream is used multiple times:
   * enabled, then disabled then re-enabled... In this case, the DMA Stream disable
   * will be effective only at the end of the ongoing data transfer and it will
   * not be possible to re-configure it before making sure that the Enable bit
   * has been cleared by hardware. If the Stream is used only once, this step might
   * be bypassed.
   */
  
  while (DMA_GetCmdStatus(SPI_PORT_TX_DMA_STREAM) != DISABLE);
  // Configure DMA controller to manage TX DMA requests for the HCMS display
  // first make sure we are using the default values
  DMA_StructInit(&DMA_InitStructure);
  // these are the only parameters that change from the defaults
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (SPI_PORT->DR);

  DMA_InitStructure.DMA_Channel = SPI_PORT_TX_DMA_CHANNEL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /*
   * It is not possible to call DMA_Init without values for the source
   * address and non-zero size even though a transfer is not done here.
   * These are checked only when the assert macro are active though.
   */
  DMA_InitStructure.DMA_Memory0BaseAddr = 0;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_Init(SPI_PORT_TX_DMA_STREAM, &DMA_InitStructure);
  // Enable the DMA transfer complete interrupt
  DMA_ITConfig(SPI_PORT_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
  // enable the interrupt in the NVIC
  // Enable dma tx request.
  SPI_I2S_DMACmd(SPI_PORT, SPI_I2S_DMAReq_Tx, ENABLE);
}

void spiPutByte(uint8_t data) {
  // make sure the transmit buffer is free
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI_PORT, data);
  // we are not reading data so be sure that the character goes to the shift register
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  // and then be sure it has been sent over the wire
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
};

void spiPutWord(uint16_t data) {
  // make sure the transmit buffer is free
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI2, data / 256);
  // we are not reading data so be sure that the character goes to the shift register
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI2, data % 256);
  // we are not reading data so be sure that the character goes to the shift register
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  // and then be sure it has been sent over the wire
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
};

void spiPutBufferPolled(char * buffer, uint16_t length) {
  while (length) {
    // make sure the transmit buffer is free
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI_PORT, *buffer++);
    length--;
  }
  // be sure the last byte is sent to the shift register
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
  // and then wait until it goes over the wire
  while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
}

void spiPutBufferDMA(char * buffer, uint16_t length) 
{
  DMA_Cmd(SPI_PORT_TX_DMA_STREAM, DISABLE);
  SPI_PORT_TX_DMA_STREAM->NDTR = length;
  SPI_PORT_TX_DMA_STREAM->M0AR = (uint32_t) buffer;
  DMA_Cmd(SPI_PORT_TX_DMA_STREAM, ENABLE);
}

void SPI_PORT_DMA_TX_IRQHandler() 
{
  // Test if DMA Stream Transfer Complete interrupt
  if (DMA_GetITStatus(SPI_PORT_TX_DMA_STREAM, DMA_IT_TCIF4)) {
    DMA_ClearITPendingBit(SPI_PORT_TX_DMA_STREAM, DMA_IT_TCIF4);
    /*
     * There is an unpleasant wait until we are certain the data has been sent.
     * The need for this has been verified by oscilloscope. The shift register
     * at this point may still be clocking out data and it is not safe to
     * release the chip select line until it has finished. It only costs half
     * a microsecond so better safe than sorry. Is it...
     *
     *  a) flushed from the transmit buffer
     */
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET) 
    {
    };
    /*
     * b) flushed out of the shift register
     */
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET) 
    {
    };

  }
}

