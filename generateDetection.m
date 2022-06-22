function [detection,flag] = generateDetection(xr,mfradar,current_job)

% Compute detection threshold
nbw         = mfradar.Receiver.SampleRate/(mfradar.Wav.SampleRate/mfradar.Wav.SweepBandwidth);
npower      = noisepow(nbw,mfradar.Receiver.NoiseFigure,mfradar.Receiver.ReferenceTemperature);
pfa         = 1e-6;
threshold   = npower * db2pow(npwgnthresh(pfa,1,'noncoherent'));

arraysz     = mfradar.Radiator.Sensor.Size(1);
ant_snrgain = pow2db(arraysz^2);

mfcoeff     = mfradar.MFCoeff;
mfgain      = pow2db(norm(mfcoeff)^2);
threshold   = threshold * db2pow(mfgain+2*ant_snrgain);
threshold   = sqrt(threshold);
rgates  = mfradar.RGates;

% Matched filtering and time varying gain
xrsmf = step(mfradar.AGC,step(mfradar.MF,xr));

sumchan = zeros(200,1);
for i =1:200
    for k = 1:2500
        sumchan(i)=sumchan(i)+abs(xrsmf(i,k));
    end
end
sumchan = abs(sumchan);
if (max(sumchan)>threshold)
    flag = true;
    fprintf('detection!\n');
    [~,tgtidx] = findpeaks([abs(sumchan);0],'MinPeakHeight',threshold,...
        'Sortstr','Descend','NPeaks',1);
    rng_est = mfradar.RGates(tgtidx-(numel(mfradar.MFCoeff)-1));
    ang_est = step(mfradar.RxFeed,xrsmf(tgtidx-1,:),current_job.BeamDirection);
    
    measNoise = diag([0.1, 0.1, 150].^2);           % Measurement noise matrix
    detection = struct( 'detection',[ang_est(1);ang_est(2);rng_est], 'MeasurementNoise', measNoise);
else
    detection = [];
    flag = false;
end

% if current_time < 0.3 || strcmp(current_job.JobType,'Track')||strcmp(current_job.JobType,'Search')
     fprintf('\n%s\t[%f %f]',current_job.JobType,current_job.BeamDirection(1),...
         current_job.BeamDirection(2));
end

