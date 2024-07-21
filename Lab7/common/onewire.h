#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include<stdint.h>

uint8_t one_wire_reset(void);
uint8_t one_wire_receive_bit(void);
void one_wire_transmit_bit(uint8_t);
uint8_t one_wire_receive_byte(void);
void one_wire_transmit_byte(uint8_t);

#endif

