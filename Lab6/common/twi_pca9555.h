#ifndef __TWI_PCA9555_H
#define __TWI_PCA9555_H

#include<stdint.h>

#define REG_INPUT_0		0
#define	REG_INPUT_1		1
#define	REG_OUTPUT_0		2
#define REG_OUTPUT_1		3
#define REG_POLARITY_INV_0	4
#define REG_POLARITY_INV_1	5
#define REG_CONFIGURATION_0	6
#define REG_CONFIGURATION_1	7

void twi_init(void);
unsigned char twi_start(uint8_t address);
void twi_stop(void);
unsigned char twi_write(uint8_t data);
unsigned char twi_readAck(void);
unsigned char twi_readNak(void);
void twi_start_wait(uint8_t address);

void PCA9555_0_write(uint8_t reg, uint8_t value);
uint8_t PCA9555_0_read(uint8_t reg);

#endif

