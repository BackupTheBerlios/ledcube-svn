#include "Cube.h"
#include "Hardware.h"
#include "hardwareSPI.h"
#define ARM_MATH_CM4
#include "arm_math.h"


// This is the model, 16 LEDs, each represented by 1 byte, (X)(Y)(Z) - Z is Layer
char internalModel[DIMENSION][DIMENSION][DIMENSION];

// This is the actual data that gets send out
char internalBuffer[DIMENSION][TRANSFER_LENGTH];



void Cube_putBuffer(char *buf, unsigned int len)
{
	// OLD : Cube_putBufferPolled(buf, len);
	Cube_putBufferDMA(buf,len);
}


void Cube_putBufferPolled(char *buf, unsigned int len)
{
	while (len)
	{
		// always make sure the transmit buffer is free
		while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI_PORT, *buf++);
		len--;
	}

	// be sure the last byte is sent to the shift register
	while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
	// and then wait until it goes over the wire
	while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_BSY) == SET);
}


void Cube_putBufferDMA(char *buf, unsigned int len)
{
	// DMA transfer - simply setting the length
	// triggers a transfer on the next systick
	DMA_Cmd(SPI_PORT_TX_DMA_STREAM, DISABLE);
	SPI_PORT_TX_DMA_STREAM->NDTR = len;
	SPI_PORT_TX_DMA_STREAM->M0AR = (uint32_t) buf;
	DMA_Cmd(SPI_PORT_TX_DMA_STREAM, ENABLE);
}


// wait as long as the DMA attached to the display is in the state given
void Cube_WaitWhileDMA(FunctionalState state)
{
	while (DMA_GetCmdStatus(SPI_PORT_TX_DMA_STREAM) == state)
	{
		;
	}
}


// Conver model to internal buffer
void Cube_Convert(unsigned int layer)
{
	int i;
	int index;

    for(i = 0; i < (DIMENSION*DIMENSION); i++)
    {
		int x = i % DIMENSION;
		int y = i / DIMENSION;
		unsigned char model = internalModel[x][y][layer];

        index = i+(i/2);

        if((i % 2) == 0)
        {
            internalBuffer[layer][index] = model;
        }
        else
        {
            internalBuffer[layer][index] = (model >> 4) & 0x0F;
            internalBuffer[layer][index+1] = (model << 4) & 0xF0;
        }
    }
}


// Advance cube model : All fade
void Cube_Tick_1()
{
	static int fade = 511;

	fade-=4;

 	for(int x = 0; x < DIMENSION; x++)
	{
 		for(int y = 0; y < DIMENSION; y++)
		{
		 	for(int z = 0; z < DIMENSION; z++)
    		{
				internalModel[x][y][z] = fade;
			}
		}
	}

	if(fade <= 0)
		fade = 511;

	// Convert to SPI values
	for(int z = 0; z < DIMENSION; z++)
    {
		Cube_Convert(z);
	}
}


