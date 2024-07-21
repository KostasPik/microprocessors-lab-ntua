.include "m328PBdef.inc"

.def temp = r17

.equ FOSC_MHZ=16
.equ DEBOUNCE_ms=5
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS


.org 0x0
    rjmp reset
.org 0x0a
    rjmp ISR_PCINT2
    
reset:
	//stack setup
    ldi temp, HIGH(RAMEND)
    out SPH, temp
    ldi temp, LOW(RAMEND)
    out SPL, temp

	//timer and fast-pwm setup
    ldi temp, (1 << WGM10 | 1 << COM1A1)
    sts TCCR1A, temp
    ldi temp, (1 << CS10 | 1 << WGM12)
    sts TCCR1B, temp

	//portd set as input, portb set as output
    clr temp
    out DDRD, temp
    ser temp
    out PORTD, temp
    out DDRB, temp //is that necessary?
    
    ;setup pcint at pd1 and pd2
    ldi temp, 1 << PCIE2
    sts PCICR, temp
    ldi temp, 1 << PCINT17 | 1 << PCINT18
    sts PCMSK2, temp

	//init at 50% dc
    ldi ZH, HIGH(DC_VALUES*2+6)
    ldi ZL, LOW(DC_VALUES*2+6)
    lpm temp, z
    sts OCR1AL, temp

    //setup sleep in idle mode only with timer1 turned on
    ldi temp, 1 << SE
    out SMCR, temp
    ldi temp, ~(1<< PRTIM1)
    sts PRR0, temp
    
    sei

main:
    sleep
    rjmp main

ISR_PCINT2:
    sbis PIND, 1
    rjmp inc_dc

    sbis PIND, 2
    rjmp dec_dc    

    reti
    
inc_dc:
	//debouncing
    ldi r24, LOW(DEBOUNCE_NU)
    ldi r25, HIGH(DEBOUNCE_NU)
    rcall delay_mS
    sbis PIND, 1
    rjmp inc_dc

	//if 98% do nothing
    cpi ZL, LOW(DC_VALUES*2+DC_SIZE-1)
    brne do_inc
    cpi ZH, HIGH(DC_VALUES*2+DC_SIZE-1)
    brne do_inc

    reti
do_inc:
    adiw ZL, 1
    lpm temp, z
    sts OCR1AL, temp
    reti

    
dec_dc:
    ldi r24, LOW(DEBOUNCE_NU)
    ldi r25, HIGH(DEBOUNCE_NU)
    rcall delay_mS
    sbis PIND, 2
    rjmp dec_dc

    //if 2% do nothing
    cpi ZL, LOW(DC_VALUES*2)
    brne do_dec
    cpi ZH, HIGH(DC_VALUES*2)
    brne do_dec

    reti
do_dec:

    sbiw ZL, 1
    lpm temp, z
    sts OCR1AL, temp
    reti

.equ DC_SIZE = 13
DC_VALUES: .DW 0x1a05,0x432e,0x6c57,0x9480,0xbda9,0xe6d2,0x00fb

delay_mS:
    ldi r23, 249
loop_inn:
    dec r23
    nop
    brne loop_inn

    sbiw r24, 1
    brne delay_mS
    ret
