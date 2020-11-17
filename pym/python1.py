import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile as waves
import sys
import wave


archivo = 'matplotlib.wav'
muestreo, sonido = waves.read(archivo)

def autocorr(x):
    y = x + 600;
    result = np.correlate(y, y, mode = "full")
    return result


final =  autocorr(sonido.astype(float))

fig, axs = plt.subplots(2)

fig.suptitle('Senyal y autocorrelacion')
axs[0].plot(sonido)
axs[1].plot(final)

plt.show()

