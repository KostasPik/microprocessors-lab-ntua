#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdbool.h>


uint8_t command;
uint8_t command_temp;
void write_2_nibbles() {
    uint8_t r25 = PIND;
    r25 &= 0x0f;
    command_temp = command & 0xf0;
    command_temp |= r25;
    PORTD = command_temp;
    PORTD |= (1<<3); // Enable Pulse
    asm("nop");
    asm("nop");
    PORTD &= ~(1<<3); // Clear Pulse
    
    command_temp = (command & 0x0f) << 4;
    command_temp |= r25;
    PORTD = command_temp;
    PORTD |= (1<<3); // Enable Pulse
    asm("nop");
    asm("nop");
    PORTD &= ~(1<<3); // Clear Pulse
    return;
}

void lcd_data() {
    PORTD |= (1<<2); //RS=1 (data)
    write_2_nibbles();
    _delay_ms(0.250);
    return;
}

void lcd_command() {
    PORTD &= ~(1 << 2); // Clear Enable
    write_2_nibbles();
    _delay_ms(0.250);
    return;
}

void lcd_clear_display(){
    command = 0x01;
    lcd_command();
    _delay_ms(5);
    return;
}

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
    
    DDRD = 0xFF;
      // lcd initialization
    _delay_ms(200); // wait for screen to initialize
    for(uint8_t i =0; i<3; ++i) {
        PORTD = 0x30; // set 8 bit mode 3 times
        PORTD |= (1<<3); // Enable pulse
        asm("nop");
        asm("nop");
        PORTD &= ~(1 << 3); // Clear Enable
        _delay_ms(0.250);
    }

    // switch to 4bit mode
    PORTD = 0x20;
    PORTD |= (1<<3); // Enable pulse
    asm("nop");
    asm("nop");
    PORTD &= ~(1 << 3); // Clear Enable
    _delay_ms(0.250);

    command = 0x28;
    lcd_command();

    command = 0x0c;
    lcd_command();
    
    while(1) {
        lcd_clear_display();
        uint16_t returned_data = get_temperature();
        if (returned_data != 0x8000) {
            float temperature = ~returned_data() + 1;
            if (temperature < 0) command = '-'; lcd_data();
            if(temperature >= 100) {
                command = temperature / 100 + '0';; lcd_data();
                command = temperature / 10 + '0';; lcd_data();
                command = temperature % 10 + '0';; lcd_data();
            } else {
                command = temperature / 10 + '0';; lcd_data();
                command = temperature % 10 + '0';; lcd_data();
                command = '.'; lcd_data();
                command = temperature * 10 % 10 + '0'; lcd_data();
            }
            continue;
        }
        
        // if data doesn't exist
        char no_device_array[] = {'N','O', ' ', 'D', 'e', 'v', 'i', 'c', 'e', '\0'}; // '\0' denotes the null terminator
        uint8_t pos = 0;
        while(no_device_array[pos] != '\0') {
            command = no_device_array[pos++]; lcd_data();
        }
    }
    return (EXIT_SUCCESS);
}

