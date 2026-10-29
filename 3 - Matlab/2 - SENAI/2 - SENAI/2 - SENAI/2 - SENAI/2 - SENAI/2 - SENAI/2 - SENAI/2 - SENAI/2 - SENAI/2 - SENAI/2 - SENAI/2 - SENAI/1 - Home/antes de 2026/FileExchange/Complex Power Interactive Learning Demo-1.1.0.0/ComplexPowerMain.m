% Complex Power Interactive Learning Demo
% Written by Dr Rodney Tan
% Version 1.01 (Dec 2015)
% This interactive learning demo performs Complex Power instantaneous 
% waveforms & magnitude computation to reinforce the understanding and 
% relationship of Real Power, Reactive Power, Apparent Power, Power Factor 
% and Power Triangle.
% For simplicity Vrms and Irms is set to 1 & lagging current is assumed.
function ComplexPowerMain
    % Setup Figure UI
    hFig = figure('Resize','off','NumberTitle','off','Toolbar','none',...
                  'Name','Complex Power Interactive Learning Demo');
    myhandles = guihandles(hFig); 
    % Variables Initialization
    Vrms = 1;   Irms = 1;
    freq = 60;  fs = 1600;  t = 0:1/fs:0.04;
    theta = pi*(30/180);
    % Instantaneous Voltage and Current
    V = (Vrms*sqrt(2))*cos(2*pi*freq*t);
    I = (Irms*sqrt(2))*cos((2*pi*freq*t)-theta);  % Lagging
    % Instantaneous Complex Power
    R = Vrms*Irms*cos(theta)*(1+cos(2*(2*pi*freq*t)+theta));
    Q = Vrms*Irms*sin(theta)*sin(2*(2*pi*freq*t)+theta);
    S = R+Q;
    % Complex Power Magnitude
    PF = cos(theta);            % Power Factor
    PR = Vrms*Irms*cos(theta);  % Real Power
    PQ = Vrms*Irms*sin(theta);  % Reactive Power
    PS = abs(complex(PR,PQ));   % Apparent Power
    % Setup Plots
    h1 = subplot(2,2,1);
    plot(S,'Color','b','LineWidth',2);
    hold on;
    plot(Q,'Color','r','LineWidth',2);
    plot(R,'Color','y','LineWidth',2);
    line([0 128],[0 0],'Color','k');
    title('Instantenous Complex Power');
    xlim([0 64]);
    h1.XTick = 0:16:64;
    h1.XTickLabel = {'0','20','40','60','80'};

    h2 = subplot(2,2,2);
    line([0 PR],[0 PQ],'Color','b','LineWidth',2);
    line([0 PR],[0 0],'Color','y','LineWidth',2);
    line([PR PR],[0 PQ],'Color','r','LineWidth',2);
    xlim([-0.25 1.25]);
    ylim([-0.25 1.25]);
    box on;
    title('Power Triangle');
    
    h3 = subplot(2,2,3);
    plot(V,'LineWidth',2);
    hold on;
    plot(I,'LineWidth',2);
    title('Voltage and Current');
    legend('V','I');
    xlim([0 64]);
    h3.XTick = 0:16:64;
    h3.XTickLabel = {'0','20','40','60','80'};
    xlabel('Time (ms)');
    
    set(h1,'position',[0.05 0.5838 0.58 0.3412]);
    set(h2,'position',[0.69 0.5838 0.28 0.3412]);
    set(h3,'position',[0.05 0.11 0.58 0.3412]);
    % Setup UI Variables
    uicontrol('Style','text','string','Real Power (W)','position',[390 170 110 20],...
    'BackgroundColor','y','HorizontalAlignment','left');
    uicontrol('Style','text','string','Reactive Power (var)','position',[390 150 110 20],...
    'ForegroundColor','w','BackgroundColor','r','HorizontalAlignment','left');
    uicontrol('Style','text','string','Apparent Power (VA)','position',[390 130 110 20],...
    'ForegroundColor','w','BackgroundColor','b','HorizontalAlignment','left');
    uicontrol('Style','text','string','Power Factor','position',[390 110 110 20],...
    'ForegroundColor','w','BackgroundColor','k','HorizontalAlignment','left');
    uicontrol('Style','text','string','Phase Angle (Deg)','position',[390 70 110 20],...
    'HorizontalAlignment','left');
    h_TPR = uicontrol('Style','text','string',sprintf('%0.3f',PR),...
    'position',[500 170 60 20]);
    h_TPQ = uicontrol('Style','text','string',sprintf('%0.3f',PQ),...
    'position',[500 150 60 20]);
    h_TPS = uicontrol('Style','text','string',sprintf('%0.3f',PS),...
    'position',[500 130 60 20]);
    h_TPF = uicontrol('Style','text','string',sprintf('%0.3f',PF),...
    'position',[500 110 60 20]);
    h_Ta = uicontrol('Style','text','string',30,...
    'position',[500 70 60 20]);
    % Setup UI Control
    h_Sa = uicontrol('Style','slider','Min',0,'Max',90,'Value',30,...
    'Position',[390 50 150 20],'Callback',@Slider_Angle);
    % Variables Handler
    myhandles.Vrms = Vrms;
    myhandles.Irms = Irms;
    myhandles.freq = freq;
    myhandles.t = t;
    myhandles.h_TPR = h_TPR;
    myhandles.h_TPQ = h_TPQ;
    myhandles.h_TPS = h_TPS;
    myhandles.h_TPF = h_TPF;
    myhandles.h_Sa = h_Sa;
    myhandles.h_Ta = h_Ta;
    guidata(hFig,myhandles);
