clear all;
close all;
clc;

load carsmall;

%% Frame diff results no post-processing
Table=csvread(['data_fd_45.csv']);
dists = Table(:, 2);
dists = flip(dists);
thetas = Table(:, 3);
thetas = flip(thetas);

f_1 = figure;
%tiledlayout(2, 1);
%nexttile
plot(dists, '.');
subtitle("Displacement magnitude frames at 45 deg constant direction")
xlim([0 length(dists)]) 
ylabel('Mode disp. magnitude [px]')
xlabel('Frame difference step [#]')

% nexttile
% plot(thetas, '.');
% subtitle("Displacement direction frames at 45 deg constant direction")
% xlim([0 length(thetas)])
% ylabel('Mode disp. directions [deg]')
% xlabel('Frame difference step [#]')
% 
% x=(1:1:length(thetas));
% y=transpose(thetas);
% coeff=polyfit(x, y, 1);
% z=polyval(coeff , x);
% err2=norm(z - y, 2);
% %fprintf('\n\t ErrorRegression norma2 : %1.2e ' , err2 ) ;
% ht =1/10000; u=0:ht:length(thetas);
% v=polyval(coeff, u);
% hold on
% %plot(u, v);
% disp("|||||\n")
% disp(mean(thetas));
% hold on
% yline(45, '--r');
% disp(std(thetas));
% hold on
% disp(trimmean(thetas, 30));
hold on
xline(30, '--r');
%text(10,0,sprintf('MODE=%.2f',m))

%% HOG analysis
Tablel=csvread('data_hog_45.csv');

dir_hog = Tablel(:, 2);
bins_hog = Tablel(:, 1);

f_3 = figure;
bar(bins_hog, dir_hog);

hold on
xline(180, '--r')
hold on
xline(135, '--r')
hold on 

subtitle("HOG frame direction detection 45 deg")
xlim([0 180])
xlabel('Direction bins [deg]')
ylabel('Sum of direction px magnitude per bin [#]')

%% HOG analysis
Tablel=csvread('data_sine.csv');

bins_hog = Tablel(:, 1);

f_3 = figure;
plot(bins_hog);

hold on
xline(180, '--r')

subtitle("HOG direction detection 45 deg")
xlim([0 360])
xlabel('Direction bins [deg]')
ylabel('Sum of px magnitudes per bin [#]')

%% HOG speed and dir
Tablel=csvread('data_sine.csv');

vel_hog = Tablel(:, 3);
dir_hog = Tablel(:, 2);
bins_hog = Tablel(:, 1);
f_4 = figure;
tiledlayout(1, 1);

% nexttile
% plot(bins_hog, vel_hog);
% subtitle("HOG speed detection 45°")
% xlabel('Blur kernel size NxN [#]')
% ylabel('Frame std dev [Intensity]')
% 
% hold on

nexttile
plot(dir_hog(30:length(dir_hog)-1), '.');
subtitle("HOG direction detection 45 deg")
xlabel('Hog analysis step [#]')
ylabel('Velocity direction [deg]')
hold on
yline(45, '--r')
hold on 
xline(30, '--r')
%% HOG speed comparison
fig_0 = figure;
subtitle("HOG speed detection at different angles")
xlabel('HOG analysis step [#]')
ylabel('Frame std dev [px]')
hold on

Table30=csvread('data_hog_tot_30.csv');
bins_hog_30 = Table30(:, 1);
dir_hog_30 = Table30(:, 2);
vel_hog_30 = Table30(:, 3);
plot(bins_hog_30, vel_hog_30)
hold on

Table45=csvread('data_hog_tot_45.csv');
bins_hog_45 = Table45(:, 1);
dir_hog_45 = Table45(:, 2);
vel_hog_45 = Table45(:, 3);
plot(bins_hog_45, vel_hog_45)
hold on

