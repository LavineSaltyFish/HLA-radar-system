function [xr] = generateEcho(mfradar,env,current_job)

% Radar position
% [radarpos radarvel] = mfradar.Position(dwellTime);
radarpos = mfradar.Position.InitialPosition;
radarvel = [0;0;0];

% Number of pulses and operating frequency
Npulses = 10;
fc = mfradar.Radiator.OperatingFrequency;

for m = 1:Npulses
    % Waveform
    x = step(mfradar.Wav);
    
    % Update target motion
    [tgtpos,tgtvel] = step(env.TargetMotion,1/mfradar.Wav.PRF);
    [~,tgtang]      = rangeangle(tgtpos,radarpos);
    
    % Transmit
    [xt,status]  = step(mfradar.Transmitter,x);
    w  = step(mfradar.STV,fc,current_job.BeamDirection);
    xt = step(mfradar.Radiator,xt,tgtang,conj(w));
    
    % Propagation
    xp = step(env.Channel,xt,radarpos,tgtpos,radarvel,tgtvel);
    xp = step(env.Target,xp);
    
    % Receive and monopulse
    xr = step(mfradar.Collector,xp,tgtang);
    % Pulse integration
    if m == 1
        xr   = step(mfradar.Receiver,xr,~(status>0));
    else
        xr   = xr+step(mfradar.Receiver,xr,~(status>0));
    end 
end

end
