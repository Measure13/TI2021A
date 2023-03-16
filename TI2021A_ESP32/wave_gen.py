import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft

n = 1 << 10
t = np.arange(0, 1, 1 / n) #1ms
relative_amp = np.array([1, 0.2, 0.1, 0.1, 0.07])
amp = 30 # mV
offset = 1500 # mV
freq = 2e3 # 1e3~1e5
phase = np.array([0, 0, 0, 0, 0])
wave = np.full(n, offset, np.float32)
for i in range(1, 6):
    wave += amp * relative_amp[i - 1] * np.sin(2 * np.pi * freq * i * t + phase[i - 1])
hanning = (1 - np.cos(2 * np.pi * np.arange(n) / n)) / 2
wave *= hanning

freq_wave = np.abs(fft(wave))
fig, ax_l = plt.subplots()
ax_r = ax_l.twinx()
ax_l.plot(t, wave, label="time")
ax_l.set_ylabel("time dominance")
ax_r.plot(t, freq_wave, c="orange", label="freq")
ax_r.set_ylabel("freq dominance")
fig.legend()
plt.show()