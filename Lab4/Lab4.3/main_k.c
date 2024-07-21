#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<stdbool.h>

uint8_t command;
uint8_t command_temp;
float vGas0 = 0.1;
float voltage_counter;
float sensitivityCode = 129;
float M_inverse = 77.5193;
float gas_concentration;
float leds_level;
uint8_t TIAGain = 100;

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
    // DDRB output
    DDRB = 0xFF;
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
        ADCSRA |= (1 << ADSC); // init conversion
        while(ADCSRA & 1<<ADSC); // it means adc is not finished
        voltage_counter = (float)ADC/1024 * 5.0;
        gas_concentration = M_inverse * (voltage_counter - vGas0);
        lcd_clear_display();

        if (gas_concentration >= 70) {
//            command = 'GAS DETECTED';
            command = 'G'; lcd_data();
            command = 'A'; lcd_data();
            command = 'S'; lcd_data();
            command = ' '; lcd_data();
            command = 'D'; lcd_data();
            command = 'E'; lcd_data();
            command = 'T'; lcd_data();
            command = 'E'; lcd_data();
            command = 'C'; lcd_data();
            command = 'T'; lcd_data();
            command = 'E'; lcd_data();
            command = 'D'; lcd_data();
            
        }
        else {
            
            command = 'C'; lcd_data();
            command = 'L'; lcd_data();
            command = 'E'; lcd_data();
            command = 'A'; lcd_data();
            command = 'R'; lcd_data();
        }
        leds_level = gas_concentration * 6.0 / 500.0;
        PORTB = (1<<(int)leds_level) - 1;
        if (gas_concentration >= 70) {
            _delay_ms(250);
            PORTB = 0;
            _delay_ms(250);
        }
        else _delay_ms(100);//delay 1 sec
        
        
    }

    return (0);
}
