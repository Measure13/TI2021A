import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft

voltage = np.load("voltage.npy")
plt.plot(np.arange(voltage.size), voltage)#, s=1
plt.show()
plt.plot(np.arange(voltage.size), np.abs(fft(voltage)))#, s=1
plt.show()
print(np.sum(voltage))