Table60=csvread('data_hog_tot_60.csv');
bins_hog_60 = Table60(:, 1);
dir_hog_60 = Table60(:, 2);
vel_hog_60 = Table60(:, 3);
plot(bins_hog_60, vel_hog_60)
hold on

Table85=csvread('data_hog_tot_85.csv');
bins_hog_85 = Table85(:, 1);
dir_hog_85 = Table85(:, 2);
vel_hog_85 = Table85(:, 3);
plot(bins_hog_85, vel_hog_85)
hold on

xline(30, '--r');
hold on

legend('0 deg', '10 deg', '20 deg', '30 deg')


% Table90=csvread('data_hog_tot_90.csv');
% bins_hog_90 = Table90(:, 1);
% dir_hog_90 = Table90(:, 2);
% vel_hog_90 = Table90(:, 3);
% plot(bins_hog_90, vel_hog_90)

%% HOG speed comparison noise blob
fig_0 = figure;
subtitle("HOG speed blob noise 45 deg")
xlabel('Blur kernel size NxN [#]')
ylabel('Frame std dev [Intensity]')
hold on

Table30=csvread('data_hog_tot_45.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 3);
plot(bins_hog_30, vel_hog_30)
hold on

Table30_blob=csvread('data_hog_tot_45_blob.csv');
bins_hog_30_blob = Table30_blob(:, 1);
vel_hog_30_blob = Table30_blob(:, 3);
plot(bins_hog_30_blob, vel_hog_30_blob, 'r')

%% HOG speed comparison noise sp
fig_0 = figure;
subtitle("HOG speed salt noise 45 deg")
xlabel('Blur kernel size NxN [#]')
ylabel('Frame std dev [Intensity]')
hold on

Table30=csvread('data_hog_tot_45.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 3);
plot(bins_hog_30, vel_hog_30)
hold on

Table30_sp=csvread('data_hog_tot_45_sp.csv');
bins_hog_30_sp = Table30_sp(:, 1);
vel_hog_30_sp = Table30_sp(:, 3);
plot(bins_hog_30_sp, vel_hog_30_sp, 'r')

%% Laplacian speed
Table0=csvread('data_lap_45.csv');

vel_hog = Table0(:, 2);
bins_hog = Table0(:, 1);
f_4 = figure;
plot(bins_hog, vel_hog, '.');
subtitle("Laplacian speed 45 deg")
xlabel('Blur kernel size NxN [#]')
ylabel('Frame standard deviation [Intensity]')

%% LAP speed comparisonbins_hog
fig_0 = figure;
subtitle("Laplacian speed comparison a different angles")
xlabel('Blur kernel size NxN [#]')
ylabel('Derivatives standard deviation [#]')
hold on

Table30=csvread('data_lap_30.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 2);
plot(bins_hog_30, vel_hog_30, '.')
hold on

Table45=csvread('data_lap_45.csv');
bins_hog_45 = Table45(:, 1);
vel_hog_45 = Table45(:, 2);
plot(bins_hog_45, vel_hog_45, '.')
hold on

Table60=csvread('data_lap_60.csv');
bins_hog_60 = Table60(:, 1);
vel_hog_60 = Table60(:, 2);
plot(bins_hog_60, vel_hog_60, '.')
hold on

Table85=csvread('data_lap_85.csv');
bins_hog_85 = Table85(:, 1);
vel_hog_85 = Table85(:, 2);
plot(bins_hog_85, vel_hog_85, '.')
hold on

Table90=csvread('data_lap_90.csv');
bins_hog_90 = Table90(:, 1);
vel_hog_90 = Table90(:, 2);
plot(bins_hog_90, vel_hog_90, '.')

hold on
x = bins_hog_30(1:17)
pfv1 = 25 * exp(-0.074*x) + 5
plot(x, pfv1, 'r', 'LineWidth',2)
hold on
x = bins_hog_30(16:91)
pfv2 = 12.4* exp(-0.0325*x) + 5
plot(x, pfv2 ,'r',  'LineWidth',2)
hold on
x = bins_hog_30(90:end)
pfv3 = 7 -0.015* x
plot(x, pfv3 ,'r',  'LineWidth',2)

hold on
xline(17, '--');
hold on
xline(91, '--');

xticks([17 91])

hold on 
legend('30 deg', '45 deg', '60 deg', '85 deg', '90 deg', 'mod1', 'mod2', 'mod3')

%% LAP speed noise blob comparison
fig_0 = figure;
subtitle("Laplacian speed with blob noise at 45 deg")
xlabel('Blur kernel size NxN [#]')
ylabel('Derivatives standard deviation [#]')
hold on

Table30=csvread('data_lap_45.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 2);
plot(bins_hog_30, vel_hog_30, '.')
hold on

Table30_blob=csvread('data_lap_45_blob.csv');
bins_hog_30_blob = Table30_blob(:, 1);
vel_hog_30_blob = Table30_blob(:, 2);
plot(bins_hog_30_blob, vel_hog_30_blob, '.')

hold on
legend('Reference', 'Blob noise')

%% LAP speed noise sp comparison
fig_0 = figure;
subtitle("Laplacian speed with salt noise at 45 deg")
xlabel('Blur kernel size NxN [#]')
ylabel('Derivatives standard deviation [#]')
hold on

Table30=csvread('data_lap_45.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 2);
plot(bins_hog_30, vel_hog_30, '.')
hold on

Table30_sp=csvread('data_lap_45_sp.csv');
bins_hog_30_sp = Table30_sp(:, 1);
vel_hog_30_sp = Table30_sp(:, 2);
plot(bins_hog_30_sp, vel_hog_30_sp, '.')

hold on
legend('Reference', 'Salt noise')

%% LAP speed 700
fig_0 = figure;
subtitle("LAP high speed")
xlabel('Sample frame [#]')
ylabel('Speed [std dev]')
hold on

Table30=csvread('data_lap_45_700.csv');
bins_hog_30 = Table30(:, 1);
vel_hog_30 = Table30(:, 2);
plot(bins_hog_30, vel_hog_30, '.')

%% TOT analysis
Table=csvread('data_sine_5_0.csv');
thetas = Table(:, 1);
dists = Table(:, 2);

f_1 = figure;
%tiledlayout(2, 1);
%nexttile
% plot(dists, '.');
% subtitle("Laplacian speed model analyisis")
% xlim([0 length(dists)])
% ylabel('Standard deviation [px]')
% xlabel('Laplacian analysis step [#]')
% hold on
% 
% hold on
% x = 1:1:6
% pfv1 = 30* exp(-0.07*x)
% plot(x, pfv1, 'r', 'LineWidth',2)
% hold on
% x = 6:1:100
% pfv2 = 24* exp(-0.03*x)
% plot(x, pfv2 ,'r',  'LineWidth',2)
% hold on
% x = 100:1:399
% pfv3 = 7 -0.015* x
% plot(x, pfv3 ,'r',  'LineWidth',2)
% 
% hold on
% xline(17, '--');
% hold on
% xline(91, '--')

% % down = 0;
% % up = 10;
% % 
% % x = (down+1:1:up)';
% % f1 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up),f1(down+1:up), 'LineWidth',2)
% % xticks([10 25 40 65 100 200])
% % hold on
% % xline(10, '--')
% % 
% % hold on
% % down = 10;
% % up = 25;
% % 
% % x = (down+1:1:up)';
% % f2 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f2(down+1:up), 'LineWidth',2)
% % hold on
% % xline(25, '--')
% % 
% % hold on
% % down = 25;
% % up = 40;
% % 
% % x = (down+1:1:up)';
% % f3 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f3(down+1:up), 'LineWidth',2)
% % hold on
% % xline(40, '--')
% % 
% % hold on
% % down = 40;
% % up = 65;
% % 
% % x = (down+1:1:up)';
% % f4 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f4(down+1:up), 'LineWidth',2)
% % hold on
% % xline(65, '--')
% % 
% % hold on
% % down = 65;
% % up = 100;
% % 
% % x = (down+1:1:up)';
% % f5 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f5(down+1:up), 'LineWidth',2)
% % hold on
% % xline(100, '--')
% % 
% % 
% % hold on
% % down = 100;
% % up = 200;
% % 
% % x = (down+1:1:up)';
% % f6 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f6(down+1:up), 'LineWidth',2)
% % hold on
% % xline(200, '--')
% % 
% % hold on
% % down = 200;
% % up = 394;
% % 
% % x = (down+1:1:up)';
% % f7 = fit(x,dists(down+1:up), 'exp1')
% % plot((down+1:up), f7(down+1:up), 'LineWidth',2)

