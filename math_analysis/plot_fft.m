function plot_fft(plot_name, signal, point_mult, fs)
  N = point_mult*size(signal, 1);
  X = fftshift(fft(signal, N));
  dF = fs/N;
  f = -fs/2:dF:fs/2-dF;
  X_norm = abs(X)/N;
  X_db = 20*log10(X_norm/max(X_norm));
  figure;
  plot(f, X_db);
  title(plot_name); 
