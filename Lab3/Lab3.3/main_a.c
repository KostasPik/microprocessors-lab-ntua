#define F_CPU 16000000UL
#include<avr/interrupt.h>
#include<util/delay.h>

const unsigned char dc_values[] = {5, 26, 46, 67, 87, 108, 128, 148, 169, 189, 210, 230, 251};

ISR(ADC_vect) {
    TIFR1 = 1 << TOV1;
    OCR0A = ADCH;
};

int main(void) {
    //setup pd1, pd2, pd5, pd7 as input, pd6 as output
    DDRD = 1<<6;
    PORTD = 1<<1 | 1<<2 | 1<<5 | 1<<7;


    //setup timer0 (used for pwm generation)
    TCCR0A = 1 << WGM10 | 1 << COM0A1 | 1 << WGM00 | 1 << WGM01;
    TCCR0B = 1 << CS00;

    //setup timer1 for overflow every 100ms (used for triggering adc).
    //timer freq set at clk/1024. initially stopped
    TCCR1A = 1 << WGM10 | 1 << WGM11;
    TCCR1B = 1 << WGM12 | 1 << WGM13;
    OCR1A = F_CPU / 1024 * 0.1 - 1; //TOP <- 100ms in cycles/1024 - 1

    //setup adc, triggered at timer1 overflow.
    //adc freq set at clk/128 = 128KHz
    ADMUX = 1 << REFS0 | 1 << ADLAR;
    ADCSRA = 1 << ADEN | 1 << ADATE | 1 << ADIE | 1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2;
    ADCSRB = 1 << ADTS1 | 1 << ADTS2;
    DIDR0 = 1 << ADC0D;


    //init to mode1 and 50%
    unsigned char mode = 1;
    const unsigned char *cur_dc = dc_values; //+ 6;
    OCR0A = *cur_dc;
    
    while (1) {
        if (!(PIND & 1 << 5)) {
            while (!(PIND & 1 << 5)) _delay_ms(5);
            //change to mode1.
            mode = 1;
            
            //stop timer1
            TCCR1B &= ~(1 << CS12 | 1 << CS10);
            
            //disabling adc
            cli();
            ADCSRA &= ~(1 << ADEN);

            OCR0A = *cur_dc;

            continue;
        }
        if (!(PIND & 1 << 7)) {
            while (!(PIND & 1 << 7)) _delay_ms(5);
            //change to mode2
            mode = 2;

            //start timer1
            TCCR1B |= 1 << CS12 | 1 << CS10;

            //enabling adc
            sei();
            ADCSRA |= 1 << ADEN | 1 << ADSC;
            
            continue;
        }

        if (mode == 1) {
            //mode1
            if (!(PIND & 1 << 1)) {
                while (!(PINC & 1 << 1)) _delay_ms(5);
                if (cur_dc == dc_values + sizeof (dc_values)) continue;
                ++cur_dc;
                OCR0A = *cur_dc;
            }
            if (!(PIND & 1 << 2)) {
                while (!(PINC & 1 << 2)) _delay_ms(5);
                if (cur_dc == dc_values) continue;
                --cur_dc;
                OCR0A = *cur_dc;
            }
        }

        //mode2 functionality happens at ISR of ADC.
    }
}
