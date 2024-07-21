#include<stdint.h>
#include<avr/io.h>
#include<util/delay.h>

#include "onewire.h"

#define SETOUTPUT() do { DDRD |= (uint8_t) 1U<<4; } while (0)
#define OUTPUT_0() do { PORTD &= (uint8_t) ~(1U<<4); } while (0)
#define OUTPUT_1() do { PORTD |= (uint8_t) 1U<<4; } while (0)
#define OUTPUT(x) OUTPUT_##x()

#define SETINPUT() do { DDRD &= (uint8_t) ~(1U<<4); PORTD &= (uint8_t) ~(1U<<4); } while (0)
#define READ() ((PIND >> 4) & 1U)

uint8_t one_wire_reset(void) {
	SETOUTPUT();
	OUTPUT(0);
	_delay_us(480);
	SETINPUT();
	_delay_us(100);
	uint8_t ret = (READ() == 0);
	_delay_us(380);
	return ret;
}

uint8_t one_wire_receive_bit(void) {
	SETOUTPUT();
	OUTPUT(0);
	_delay_us(2);
	SETINPUT();
	_delay_us(10);
	uint8_t ret = READ();
	_delay_us(49);
	return ret;
}

void one_wire_transmit_bit(uint8_t datum) {
	SETOUTPUT();
	OUTPUT(0);
	_delay_us(2);
	if (datum & 1) OUTPUT(1);
	else OUTPUT(0);
	_delay_us(58);
	SETINPUT();
	_delay_us(1);
}

uint8_t one_wire_receive_byte(void) {
	uint8_t ret = 0;
	for (int8_t i=0; i<8; ++i) ret |= one_wire_receive_bit() << i;
	return ret;
}

void one_wire_transmit_byte(uint8_t data) {
	for (int8_t i=0; i<8; ++i) {
		one_wire_transmit_bit(data & 1);
		data >>= 1;
	}	
}

