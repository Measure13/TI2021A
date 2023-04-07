import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft

voltage = np.load("voltage.npy")
plt.scatter(np.arange(voltage.size), voltage, s=1)
plt.show()
plt.scatter(np.arange(voltage.size), np.abs(fft(voltage)), s=1)
plt.show()
print(np.sum(voltage))