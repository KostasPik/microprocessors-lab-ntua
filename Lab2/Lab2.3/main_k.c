#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<stdbool.h>


bool active = false; // active = false means that PB0 is on for 3 seconds
                     // only when light PB0 is already on (active = true), should we light up all lights for 0.5sec
int normal_counter = 3000;     // counter for timer. (30 * 100 msec  = 3sec)
int blink_timer = 500; //counter for timer.
ISR (INT1_vect) {
    //debouncing
    do {
        EIFR = 1<<INTF1;
        _delay_ms(5);
    } while (EIFR & 1<<INTF1);

    if (active) {
        normal_counter = 3000;
        blink_timer = 500;
        PORTB = 0xFF;
        while(blink_timer-- > 0) {
            sei();
            _delay_ms(1);
            cli(); //ensuring that blink_timer-- >0 happens atomically
        }
        PORTB = 0x01;
    } else {
        active = true;
        PORTB = 0x01;
        normal_counter = 3000;
        while(normal_counter-- > 0) {
            sei();
            _delay_ms(1);
            cli(); //ensuring that normal_counter-->0 happens atomically
        }
        PORTB = 0x00; 
        active = false;
    }
}

int main(int argc, char** argv) {
    EICRA = 1 << ISC11 | 1 << ISC10;
    EIMSK = 1 << INT1;
    sei();

    DDRB  = 0xFF; // set PORTB as output

    while(1); // infinite loop

    return 0;
}
