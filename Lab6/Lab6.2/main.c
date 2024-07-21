#include "../common/lcd.h"
#include "../common/keypad.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	lcd_init();
	keypad_init();
	DDRD = 0xff;
	while (1) {
		char ch = keypad_to_ascii();
		if (ch == 0) continue;
		lcd_clear_display();
		lcd_data(ch);
	}
}
