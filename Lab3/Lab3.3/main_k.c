/* 
 * File:   main_k.c
 * Author: kosta
 *
 * Created on 24 October 2023, 15:52
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
bool mode1 = true; // if mode1 = true then mode 1 else mode 2
int main(int argc, char** argv) {
    int array_ptr = 6;
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | ( 1 << CS11);
    DDRD = 0x00; // INPUT
    DDRB = 0xFF; // OUTPUT
    OCR1AL = DC_VALUES[array_ptr];
    
    // init ADC
    ADMUX = 0b01100000;
    
    //ADEN = 1 => ADC Enable
    //ADCS = 0 => No conversion (yet)
    // ADIE = 0 => Disable ADC Interrupt
    // ADPS[2:0]=111 => fADC = 16MHz/128=125KHz
    ADCSRA = 0b10000111;
    
    while(1){
        while(mode1) {
            if (!(PIND & 1<<1)) {
                while(!(PIND & 1<<1)) _delay_ms(5);
                if (!(array_ptr == 12))
                    OCR1AL = DC_VALUES[++array_ptr];
            }
            if(!(PIND & 1<<2)) {
                while(!(PIND & 1<<2)) _delay_ms(5);
                if(!(array_ptr == 0))
                    OCR1AL = DC_VALUES[--array_ptr];
            }
            if (!(PIND & 1<<7)) {mode1 = false;}

        }
        while(!mode1) {
            ADCSRA |= (1 << ADSC); // init conversion
            while(ADCSRA & 1<<ADSC); // it means adc is not finished
            OCR1AL = ADCH;
            
            if (!(PIND & 1<<6)) {mode1 = true;}

        }
    }
    
    return 0;
}

