#include "onewire.h"
#include "thermometer.h"

int16_t get_temperature(void) {
	if (!one_wire_reset()) return 0x8000L;
	one_wire_transmit_byte(0xCC);
	one_wire_transmit_byte(0x44);
	while (!one_wire_receive_bit());
	if (!one_wire_reset()) return 0x8000L;
	one_wire_transmit_byte(0xCC);
	one_wire_transmit_byte(0xBE);
	
	int16_t ret;
	ret = one_wire_receive_byte(); //LSB byte
	ret |= one_wire_receive_byte() << 8; //MSB byte

	return ret;
}

