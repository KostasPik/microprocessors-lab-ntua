#include "../common/thermometer.h"
#include "../common/lcd.h"
#include "../common/onewire.h"

#include<stdio.h>

int main(void) {
    const char nodevmsg[] = "NO device";

    lcd_init();

    int16_t t = 0xffff;
    while(1) {
        int16_t t_new = get_temperature();
        //if the same message is to be displayed, dont redisplay it.
        if (t == t_new) continue;
        t = t_new;

        if (t == 0x8000) {
            //no device
            lcd_clear_display();
            for (int8_t i=0; i<sizeof(nodevmsg)-1; ++i) lcd_data(nodevmsg[i]);
            continue;
        }

        uint8_t is_neg = t<0;
        if (t<0) t=-t;

        float t_fl = ((int16_t) (t >> 4)) + ((float)(t & 0xf))/16.0;
        if (is_neg) t_fl = -t_fl;

        char message[15];
        int8_t length;
        length = snprintf(message, sizeof(message), "%+.3f""\xdf""C", t_fl);
        lcd_clear_display();
        for (int8_t i=0; i<length && i<sizeof(message); ++i) lcd_data(message[i]);
    }
}

