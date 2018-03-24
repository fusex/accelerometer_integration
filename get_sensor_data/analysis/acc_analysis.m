close all;

csv = csvread("../data/20180321_204022/merged_data");
t_initial = csv(1, 1);
t = (10**-9).*(csv(:, 1)-t_initial);

c_m_s = 9.8*(0.244*10**-3);
acc_x = c_m_s.*csv(:, 2);
acc_y = c_m_s.*csv(:, 3);
acc_z = c_m_s.*csv(:, 4);
  
plot_acc("Acceleration (m/s2)", t, acc_x, acc_y, acc_z)
