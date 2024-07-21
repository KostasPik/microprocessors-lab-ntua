#include<stdio.h>
#include<util/delay.h>
#include<avr/io.h>
#include "../common/esp8266.h"
#include "../common/lcd.h"
#include "../common/usart.h"

#include "../common/thermometer.h"
#include "../common/adc.h"
#include "../common/keypad.h"

#define DELAY 1500
#define NURSE_DIGIT '7'
#define NURSE_CANCEL '#'

#define TEMP_OFFSET 11

int main() {
	static uint8_t url[] = "\"http://192.168.1.250:5000/data\"";

	static uint8_t success1_msg[] = "1.Success";
	static uint8_t fail1_msg[] = "1.Fail";
	static uint8_t success2_msg[] = "2.Success";
	static uint8_t fail2_msg[] = "2.Fail";
	static uint8_t success3_msg[] = "3.Success";
	static uint8_t fail3_msg[] = "3.Fail";

	int8_t ret;

	esp8266_init();
	lcd_init();
	keypad_init();

	ret = esp8266_command(CMD_CONNECT, 0, 0);
	lcd_clear_display();
	switch (ret) {
		case 0:
			lcd_data_buf(success1_msg, sizeof(success1_msg)-1);
			break;
		case 1:
			lcd_data_buf(fail1_msg, sizeof(fail1_msg)-1);
			goto end;
			break;
		default:
			goto end;
			break;
	}

	_delay_ms(DELAY);
	ret = esp8266_command(CMD_URL, url, sizeof(url)-1);
	lcd_clear_display();
	switch (ret) {
		case 0:
			lcd_data_buf(success2_msg, sizeof(success2_msg)-1);
			break;
		case 1:
			lcd_data_buf(fail2_msg, sizeof(fail2_msg)-1);
			goto end;
			break;
		default:
			goto end;
			break;
	}

	adc_init();

	static uint8_t ok_msg[] = "OK";
	static uint8_t nurse_msg[] = "NURSECALL";
	static uint8_t checkpres_msg[] = "CHECKPRESSURE";
	static uint8_t checktemp_msg[] = "CHECKTEMP";

	uint8_t nurse = 0;
	while (1) {
		float t = get_temperature_f();
		t += TEMP_OFFSET;
		float p = (((float) adc_measure()) * 20.0 / 1024.0);
		switch (keypad_to_ascii()) {
			case NURSE_DIGIT: nurse = 1; break;
			case NURSE_CANCEL: nurse = 0; break;
		}

		char *status = ok_msg;
		int status_lim = sizeof(ok_msg)-1;
		if (nurse) {
			status = nurse_msg;
			status_lim = sizeof(nurse_msg)-1;
		} else {
			if (p > 12 || p < 4) {
				status = checkpres_msg;
				status_lim = sizeof(checkpres_msg)-1;
			}
			if (t < 34 || t > 37) {
				status = checktemp_msg;
				status_lim = sizeof(checktemp_msg)-1;
			}
		}

		char buf[300];
		int buf_lim;

		buf_lim = snprintf(buf, sizeof(buf), "%.1f %.1f", t, p);
		lcd_clear_display();
		lcd_data_buf(buf, buf_lim);
		lcd_goto_line2();
		lcd_data_buf(status, status_lim); 
		_delay_ms(500);

		buf_lim = snprintf(buf, sizeof(buf), "[{\"name\": \"temperature\",\"value\": \"%.1f\"},{\"name\": \"pressure\",\"value\""
				": \"%.1f\"},{\"name\": \"team\",\"value\": \"17\"},{\"name\": \"status\",\"value\": \"%s\"}]", t, p, status);

		ret = esp8266_command(CMD_PAYLOAD, buf, buf_lim);
		lcd_clear_display();
		switch (ret) {
			case 0:
				lcd_data_buf(success3_msg, sizeof(success3_msg)-1);
				break;
			case 1:
				lcd_data_buf(fail3_msg, sizeof(fail3_msg)-1);
				goto end;
				break;
			default:
				goto end;
				break;
		}
		_delay_ms(500);

		buf[0] = '4';
		buf[1] = '.';
		buf_lim = esp8266_command(CMD_TRANSMIT, buf+2, sizeof(buf)-2);
		buf_lim += 2;
		lcd_clear_display();
		if (buf_lim<0) goto end;
		lcd_data_buf(buf, buf_lim);
		_delay_ms(500);


	}
end:
	while(1);
}

