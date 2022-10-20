#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

void SPIInit(void);
void SPI1Init(void);
void SPI2Init(void);
void SPI3Init(void);
void SPIWrite(SPI_TypeDef* spi,u8 b);
u8 SPIRead(SPI_TypeDef* spi);
u8 SPIReadWrite(SPI_TypeDef* spi,u8 b);
void SPIClearBuff(SPI_TypeDef* spi);
#endif
