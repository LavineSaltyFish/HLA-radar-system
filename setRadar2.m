function [mfradar2,BeamWidth] = setRadar2(fc,fs,posx,posy,posz)
pd = 0.9;            % Probability of detection
pfa = 1e-6;          % Probability of false alarm
Rmax = 5000;    % Maximum unambiguous range
Rres = 150;      % Required range resolution
tgt_rcs = 1;         % Required target radar cross section
pulNum = 10;   % Number of pulses to integrate

v = 3e8;
bandwidth = v/(2*Rres); % Rres = c*tao*(Kr)/2 = c*(Kr)/2B % Kr带宽展宽因子，与进行IDFT处理前使用的权函数有关
lambda = v/fc;
prf    = 1/range2time(Rmax,v); % Rmax = c*T/2 % range2time: 2-way propogation

EleNum=50;
waveform = phased.LinearFMWaveform('SampleRate', fs, 'PRF', prf,'PulseWidth',0.25/prf,'SweepBandwidth', bandwidth);

ura = phased.URA('Size',[EleNum EleNum],'ElementSpacing',[lambda/2, lambda/2]);
ura.Element.BackBaffled = true;

radar_pos=[posx;posy;posz];
sensormotion =  phased.Platform('InitialPosition',radar_pos,'Velocity',[0;0;0]);% phased.platform: positions+velocity+acceleration of objects in global coordinates

% Calculate the array gain
arraygain = phased.ArrayGain('SensorArray',ura,'PropagationSpeed',v);
ag = step(arraygain,fc,[0;0]);
BeamWidth = rad2deg(sqrt(4*pi/db2pow(ag)));% G=4π/θ?

radiator = phased.Radiator('OperatingFrequency',fc,'WeightsInputPort',true);
collector = phased.Collector('OperatingFrequency',fc);
radiator.Sensor = ura; % radiator:signals→wavefield % output the field of 1m from the phase center, then use phase.Freespace to transmit % default combineRadiatedSignals true
collector.Sensor = ura;% collector:narrowband wave field→signals % use STEERANG to change the direction that the array faces

transmitter = phased.Transmitter('InUseOutputPort',true);
receiver = phased.ReceiverPreamp('NoiseFigure',7,'SampleRate',fs,'EnableInputPort',true);

snr_min = albersheim(pd, pfa, pulNum); % noncoherent detection scheme's approximation for SNR % to achieve the specified detection and false-alarm probabilities for a nonfluctuating target in independent and identically distributed Gaussian noise
% (pd,pfa,number of pulses)
peak_power = radareqpow(lambda,Rmax,snr_min,waveform.PulseWidth,'RCS',tgt_rcs,'Gain',transmitter.Gain + ag);
transmitter.PeakPower = peak_power;

npower = noisepow(bandwidth,receiver.NoiseFigure,receiver.ReferenceTemperature);
steeringvec = phased.SteeringVector('SensorArray',ura,'PropagationSpeed',v);% steering vectors for a sensor array of multiple directions and frequencies


% ______________________matched filter, AGC and monopulse__________________________

% matched filter
mfcoeff = getMatchedFilter(waveform);
mf      = phased.MatchedFilter('Coefficients',mfcoeff,'GainOutputPort', true);

% AGC
fast_time_grid = unigrid(0, 1/fs, 1/prf, '[)');% creating grid [0:1/fs:1/prf]% fast-time: a single pulse's sampling
range_gates = v*fast_time_grid/2;
agc = phased.TimeVaryingGain('RangeLoss',2*fspl(range_gates,lambda),'ReferenceLoss',2*fspl(max(range_gates),lambda)); % applies a time varying gain to input signals % free space path loss
% Loss=20lg(4πR/lambda)

% monopulse
monoFeed = phased.SumDifferenceMonopulseTracker2D('SensorArray',ura,'PropagationSpeed',v,'OperatingFrequency',fc);
%======================= multi-fuction radar system struct==================
mfradar2.ArrayGain = arraygain;
mfradar2.Transmitter      = transmitter;
mfradar2.Receiver      = receiver;
mfradar2.Radiator   = radiator;
mfradar2.Collector   = collector;
mfradar2.Wav     = waveform;
mfradar2.NPower = npower;

mfradar2.FGates = fast_time_grid;
mfradar2.RGates = range_gates;
mfradar2.RxFeed  = monoFeed;
mfradar2.MFCoeff = mfcoeff;
mfradar2.MF      = mf;
mfradar2.AGC     = agc;
mfradar2.STV     = steeringvec;
mfradar2.Position = sensormotion;
end
