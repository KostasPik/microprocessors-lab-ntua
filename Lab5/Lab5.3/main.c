#define F_CPU 16000000UL

#include "../common/lcd.c"

const char message1[] = "JOHN"; //null-terminated
const char message2[] = "SMITH"; //null-terminated

int main(void) {
	lcd_init();

	for (uint8_t i=0; i<sizeof(message1)-1; ++i) lcd_data(message1[i]);
	lcd_goto_line2();
	for (uint8_t i=0; i<sizeof(message2)-1; ++i) lcd_data(message2[i]);
}

