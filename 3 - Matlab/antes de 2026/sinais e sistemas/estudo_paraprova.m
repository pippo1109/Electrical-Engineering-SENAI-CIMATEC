%Zi = filtic([.4 .12], [1 .4 .3], [2 1]);
%y = filter([.4 .12], [1 .4 .3], ones(1000,1));
%n = 0:999;
%n = n';
%stem(n,y)

close all
n=0:20;
x=ones(length(n),1);
y=-7/12*(1/3).^n-13/24*(-1/3).^n+9/8;
stem(n,y)

%coeficientes determinados a partir do menor atraso (x[n]) para o maior o
%atraso (x[n-...]).
%X[n] é o sinal de entrada e os seus coeficientes sao associados à entrada
%b da função 'filter'.

zi=filtic([0 1],[1 0 -1/9],[1 0]);
yf=filter([0 1],[1 0 -1/9],x,zi);
hold
stem(n,yf,'--red')