%nexttile
plot(thetas, '.');
subtitle("FFT Direction of velocity vector at 15 deg")
xlim([0 length(thetas)])
ylabel('Direction [deg]')
xlabel('FFT analysis step [#]')
hold on
yline(0, '--r');
% yline(15, '--r')
% hold on
%
%x=(1:1:length(thetas));
%y=transpose(thetas);
% coeff=polyfit(x, y, 1);
% z=polyval(coeff , x);
% err2=norm(z - y, 2);
%fprintf('\n\t ErrorRegression norma2 : %1.2e ' , err2 ) ;
%ht =1/10000; u=0:ht:length(thetas);
%v=polyval(coeff, u);
%hold on
%plot(u, v);
%disp(mean(thetas));
%disp("|||||\n")

%hold on
%yline(3, '--r');
%disp(std(thetas));
%hold on
%disp(trimmean(thetas, 30));
%text(10,0,sprintf('MODE=%.2f',m))


%% Motion blur
Table = load(['../../../sim_log/sine_steer1.mat']);

u = Table.VehicleModel_VM_Outputs_States_u;
v = Table.VehicleModel_VM_Outputs_States_v;
fl = Table.VehicleModel_VM_Outputs_AdditionalStates_delta_fl;
fr = Table.VehicleModel_VM_Outputs_AdditionalStates_delta_fr;

