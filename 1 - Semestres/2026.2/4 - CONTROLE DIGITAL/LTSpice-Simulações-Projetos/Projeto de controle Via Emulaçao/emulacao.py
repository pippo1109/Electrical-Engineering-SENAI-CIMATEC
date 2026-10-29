import sympy as sp
import numpy as np
import matplotlib.pyplot as plt

def coef_calc(B, A, C, D, Ts_valor, N, metodo='tustin'):
    s, z, Ts = sp.symbols('s z Ts')

    def vetor_para_sympy(vetor, var):
        n = len(vetor)
        expr = 0
        for i, coef in enumerate(vetor):
            grau = n - 1 - i
            expr += coef * (var ** grau)
        return expr

    num_G = vetor_para_sympy(B, s)
    den_G = vetor_para_sympy(A, s)
    G = num_G / den_G

    num_Gc = vetor_para_sympy(C, s)
    den_Gc = vetor_para_sympy(D, s)
    Gc = num_Gc / den_Gc

    # Seleção do método de discretização
    metodo = metodo.lower()
    if metodo == 'forward':
        sub_s = (z - 1) / Ts
    elif metodo == 'backward':
        sub_s = (z - 1) / (Ts * z)
    elif metodo == 'tustin':
        sub_s = (2 * (z - 1)) / (Ts *(z + 1))
    else:
        raise ValueError ("Método inválido! Escolha entre 'tustin', 'forward' ou 'backward'.")

    # Aplicação da substituição escolhida
    G_z = sp.simplify(G.subs(s, sub_s))
    Gc_z = sp.simplify(Gc.subs(s, sub_s))

    # Separando numerador e denominador para simulação numérica
    num_Gz, den_Gz = sp.fraction(sp.together(G_z))
    num_Gc_z, den_Gc_z = sp.fraction(sp.together(Gc_z))

    poly_bG = sp.Poly(sp.expand(num_Gz), z)
    poly_aG = sp.Poly(sp.expand(den_Gz), z)
    poly_cC = sp.Poly(sp.expand(num_Gc_z), z)
    poly_dC = sp.Poly(sp.expand(den_Gc_z), z)

    den_g_lead = poly_aG.LC()
    b_n = [c / den_g_lead for c in poly_bG.all_coeffs()]
    a_n = [c / den_g_lead for c in poly_aG.all_coeffs()]

    den_c_lead = poly_dC.LC()
    c_n = [c / den_c_lead for c in poly_cC.all_coeffs()]
    d_n = [c / den_c_lead for c in poly_dC.all_coeffs()]

    # depois de calcular b_n, a_n, c_n, d_n
    b_n = [sp.Integer(0)]*(len(a_n)-len(b_n)) + b_n
    c_n = [sp.Integer(0)]*(len(d_n)-len(c_n)) + c_n

    # Convertendo para float numérico com o Ts substituído
    b = [float(sp.N(coef.subs(Ts, Ts_valor))) for coef in b_n]
    a = [float(sp.N(coef.subs(Ts, Ts_valor))) for coef in a_n]
    c = [float(sp.N(coef.subs(Ts, Ts_valor))) for coef in c_n]
    d = [float(sp.N(coef.subs(Ts, Ts_valor))) for coef in d_n]

    print(f"\nMétodo: {metodo.capitalize()}, Ts = {Ts_valor}")
    print(f"a = {a}")
    print(f"b = {b}")
    print(f"c = {c}")
    print(f"d = {d}")

    # Inicialização dos vetores de simulação
    e = np.zeros(N)
    u = np.zeros(N)
    y = np.zeros(N)
    r = np.zeros(N)

    max_delay = max(len(a), len(b), len(c), len(d))

    # Sinal de referência (Degrau unitário)
    r[max_delay-1:] = 1  

    for i in range(max_delay-1, N):
        e[i] = r[i] - y[i-1]
        
        termo_c = sum(c[k] * e[i - k] for k in range(len(c)))
        termo_d = sum(d[k] * u[i - k] for k in range(1, len(d))) if len(d) > 1 else 0
        u[i] = termo_c - termo_d

        termo_b = sum(b[k] * u[i - k] for k in range(len(b)))
        termo_a = sum(a[k] * y[i - k] for k in range(1, len(a))) if len(a) > 1 else 0
        y[i] = termo_b - termo_a

    # Eixo de tempo e Plotagem
    tempo = (np.arange(N) - (max_delay - 1)) * Ts_valor

    return tempo, y, r

def main(ts_valores, metodos, tempo_total, B, A, C, D):
    # 2. Cria uma figura com subplots lado a lado (1 linha, N colunas)
    fig, axes = plt.subplots(1, len(ts_valores), figsize=(14, 5), sharey=True)

    # Garante que 'axes' seja iterável caso haja apenas um Ts na lista
    if len(ts_valores) == 1:
        axes = [axes]

    # 3. Laço principal para cada tempo de amostragem (cada gráfico/coluna)
    for idx, ts in enumerate(ts_valores):
        ax = axes[idx]
        N_pontos = int(tempo_total / ts)
        
        # Roda os três métodos para este Ts específico
        for metodo in metodos:
            tempo, y, r = coef_calc(B, A, C, D, Ts_valor=ts, N=N_pontos, metodo=metodo)
            
            # Define um estilo de linha diferente para cada método ficar bem visível
            if metodo == 'forward':
                estilo = {'color': '#d95f02', 'linestyle': '-'}
            elif metodo == 'backward':
                estilo = {'color': '#1f78b4', 'linestyle': '--'}
            else:  # tustin
                estilo = {'color': '#1b9e77', 'linestyle': '-.'}
                
            ax.plot(tempo, y, label=metodo.capitalize(), linewidth=2, **estilo)
        
        # Plota a referência tracejada por cima em todos os gráficos
        ax.plot(tempo, r, color='gray', linestyle=':', label='Referência', linewidth=1.5)
        
        # Linha pontilhada horizontal em y = 1.0 para referência visual
        ax.axhline(1.0, color='gray', linestyle=':', alpha=0.5)
        
        # Configurações estéticas do painel
        ax.set_title(f'T = {ts} s', fontsize=12)
        ax.set_xlabel('Tempo (s)')
        if idx == 0:
            ax.set_ylabel('Saída y')
        ax.grid(True, linestyle='--', alpha=0.1)
        ax.legend(loc='lower right')
        ax.set_xlim(0, tempo_total)
    
