#define F_CPU 16000000UL
#include "../common/twi_pca9555.c"

#define SWAP_NIBBLE(op) { asm("swap %0" : "+r" (op)); }

int main() {
	twi_init();

	//pca9555 io0 as output
	PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
	//pca9555 io1[0] as output, io1[4:7] as input;
	PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);

	//io1_0 pull down to zero.
	PCA9555_0_write(REG_OUTPUT_1, 0x00);
	while (1) {
		uint8_t pressed = PCA9555_0_read(REG_INPUT_1);
		//SWAP_NIBBLE(pressed);
		pressed = ~pressed >> 4;
		PCA9555_0_write(REG_OUTPUT_0, pressed);
	}
}
