#ifndef __USART_H
#define __USART_H

#include<stdint.h>

void usart_init(uint16_t ubrr);
void usart_transmit(uint8_t data);
uint8_t usart_receive(void);

void usart_transmit_buffer(uint8_t *buf, int length);
int usart_receive_buffer(uint8_t *buf, int length);

#endif

