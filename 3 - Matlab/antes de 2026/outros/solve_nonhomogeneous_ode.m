function [t, Y] = solve_nonhomogeneous_ode(s,b,c,y0,v0,F,tspan)
    % Este código resolve uma EDO não homogênea de 2ª ordem com condições iniciais
    
    %% Definição do Problema
    % Exemplo: Sistema massa-mola-amortecedor forçado
    % m*y'' + c*y' + k*y = F(t)
    % Com condições iniciais y(0) = y0, y'(0) = v0
    
    if nargin < 7
        error('São necessários 7 argumentos de entrada');
    end
    
    %% Parâmetros do Sistema (MODIFIQUE ESTES VALORES)
    s = @s;      % massa (kg)
    b = @b;      % coeficiente de amortecimento (N.s/m)
    c = @c;      % constante da mola (N/m)
    
    %% Intervalo de Tempo para Simulação
    tspan = 0:0.01:10; % de 0 a 10 segundos
    
    %% Resolução da EDO
    % Converter a EDO de 2ª ordem para um sistema de EDOs de 1ª ordem
    % Definimos:
    % Y(1) = y (posição)
    % Y(2) = y' (velocidade)
    
    % Função que define o sistema de EDOs
    odefun = @(t,Y) [Y(2);                     % y' = velocidade
                    (F(t) - b*Y(2) - c*Y(1))]; % y'' = (F(t) - c*y' - k*y)/m
    
    % Condições iniciais no formato [y(0); y'(0)]
    Y0 = [y0; v0];
    
    % Resolver usando ode45
    [t, Y] = ode45(odefun, tspan, Y0);
    
    %% Visualização dos Resultados
    figure;
    
    % Gráfico da Posição
    subplot(2,1,1);
    plot(t, Y(:,1), 'b-', 'LineWidth', 2);
    xlabel('Tempo (s)');
    ylabel('Posição (m)');
    title('Resposta do Sistema: Posição vs Tempo');
    grid on;
    
    % Gráfico da Velocidade
    subplot(2,1,2);
    plot(t, Y(:,2), 'r-', 'LineWidth', 2);
    xlabel('Tempo (s)');
    ylabel('Velocidade (m/s)');
    title('Resposta do Sistema: Velocidade vs Tempo');
    grid on;
    
    %% Exibição da Equação Resolvida
    fprintf('\nEDO resolvida:\n');
    fprintf('%g*y'''' + %g*y'' + %g*y = F(t)\n\n', s,b,c);
    fprintf('Condições iniciais:\n');
    fprintf('y(0) = %g\n', y0);
    fprintf('y''(0) = %g\n\n', v0);
    
    % Plot adicional da força aplicada
    figure;
    f_values = arrayfun(F, t);
    plot(t, f_values, 'k-', 'LineWidth', 2);
    xlabel('Tempo (s)');
    ylabel('Força Aplicada (N)');
    title('Forçamento Externo Aplicado');
    grid on;
end