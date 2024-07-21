.include "m328PBdef.inc"

.equ FOSC_MHZ=16
.def counter=r19 ; counter for interrupts
.def temp=r20
.equ DEL_ms=500
.equ DEL_NU=FOSC_MHZ*DEL_mS
.equ DEBOUNCE_ms=10 ; ms
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
    
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1

ISR1: ; interrupt handle
	in r29, SREG
	push r23
	push r24
	push r25

fix:	
	;START: for ex 1.b ;
	ldi r18, (1<<INTF1)
	out EIFR, r18 ; set INT0 to zero (logic 1)
	ldi r24, low(DEBOUNCE_NU)
	ldi r25, high(DEBOUNCE_NU)
	rcall delay_ms
	sbic EIFR, INTF1 ; skip if bit INTF1 is 0
	rjmp fix
	;END: for ex 1.b ;
	
	sbic PIND, 6 ; skip if PD6 is cleared (we pressed the button)
	inc counter
	cpi counter, 0x20 ; compare to 32
	breq zero_counter
return_interrupt:
	out PORTC, counter
	pop r25
	pop r24
	pop r23
	out SREG, r29
	reti
zero_counter:
    clr counter ; clear counter because we got to 32
    jmp return_interrupt
    

reset:
    ldi r24, high(RAMEND)
    out SPH, r24
    ldi r25, low(RAMEND)
    out SPL, r25
    
    ; init PORTD as input
    clr temp
    out DDRD, temp
    ser temp
    out PORTD, temp
    ; init PORTC as output

    out DDRC, temp
    out DDRB, temp

    ; init interrupts
    clr temp
    ldi temp, (1 << ISC11 | 1 << ISC10)
    sts EICRA, temp
    
    clr temp
    ldi temp, (1 << INT1)
    out EIMSK, temp
    
    clr counter
    out PORTC, counter
    sei ; set global interrupt flag
    


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
