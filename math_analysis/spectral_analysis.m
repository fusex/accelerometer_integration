close all;

# Raw acceleration analysis
Fs = 50
#csv = csvread("y_acc.csv");
#t = csv(579:729, 1);

#ROI -> (11.5 - 14.5s) 
#acc_x = 9.8.*csv(579:729, 6);
#acc_y = 9.8.*csv(579:729, 7);
#acc_z = 9.8.*csv(579:729, 8);
  
#plot_acc("Acceleration raw", t, acc_x, acc_y, acc_z)
#plot_fft("x-Acceleration raw FFT", acc_x, 1000, Fs)
#plot_fft("y-Acceleration raw FFT", acc_y, 1000, Fs)
#plot_fft("z-Acceleration raw FFT", acc_z, 1000, Fs)

# Acceleration on inertial reference
#csv = csvread("./output/acceleration_bo.csv");
#t = csv(230:380, 1);
#acc_x = csv(230:380, 2);
#acc_y = csv(230:380, 3);
#acc_z = csv(230:380, 4);
#plot_acc("Acceleration inertial", t, acc_x, acc_y, acc_z)
#plot_fft("x-Acceleration inertial FFT", acc_x, 1000, Fs)
#plot_fft("y-Acceleration inertial FFT", acc_y, 1000, Fs)
#plot_fft("z-Acceleration inertial FFT", acc_z, 1000, Fs)

# Acceleration after gravity removal (whole)
#csv = csvread("../output/acceleration_bf.csv");
#t = csv(:, 1);
#acc_x = csv(:, 2);
#acc_y = csv(:, 3);
#acc_z = csv(:, 4);
#plot_acc("Acceleration no gravity (whole)", t, acc_x, acc_y, acc_z)

# Acceleration after gravity removal
#csv = csvread("../output/acceleration_bf.csv");
#t = csv(230:380, 1);
#acc_x = csv(230:380, 2);
#acc_y = csv(230:380, 3);
#acc_z = csv(230:380, 4);
#plot_acc("Acceleration no gravity", t, acc_x, acc_y, acc_z)
#plot_fft("x-Acceleration no gravity FFT", acc_x, 1000, Fs)
#plot_fft("y-Acceleration no gravity FFT", acc_y, 1000, Fs)
#plot_fft("z-Acceleration no gravity FFT", acc_z, 1000, Fs)
  
csv = csvread("../output/acceleration.csv");
t = csv(230:380, 1);
acc_x = csv(230:380, 2);
acc_y = csv(230:380, 3);
acc_z = csv(230:380, 4);
plot_acc("Acceleration after low-pass filter", t, acc_x, acc_y, acc_z)
plot_fft("x-Acceleration after LP FFT", acc_x, 1000, Fs)
plot_fft("y-Acceleration after LP FFT", acc_y, 1000, Fs)
plot_fft("z-Acceleration after LP FFT", acc_z, 1000, Fs)

csv = csvread("../output/velocity_bf.csv");
t = csv(:, 1);
v_x = csv(:, 2);
v_y = csv(:, 3);
v_z = csv(:, 4);
plot_acc("Velocity before filter", t, v_x, v_y, v_z)

#
#csv = csvread("./output/velocity.csv");
#t = csv(:, 1);
#acc_x = csv(:, 2);
#acc_y = csv(:, 3);
#acc_z = csv(:, 4);
#figure
#plot(t,acc_x,'color','r'); hold on;
#plot(t,acc_y,'color','b'); hold on;
#plot(t,acc_z,'color','g');
#title("Velocity after high-pass filter")
