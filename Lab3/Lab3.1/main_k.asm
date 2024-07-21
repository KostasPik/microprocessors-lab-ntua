.include "m328PBdef.inc"

    
.def temp=r16    
.def duty_control=r17
.def DUTY_CYCLE=r28
.equ FOSC_MHZ=16
.equ DEBOUNCE_ms=100 ; ms
.equ DEBOUNCE_NU=FOSC_MHZ*DEBOUNCE_mS
   
.org 0x0
rjmp reset

    
reset:
    ldi temp, LOW(RAMEND)
    out SPL, temp
    ldi temp, HIGH(RAMEND)
    out SPH, temp
    
    // init pwd pulses
    ldi temp, (1 <<WGM10) | (1 << COM1A1)
    sts TCCR1A, temp
    ldi temp, (1 << WGM12) | (1 << CS11)
    sts TCCR1B, temp
   
    // PORTB output
    ser temp
    out DDRB, temp
    // PORTD input
    clr temp
    out DDRD, temp
    
    ldi DUTY_CYCLE, 50 ; initially 50%
    // init table management
    ldi ZH, HIGH(DC_CONTROL*2)
    ldi ZL, LOW(DC_CONTROL*2)
    
    clr temp
    adiw ZL, 12 // 50% DC at start
    lpm duty_control, z // duty_control is 0x0080 at first (128 - 50% dc)
    sts OCR1AL, duty_control
main:
    sbic PIND, 1
    rjmp check_2
    ;START: Debounce;
debounce1:
    ldi r24, low(DEBOUNCE_NU)
    ldi r25, high(DEBOUNCE_NU)
    rcall delay_ms
    sbis PIND, 1 ; skip if bit INTF1 is 0
    rjmp debounce1
    ;END: Debounce ;
    rcall inc_dc
check_2:
    sbic PIND, 2
    rjmp light_control
debounce2:
    ldi r24, low(DEBOUNCE_NU)
    ldi r25, high(DEBOUNCE_NU)
    rcall delay_ms
    sbis PIND, 2 ; skip if bit INTF1 is 0
    rjmp debounce2
    rcall dec_dc
light_control:
    lpm duty_control, z
    sts OCR1AL, duty_control
    rjmp main
    
inc_dc:
    cpi DUTY_CYCLE, 98
    breq exit_inc
    subi DUTY_CYCLE,-8 // increase by 8
    adiw ZL, 2 // get next array element  
exit_inc:
    ret
    
    
dec_dc:
    cpi DUTY_CYCLE, 2
    breq exit_dec
    subi DUTY_CYCLE, 8
    sbiw ZL, 2
exit_dec:
    ret

    
    
delay_mS:
    ldi r23, 249
    loop_inn:
    dec r23
    nop
    brne loop_inn
    
    sbiw r24, 1
    brne delay_mS
    ret
    
DC_CONTROL:.DW 0x0005,0x0019,0x002e,0x0042,0x0057,0x006b,0x0080,0x0094,0x00a8,0x00bd,0x00d1,0x00e6,0x00fa