# 100   100
# 150   106 150
# 200   55 200
# 250   5
# 300   44
# 350   94
# 400   112（143）275（305）
# 450   61
# 500   12

import numpy as np
import matplotlib.pyplot as plt

x = np.arange(50, 550, 50)
y = np.array([50, 100, 106, 55, 5, 44, 94, 112, 61, 12])
plt.plot(x, y)
plt.show()