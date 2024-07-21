.include "m328PBdef.inc"

.equ FOSC_MHZ=16
.equ PD0=0
.equ PD1=1
.equ PD2=2
.equ PD3=3
.equ PD4=4
.equ PD5=5
.equ PD6=6
.equ PD7=7
    
.org 0x0
    jmp reset
.org 0x2a
    rjmp ISR_ADC

write_2_nibbles:
    push r24 ; save r24(LCD_Data)
    in r25 ,PIND ; read PIND
    andi r25 ,0x0f ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    pop r24 ; Recover r24(LCD_Data)
    swap r24 ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ret
    
lcd_data:
    sbi PORTD ,PD2 ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles ; send data
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret

lcd_command:
    cbi PORTD ,PD2 ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles ; send Instruction
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret

lcd_clear_display:
    ldi r24 ,0x01 ; clear display command
    rcall lcd_command
    ldi r24 ,low(5) ;
    ldi r25 ,high(5) ; Wait 5 mSec
    rcall wait_msec ;
    ret

lcd_init:
    ldi r24 ,low(200) ;
    ldi r25 ,high(200) ; Wait 200 mSec
    rcall wait_msec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x20 ; command to switch to 4 bit mode
    out PORTD ,r24
    sbi PORTD ,PD3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,PD3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x28 ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24 ,0x0c ; dislay on, cursor off
    rcall lcd_command
    rcall lcd_clear_display
    ldi r24 ,0x06 ; Increase address, no display shift
    rcall lcd_command ;
    ret

wait_msec:
    push r24 ; 2 cycles
    push r25 ; 2 cycles
    ldi r24 , low(999) ; 1 cycle
    ldi r25 , high(999) ; 1 cycle
    rcall wait_usec ; 998.375 usec
    pop r25 ; 2 cycles
    pop r24 ; 2 cycles
    nop ; 1 cycle
    nop ; 1 cycle
    sbiw r24 , 1 ; 2 cycles
    brne wait_msec ; 1 or 2 cycles
    ret ; 4 cycles
wait_usec:
    sbiw r24 ,1 ; 2 cycles (2/16 usec)
    call delay_8cycles ; 4+8=12 cycles
    brne wait_usec ; 1 or 2 cycles
    ret
delay_8cycles:
    nop
    nop
    nop
    ret

mul10:
	;r31:r30 *= 10 (in-place) (10*x = 8*x + 2*x)

    mov r28, r30
    mov r29, r31

    lsl r28
    rol r29
    lsl r28
    rol r29
    lsl r28
    rol r29

    lsl r30
    rol r31

    add r30, r28
    adc r31, r29

    ret

ISR_ADC:
    ldi r24, 1 << TOV1
    out TIFR1, r24
    


    ;calc 5*ADC
    lds r30, ADCL
    lds r31, ADCH
    

    mov r28, r30
    mov r29, r31

    lsl r30
    rol r31
    lsl r30
    rol r31

    add r30, r28
    adc r31, r29

	;r31:r30 is 5*ADC

	;r17=d0 <- 5*ADC << 10
    mov r17, r31
    lsr r17
    lsr r17

    andi r31, 0x03
    rcall mul10

	;r18=d1
    mov r18, r31
    lsr r18
    lsr r18

    andi r31, 0x03
    rcall mul10

	;r19=d2
    mov r19, r31
    lsr r19
    lsr r19

	;r17,r18,r29 := d0,d1d2
	;convert to ascii (add '0' = 48)
    
    subi r17, -'0'
    subi r18, -'0'
    subi r19, -'0'

    rcall lcd_clear_display

    mov r24, r17
    rcall lcd_data	

    ldi r24, '.'
    rcall lcd_data	
 
    mov r24, r18
    rcall lcd_data	

    mov r24, r19
    rcall lcd_data	
    
    reti	

reset:
	;stack init
    ldi r24, HIGH(RAMEND)
    out SPH, r24
    ldi r24, LOW(RAMEND)
    out SPL, r24

	;read adc0, pc0
    clr r24
    out DDRC, r24
    
    ser r24
    out DDRB, r24
    out PORTB, r24

	;setup timer1 for overflow every 1sec (used for triggering adc).
    ldi r24, 1 << WGM10 | 1 << WGM11
    sts TCCR1A, r24
    ldi r24, 1 << WGM12 | 1 << WGM13 | 1 << CS12 | 1 << CS10
    sts TCCR1B, r24

    ldi r24, HIGH(FOSC_MHZ * 1000000.0 * 1.0 / 1024.0 - 1)  ;TOP <- 1sec in cycles/1024 - 1
    sts OCR1AH, r24
    ldi r24, LOW(FOSC_MHZ * 1000000.0 * 1.0 / 1024.0 - 1)
    sts OCR1AL, r24

	;setup adc, triggered at timer1 overflow.
    ldi r24, 1 << REFS0 | 1 << MUX1
    sts ADMUX, r24
    ldi r24, 1 << ADEN |  1 << ADATE | 1 << ADIE | 1 << ADPS0 | 1 << ADPS1 | 1 << ADPS2
    sts ADCSRA, r24
    ldi r24, 1 << ADTS1 | 1 << ADTS2
    sts ADCSRB, r24
    ldi r24, 1 << ADC2D
    sts DIDR0, r24

	;setup lcd
    ser r24
    out DDRD, r24
    clr r24
    call lcd_init
    ldi r24, low(100)
    ldi r25, high(100) ; delay 100 mS
    call wait_msec

    
    
	;start adc
    lds r24, ADCSRA
    ori r24, 1 << ADSC
    sts ADCSRA, r24

    sei
main:
    rjmp main
