import numpy as np
import matplotlib.pyplot as plt

voltage = np.load("voltage.npy")
plt.plot(voltage)
plt.show()