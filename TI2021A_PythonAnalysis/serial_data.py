import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft

datas = '88 02 87 02 94 02 97 02 9A 02 A0 02 A4 02 A8 02 7F 02 BA 02 C3 02 CD 02 D3 02 DA 02 E2 02 EB 02 F5 02 F1 02 08 03 11 03 1C 03 21 03 37 03 37 03 46 03 4F 03 5F 03 67 03 72 03 7D 03 8C 03 92 03 A1 03 AF 03 BF 03 CD 03 E0 03 E7 03 F5 03 06 04 13 04 1C 04 36 04 43 04 50 04 5F 04 6E 04 7D 04 88 04 9B 04 AE 04 BE 04 D2 04 E7 04 F0 04 FA 04 10 05 21 05 3B 05 48 05 5A 05 65 05 7B 05 85 05 9E 05 B1 05 C3 05 DB 05 F3 05 FD 05 0E 06 1D 06 2F 06 43 06 58 06 6B 06 7F 06 8D 06 A2 06 B3 06 C7 06 DD 06 F9 06 03 07 14 07 29 07 3B 07 4D 07 65 07 74 07 86 07 99 07 AE 07 BF 07 D3 07 E5 07 F9 07 0A 08 1B 08 29 08 47 08 51 08 69 08 7D 08 92 08 9F 08 B1 08 C3 08 D5 08 E5 08 F8 08 09 09 1A 09 29 09 3E 09 4B 09 5D 09 70 09 85 09 8F 09 A4 09 AF 09 C2 09 CB 09 DF 09 EF 09 00 0A 0E 0A 1F 0A 2A 0A 37 0A 47 0A 56 0A 65 0A 70 0A 82 0A 8B 0A 9B 0A A7 0A B7 0A C2 0A CD 0A D9 0A E6 0A EB 0A F7 0A 06 0B 12 0B 14 0B 21 0B 29 0B 38 0B 40 0B 4E 0B 59 0B 64 0B 65 0B 6E 0B 75 0B 75 0B 82 0B 80 0B 92 0B 98 0B 98 0B A3 0B AB 0B AE 0B B5 0B B4 0B BB 0B BF 0B C6 0B CA 0B CF 0B D1 0B D7 0B D7 0B D8 0B DA 0B DC 0B DE 0B DE 0B DF 0B DF 0B DB 0B D4 0B DD 0B DA 0B DC 0B DB 0B D3 0B D5 0B D4 0B CE 0B D3 0B D2 0B C4 0B C6 0B C6 0B C0 0B B7 0B B4 0B AC 0B A8 0B 9C 0B 9A 0B 93 0B 88 0B 88 0B 87 0B 79 0B 6C 0B 69 0B 61 0B 59 0B 52 0B 4A 0B 38 0B 39 0B 2B 0B 20 0B 15 0B 0B 0B 00 0B FC 0A EB 0A E5 0A D2 0A C6 0A BA 0A AE 0A A5 0A 96 0A 86 0A 7A 0A 67 0A 57 0A 46 0A 35 0A 34 0A 2A 0A 1C 0A 07 0A F8 09 E9 09 D7 09 CC 09 BB 09 A9 09 99 09 85 09 78 09 68 09 58 09 46 09 3C 09 2C 09 13 09 01 09 F1 08 D9 08 CD 08 BB 08 A8 08 97 08 85 08 75 08 60 08 4D 08 3D 08 2A 08 17 08 05 08 F1 07 DE 07 CB 07 BB 07 AB 07 93 07 7E 07 6D 07 5B 07 48 07 31 07 1F 07 0F 07 F5 06 E5 06 D0 06 C1 06 AD 06 9F 06 87 06 77 06 64 06 4D 06 42 06 2D 06 17 06 06 06 F2 05 DF 05 CD 05 B8 05 AA 05 97 05 85 05 77 05 63 05 51 05 3E 05 2D 05 1D 05 10 05 FC 04 E7 04 D8 04 C5 04 B6 04 A5 04 99 04 86 04 78 04 69 04 59 04 48 04 39 04 28 04 13 04 0D 04 FD 03 EE 03 E1 03 DB 03 C5 03 BC 03 B1 03 A1 03 91 03 86 03 76 03 6C 03 5D 03 55 03 47 03 3F 03 31 03 32 03 12 03 13 03 0A 03 FF 02 F1 02 EF 02 E9 02 D5 02 D1 02 CD 02 C3 02 B9 02 B5 02 AF 02 AA 02 A7 02 A2 02 97 02 90 02 88 02 84 02 7F 02 7D 02 7A 02 77 02 75 02 73 02 6A 02 6B 02 68 02 65 02 5C 02 62 02 64 02 5B 02 65 02 65 02 60 02 61 02 61 02 61 02 62 02 65 02 66 02 62 02 63 02 6E 02 6E 02 74 02 78 02 84 02 84 02 86 02 89 02 8D 02 90 02 99 02 9E 02 A4 02 AA 02 B3 02 B9 02 BD 02 C8 02 D1 02 DC 02 E1 02 E5 02 F4 02 FE 02 06 03 10 03 16 03 23 03 2E 03 39 03 41 03 4B 03 5B 03 65 03 72 03 7F 03 8D 03 91 03 A4 03 B1 03 BF 03 C8 03 DC 03 E9 03 F4 03 04 04 19 04 1B 04 29 04 38 04 4A 04 5B 04 6B 04 7E 04 8D 04 9E 04 AC 04 BC 04 CB 04 E1 04 F0 04 07 05 15 05 25 05 35 05 42 05 57 05 67 05 7B 05 8D 05 9D 05 B4 05 C7 05 D6 05 E6 05 FB 05 0B 06 18 06 33 06 46 06 5A 06 6F 06 84 06 91 06 A5 06 B7 06 CF 06 D9 06 F1 06 03 07 16 07 2C 07 41 07 50 07 65 07 77 07 87 07 8B 07 B1 07 C1 07 D2 07 F1 07 FC 07 0D 08 20 08 32 08 3E 08 45 08 68 08 7B 08 8E 08 A5 08 B0 08 C5 08 D3 08 E6 08 F2 08 0F 09 19 09 2F 09 42 09 5B 09 61 09 73 09 81 09 91 09 9A 09 B2 09 C0 09 D1 09 E2 09 F2 09 FF 09 11 0A 1D 0A 25 0A 39 0A 4D 0A 5D 0A 65 0A 72 0A 80 0A 8A 0A 97 0A A9 0A B2 0A C8 0A CC 0A D8 0A E7 0A F0 0A FB 0A 05 0B 10 0B 18 0B 25 0B 33 0B 39 0B 3F 0B 4C 0B 56 0B 5D 0B 66 0B 70 0B 6E 0B 7D 0B 87 0B 84 0B 92 0B 9B 0B A0 0B A6 0B AD 0B AF 0B AD 0B B8 0B BC 0B BE 0B C4 0B CB 0B BF 0B D1 0B DC 0B D6 0B DA 0B DF 0B DC 0B D8 0B DC 0B DC 0B DE 0B DF 0B DF 0B DF 0B E0 0B D6 0B DA 0B DB 0B D2 0B D2 0B D5 0B CE 0B CC 0B CA 0B C9 0B C1 0B BB 0B B8 0B B2 0B B1 0B A9 0B 9F 0B 99 0B 92 0B 8B 0B 86 0B 7E 0B 77 0B 6E 0B 69 0B 61 0B 57 0B 4F 0B 4A 0B 3E 0B 31 0B 27 0B 1F 0B 19 0B 0F 0B FE 0A EF 0A E4 0A DB 0A C7 0A C3 0A B9 0A AB 0A 9F 0A 91 0A 86 0A 78 0A 69 0A 57 0A 47 0A 3C 0A 31 0A 21 0A 19 0A 05 0A F7 09 E3 09 D5 09 C9 09 B3 09 A8 09 98 09 85 09 70 09 64 09 58 09 3F 09 32 09 23 09 10 09 03 09 E1 08 DB 08 CA 08 B5 08 9D 08 8F 08 82 08 6D 08 62 08 50 08 38 08 25 08 13 08 FD 07 E7 07 D9 07 CA 07 B6 07 A4 07 93 07 78 07 67 07 58 07 3F 07 2A 07 1D 07 07 07 F9 06 E7 06 D9 06 BD 06 AF 06 95 06 7F 06 6F 06 60 06 4D 06 39 06 22 06 12 06 FD 05 EE 05 DD 05 D2 05 B8 05 A9 05 95 05 81 05 69 05 5B 05 4A 05 38 05 2B 05 19 05 09 05 F3 04 E4 04 CF 04 C2 04 B2 04 A7 04 92 04 84 04 75 04 62 04 50 04 47 04 37 04 1F 04 16 04 09 04 FB 03 E9 03 DE 03 D0 03 BD 03 BA 03 AA 03 9A 03 93 03 85 03 78 03 69 03 5D 03 4C 03 43 03 3B 03 35 03 26 03 1D 03 10 03 07 03 FE 02 F7 02 EB 02 E3 02 DA 02 D5 02 CB 02 C8 02 BB 02 B3 02 B3 02 A7 02 AA 02 9C 02 96 02 91 02 8C 02 88 02 80 02 77 02 79 02 75 02 75 02 65 02 6E 02 6A 02 67 02 63 02 62 02 62 02 62 02 60 02 5F 02 5B 02 60 02 62 02 54 02 62 02 61 02 63 02 64 02 67 02 6D 02 6C 02 76 02 76 02 7A 02 7D 02 7B 02 89 02 8A 02 8F 02 94 02 9D 02 A0 02 A8 02 B3 02 B3 02 B6 02 C0 02 CA 02 D1 02 DD 02 EB 02 ED 02 F5 02 01 03 08 03 0F 03 1B 03 28 03 25 03 43 03 43 03 50 03 56 03 68 03 75 03 80 03 90 03 98 03 A9 03 B8 03 C2 03 D0 03 DE 03 EB 03 F9 03 FF 03 13 04 27 04 35 04 45 04 53 04 67 04 7B 04 82 04 95 04 9F 04 AB 04 BF 04 CE 04 E3 04 F4 04 0A 05 16 05 27 05 3B 05 49 05 64 05 72 05 84 05 93 05 A3 05 B5 05 C9 05 D9 05 EB 05 FF 05 16 06 27 06 35 06 47 06 5D 06 70 06 83 06 93 06 A9 06 BC 06 CF 06 DF 06 EF 06 06 07 1D 07 2D 07 44 07 59 07 67 07 79 07 8E 07 99 07 AA 07 C6 07 D6 07 EB 07 04 08 11 08 27 08 34 08 4D 08 55 08 6B 08 7C 08 92 08 A4 08 C0 08 C5 08 D9 08 EA 08 01 09 0F 09 1F 09 34 09 40 09 53 09 65 09 76 09 82 09 94 09 A5 09 B5 09 CB 09 D9 09 E5 09 F7 09 07 0A 10 0A 1B 0A 2F 0A 40 0A 49 0A 5C 0A 67 0A 77 0A 83 0A 90 0A 9D 0A A2 0A B1 0A C3 0A CE 0A D9 0A DF 0A F0 0A FD 0A 08 0B 19 0B 1F 0B 25 0B 33 0B 41 0B 3D 0B 4B 0B 55 0B 5F 0B 68 0B 74 0B 7A 0B 7B 0B 8D 0B 8E 0B 8D 0B 9A 0B A1 0B A8 0B AD 0B BB 0B AF 0B BD 0B C3 0B C2 0B C2 0B CA 0B CF 0B D2 0B D3 0B D5 0B D6 0B D8 0B DB 0B DD 0B DF 0B DF 0B D9 0B DF 0B DC 0B DE 0B D9 0B D5 0B DA 0B D8 0B D5 0B D7 0B D6 0B CF 0B CA 0B C8 0B BF 0B B8 0B BD 0B B6 0B B0 0B B2 0B AD 0B A1 0B 9B 0B 92 0B 90 0B 87 0B 7B 0B 77 0B 6F 0B 65 0B 5E 0B 54 0B 57 0B 41 0B 6A 0B 2E 0B 2A 0B 1E 0B '
datas = datas.split(" ")[:2048]
together = []
for num in range(len(datas) // 2):
    together.append(datas[num * 2 + 1] + datas[num * 2])
y = np.zeros(1024, np.uint16)
for (index, data) in enumerate(together):
    y[index] = int(data, 16)
print(y[0])
np.save("vol.npy", y)
plt.plot(np.arange(y.size), y / 4096 * 3.3)
plt.show()
plt.plot(np.arange(y.size), np.abs(fft(y)))
plt.show()