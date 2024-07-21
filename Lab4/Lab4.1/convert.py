def f(x):
    x *= 5

    d0 = x // 1024

    no_d0 = (x - d0 * 1024) * 10
    d1 = no_d0 // 1024
    no_d1 = (no_d0 - d1 * 1024) * 10
    d2 = no_d1 // 1024

    print(d0, ',', d1, d2)

f(526)

