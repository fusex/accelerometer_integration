function plot_acc(plot_name, t, acc_x, acc_y, acc_z)
  figure
  plot(t,acc_x,'color','r', 'LineWidth', 1.5); hold on;
  plot(t,acc_y,'color','b', 'LineWidth', 1.5); hold on;
  plot(t,acc_z,'color','g', 'LineWidth', 1.5);
  legend('x', 'y', 'z')
  xlabel("Time (s)", 'FontSize', 12)
  ylabel("Acceleration (m/s^2)", 'FontSize', 12)
