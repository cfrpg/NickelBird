#ifndef __AD7606FSMC_H
#define __AD7606FSMC_H
#include "sys.h"

#define ADIF_CS		PCout(13)
#define ADIF_CV		PBout(0)
#define ADIF_BUSY	PBin(2)
#define ADIF_RST		PFout(11)

#define AD7606_RESULT()	*(__IO uint16_t *)0x6C400000

void AD7606FSMCInit(void);
void AD7606FSMCReset(void);
void AD7606FSMCStart(void);
void AD7606FSMCRead(s16* data);

#endif