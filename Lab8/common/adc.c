#include<stdint.h>
#include<avr/io.h>
#include "adc.h"

void adc_init() {
	ADMUX = 1 << REFS0;
	ADCSRA = 1 << ADEN | 1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2;
	ADCSRB = 0;
	DIDR0 = 1 << ADC0D;
}

uint16_t adc_measure() {
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & 1<<ADSC);
	return ADC;
}

