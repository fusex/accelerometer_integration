close all;

csv = csvread("../data/20180321_204022/merged_data");
t_initial = csv(1, 1);

csv_acc = csv;
a_invalid = csv(:, 13) == 0;

# Remove invalid accelerations
csv_acc(a_invalid, :) = [];
t = (10**-9).*(csv_acc(:, 1)-t_initial);

c_m_s = 9.8*(0.244*10**-3);
acc_x = c_m_s.*csv_acc(:, 2);
acc_y = c_m_s.*csv_acc(:, 3);
acc_z = c_m_s.*csv_acc(:, 4);

figure(1,"position",get(0,"screensize"));
hold on;
[hax, h1, ~] = plotyy(t, acc_x, [0], [0]);
[~, h2, ~] = plotyy(t, acc_y, [0], [0]);
[~, h3, ~] = plotyy(t, acc_z, [0], [0]);
set(h1, 'color', 'r');
set(h2, 'color', 'b');
set(h3, 'color', 'g');
set([h1, h2, h3], 'LineWidth', 1.5);
xlabel("Time (s)", 'FontSize', 12)
ylabel(hax(1), "Acceleration (m/s^2)", 'FontSize', 12)

# Pressure
csv_pressure = csv;
p_invalid = csv(:, 18) == 0;

# Remove invalid pressure
csv_pressure(p_invalid, :) = [];
t = (10**-9).*(csv_pressure(:, 1)-t_initial);

# Convert pressure to meters
c_hpa = 1/4096;
p_hpa = c_hpa.*csv_pressure(:, 11);
t_c = 42.5 + csv_pressure(:, 12)./480;
h_m = ((1013.25./p_hpa).^(1/5.257)-1).*(t_c+273.15)./0.0065;
h_m_n = h_m-h_m(1);

# Plot
[hax, ~, h4] = plotyy([0], [0], t, h_m_n);
ylabel(hax(2), "Height (m)", 'FontSize', 12)
set(h4, 'color', 'k');
set(h4, 'LineWidth', 1.5);
set(hax(1), 'ycolor', 'k');
set(hax(2), 'ycolor', 'k');
ytick_1 = -100:10:100;
ytick_2 = -10:0.5:0;
xtick = 152.5:0.2:157;
set(hax(1), 'ytick', ytick_1);
set(hax(2), 'ytick', ytick_2);
set(hax(1), 'xtick', xtick);
set(hax(2), 'xtick', xtick);
axis(hax(1), [152.5 157 -100 100]);
axis(hax(2), [152.5 157 -10 0]);
linkaxes(hax, "x");
lg = legend([h1, h2, h3, h4], 'x-acc', 'y-acc', 'z-acc', 'height');
set(lg, 'FontSize', 12);
