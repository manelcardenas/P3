[y,Fs] = audioread('matplotlib.wav');


[c,lags] = xcorr(y);

subplot(2,1,1);
plot(y)
title('Señal')

subplot(2,1,2); 
plot(lags,c)
title('Autocorrelación')

%plot(lags, c);

%axis([0 255 -8 10]);

