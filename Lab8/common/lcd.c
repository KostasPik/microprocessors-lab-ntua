#include<util/delay.h>

#include "lcd.h"

#define NOP() { asm volatile ("nop"); }

#define RS_DATA 1
#define RS_COMMAND 0

#if defined(LCD_PORTD) && defined(LCD_PCA9555)
#error "Both LCD_PORTD and LCD_PCA9555 are defined. Only one should be defined."
#elif !defined(LCD_PORTD) && !defined(LCD_PCA9555)
#error "Neither LCD_PORTD nor LCD_PCA9555 is defined. One of them must be defined."
#endif

#ifdef LCD_PORTD
#include<avr/io.h>
#define WRITE_LCD(x) PORTD = (uint8_t) (x)
#endif

#ifdef LCD_PCA9555
#include "twi_pca9555.h"
#define WRITE_LCD(x) PCA9555_0_write(REG_OUTPUT_0, (uint8_t) (x))
#endif

static void write_2_nibbles(uint8_t command, uint8_t type) {
	WRITE_LCD((command & 0xf0) | type<<2 | 1U<<3);
	NOP();
	NOP();
	WRITE_LCD((command & 0xf0) | type<<2);

	WRITE_LCD(((command & 0x0f) << 4) | type<<2 | 1U<<3);
	NOP();
	NOP();
	WRITE_LCD(((command & 0x0f) << 4) | type<<2);
}

void lcd_data(uint8_t data) {
	write_2_nibbles(data, RS_DATA);
	_delay_ms(0.250);
}

void lcd_command(uint8_t command) {
	write_2_nibbles(command, RS_COMMAND);
	_delay_ms(0.250);
}

void lcd_goto_line1(void) {
	lcd_command((1U<<7) | 0x00); //set ddram address to 0x00
}

void lcd_goto_line2(void) {
	lcd_command((1U<<7) | 0x40); //set ddram address to 0x40
}

void lcd_clear_display(void) {
	lcd_command(0x01);
	_delay_ms(5);
}

void lcd_init(void) {

#ifdef LCD_PCA9555
	twi_init();
	//configure pca9555_io0 as output
	PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
#endif

#ifdef LCD_PORTD
	DDRD = 0xFF;
#endif

	_delay_ms(200); // wait for screen to initialize
	for(uint8_t i=0; i<3; ++i) {
		WRITE_LCD(0x38); // set 8 bit mode 3 times
		NOP();
		NOP();
		WRITE_LCD( 0x30);
		_delay_ms(0.250);
	}

	// switch to 4bit mode
	WRITE_LCD(0x28);
	NOP();
	NOP();
	WRITE_LCD(0x20);
	_delay_ms(0.250);

	lcd_command(0x28);
	lcd_command(0x0c);
	lcd_clear_display();
}


void lcd_data_buf(uint8_t *buf, int length) {
	while (length-->0) lcd_data(*buf++);
}
