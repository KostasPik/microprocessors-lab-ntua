#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdbool.h>

bool one_wire_reset() {

    DDRD = 0x10; // set PD4 as output
    PORTB = 0x00; // clear PD4
    _delay_ms(0.48); // delay 480usec
    
    DDRD = 0x00; // set PD4 as input
    PORTD = 0x00; // disable pull-up resistor
    _delay_ms(0.1); // wait 100usec
    
    uint8_t port_state = PIND; // read PORTD state
    
    _delay_ms(0.38); // wait 380usec
    
    // if device is detected PD4 = 0
    // if device is not detected PD4 = 1
    if (((port_state >> PD4) & 0x01) == 0x01) return false;
    return true;
}


uint8_t one_wire_receive_bit() {
    DDRD = 0x10; // set PD4 as output
    PORTD = 0; // set PD4 to 0
    
    _delay_ms(0.002); // wait 2 usec        
    
    DDRD = 0xff; // set PORTD as input
    _delay_ms(0.01); // wait 10 usec
    
    uint8_t data = PIND;
    
    _delay_ms(0.049); // wait 49usec to meet standards
    return (data >> PD4) & 0x01;
}

// bit_to_transmit should already have the bit to position PD4!
void one_wire_transmit_bit(uint8_t bit_to_transmit) {
    DDRD = 0xFF; // set PORTD as output
    PORTD = 0x00; // set PORTD 0
    _delay_ms(0.002); // delay 2usec
    PORTD = (bit_to_transmit & 0x10); // send PD4 to device
    _delay_ms(0.58); // wait 58usec
    DDRD = 0x00; // set PORTD as input
    _delay_ms(0.001); // wait 1usec
    return;
}

uint8_t one_wire_receive_byte() {
    uint8_t byte_received = 0;
    for(uint8_t i = 0; i < 8; ++i) {
        uint8_t bit_received = one_wire_receive_bit();
        byte_received = byte_received >> 1;
        byte_received |= bit_received;
    }
    return byte_received;
}

void one_wire_transmit_byte(uint8_t byte_to_transmit) {
    for(uint8_t i = 0; i < 8; ++i) {
        uint8_t bit_to_send = byte_to_transmit << PD4;
        one_wire_transmit_bit(bit_to_send);
        byte_to_transmit = byte_to_transmit >> 1;
    }
    return;
}

// Get temperature from DS18B20
int get_temperature() {
    bool device_exists = one_wire_reset();
    if (!device_exists) return 0x8000;
    one_wire_transmit_byte(0xCC);
    one_wire_transmit_byte(0x44);
    // while PD4 is 1 it means device is not finished 
    while (one_wire_receive_bit() == 0x01);
    one_wire_reset();
    one_wire_transmit_byte(0xCC);
    one_wire_transmit_byte(0xBE);
    uint16_t temperature = 0;
    temperature |= one_wire_receive_byte();
    temperature |= ((uint16_t)one_wire_receive_byte() << 8);
    return temperature;
}

int main(int argc, char** argv) {
    while(1) {
        float temperature = ~get_temperature() + 1 ;
        asm("nop");
    }
    return (EXIT_SUCCESS);
}

