#define F_CPU 16000000UL
#include<avr/interrupt.h>
#include<util/delay.h>

const unsigned char dc_values[] = {5, 26, 46, 67, 87, 108, 128, 148, 169, 189, 210, 230, 251};

ISR(ADC_vect) {
    TIFR1 = 1 << TOV1;
    unsigned char level = ADCH >> 5; //8 levels. therefore keep the 3 msb bits of ADCH 
    PORTD = 1 << level;
};

int main(void) {
    //setup portc as input, portb and portd as output
    DDRC = 0;
    PORTC = 1 << 4 | 1 << 5;
    DDRB = 0xff;
    DDRD = 0xff;

    //setup timer0 (used for pwm generation)
    TCCR0A = 1 << WGM10 | 1 << COM0A1 | 1 << WGM00 | 1 << WGM01;
    TCCR0B = 1 << CS00;

    //setup timer1 for overflow every 100ms (used for triggering adc).
    //timer freq set at clk/1024
    TCCR1A = 1 << WGM10 | 1 << WGM11;
    TCCR1B = 1 << WGM12 | 1 << WGM13 | 1 << CS12 | 1 << CS10;
    OCR1A = F_CPU / 1024 * 0.1 - 1; //TOP <- 100ms in cycles/1024 - 1

    //setup adc, triggered at timer1 overflow.
    //adc freq set at clk/128 = 128KHz
    ADMUX = 1 << REFS0 | 1 << ADLAR;
    ADCSRA = 1 << ADEN | 1 << ADATE | 1 << ADIE | 1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2;
    ADCSRB = 1 << ADTS1 | 1 << ADTS2;
    DIDR0 = 1 << ADC0D;

    sei();
    ADCSRA |= 1 << ADSC;

    const unsigned char *cur_dc = dc_values + 6;
    OCR0A = *cur_dc;

    while (1) {
        if (!(PINC & 1 << 4)) {
            while (!(PINC & 1 << 4)) _delay_ms(5); //debouncing
            if (cur_dc == dc_values + sizeof (dc_values)) continue;
            ++cur_dc;
            OCR0A = *cur_dc;
        }
        if (!(PINC & 1 << 5)) {
            while (!(PINC & 1 << 5)) _delay_ms(5); //debouncing
            if (cur_dc == dc_values) continue;
            --cur_dc;
            OCR0A = *cur_dc;
        }
    }
}
