.include "m328PBdef.inc"

.def A=r21
.def B=r22
.def C=r23
.def D=r24
.def F0=r25
.def F1=r26
.def temp=r27
.def temp2=r29
.equ LOOP_TIMES=5
    
reset:
   ldi r24,low(RAMEND)
   out SPL,r24
   ldi r24,high(RAMEND)
   out SPH,r24
   
   clr F0
   clr F1
   ldi A, 0x45
   ldi B, 0x23
   ldi C, 0x21
   ldi D, 0x01
   ldi r28, LOOP_TIMES ; r28 is the counter
   
	    ; first expression simplifies to (A+B+C)*D'
loop0:
    mov temp, A ; move A to temp
    or temp, B
    or temp, C ; now temp = A+B+C
    com D ; D = D'
    and temp, D
    mov F0, temp ; F0 = (A+B+CS)*D'
    com A
    mov temp, A
    or temp, C
    com B
    mov temp2, D
    or temp2,B
    and temp, temp2
    mov F1, temp ; F1 = (A'+C)*(B'+D')
    ; now we fix tampered registers
    com B
    com D
    com A
    subi A, -0x01
    subi B, -0x02
    subi C, -0x04
    subi D, -0x05
    dec r28
    brne loop0
    
    

 
