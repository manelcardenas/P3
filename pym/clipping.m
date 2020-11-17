
[wave,fs]=audioread('matplotlib.wav'); 


subplot(2,1,1);
plot(wave)


for i = 1:length(wave)
    if abs(wave(i)) < 0.05
        wave(i) = 0;
    end
    if wave(i) > 0.05
        wave(i) = wave(i) - 0.05;
    end
    if wave(i) < -0.05
        wave(i) = wave(i) + 0.05;
    end
end

subplot(2,1,2); 
plot(wave)
title('senyal amb central clipping')
plot(wave)
