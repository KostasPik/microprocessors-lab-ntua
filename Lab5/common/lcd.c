#ifndef __LCD_C
#define __LCD_C

#include<util/delay.h>
#include<avr/io.h>
#include "twi_pca9555.c"

#define NOP() { asm volatile ("nop"); }

#define RS_DATA 1
#define RS_COMMAND 0

void write_2_nibbles(uint8_t command, uint8_t type) {
	PCA9555_0_write(REG_OUTPUT_0, (command & 0xf0) | type<<2 | 1<<3);
	NOP();
	NOP();
	PCA9555_0_write(REG_OUTPUT_0, (command & 0xf0) | type<<2);

	PCA9555_0_write(REG_OUTPUT_0, ((command & 0x0f) << 4) | type<<2 | 1<<3);
	NOP();
	NOP();
	PCA9555_0_write(REG_OUTPUT_0, ((command & 0x0f) << 4) | type<<2);
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
	lcd_command((1<<7) | 0x00); //set ddram address to 0x00
}

void lcd_goto_line2(void) {
	lcd_command((1<<7) | 0x40); //set ddram address to 0x40
}

void lcd_clear_display(void) {
	lcd_command(0x01);
	_delay_ms(5);
}

void lcd_init(void) {
	twi_init();

	//configure pca9555_io0 as output
	PCA9555_0_write(REG_CONFIGURATION_0, 0x00);

	_delay_ms(200); // wait for screen to initialize
	for(uint8_t i=0; i<3; ++i) {
		PCA9555_0_write(REG_OUTPUT_0, 0x38); // set 8 bit mode 3 times
		NOP();
		NOP();
		PCA9555_0_write(REG_OUTPUT_0, 0x30);
		_delay_ms(0.250);
	}

	// switch to 4bit mode
	PCA9555_0_write(REG_OUTPUT_0, 0x28);
	NOP();
	NOP();
	PCA9555_0_write(REG_OUTPUT_0, 0x20);
	_delay_ms(0.250);

	lcd_command(0x28);
	lcd_command(0x0c);
	lcd_clear_display();
}

#endif

