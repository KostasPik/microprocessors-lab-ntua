.include "m328PBdef.inc"

.def A=r16
.def B=r17
.def C=r18
.def D=r19
.def F1=r20
.def F2=r21
.def temp=r22
.def counter=r23

reset:
    ldi A, 0x45
    ldi B, 0x23
    ldi C, 0x21
    ldi D, 0x01
    ldi counter, 5
loop:
    com A
    com B
    com C
    
    mov F1, A
    and F1, B
    and F1, C
    or F1, D
    com F1
    
    ; A,B,C commed
    
    com C
    
    mov F2, A
    or F2, C
    mov temp, D
    com temp
    or temp, B
    and F2, temp
    
    ; A,B commed
    
    com A
    com B
    
    dec counter
    breq exit
    
    subi A, -0x01
    subi B, -0x02
    subi C, -0x04
    subi D, -0x05
    
    rjmp loop

exit:
    rjmp exit