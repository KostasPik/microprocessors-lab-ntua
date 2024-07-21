#include<util/delay.h>
#include<avr/io.h>
#include "../common/esp8266.h"
#include "../common/lcd.h"
#include "../common/usart.h"

#define DELAY 1500

int main() {
	uint8_t url[] = "\"http://192.168.1.250:5000/data\"";

	uint8_t success1_msg[] = "1.Success";
	uint8_t fail1_msg[] = "1.Fail";
	uint8_t success2_msg[] = "2.Success";
	uint8_t fail2_msg[] = "2.Fail";

	int8_t ret;

	esp8266_init();
	lcd_init();

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

end:
	while(1);
}

