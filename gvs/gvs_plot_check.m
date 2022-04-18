% Table=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data_csv/data.csv');
% dists = Table(:, 1);
% thetas = Table(:, 2);
% 
% tiledlayout(2, 1)
% nexttile
% plot(dists, '.')
% 
% nexttile
% plot(thetas, '.')

Table0=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data_csv/data_freq_0.csv');
Table1=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data_csv/data_freq_1.csv');
Table2=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data_csv/data_freq_2.csv');
Table3=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data_csv/data_freq_3.csv');

freqs0 = Table0(:, 1);
freqs1 = Table1(:, 1);
freqs2 = Table2(:, 1);
freqs3 = Table3(:, 1);


f = figure;
f.Position = [200 200 1000 1000];
tiledlayout(2, 2);
nexttile
plot(freqs0);
subtitle("freq 0")
xlim([0 length(freqs0)])
nexttile
plot(freqs1);
subtitle("freq 1")
xlim([0 length(freqs1)])
nexttile
plot(freqs2);
subtitle("freq 2")
xlim([0 length(freqs2)])
nexttile
plot(freqs3);
subtitle("freq 3")
xlim([0 length(freqs3)])