end

function Slider_Angle(~,~)
    myhandles = guidata(gcbo);
    Vrms = myhandles.Vrms;
    Irms = myhandles.Irms;
    freq = myhandles.freq;
    t = myhandles.t;
    % Get Phase Angle
    h_Sa = get(myhandles.h_Sa,'Value');
    set(myhandles.h_Ta,'String',sprintf('%0.1f',h_Sa));
    theta = pi*(h_Sa/180);
    % Instantaneous Voltage and Current
    V = (Vrms*sqrt(2))*cos(2*pi*freq*t);
    I = (Irms*sqrt(2))*cos((2*pi*freq*t)-theta);  % Lagging
    % Instantaneous Complex Power
    R = Vrms*Irms*cos(theta)*(1+cos(2*(2*pi*freq*t)+theta));
    Q = Vrms*Irms*sin(theta)*sin(2*(2*pi*freq*t)+theta);
    S = R+Q; 
    % Complex Power Magnitude
    PF = cos(theta);            % Power Factor
    PR = Vrms*Irms*cos(theta);  % Real Power
    PQ = Vrms*Irms*sin(theta);  % Reactive Power
    PS = abs(complex(PR,PQ));   % Apparent Power
    % Update Plots
    h1 = subplot(2,2,1);
    plot(S,'Color','b','LineWidth',2);
    hold on;
    plot(Q,'Color','r','LineWidth',2);
    plot(R,'Color','y','LineWidth',2);
    line([0 128],[0 0],'Color','k');
    title('Instantenous Complex Power');
    xlim([0 64]);
    h1.XTick = 0:16:64;
    h1.XTickLabel = {'0','20','40','60','80'};
    ylim([-1 2]);
    
    h2 = subplot(2,2,2);
    line([0 PR],[0 PQ],'Color','b','LineWidth',2);
    line([0 PR],[0 0],'Color','y','LineWidth',2);
    line([PR PR],[0 PQ],'Color','r','LineWidth',2);
    xlim([-0.25 1.25]);
    ylim([-0.25 1.25]);
    title('Power Triangle');
    box on;
    
    h3 = subplot(2,2,3);
    plot(V,'LineWidth',2);
    hold on;
    plot(I,'LineWidth',2);
    title('Voltage and Current');
    legend('V','I');
    xlim([0 64]);
    h3.XTick = 0:16:64;
    h3.XTickLabel = {'0','20','40','60','80'};
    xlabel('Time (ms)');
    
    set(h1,'position',[0.05 0.5838 0.58 0.3412]);
    set(h2,'position',[0.69 0.5838 0.28 0.3412]);
    set(h3,'position',[0.05 0.11 0.58 0.3412]);
    % Update UI Variables
    set(myhandles.h_TPR,'String',sprintf('%0.3f',PR));
    set(myhandles.h_TPQ,'String',sprintf('%0.3f',PQ));
    set(myhandles.h_TPS,'String',sprintf('%0.3f',PS));
    set(myhandles.h_TPF,'String',sprintf('%0.3f',PF));
end