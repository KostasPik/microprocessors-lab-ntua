dc = []

for p in range(2,101,8):
    dc.append(round(256*p/100))

print(f"size: {len(dc)}")

if len(dc) % 2 != 0: dc.append(0)

for i in range(len(dc)//2):
    print(f"0x{dc[2*i+1]:02x}{dc[2*i]:02x}", end=',')

print()

