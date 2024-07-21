#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<stdbool.h>

uint8_t command;
uint8_t command_temp;
float voltage_counter;

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

int main(int argc, char** argv) {
    // REFSn[1:0]=01 => Vref = 5V
    // MUXn[3:0] = 0011
    // ADC3
    ADMUX = 0b01000011;

    //ADEN = 1 => ADC Enable
    //ADCS = 0 => No conversion (yet)
    // ADIE = 0 => Disable ADC Interrupt
    // ADPS[2:0]=111 => fADC = 16MHz/128=125KHz
    ADCSRA = 0b10000111;

    // init screen
    // DDRD output
    DDRD = 0xFF;
   _delay_ms(200); // wait for screen to initiali\e
    for(int i =0; i<3; ++i) {
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

    lcd_clear_display();

    while(1) {
        _delay_ms(1000);//delay 1 sec
        ADCSRA |= (1 << ADSC); // init conversion
        while(ADCSRA & 1<<ADSC); // it means adc is not finished
        voltage_counter = (float)ADC/1024 * 5.0;
        lcd_clear_display();
        voltage_counter *= 100;
        
        command = (int)voltage_counter%1000/100 + '0';
        lcd_data();
        command='.';
        lcd_data();
        command = (int)voltage_counter%100/10 + '0';
        lcd_data();
        command = (int)voltage_counter%10 + '0';
        lcd_data();               
    }

    return (0);
}