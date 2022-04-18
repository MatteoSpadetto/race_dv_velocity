Table=csvread('/home/matteospadetto/Documents/unitn_ms/Thesis_sw/gvs/data.csv');
dists = Table(:, 1);
thetas = Table(:, 2);

tiledlayout(2, 1)
nexttile
plot(dists, '.')

nexttile
plot(thetas, '.')
