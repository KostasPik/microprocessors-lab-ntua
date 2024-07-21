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

float get_temperature_f(void) {
	int16_t t = get_temperature();

        uint8_t is_neg = t<0;
        if (t<0) t=-t;

        float t_fl = ((int16_t) (t >> 4)) + ((float)(t & 0xf))/16.0;
        if (is_neg) t_fl = -t_fl;

	return t_fl;
}
