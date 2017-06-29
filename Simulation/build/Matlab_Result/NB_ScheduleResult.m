clear;
clc;
%=============模擬結果===========%
%% 檔案宣告 & 讀檔
%A(1,:) 第一Row ; A（：，1）第一Col
%TrafficModel1_Simulation_PDF = csvread('SimArrivalMode1_M_5000.csv');
%TrafficModel1_Analysis_PDF = csvread('AnalArrivalModel_M_5000.csv');
AverageDelay_pp_sameCE0 = csvread('AverageDelay_pp_sameCE0.csv');
AverageDelay_pp_sameCE1 = csvread('AverageDelay_pp_sameCE1.csv');
AverageDelay_pp_sameCE2 = csvread('AverageDelay_pp_sameCE2.csv');
AverageDelay_pp_not_sameCE0 = csvread('AverageDelay_pp_not_sameCE0.csv');
AverageDelay_pp_not_sameCE1 = csvread('AverageDelay_pp_not_sameCE1.csv');
AverageDelay_pp_not_sameCE2 = csvread('AverageDelay_pp_not_sameCE2.csv');
resourceUtilization_pp_same = csvread('resourceUtilization_pp_same.csv');
resourceUtilization_pp_not_same = csvread('resourceUtilization_pp_not_same.csv');
% TrafficModel1_Simulation =
% csvread('SimArrivalMode1_M_5000_RA_Slot_1_12000.csv');
% TrafficModel2_Analysis =
% csvread('Analysis_TrafficMode2_M_5000_RA_Slot_0_2000.csv');
% xs0 = TrafficModel1_Simulation_PDF(:,1);
% ys0 = TrafficModel1_Simulation_PDF(:,2);
%xs1 = TrafficModel1_Analysis_PDF(:,1);
%ys1 = TrafficModel1_Analysis_PDF(:,2);
xsCE0 = AverageDelay_pp_sameCE0(:,1);
xsCE1 = AverageDelay_pp_sameCE1(:,1);
xsCE2 = AverageDelay_pp_sameCE2(:,1);
ysCE0 = AverageDelay_pp_sameCE0(:,2);
ysCE1 = AverageDelay_pp_sameCE1(:,2);
ysCE2 = AverageDelay_pp_sameCE2(:,2);

xs1CE0 = AverageDelay_pp_not_sameCE0(:,1);
xs1CE1 = AverageDelay_pp_not_sameCE1(:,1);
xs1CE2 = AverageDelay_pp_not_sameCE2(:,1);
ys1CE0 = AverageDelay_pp_not_sameCE0(:,2);
ys1CE1 = AverageDelay_pp_not_sameCE1(:,2);
ys1CE2 = AverageDelay_pp_not_sameCE2(:,2);

xs2 = resourceUtilization_pp_same(:,1);
ys2 = resourceUtilization_pp_same(:,2);
xs3 = resourceUtilization_pp_not_same(:,1);
ys3 = resourceUtilization_pp_not_same(:,2);
%% Check Arrival Model: Poission Distribution
% figure(6);
% xs0 = TrafficModel1_Simulation(1:12000,1);
% ys0 = TrafficModel1_Simulation(1:12000,2);

% hsa = plot(xs0, ys0,'bsquare',xs0, ys0, 'gdiamond', xs0, ys2, 'ro'); 
%hsa = plot(xs0, ys0,'ro',xs1, ys1, 'b-'); 
% hs = plot(xs0, ys0,'bo');
% %  ha=plot(xa0, ya0,'b-'); 
% set(hs, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:2000:18000],'ytick',[0:500:5000]);
% set(gca,'ytick',[0:1:10]);
% axis([0, 20, 0,0.4]);
% xlabel('k'),ylabel('P(X=k)');
% title('Sim. Arrival Model 1');
% legend('Sim');
% legend('Sim','Anal');
% %legend('Nc/N Simulation','Nc/N Numerical','(Nc+Ns)/N Simulation','(Nc+Ns)/N Numerical');
% legend('Sim Mi[1], M=5000','Sim Mi[1], M=10000','Sim Mi[1], M=30000');
% % legend('Analysis');
% grid on
%% Check Simulation: Utilization
% hs = plot(xs, ys,'bo-');
figure(1)
% hs = plot(xsCE0, ysCE0,'bo-',xsCE1,ysCE1,'rsquare-',xsCE2,ysCE2,'gdiamond-');
hs = plot(xsCE0, ysCE0,'bo-',xsCE1,ysCE1,'rsquare-',xsCE2,ysCE2,'gdiamond-',xs1CE0, ys1CE0,'co-',xs1CE1,ys1CE1,'ksquare-',xs1CE2,ys1CE2,'ydiamond-');
% hs = plot(xs, ys,'bo',xs1, ys1,'rsquare',xs2, ys2,'gdiamond');
set(hs, 'linewidth', 1.5);    % 曲線寬度改為1.5
axis([0, 60, 0,2000]);
xlabel('Total # of UE'),ylabel('(Da)(ms)');
title('Average Delay for each UE in three CE levels');
legend('Case1:CE0','Case1:CE1','Case1:CE2','Case2:CE0','Case2:CE1','Case2:CE2');
hold on;
figure(2)
% hs1 = plot(xs2, ys2,'bo-');
hs1 = plot(xs2, ys2,'bo-',xs3,ys3,'rsquare-');
set(hs1, 'linewidth', 1.5);    % 曲線寬度改為1.5
set(hs1, 'linewidth', 1.5);    % 曲線寬度改為1.5
axis([0, 60, 0,1]);
set(gca,'ytick',[0:0.1:1]);
xlabel('Total # of UE'),ylabel('(U)(resource utilization)');
title('Resource utilization');
legend('Case1:pp are the same for three CE levels','Case2:pp are different for three CE levels');

