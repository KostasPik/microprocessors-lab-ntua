#define F_CPU 16000000UL
#include "../common/twi_pca9555.c"
#include<avr/io.h>
#include<stdint.h>

#define A PINB
#define B (PINB >> 1)
#define C (PINB >> 2)
#define D (PINB >> 3)

int main(void) {
	//portb as input
	DDRB = 0x00;
	PORTB = 0xff;
    
	twi_init();

	//PCA9555 port 0 as output
	PCA9555_0_write(REG_CONFIGURATION_0, 0x00);

	while (1) {
		uint8_t res = 0;
		//calc f0
		res |= ~((~A & B) | (~B & C & D)) & 0x01;
		//calc f1
		res |= ((A & C & (B | D)) & 0x01) << 1;

		PCA9555_0_write(REG_OUTPUT_0, res);
	}
}
