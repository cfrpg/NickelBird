#ifndef __AD7606FSMC_H
#define __AD7606FSMC_H

#include "sys.h"

#define ADIF_CS		PDout(7)
#define ADIF_CV		PCout(6)
#define ADIF_BUSY	PDin(13)
#define ADIF_RST	PDout(12)
#define ADIF_CV_GPIO	GPIOC
#define ADIF_CV_Pin		6

#define AD7606_RESULT()	*(__IO uint16_t *)0x60000000

void AD7606FSMCInit(void);
void AD7606FSMCReset(void);
void AD7606FSMCStart(void);
void AD7606FSMCRead(s16* data);
void AD7606FSMCSetInternalClk(void);
void AD7606FSMCSetExternalClk(void);

#endif
