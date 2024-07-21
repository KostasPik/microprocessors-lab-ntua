#ifndef __LCD_H
#define __LCD_H

#include<stdint.h>

void lcd_data(uint8_t data);
void lcd_command(uint8_t command);
void lcd_goto_line1(void);
void lcd_goto_line2(void);
void lcd_clear_display(void);
void lcd_init(void);

void lcd_data_buf(uint8_t *buf, int length);

#endif

