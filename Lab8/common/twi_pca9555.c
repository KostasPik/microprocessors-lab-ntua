#include<avr/io.h>
#include<util/twi.h>
#include<stdint.h>

#include "twi_pca9555.h"

#define PCA9555_0_ADDRESS 0x40

#define SCL_CLOCK 100000L
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK-16)/2)

#define TW0_STATUS (TWSR0 & 0xF8)


void twi_init(void) {
	TWSR0 = 0; //prescaler = 1
	TWBR0 = TWBR0_VALUE;
}

unsigned char twi_start(uint8_t address) {
	//7 msb of address are address and lsb is set if R and not set if W

	TWCR0 = 1<<TWSTA | 1<<TWINT | 1<<TWEN; //START
	while (!(TWCR0 & (1<<TWINT))); //wait till START transmitted
	if (TW0_STATUS != TW_START && TW0_STATUS != TW_REP_START) return 1; //failed

	TWDR0 = address; //SLA_W or SLA_R
	TWCR0 = 1<<TWINT | 1<<TWEN;

	while (!(TWCR0 & (1<<TWINT))); //wait till SLA transmitted

	//if SLA_R
	if (address & 0x01) {
		if (TW0_STATUS != TW_MR_SLA_ACK) return 1; //failed
	} else {
	//if SLA_W
		if (TW0_STATUS != TW_MT_SLA_ACK) return 1; //failed
	}

	return 0;
}

void twi_stop(void) {
	TWCR0 = 1<<TWSTO | 1<<TWINT | 1<<TWEN; //STOP
	while (!(TWCR0 & (1<<TWSTO))); //wait till STOP transmitted
}

unsigned char twi_write(uint8_t data) {
	TWDR0 = data;
	TWCR0 = 1<<TWINT | 1<<TWEN;
	while (!(TWCR0 & (1<<TWINT))); //wait till transmitted
	if (TW0_STATUS != TW_MR_DATA_ACK) return 1;
	return 0;
}

unsigned char twi_readAck(void) {
	TWCR0 = 1<<TWINT | 1<<TWEA | 1<<TWEN;
	while (!(TWCR0 & (1<<TWINT))); //wait till received
	return TWDR0;
}

unsigned char twi_readNak(void) {
	TWCR0 = 1<<TWINT | 1<<TWEN;
	while (!(TWCR0 & (1<<TWINT))); //wait till received
	return TWDR0;
}

void twi_start_wait(uint8_t address) {
	while (twi_start(address));
}

void PCA9555_0_write(uint8_t reg, uint8_t value) {
	twi_start_wait(PCA9555_0_ADDRESS | TW_WRITE);
	twi_write(reg); //what if PCA9555 NACKs?
	twi_write(value);
	twi_stop();
}

uint8_t PCA9555_0_read(uint8_t reg) {
	uint8_t ret_val;
	twi_start_wait(PCA9555_0_ADDRESS | TW_WRITE);
	twi_write(reg); //what if PCA9555 NACKs?
	twi_start(PCA9555_0_ADDRESS | TW_READ); //what if fails? should we wait instead?
	ret_val = twi_readNak();
	twi_stop();
	return ret_val;
}

