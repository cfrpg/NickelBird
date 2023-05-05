#ifndef __AD7606FSMC_H
#define __AD7606FSMC_H

#include "sys.h"

#define ADIF_CS1		PDout(6)
#define ADIF_CS2		PDout(7)
#define ADIF_CV			PCout(6)
#define ADIF_BUSY		PDin(13)
#define ADIF_RST		PDout(12)
#define ADIF_CV_GPIO	GPIOC
#define ADIF_CV_Pin		6

#define ADIF_MODE_SENS	PDin(3)
#define ADIF_RNG_SENS	PCin(13)

#define AD7606_RESULT()	*(__IO uint16_t *)0x60000000

void AD7606FSMCInit(void);
void AD7606FSMCReset(void);
void AD7606FSMCStart(void);
void AD7606FSMCRead(s16* data);

#endif