% amp = 0;2
% theta = [];

for i = 1:960
    if i <= 130
        A = imread('../../../test_sine_60fps/frame_3.png');
        imwrite(A, ['../../../test_sine_5_0_60fps_matlab/frame_'  num2str(i,'%d')  '.png']);
    elseif i > 130 && i <= 867
        tmp_i = fix(i * 16.66667);
        amp = sqrt((u(tmp_i) * u(tmp_i)) + ( v(tmp_i) * v(tmp_i)));
        theta = (fl(tmp_i) + fr(tmp_i)) / 2;
        k=90+i-131;
        A = imread(['../../../test_sine_60fps/frame_' num2str(k, '%d') '.png']);
        H = fspecial('motion', amp*5, ((theta * 180 / pi)) + 90);
        MotionBlur = imfilter(A ,H,'replicate');
        imwrite(MotionBlur, ['../../../test_sine_5_0_60fps_matlab/frame_'  num2str(i,'%d')  '.png']);
    else
        A = imread('../../../test_sine_60fps/frame_830.png');
        imwrite(A, ['../../../test_sine_5_0_60fps_matlab/frame_'  num2str(i,'%d')  '.png']);
    end
    disp(i);
end       

% 
% for n = 1:length(u)
%     amp(end + 1) = sqrt((u(n) *u(n)) + ( v(n) * v(n)));
%     theta(end + 1) = (fl(n) + fr(n))/2;
% end