// Advance cube model Night Rider
void Cube_Tick_2()
{
	static int dir = 1;
	static int pos = 0;
	static int level = 0;

	// Start empty
	for(int x = 0; x < DIMENSION; x++)
	{
 		for(int y = 0; y < DIMENSION; y++)
		{
		 	for(int z = 0; z < DIMENSION; z++)
    		{
				internalModel[x][y][z] = 0;
			}
		}
	}

	// Advance step
	pos += dir;

	// Set values
	if(((pos) >= 0) && ((pos) < (DIMENSION*DIMENSION)))
	{
		int x = pos % DIMENSION;
		int y = pos / DIMENSION;
		internalModel[x][y][level] = 255;
	}
	if(((pos + (-dir * 1) >= 0) && ((pos + (-dir * 1) < (DIMENSION*DIMENSION)))))
	{
		int x = (pos + (-dir * 1)) % DIMENSION;
		int y = (pos + (-dir * 1)) / DIMENSION;
		internalModel[x][y][level] = 200;
	}
	if(((pos + (-dir * 2) >= 0) && ((pos + (-dir * 2) < (DIMENSION*DIMENSION)))))
	{
		int x = (pos + (-dir * 2)) % DIMENSION;
		int y = (pos + (-dir * 2)) / DIMENSION;
		internalModel[x][y][level] = 150;
	}
	if(((pos + (-dir * 3) >= 0) && ((pos + (-dir * 3) < (DIMENSION*DIMENSION)))))
	{
		int x = (pos + (-dir * 3)) % DIMENSION;
		int y = (pos + (-dir * 3)) / DIMENSION;
		internalModel[x][y][level] = 100;
	}
	if(((pos + (-dir * 4) >= 0) && ((pos + (-dir * 4) < (DIMENSION*DIMENSION)))))
	{
		int x = (pos + (-dir * 4)) % DIMENSION;
		int y = (pos + (-dir * 4)) / DIMENSION;
		internalModel[x][y][level] = 50;
	}
	if(((pos + (-dir * 5) >= 0) && ((pos + (-dir * 5) < (DIMENSION*DIMENSION)))))
	{
		int x = (pos + (-dir * 5)) % DIMENSION;
		int y = (pos + (-dir * 5)) / DIMENSION;
		internalModel[x][y][level] = 5;
	}

	// Turn
	if(pos > (DIMENSION*DIMENSION))
	{
		dir = -dir;
	}
	if(pos < 0)
	{
		dir = -dir;
		level = (level + 1) % DIMENSION;
	}

	// Convert to SPI values
	for(int z = 0; z < DIMENSION; z++)
    {
		Cube_Convert(z);
	}
}




// Advance cube model single layer
void Cube_Tick_3()
{
	static int level = 0;

	// Start empty
	for(int x = 0; x < DIMENSION; x++)
	{
 		for(int y = 0; y < DIMENSION; y++)
		{
		 	for(int z = 0; z < DIMENSION; z++)
    		{
				internalModel[x][y][z] = 0;
			}

			internalModel[x][y][level] = 255;
		}
	}

	level = (level + 1) % DIMENSION;

	// Convert to SPI values
	for(int z = 0; z < DIMENSION; z++)
    {
		Cube_Convert(z);
	}
}


// Advance cube model single led
void Cube_Tick_4()
{
	static int nr = 0;

	// Start empty
	for(int x = 0; x < DIMENSION; x++)
	{
 		for(int y = 0; y < DIMENSION; y++)
		{
		 	for(int z = 0; z < DIMENSION; z++)
    		{
				internalModel[x][y][z] = 0;
			}
		}
	}

	int x = nr % DIMENSION;
	int y = (nr / DIMENSION) % DIMENSION;
	int z = nr / (DIMENSION * DIMENSION);
	internalModel[x][y][z] = 255;

	nr = (nr + 1) % (DIMENSION*DIMENSION*DIMENSION);

	// Convert to SPI values
	for(int z = 0; z < DIMENSION; z++)
    {
		Cube_Convert(z);
	}
}



/*double map(double in, double inMin, double inMax, double outMin, double outMax)
{
    double out;
    out = (in-inMin)/(inMax-inMin)*(outMax-outMin) + outMin;
    return out;
}*/

/*
// 3D Wave
void Cube_Tick_3()
{
  double z,dummy1,dummy2,dummy3;
  double phase=0;
    for(int x = 0; x < 8; x++)
    {
       for(int y = 0; y < 8; y++)
       {
            dummy1 = map(x,0,7,-PI,PI);
            dummy1 = pow(dummy1,3);
            dummy2 = map(y,0,7,-PI,PI);
            dummy2 = pow(dummy2,3);
            dummy3 = sqrt(dummy2 + dummy1);
            z = sin(phase + dummy3);
            z = round(map(z,-1,1,0,7));
            cubeArray[x][y][(int)z] = 0xFF;
        }
    }
//    Cube_Convert(internalModel);
//    Cube_Convert(cubeArray);
}
*/


void Cube_Send(unsigned int layer)
{
	Cube_putBuffer(internalBuffer[layer], TRANSFER_LENGTH);
}
