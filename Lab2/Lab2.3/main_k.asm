.include "m328PBdef.inc"

    
.equ FOSC_MHZ=16	;MHz
.equ NORMAL_TIMEOUT=3000 ;ms
.equ BLINK_TIMEOUT=500 ;ms
.def temp=r18
.def flag=r17	; this also represents a flag
		; temp = 0XFF means an ongoing timer is active
		; temp = 0X00 means no ongoing timer is active
.equ DEBOUNCE_ms=100 ; ms
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
.equ NORMAL_CYCLES = NORMAL_TIMEOUT * FOSC_MHZ
.equ BLINKING_CYCLES = BLINK_TIMEOUT * FOSC_MHZ
.def active_interrupt_counter=r30
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1



    
 reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24

    ; init interrupts
    ldi r24, (1 << ISC11) | (1 << ISC10)
    sts EICRA, r24
    
    ldi r24, (1 << INT1)
    out EIMSK, r24
    sei
    
    ldi r24, low(NORMAL_CYCLES) ; load timeout
    ldi r25, high(NORMAL_CYCLES)
    ldi r26, low(BLINKING_CYCLES)
    ldi r27, high(BLINKING_CYCLES)
   ;set PORTB as output
   ser temp
   out DDRB, temp
   clr temp
   out PORTB, temp
   clr flag

main:
    jmp main
   
   
ISR1:
    in r29, SREG
    push r29
    sei
    cpi flag, 0x00 ; check if light is still on
    brne LIGHT_EM_UP
    ldi temp, 0x01
    out PORTB, temp ; PB0 lights up
    ser flag ; set flag
    rcall delay_ms_normal ; 3 sec
    clr temp
    out PORTB, temp
    clr flag ; clear flag
    ldi r24, low(NORMAL_CYCLES)
    ldi r25, high(NORMAL_CYCLES)
    pop r29
    out SREG, r29
    reti
LIGHT_EM_UP:
    ser temp ; turn on leds
    out PORTB, temp
    inc active_interrupt_counter
    cpi active_interrupt_counter, 0x01
    ldi r24, low(NORMAL_CYCLES) ; renew timeout
    ldi r25, high(NORMAL_CYCLES)
    ldi r26, low(BLINKING_CYCLES)
    ldi r27, high(BLINKING_CYCLES)
    brne exit_int
    
    rcall delay_mS_blinking
    ldi temp , 0x01
    out PORTB, temp
exit_int:
    dec active_interrupt_counter
    pop r29
    out SREG, r29
    reti
    
 delay_mS_normal:
    ldi r23, 249
    loop_inn:
    dec r23
    nop
    brne loop_inn
    
    sbiw r24, 1
    brne delay_mS_normal
    
    
    ret
 delay_mS_blinking:
    ldi r31, 249
    loop_inn_blinking:
    dec r31
    nop
    brne loop_inn_blinking
    
    sbiw r26, 1
    brne delay_mS_blinking
    
    
    ret