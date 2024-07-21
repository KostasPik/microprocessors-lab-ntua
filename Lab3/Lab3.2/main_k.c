/* 
 * File:   main_k.c
 * Author: kosta
 *
 * Created on 23 October 2023, 14:16
 */

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<stdbool.h>

/*
 * 
 */
unsigned const char DC_VALUES[] = {5,26,47,68,89,110,128,152,173,194,215,236,250};
float voltage_input = 0.0;
int main(int argc, char** argv) {
    int array_ptr = 6;
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | ( 1 << CS11);
    DDRB = 0xFF; // OUTPUT
    DDRD = 0xFF; // OUTPUT
    DDRC = 0; //INPUT
    OCR1AL = DC_VALUES[array_ptr];
        
    // REFSn[1:0]=01 => Vref = 5V
    // MUXn[3:0] = 0001
    // ADLAR = 1 => Left adjusted
    ADMUX = 0b01000001;
    
    //ADEN = 1 => ADC Enable
    //ADCS = 0 => No conversion (yet)
    // ADIE = 0 => Disable ADC Interrupt
    // ADPS[2:0]=111 => fADC = 16MHz/128=125KHz
    ADCSRA = 0b10000111;
    while(1) {
        if (!(PINC & 1<<4)) {
            while(!(PINC & 1<<4)) _delay_ms(5);
            if (!(array_ptr == 12))
                OCR1AL = DC_VALUES[++array_ptr];
        }
        if(!(PINC & 1<<5)) {
            while(!(PINC & 1<<5)) _delay_ms(5);
            if(!(array_ptr == 0))
                OCR1AL = DC_VALUES[--array_ptr];
        }
        _delay_ms(100);
        
        // Start conversion
        ADCSRA |= (1 << ADSC); // init conversion
        while(ADCSRA & 1<<ADSC); // it means adc is not finished
        
        // we need to divide by 2^10
        // dividing by 1024 means 10 shift to the right
        // we have ADCH:ADCL 16 bits and 10bits used
        // so when shifting 10 times right we get the value of ADCH in the ADCL reg
        
        // so just mess with ADCH reg
        voltage_input = (float)ADC/1024 * 5.0;
        if (voltage_input > 0 && voltage_input <= 0.625) PORTD = 0x01;
        else if (voltage_input > 0.625 && voltage_input <= 1.25) PORTD = 0x02;
        else if (voltage_input > 1.25 && voltage_input <= 1.875) PORTD = 0x04;
        else if (voltage_input > 1.875 && voltage_input <= 2.5) PORTD = 0x08;
        else if (voltage_input > 2.5 && voltage_input <=3.125) PORTD = 0x10;
        else if (voltage_input > 3.125 && voltage_input <=3.75) PORTD = 0x20;
        else if (voltage_input > 3.75 && voltage_input <= 4.375) PORTD = 0x40;
        else if (voltage_input > 4.375 && voltage_input <= 5) PORTD = 0x80;
    }
    
    return 0;
}

