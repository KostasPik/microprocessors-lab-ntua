def mul10(x):
    return (x << 3) + (x << 1) #10*x = 8*x + 2*x

def f(x):
    x = (x << 2) + x # x *= 5

    #integer part
    d0 = x >> 10

    #keep fractional part and mul by 10
    no_d0 = mul10(x & 0x03ff)

    #keep integer part
    d1 = no_d0 >> 10
    
    #keep fractional part and mul by 10
    no_d1 = mul10(no_d0 & 0x03ff)
    
    #keep integer part
    d2 = no_d1 >> 10

    print(d0, ',', d1, d2)

f(526)