%for i = 0:
% %     if (c < 10)
% %         path = '000' + string(c);
% %     elseif(c >= 10 && c < 100)
% %         path = '00' + string(c);
% %     elseif(c >= 100 && c < 1000)
% %         path = '0' + string(c);
% %     else
% %         path = string(c);
% %     end
%     
%     A = imread('../../../test_speed_v1/mb_speed_' + i + '.png');
% 
% 
% 
% 
%     k = 0;
%     if (c > 115)
%         H = fspecial('motion',amp(c+5120+k)*7,(theta(c+5120+k)*180/pi)-90);
%         MotionBlur = imfilter(A ,H,'replicate');
%         k= k+ 40;
%     else
%         MotionBlur = A;
%     end
%     imwrite(MotionBlur, '../../../test_60fps_1cd_matlab/frame_'+string(c-95)+ '.png');
%     disp(c)
% end

%% Open sim log

Table = load(['../../../sim_log/sine_steer1.mat']);

u = Table.VehicleModel_VM_Outputs_States_u;
v = Table.VehicleModel_VM_Outputs_States_v;
yaw = Table.VehicleModel_VM_Outputs_States_yaw;
fl = Table.VehicleModel_VM_Outputs_AdditionalStates_delta_fl;
fr = Table.VehicleModel_VM_Outputs_AdditionalStates_delta_fr;

amp = [];
theta = [];

for c = 1:length(u)
    amp(end + 1) = sqrt((u(c) *u(c)) + ( v(c) * v(c)));
    theta(end + 1) = (fl(c) + fr(c))/2;
end

tit = 'Sine steer'

% fig1 = figure;
% tiledlayout(3, 1);
% 
% nexttile
% plot(u);
% subtitle("Longitudinal velocity")
% ylabel('Velocity [m/s]')
% xlabel('Time[ms]')
% hold on
% plot(amp, 'r');
% hold on
% 
% t1 = title(tit);
% t1.Visible = 'on';
% 
% nexttile
% plot(v);
% subtitle("Lateral velocity")
% ylabel('Velocity [m/s]')
% xlabel('Time[ms]')
% hold on
% 
% nexttile
% plot(fl);
% hold on
% plot(fr, 'r');
% hold on
% plot(theta, 'g');
% subtitle("wheels fl/fr angle")
% ylabel('Wheel angle [rad]')
% xlabel('Time[ms]')


Table=csvread('data_sine_5_0.csv');
points = Table(:, 1);
thetas = Table(:, 2);
dists = Table(:, 3);

fig2 = figure;
tiledlayout(2, 1);

nexttile
plot(amp);
subtitle("Amplitude velocity vector")
ylabel('Velocity [m/s]')
xlabel('Time[ms]')
hold on

t1 = title(tit);
t1.Visible = 'on';

nexttile
plot(theta * 180/pi, LineWidth= 1);
hold on


plot(points*16.666667, thetas, 'r');
hold on
yline(0, '--g');

subtitle("Direction velocity vector")
ylabel('Velcoity angle [rad]')
xlabel('Time[ms]')

legend('sim data', 'GVS data')
%% MSE ERROR OF FFT
x = 0:5:60
y = [0.04^2 0.35^2 0.63^2 0.4^2 0.42^2 0.67^2 0.95^2 0.86^2 1.01^2 1.28^2 1.35^2 1.69^2 1.77^2]

bar(x, y);

subtitle("Mean squared error at different directions")
ylabel('Mean squared error [deg^2]')
xlabel('Direction orientation [deg]')

% 60 --- 58.23 --- 1.77
% 55 --- 53.31 --- 1.69
% 50 --- 48.65 --- 1.35
% 45 --- 43.72 --- 1.28
% 40 --- 38.99 --- 1.01
% 35 --- 34.14 --- 0.86
% 30 --- 29.05 --- 0.95
% 25 --- 24.33 --- 0.67
% 20 --- 19.58 --- 0.42
% 15 --- 14.6 --- 0.4
% 10 --- 9.37 --- 0.63
% 5 --- 4.65 --- 0.35
% 0 --- 0.04 --- 0.04