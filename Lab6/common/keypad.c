#include<util/delay.h>
#include "twi_pca9555.h"
#include<avr/io.h>

static uint8_t scan_row(uint8_t row) {
	//io1_{row} pull down to zero. (rows measured from bottom to top)
	PCA9555_0_write(REG_OUTPUT_1, (uint8_t) ~(1U<<row));
	uint8_t pressed = PCA9555_0_read(REG_INPUT_1);
	return (uint8_t) (~pressed >> 4) & 0x0f;
}

static uint16_t scan_keypad(void) {
	//MSB 4-bits are 1st line, 2nd MSB 4-bits are 2nd line, ...
	return (uint16_t) ((scan_row(3) << 12) | (scan_row(2) << 8) | (scan_row(1) << 4) | scan_row(0));
}

static uint16_t scan_keypad_rising_edge(void) {
	static uint16_t keypad_before = 0; //static variable preserved across all function calls
	uint16_t keypad_1, keypad_2;
	keypad_1 = scan_keypad();
	_delay_ms(30);
	keypad_2 = scan_keypad();
	uint16_t ret = keypad_1 & keypad_2 & ~keypad_before; //should be pressed both times now, and not be pressed in previous call
	keypad_before = keypad_1 & keypad_2; //storing currently pressed keys
	return ret;
}

static char uint16_to_ascii(uint16_t keypad) {
	//unset all bits except the MSB 1, to ensure that it will work even if two buttons are pressed
	uint16_t msb_mask = 0x8000;
	while (msb_mask > 0 && ~keypad & msb_mask) msb_mask >>= 1;
	keypad &= msb_mask;

	switch (keypad) {
		case 1U<<12: return '1'; break;
		case 1U<<13: return '2'; break;
		case 1U<<14: return '3'; break;
		case 1U<<15: return 'A'; break;

		case 1U<<8:  return '4'; break;
		case 1U<<9:  return '5'; break;
		case 1U<<10: return '6'; break;
		case 1U<<11: return 'B'; break;

		case 1U<<4:  return '7'; break;
		case 1U<<5:  return '8'; break;
		case 1U<<6:  return '9'; break;
		case 1U<<7:  return 'C'; break;

		case 1U<<0:  return '*'; break;
		case 1U<<1:  return '0'; break;
		case 1U<<2:  return '#'; break;
		case 1U<<3:  return 'D'; break;
	}
	return 0;
}

char keypad_to_ascii(void) {
	return uint16_to_ascii(scan_keypad_rising_edge());
}

char keypad_to_ascii_pressed(void) {
	return uint16_to_ascii(scan_keypad());
}

void keypad_init(void) {
	twi_init();
	//pca9555 io1[0] as output, io1[4:7] as input;
	PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);
}

