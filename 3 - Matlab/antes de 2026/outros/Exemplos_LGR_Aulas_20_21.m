
clc
clear all
close all

s = tf ('s');

%% Lugar Geométrico das Raízes (LGR)

%% Exemplo 2 - Aula 20

L_s = 1/(s*(s + 10)) 

figure(1), rlocus(L_s) %Ganho de Malha


%% Exemplo 1 - Aula 21

L1_s = ((s + 3)*(s + 4))/((s + 1) * (s + 2)) 

figure(2), rlocus(L1_s) %Ganho de Malha


%% Exemplo 2 - Aula 21 

L2_s = ((s + 2)*(s + 3))/(s^2 - 2*s + 2) 

figure(3), rlocus(L2_s) %Ganho de Malha


L3_s = (s + 1)/(s*(s+2)*((s+4)^2)) 

figure(4), rlocus(L3_s) %Ganho de Malha


L4_s = (s + 4)/((s+2)*(s+8)) 

figure(5), rlocus(L4_s) %Ganho de Malha

