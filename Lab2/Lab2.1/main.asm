.include "m328PBdef.inc"
    
.equ FOSC_MHZ=16
.equ DEL_ms=500
.equ DEL_NU=FOSC_MHZ*DEL_mS
.equ DEBOUNCE_ms=5
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
    
    
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1

    ;r27, r28, r29 are used exclusively by ISR1
    ;r27 is the interrupts counter
ISR1:
    in r29, SREG
    push r23
    push r24
    push r25
    
    ;debouncing isr1 (due to sparking of button)
    isr1_loop:
    ldi r28, (1<<INTF1)
    out EIFR, r28
    ldi r24, LOW(DEBOUNCE_NU)
    ldi r25, HIGH(DEBOUNCE_NU)
    rcall delay_mS
    sbic EIFR, INTF1
    rjmp isr1_loop
    
    ;check if PD6==0 (pressed) and if so return
    sbis PIND, 6
    rjmp isr1_exit
    
    inc r27
    cpi r27, 32
    brne isr1_no32
    clr r27
    isr1_no32:
    out PORTC, r27
    
    isr1_exit:
    pop r25
    pop r24
    pop r23
    out SREG, r29
    reti
    
    
    
reset:
;Init Stack Pointer
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r25, HIGH(RAMEND)
    out SPH, r25

;Init PORTD as input
    clr r26
    out DDRD, r26
    ser r26
    out PORTD, r26
;Init PORTB,PORTC as output
    out DDRB, r26
    out DDRC, r26

    clr r27 ;r27 used only by ISR1
    
    ldi r24, (1<<ISC11 | 1<<ISC10) ;positive-edge triggered
    sts EICRA, r24
    ldi r24, (1 << INT1)
    out EIMSK, r24
    
    sei
    
;other function (according to exercise) unrelated to counting interrupts
loop1:
    clr r26
    loop2:
    out PORTB, r26
    
    ldi r24, LOW(DEL_NU)
    ldi r25, HIGH(DEL_NU)
    
    rcall delay_mS
    
    inc r26
    
    cpi r26, 16
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
