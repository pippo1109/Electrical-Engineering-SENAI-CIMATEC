# Guia de Uso - PLL Digital no LTspice

## Arquivos fornecidos
- `PLL_digital_netlist.sp` — Netlist SPICE completo (recomendado)
- `PLL_digital.asc` — Esquemático LTspice (referência visual)

---

## Como importar no LTspice

### Opção A — Netlist direto (mais simples)
1. Abra o LTspice
2. Menu **File → Open** → selecione `PLL_digital_netlist.sp`
3. O LTspice abrirá como netlist de texto
4. Clique em **Run** (botão play ▶) para simular

### Opção B — Colar no editor de texto
1. No LTspice: **File → New Schematic**
2. Pressione `S` para abrir o editor SPICE
3. Cole o conteúdo do arquivo `.sp`
4. Salve como `.asc` e execute

---

## Parâmetros do circuito

| Bloco | Parâmetro | Valor |
|---|---|---|
| Sinal Vin | Frequência | 10 MHz |
| VCO | Frequência central (F0) | 10 MHz |
| VCO | Sensibilidade (Kvco) | 2 MHz/V |
| Filtro RC | R1 | 10 kΩ |
| Filtro RC | C1 | 10 nF |
| Filtro RC | C2 | 1 nF |
| MOSFET | VTO (NMOS/PMOS) | ±1 V |
| Alimentação | VDD | 5 V |

---

## O que observar nas formas de onda

Adicione as seguintes probes no LTspice:

```
V(RX)     — Sinal de entrada (referência, 10 MHz)
V(Vout)   — Saída do VCO (deve sincronizar com RX)
V(Vfilt)  — Tensão de controle (converge para ~2.5V em lock)
V(QA)     — Pulsos UP do detector de fase
V(QB)     — Pulsos DOWN do detector de fase
```

**Comportamento esperado:**
- Inicialmente, QA e QB emitem pulsos de erro de fase
- Vfilt sobe ou desce ajustando a frequência do VCO
- Após ~1–3 µs o loop trava (*lock*): Vfilt ≈ 2.5 V, Vout sincronizado com RX

---

## Topologia implementada

```
Vin(RX) ──► [DFF A1]──QA──► [PMOS M2]──┐
                ▲                        ├──► Vctrl──[R1]──Vfilt──► VCO ──► Vout
             NAND_OUT                   │                  │
                ▲              [NMOS M1]─┘                [C1][C2]
[DFF A2]──QB──►─┘                                          │
    ▲                                                      GND
Vout (realimentação)
```

---

## Ajuste de parâmetros

Para alterar a **largura de banda do loop** (tradeoff velocidade x ruído):
- Aumente C1 → loop mais lento, mais filtrado
- Diminua C1 → loop mais rápido, mas menos estável

Para alterar a **frequência de operação**:
- Mude o período de `Vin`: `PULSE(0 5 0 1n 1n T/2 T)`
- Ajuste F0 no VCO: `10Meg` → nova frequência
- Mantenha `Kvco` proporcional
