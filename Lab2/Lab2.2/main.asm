.include "m328PBdef.inc"
    
.equ FOSC_MHZ=16
.equ DEL_ms=1000
.equ DEL_NU=FOSC_MHZ*DEL_mS
.equ DEBOUNCE_ms=5
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
    
.org 0x0
rjmp reset
.org 0x2
rjmp ISR0

add1:
    lsl r28
    ori r28, 0x1
    ret
    
    ;r27, r28 are used exclusively by ISR1
ISR0:
    push r24
    push r25
    in r27, SREG   
    
    isr0_loop:
    clr r28
    
    sbis PINB, 0
    rcall add1
    sbis PINB, 1
    rcall add1
    sbis PINB, 2
    rcall add1
    sbis PINB, 3
    rcall add1
    sbis PINB, 4
    rcall add1
    
    ;store what was displayed in portc before
    in r29, PORTC
    
    out PORTC, r28
    
    ;while PD2 is pressed the 'interrupt' function should happen.
    ldi r30, (1<<INTF0)
    out EIFR, r30
    ldi r24, LOW(DEBOUNCE_NU)
    ldi r25, HIGH(DEBOUNCE_NU)
    rcall delay_mS
    sbic EIFR, INTF0
    rjmp isr0_loop
    
    ;recover to portc what was displayed before
    out PORTC, r29
    
    pop r25
    pop r24
    out SREG, r27
    reti
    
    
    
reset:
;Init Stack Pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r25, HIGH(RAMEND)
    out SPH, r25

;Init PORTB,PORTD as input
    clr r26
    out DDRB, r26
    out DDRD, r26
    ser r26
    out PORTB, r26
    out PORTD, r26
;Init PORTC as output
    out DDRC, r26
    
    ldi r24, 0 ;negative-edge triggered
    sts EICRA, r24
    ldi r24, (1 << INT0)
    out EIMSK, r24
    
    sei
    
;other function (according to exercise) unrelated to counting interrupts
loop1:
    clr r26
    loop2:
    out PORTC, r26
    
    ldi r24, LOW(DEL_NU)
    ldi r25, HIGH(DEL_NU)
    
    rcall delay_mS
    
    inc r26
    
    cpi r26, 32
    breq loop1
    rjmp loop2
    
delay_mS:
    ldi r23, 249
    loop_inn:
    dec r23
    nop
    brne loop_inn
    
    sbiw r24, 1
    brne delay_mS
    
    ret
