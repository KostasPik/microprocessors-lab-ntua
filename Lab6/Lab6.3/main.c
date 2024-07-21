#include<avr/io.h>
#include<util/delay.h>
#include "../common/keypad.h"

#define TEAM_NO_D1 '1'
#define TEAM_NO_D2 '7'

int main(void) {
	DDRB = 0xff;
	keypad_init();

	while (1) {
		char t, d1, d2;
		while (!(t = keypad_to_ascii()));
		do {
			d1 = t;
		} while (t = keypad_to_ascii());

		while (!(t = keypad_to_ascii()));
		do {
			d2 = t;
		} while (t = keypad_to_ascii());

		if (d1 == TEAM_NO_D1 && d2 == TEAM_NO_D2) {
			PORTB = 0xff;
			_delay_ms(4000);
			PORTB = 0;
			_delay_ms(1000); //up to 5sec do not accept other number
			continue;
		}
		else {
			for (uint8_t i=0; i<10; ++i) {
				PORTB = 0xff;
				_delay_ms(250);
				PORTB = 0;
				_delay_ms(250);
			}
			continue;
		}
	}
}

