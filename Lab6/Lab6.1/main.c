#define F_CPU 16000000UL

#include "../common/keypad.h"
#include<avr/io.h>
#include<util/delay.h>

int main(void) {
	DDRB = 0xFF;
	keypad_init();
	DDRD = 0xff;
	PORTB = 0x08;
	while (1) {
		switch (keypad_to_ascii_pressed()) {
			case '1': PORTB = 1<<0; break;
			case '5': PORTB = 1<<1; break;
			case '9': PORTB = 1<<2; break;
			case 'D': PORTB = 1<<3; break;
			default: PORTB = 0; break;
		}
	}
}

