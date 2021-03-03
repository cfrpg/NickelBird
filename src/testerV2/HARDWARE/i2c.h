#ifndef __I2C_H
#define __I2C_H
#include "sys.h"

#define I2C_IN() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<(7*2);delay_us(1);}
#define I2C_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<(7*2);delay_us(1);}
#define I2C_SCL_OUT PBout(6)
#define I2C_SDA_IN PBin(7)
#define I2C_SDA_OUT PBout(7)

#define I2C_WR 0
#define I2C_RD 1

void I2CInit(void);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(u8 b);
u8 I2CReadByte(void);
u8 I2CWaitAck(void);
void I2CAck(void);
void I2CNAck(void);
u8 I2CCheckDevice(u8 addr);

#endif
