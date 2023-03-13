import struct
import numpy as np

# a = np.array([1.0, 2.0], np.float32)
a = 0.00010022753849625587
# l = list("qwertyuiopasdfghjklzxcvbnm12")
# for i in range(len(l) - 1, -1, -4):
#     # print(l[i:i - 4:-1], i)
#     print(l[3:0:-1])
# l = l[::-1]
# for i in range(0, len(l), 4):
#     print(l[i:i + 4])

print(b := struct.pack("f", a))
# for byte in b:
#     print(byte)
print(b[0:4][::-1])
print(struct.unpack("!f", b))