import numpy as np
import matplotlib.pyplot as plt

real_freq = np.array([100, 78, 89, 53, 41, 33, 67, 12, 16, 10, 22, 27, 33])
repo_freq = np.array([122, 95, 109, 65, 50, 40, 82, 15, 20, 12, 27, 33, 40])
k = np.polyfit(real_freq, repo_freq, 1)
print(k)
pol = np.poly1d(k)

plt.scatter(real_freq, repo_freq)
plt.plot(real_freq, pol(real_freq))
plt.show()

# 100   122
# 78    95
# 89    109
# 53    65
# 41    50
# 33    40
# 67    82
# 12    15
# 16    20(19)
# 10    12
# 22    27
# 27    33
# 33    40