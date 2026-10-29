
%% QUESTÃO 5 
clear; clc; close all;

%% DEFINIÇÃO DO SISTEMA
% H(z) = (1 - 0.5*z^-1) / (1 - 0.25*z^-1 - 0.375*z^-2)
b = [1, -0.5];
a = [1, -0.25, -0.375];

%% =========================================================================
% ITEM A: Polos, Zeros e Frações Parciais
% =========================================================================
fprintf('=======================================================\n');
fprintf('  QUESTÃO 5 (a): Polos, Zeros e Frações Parciais\n');
fprintf('=======================================================\n');

% Cálculo de Polos e Zeros
zeros_H = roots(b);
polos_H = roots(a);

fprintf('Zero finito : z_0 = %.2f (há também um zero na origem z = 0)\n', zeros_H);
fprintf('Polos       : p1  = %.2f,  p2 = %.2f\n\n', polos_H(1), polos_H(2));

% Expansão em Frações Parciais em z^-1
[r, p, k] = residuez(b, a);

fprintf('Expansão em Frações Parciais:\n');
fprintf('H(z) = (%.1f) / (1 - %.2f*z^-1) + (%.1f) / (1 - (%.1f)*z^-1)\n\n', ...
        r(1), p(1), r(2), p(2));

% PLOTAGEM ITEM (a): Diagrama de Polos e Zeros no Plano Z
figure('Name', 'Questão 5(a) -- Plano Z', 'NumberTitle', 'off');
zplane(b, a);
hold on;
% Plotagem explicita das fronteiras das ROCs
th = linspace(0, 2*pi, 300);
plot(abs(p(1))*cos(th), abs(p(1))*sin(th), 'r--', 'LineWidth', 1.2, 'DisplayName', 'Fronteira |z|=0.75');
plot(abs(p(2))*cos(th), abs(p(2))*sin(th), 'm--', 'LineWidth', 1.2, 'DisplayName', 'Fronteira |z|=0.5');
title('Questão 5(a): Mapa de Polos e Zeros e Fronteiras de ROC');
grid on;
legend show;

%% =========================================================================
% ITEM B: Respostas ao Impulso Analíticas para Cada ROC
% =========================================================================
fprintf('=======================================================\n');
fprintf('  QUESTÃO 5 (b): Expressões Analíticas para as ROCs\n');
fprintf('=======================================================\n');

fprintf('1. ROC Exterior (|z| > 0.75) -- Causal:\n');
fprintf('   h1[n] = 0.2*(0.75)^n * u[n] + 0.8*(-0.5)^n * u[n]\n\n');

fprintf('2. ROC Anular (0.5 < |z| < 0.75) -- Bilateral e Estável:\n');
fprintf('   h2[n] = -0.2*(0.75)^n * u[-n-1] + 0.8*(-0.5)^n * u[n]\n\n');

fprintf('3. ROC Interior (|z| < 0.5) -- Anticausal:\n');
fprintf('   h3[n] = -0.2*(0.75)^n * u[-n-1] - 0.8*(-0.5)^n * u[-n-1]\n\n');

% Eixo temporal para os três casos
n_b = -15:25;
u_pos = double(n_b >= 0);
u_neg = double(n_b < 0);

h1_analitica = (r(1)*(p(1).^n_b) .* u_pos) + (r(2)*(p(2).^n_b) .* u_pos);
h2_analitica = (-r(1)*(p(1).^n_b) .* u_neg) + (r(2)*(p(2).^n_b) .* u_pos);
h3_analitica = (-r(1)*(p(1).^n_b) .* u_neg) - (r(2)*(p(2).^n_b) .* u_neg);

% PLOTAGEM ITEM (b): Comparação das 3 Respostas ao Impulso
figure('Name', 'Questão 5(b) -- Respostas ao Impulso Analíticas', 'NumberTitle', 'off');

subplot(3,1,1);
stem(n_b, h1_analitica, 'b', 'filled', 'LineWidth', 1.2);
title('1. ROC Causal (|z| > 0.75): h_1[n]');
xlabel('n'); ylabel('Amplitude'); grid on;

subplot(3,1,2);
stem(n_b, h2_analitica, 'm', 'filled', 'LineWidth', 1.2);
title('2. ROC Bilateral / Estável (0.5 < |z| < 0.75): h_2[n]');
xlabel('n'); ylabel('Amplitude'); grid on;

subplot(3,1,3);
stem(n_b, h3_analitica, 'r', 'filled', 'LineWidth', 1.2);
title('3. ROC Anticausal (|z| < 0.5): h_3[n]');
xlabel('n'); ylabel('Amplitude'); grid on;

