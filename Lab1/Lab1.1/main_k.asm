.include "m328PBdef.inc"

.equ FOSC_MHZ=7	;MHz
	
 reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  
    
loop1:
    ldi r24, low(1256)
    ldi r25, high(1256)

   
    rcall wait_x_msec	    ; 3 cycles
    rjmp loop1
    
    
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