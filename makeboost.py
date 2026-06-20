import math
import sys

try:
    MULTIPLIER = int(sys.argv[1])
except:
    MULTIPLIER = 12

print("const uint16_t boost%d[256] = {" % MULTIPLIER)
for x in range(256):
    y = round(math.tanh((x-128)/128.*MULTIPLIER)*512+512)
    if y<0:
        y = 0
    elif y > 1023:
        y = 1023
    print("%d," % y)
print("};")
    