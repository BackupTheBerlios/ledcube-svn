#ifndef CUBE_H
#define CUBE_H

#ifdef __cplusplus
extern "C" {
#endif



// Dimension of the cube - DON'T CHANGE THIS
#define DIMENSION 8

// Dimension of the internal buffer (DIMENSION * DIMENSION * 1.5)
#define TRANSFER_LENGTH 96


void Cube_init(void);
void Cube_putBuffer(char *buf, unsigned int len);
void Cube_putBufferPolled(char *buf, unsigned int len);
void Cube_putBufferDMA(char *buf, unsigned int len);

// Advance cube model
void Cube_Tick_1();
void Cube_Tick_2();
void Cube_Tick_3();
void Cube_Tick_4();

void Cube_Send(unsigned int layer);

void Cube_Convert(unsigned int layer);

#endif //CUBE_H
