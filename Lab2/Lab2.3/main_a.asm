.include "m328PBdef.inc"
    

.equ FOSC_MHZ=16  
.equ DEBOUNCE_mS=50
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
.equ ALLLEDS_mS=500
.equ ALLLEDS_NU=FOSC_MHZ*ALLLEDS_mS
.equ ONELED_mS=2500
.equ ONELED_NU=FOSC_MHZ*ONELED_mS
    
.def ISR1_STATE=r30
    
.org 0x0
    rjmp reset
.org 0x4
    rjmp ISR1

reset:
    ;init stack
    ldi r24, LOW(RAMEND)
    out SPL, r24
    ldi r25, HIGH(RAMEND)
    out SPH, r25
    
    ;init portb as output
    ser r26
    out DDRB, r26
    
    ;setup int1
    ldi r26, (1<<ISC11 | 1<<ISC10)
    sts EICRA, r26
    ldi r26, (1<<INT1)
    out EIMSK, r26
   
    clr ISR1_STATE
    
    ;initially leds are off
    clr r27
    out PORTB, r27
    
    sei
    
main:
    rjmp main
    
ISR1:
    ;debouncing isr1
    isr1_loop:
    ldi r27, (1<<INTF1)
    out EIFR, r27
    ldi r24, LOW(DEBOUNCE_NU)
    ldi r25, HIGH(DEBOUNCE_NU)
    rcall delay_mS
    sbic EIFR, INTF1
    rjmp isr1_loop
    
    
    cpi ISR1_STATE, 1
    breq ISR1_STATE1
    cpi ISR1_STATE, 2
    breq ISR1_STATE2
    
    ISR_STATE0:
    ldi ISR1_STATE, 1
    ldi r26,0x1
    out PORTB, r26
    
    ;wait 3000ms
    ldi r24, LOW(ALLLEDS_NU+ONELED_NU)
    ldi r25, HIGH(ALLLEDS_NU+ONELED_NU)
    sei
    rcall delay_mS
    ldi ISR1_STATE, 0
    
    clr r26
    out PORTB, r26
    
    reti
    
    ISR1_STATE1:
    ldi ISR1_STATE, 2
    ser r27
    out PORTB, r27
    
    ;wait 500ms
    ldi r24, LOW(ALLLEDS_NU)
    ldi r25, HIGH(ALLLEDS_NU)
    sei
    rcall delay_mS
    
    ldi r27, 0x1
    out PORTB, r27
    
    ldi r24, LOW(ONELED_NU)
    ldi r25, HIGH(ONELED_NU)
   
    ldi ISR1_STATE, 1
    reti
   
    ISR1_STATE2:
    ldi r24, LOW(ALLLEDS_NU)
    ldi r25, HIGH(ALLLEDS_NU)
    reti

delay_mS:
    ldi r23, 249
    loop_inn:
    dec r23
    nop
    brne loop_inn

    sbiw r24, 1
    brne delay_mS
    cli
    ret
