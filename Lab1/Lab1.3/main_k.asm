.include "m328PBdef.inc"

.equ FOSC_MHZ=16	;MHz
.equ NORMAL_TIMEOUT=1500 ;ms
		    ;Big timeout is the extra time we have to wait when changing rotation
.equ BIG_TIMEOUT=3500-NORMAL_TIMEOUT ;ms
.def rail=r21    

reset:
    ldi r16,low(RAMEND)
    out SPL,r16
    ldi r16,high(RAMEND)
    out SPH,r16  
    
    ser r16
    out DDRD, r16 ; set PORTD as output
    
    ; T=1 means we are going to the left
    ; T=0 means we are going to the right
    
    set ; in the beginning we are going to the left so T=1
    ldi rail, 0x01
    out PORTD, rail
    ldi r24, low(NORMAL_TIMEOUT)
    ldi r25, high(NORMAL_TIMEOUT)

main_loop:
    rcall wait_x_msec
check_right_edge:
    cpi rail, 0x01 ; check if we are on the right edge
    brne check_left_edge ; if we are not then check if we are on the left edge
    brbc 6, change_direction_to_left ; if we are going to right change direction
    jmp rotate
check_left_edge:
    cpi rail, 0x80 ; check if we are on the left edge
    brne rotate ; if we are not then rotate
    brbs 6, change_direction_to_right ; if we are heading right then change direction
    jmp rotate
change_direction_to_left:
    set ; T = 1
    ldi r24, low(BIG_TIMEOUT) ; make additional timeout
    ldi r25, high(BIG_TIMEOUT)
    rcall wait_x_msec
    lsl rail ; shift left
    out PORTD, rail 
    ldi r24, low(NORMAL_TIMEOUT) ; restore normal timeout
    ldi r25, high(NORMAL_TIMEOUT)
    jmp main_loop
change_direction_to_right:
    clt ; T = 0
    ldi r24, low(BIG_TIMEOUT) ; make additional timeout
    ldi r25, high(BIG_TIMEOUT)
    rcall wait_x_msec
    lsr rail ; shift right
    out PORTD, rail
    ldi r24, low(NORMAL_TIMEOUT) ; restore old timeout
    ldi r25, high(NORMAL_TIMEOUT)
    jmp main_loop

rotate:
    brtc rotate_right ; if T = 0 rotate right
    lsl rail
    out PORTD, rail
    jmp main_loop
rotate_right:
    lsr rail
    out PORTD, rail
    jmp main_loop
    
    
    

wait_x_msec:   
    ldi r16, FOSC_MHZ	    ; 1 cycle
			    ; load frequency (16 MHz) THIS IS THE COUNTER FOR THE LOOP
			    ; r27:r26 will hold the result of the multiplication of FOSC_MHZ * r25:r24
			    ; multiplying as follows: (r27 * 2^8 + r26) * r16 = (r27 * r16 * 2^8) + (r26 * r16)
    mul r16, r24	    ; 1 cycle
    movw r26, r0	    ; 1 cycle
    mul r16, r25	    ; 1 cycle
    add r27, r0		    ; 1 cycle
    
    sbiw r26, 1		    ; 2 cycles, subtract one to account for the overhead
 
    ; need 994-7=987=246*4+3 more cycles (overhead)
    ldi	r23, 246	    ; (1 cycle)	
loop0:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop0		    ; 1 or 2 cycles

    nop			    ; 1 cycle
    nop			    ; 1 cycle
    nop			    ; 1 cycle

    ;total group delay 996 cycles
delay_inner:		    
    ldi	r23, 249	    ; (1 cycle)	
loop_inn:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_inn	    ; 1 or 2 cycles
    
    sbiw r26 ,1		    ; 2 cycles
    brne delay_inner	    ; 1 or 2 cycles

    ret			    ; 4 cycles