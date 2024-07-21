#ifndef __ESP8266_H
#define __ESP8266_H

#include<stdint.h>

#define CMD_CONNECT	0
#define CMD_URL		1
#define CMD_PAYLOAD	2
#define CMD_TRANSMIT	3


void esp8266_init(void);
int8_t esp8266_command(uint8_t command_type, uint8_t *arg, int arg_length);

#endif

