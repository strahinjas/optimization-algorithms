import matplotlib.pyplot as plt
import numpy as np
import scipy.special as sp

x = np.linspace(0, 20, 100)

plt.plot(x, sp.spherical_jn(1, x), label=r'$j_1$')
plt.plot(x, sp.spherical_jn(2, x), label=r'$j_2$')
plt.legend()

plt.title('Spherical Bessel functions of the first kind for n = 1 & n = 2')
plt.xlabel('z')
plt.ylabel(r'$j_n(z)$')

plt.grid()
plt.show()