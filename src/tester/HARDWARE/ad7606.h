#ifndef __AD7606_H
#define __AD7606_H
#include "sys.h"

#define ADI_CS		PGout(6)
#define ADI_CV		PBout(5)
#define ADI_BUSY	PGin(7)
#define ADI_RST		PGout(8)


void AD7606Init(void);
void AD7606Reset(void);
void AD7606Start(void);
void AD7606Read(s16* data);

#endif