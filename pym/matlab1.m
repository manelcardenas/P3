[y,Fs] = audioread('matplotlib.wav');


[c,lags] = xcorr(y);

subplot(2,1,1);
plot(y)
title('Se�al')

subplot(2,1,2); 
plot(lags,c)
title('Autocorrelaci�n')

%plot(lags, c);

%axis([0 255 -8 10]);

