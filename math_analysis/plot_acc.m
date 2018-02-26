function plot_acc(plot_name, t, acc_x, acc_y, acc_z)
  figure
  plot(t,acc_x,'color','r'); hold on;
  plot(t,acc_y,'color','b'); hold on;
  plot(t,acc_z,'color','g');
  title(plot_name)
