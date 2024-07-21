c = 0

ADCH = 0
ADCL = 0
r28 = 0
r29 = 0
r30 = 0
r31 = 0

def lsl(r):
    global c
    c = (r >> 7) & 0x01
    return (r<<1) & 0xff

def rol(r):
    global c
    return ((r<<1) | c) & 0xff

def add(r1,r2):
    global c
    c = ((r1+r2) >> 8) & 0x01
    return (r1+r2) & 0xff

def adc(r1,r2):
    global c
    return (r1+r2+c) & 0xff

def mul10():
    global r28,r29,r30,r31
	#;r31:r30 *= 10 (in-place) (10*x = 8*x + 2*x)
    r28 = r30
    r29 = r31

    r28 = lsl(r28)
    r29 = rol(r29)
    r28 = lsl(r28)
    r29 = rol(r29)
    r28 = lsl(r28)
    r29 = rol(r29)

    r30 = lsl(r30)
    r31 = rol(r31)

    r30 = add(r28, r30)
    r31 = adc(r31, r29)
    
def calc():
    global r28,r29,r30,r31, r17, r18,r19,ADCH,ADCL
	#calc 5*ADC
    r31 = ADCH
    r30 = ADCL

    r28 = r30
    r29 = r31

    r30 = lsl(r30)
    r31 = rol(r31)
    r30 = lsl(r30)
    r31 = rol(r31)
#    r28 = ADCL
 #   r29 = ADCH

    r30 = add(r30, r28)
    r31 = adc(r31, r29)


	#r31:r30 is 5*ADC

	#r17=d0 <- 5*ADC << 10
    r17 = r31
    r17 >>= 1
    r17 >>= 1

    r31 &= 0x03
    mul10()

    #r18=d1
    r18= r31
    r18 >>=1
    r18 >>=1

    r31 &= 0x03
    mul10()

    #;r19=d2
    r19= r31
    r19 >>=1
    r19 >>=1

    r17 += ord('0')
    r18 += ord('0')
    r19 += ord('0')


import random
TESTS = 1000000

for _ in range(TESTS):
    ADCH = random.randint(0,0x03)
    ADCL = random.randint(0,0xff)
    d = ((ADCH & 0x03) << 8) | ADCL
    d = f"{500*d//1024:03d}"
    d = list(map(ord, d))

    calc()
    assert(d == [r17, r18, r19])

