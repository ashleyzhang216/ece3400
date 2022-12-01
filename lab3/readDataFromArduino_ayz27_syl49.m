% Carl Poitras
% Fall 2022
%
% This file plays a sound of (possibly) varying frequency over your computer's
% speakers, 
% of duration soundDuration secs, 
% initial frequency of initialFreq Hz 
% to a final frequency finalFreq Hz, 
% with the frequency changing uniformly throughout the duration of the sound.
% While the sound is playing, MATLAB collects data from the Arduino 
% over the serial port. When all data is received, Fourier analysis is 
% performed.
% Make sure that the Arduino Serial monitor is closed when you run this
% file.

% Variables that are global
global initialFreq finalFreq 

clear serialComm; % Ensure that no serial port is open

initialFreq = 500; % Hz, Starting frequency of played sound
finalFreq = 501;% Duration of sound to be played, in secs.

% Sound duration in seconds.
% This doesn't have to be long (~1-2 sec) for a single frequency, but should
% be longer for a range of frequencies (a few seconds). 
soundDuration = 2; 

% countRead is the number of items to read over the serial port, 
% i.e., from the Nano. This is an approximation.
countRead = 25000 * soundDuration;


% Open a serial connection to the Arduino
% 1. Close the Arduino Serial Monitor
% 2. Make sure that 'COM4' below is replaced with the correct port number
%    that your Arduino uses.
serialComm = serialport('COM7', 230400); 

% Play a sound through your computer's speakers that will have a
% duration of soundDuration seconds, and whose frequency will vary
% from initialFreq to finalFreq. 
generateSound(soundDuration)

% Read the data from the serial port, i.e., the Arduino, while the
% sound is playing. 
tic % Start a timer to determine how long it takes to read the data
dataDouble  = read(serialComm, countRead, 'int8'); 

% Stop the timer: durationOfDataRead contains the time that it took,
% in seconds, to read the data. THIS IS APPROXIMATE
durationOfDataRead = toc

% Make sure that the serial communication between MATLAB and the Nano
% is closed.
clear serialComm;

% Create a time axis that has a time length of durationOfDataRead seconds,
% that will have as many elements as there are data points.
durationOfDataRead = durationOfDataRead - 0.062;
timeAxis = linspace(0, durationOfDataRead, length(dataDouble));

%
% Fourier stuff
%
% We need the sampling period for the data that MATLAB acquired.
samplingPeriodT = mean(diff(timeAxis)); 

% To the sampling period corresponds a Sampling Frequency Fs.
Fs = 1/samplingPeriodT;
FNyq = Fs/2; % Nyquist frequency
numberDataPoints = length(timeAxis); % Number of points in our signal.

% Take the FFT of the data
FFTofData = fft(dataDouble);

% Normalize the FFT
NormalizedFFT = abs(FFTofData/numberDataPoints); 

% Due to the cyclic nature of the FFT, only keep the first 1/2 of
% the FFT points (the second 1/2 contains the same information).
FFTSpectrum = NormalizedFFT(1 : round(numberDataPoints/2));
FFTSpectrum=FFTSpectrum(3:end);

% Generate a frequency axis to visualize the spectrum
frequencyAxis = linspace(0, 1, round(numberDataPoints/2))*FNyq; 
frequencyAxis=frequencyAxis(3:end);

% Plot the results
% Time signal
subplot(2,1,1)
plot(timeAxis, dataDouble)
xlabel('Time (secs)');
ylabel('Time Signal Value (a.u.)');
title('Time signal')
axis([0 max(timeAxis) 0 max(dataDouble)*1.25])


%Spectrum
subplot(2,1,2)
plot(frequencyAxis, smoothdata(FFTSpectrum))
%  hold on
%  plot(frequencyAxis, smoothdata(FFTSpectrum,'sgolay',1200))
% hold on
xlabel('Frequency (Hz)');
ylabel('Frequency Value (a.u.)');
title('Spectrum signal')
axis tight
%axis([frequencyAxis(1) 2000 0 20])
hold off
axis([0 3000 0 max(smoothdata(FFTSpectrum(1:1000)))*1.3])

function generateSound(soundDuration)
global initialFreq finalFreq

% Sampling frequency for playing sound on speakers
soundPlaySamplingFreq = 44.1e3;

% Amplitude of sound to be played
Amplitude = 1.0; 

% Vary the frequencies: Create entries of time in flowTime
% that will be used in the sunusoid that will be played. We want
% to play a sound whose frequency in the beginning will be initialFreq Hz
% and at the end (after soundDuration secs) will be finalFreq Hz.
iniStepwidth = initialFreq / soundPlaySamplingFreq;
finStepwidth = finalFreq / soundPlaySamplingFreq;
flowStepwidth = linspace(iniStepwidth, finStepwidth, ...
    soundDuration * soundPlaySamplingFreq);
flowTime = cumsum(flowStepwidth);
soundWave = Amplitude * sin(2*pi*flowTime);

% Play the sound
sound(soundWave, soundPlaySamplingFreq);
end