%% =========================================================================
% ITEM C: Verificação Numérica no Intervalo -40 <= n <= 40
% =========================================================================
fprintf('=======================================================\n');
fprintf('  QUESTÃO 5 (c): Verificação Numérica (-40 <= n <= 40)\n');
fprintf('=======================================================\n');

n_c = -40:40;

% Avaliação analítica das respostas no intervalo amplo
u_pos_c = double(n_c >= 0);
u_neg_c = double(n_c < 0);

h1_c = (r(1)*(p(1).^n_c) .* u_pos_c) + (r(2)*(p(2).^n_c) .* u_pos_c);
h2_c = (-r(1)*(p(1).^n_c) .* u_neg_c) + (r(2)*(p(2).^n_c) .* u_pos_c);

% Algoritmo por Equação de Diferenças (Recorrência Causal)
x_imp = double(n_c == 0);
y_num = zeros(size(n_c));
idx_0 = find(n_c == 0);

for i = idx_0:length(n_c)
    y_p1 = 0; y_p2 = 0; x_p1 = 0;
    if i - 1 >= idx_0, y_p1 = y_num(i-1); end
    if i - 2 >= idx_0, y_p2 = y_num(i-2); end
    if i - 1 >= 1,     x_p1 = x_imp(i-1); end
    
    y_num(i) = 0.25*y_p1 + 0.375*y_p2 + x_imp(i) - 0.5*x_p1;
end

% Cálculo do erro de aproximação
erro_causal = max(abs(y_num - h1_c));
fprintf('Erro absoluto máximo (Simulação Numérica vs Expressão Analítica Causal): %e\n\n', erro_causal);

% Exibição de Amostras Selecionadas no Terminal
fprintf('Tabela de Amostras Selecionadas:\n');
fprintf('  n   | h1_causal (Analítica) | h1_causal (Numérica) | h2_bilateral (Analítica)\n');
fprintf('------+-----------------------+----------------------+-------------------------\n');
amostras = [-5, -1, 0, 1, 2, 5, 10];
for k = amostras
    idx = find(n_c == k);
    fprintf('%5d | %21.6f | %20.6f | %23.6f\n', ...
            k, h1_c(idx), y_num(idx), h2_c(idx));
end
fprintf('\n');

% PLOTAGEM ITEM (c): Confronto Numérico vs Analítico
figure('Name', 'Questão 5(c) -- Verificação Numérica', 'NumberTitle', 'off');

subplot(2,1,1);
stem(n_c, h1_c, 'b', 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', 'Analítica h_1[n]');
hold on;
stem(n_c(1:3:end), y_num(1:3:end), 'r--', 'LineWidth', 1, 'Marker', 'o', 'DisplayName', 'Numérica (Equação de Dif.)');
title('Questão 5(c): Validação da Resposta Causal (ROC: |z| > 0.75)');
xlabel('n'); ylabel('Amplitude'); grid on; legend show;

subplot(2,1,2);
stem(n_c, h2_c, 'm', 'LineWidth', 1.2, 'MarkerSize', 3);
title('Questão 5(c): Resposta Bilateral Estável (ROC: 0.5 < |z| < 0.75)');
xlabel('n'); ylabel('Amplitude'); grid on;

%% =========================================================================
% ITEM D: Equação de Diferenças e Realizabilidade
% =========================================================================
fprintf('=======================================================\n');
fprintf('  QUESTÃO 5 (d): Equação de Diferenças e Realizabilidade\n');
fprintf('=======================================================\n');

fprintf(['1. Dedução da Equação de Diferenças:\n' ...
         '   Y(z)/X(z) = (1 - 0.5*z^-1) / (1 - 0.25*z^-1 - 0.375*z^-2)\n' ...
         '   Multiplicando em cruz:\n' ...
         '   Y(z)(1 - 0.25*z^-1 - 0.375*z^-2) = X(z)(1 - 0.5*z^-1)\n' ...
         '   Aplicando a Z^-1:\n' ...
         '   y[n] - 0.25*y[n-1] - 0.375*y[n-2] = x[n] - 0.5*x[n-1]\n' ...
         '   Isolando y[n]:\n' ...
         '   y[n] = 0.25*y[n-1] + 0.375*y[n-2] + x[n] - 0.5*x[n-1]\n\n']);

fprintf(['2. Por que a equação algébrica não determina uma única resposta?\n' ...
         '   A equação de diferenças especifica apenas a relação de recorrência local\n' ...
         '   entre as amostras. A solução completa exige estabelecer a Região de Convergência (ROC),\n' ...
         '   a qual define a causalidade, a direção do tempo na recursão e a estabilidade do sistema.\n\n']);
