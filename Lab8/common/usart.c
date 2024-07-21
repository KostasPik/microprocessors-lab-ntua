#include<stdint.h>
#include<avr/io.h>

#include "usart.h"

#define BUFFER_END '\n'

void usart_init(uint16_t ubrr) {
	UCSR0A = 0;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr & 0xff);
	UCSR0C = 3 << UCSZ00; //1<<UCSZ01 | 1<<UCSZ00; //8-bit FIXME (maybe wrong)
	//no parity
	//1 stop bit
	//async mode
	//no interrupts
}

void usart_transmit(uint8_t data) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

uint8_t usart_receive(void) {
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void usart_transmit_buffer(uint8_t *buf, int length) {
	while (length-- > 0) usart_transmit(*buf++);
}

int usart_receive_buffer(uint8_t *buf, int length) {
	int bread = 0;
	while (bread<length && ((*buf++ = usart_receive()) != BUFFER_END)) ++bread;
	return bread;
}

