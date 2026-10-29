import sympy as sp
import numpy as np
import matplotlib.pyplot as plt
from emulacao import coef_calc, main

# Coeficientes da planta e do controlador
# G(s) = (B(s) / A(s))
B = [1]
A = [0.5, 1.5, 1, 0]

# Gc(s) = (C(s) / D(s))
C = [108.7, 5]
D = [205.2265, 1]

# 1. Tempos de amostragem (Ts)
ts_valores = [0.1, 0.01] 
metodos = ['forward', 'backward', 'tustin']
tempo_total = 45.0  # Tempo total de simulação para o gráfico

main(ts_valores, metodos, tempo_total, B, A, C, D)

plt.suptitle('Exercício 3', fontsize=14, fontweight='bold', y=1)
plt.tight_layout()
plt.show()
