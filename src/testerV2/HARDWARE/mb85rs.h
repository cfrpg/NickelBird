#ifndef __MB85RS_H
#define __MB85RS_H
#include "sys.h"

#define FRAM_CS PBout(12)

#define FRAM_WREN 0x06
#define FRAM_WRDI 0x04
#define FRAM_RDSR 0x05
#define FRAM_WRSR 0x01
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_RDID 0x9F

#define FRAM_24BIT_ADDR

void FRAMInit(void);
void FRAMWrite(u32 addr,u32 len,u8* data);
void FRAMRead(u32 addr,u32 len,u8* data);

#endif
