.include "m328PBdef.inc"

.equ FOSC_MHZ=16	;MHz
.equ DEL_mS=100	;mS
.equ F1=FOSC_MHZ*DEL_mS
	
 reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  
    
    ser r24
    out DDRD,r24
    out PORTD,r24
    
    clr r26
    
loop1:
    ldi r24, low(F1-1)
    ldi r25, high(F1-1)
    rcall wait_x_msec ; 3 cycles
    com r26
    out PORTD,r26
    rjmp loop1
    
         
wait_x_msec:
    
delay_inner:		    
    ldi	r23, 249	    ; 1 cycle
loop_inn:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_inn	    ; 1 or 2 cycles
    
    sbiw r24 ,1		    ; 2 cycles
    brne delay_inner	    ; 2 cycles, except for last repetition of loop
			    ; from start of function up to here we have
			    ; (1 + (1+1+2)*249 - 1 + 2+2)*(F1-1) - 1 = 1000*(F1-1) - 1
			    ; rcall also took 3 cycles.
			    ; totally 3 + 1000*(F1-1) - 1 = 1000*F1 - 998
			    ; ret will take 4 cycles. so we need 994 more cycles.

ldi r23, 248		    ; 1 cycle
loop_final:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_final	    ; 2 cycles, except for last repetition of loop
			    ; from ldi r23 up to here we have
			    ; 1 + 248*4 - 1 = 992 cycles. we need 2 more cycles to reach 994.
    nop			    ; 1 cycle
    nop			    ; 1 cycle
    
    ret			    ; 4 cycles