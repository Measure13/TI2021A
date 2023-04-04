import numpy as np
import matplotlib.pyplot as plt

datas = '''

'''
datas = datas.split(" ")
together = []
for num in range(len(datas) // 2):
    together.append(datas[num * 2 + 1] + datas[num * 2])
y = np.zeros(1024, np.uint16)
for (index, data) in enumerate(together):
    y[index] = int(data, 16)
print(y[0])
plt.plot(np.arange(y.size), y)
plt.show()