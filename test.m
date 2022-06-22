clear all
fs = 6e6;
fc = 1e10;
mode = 1;
rcs = 10;
tgt_Az = 0;
tgt_el = 8;
tgt_r = 1500;
tgt_vx = 10;
tgt_vy = 10;
tgt_vz = 0;
tgt_ax = 0;
tgt_ay = 0;
tgt_az = 0;
env  = setTarget( fs,fc,mode,rcs,tgt_Az,tgt_el,tgt_r,tgt_vx,tgt_vy,tgt_vz,tgt_ax,tgt_ay,tgt_az );

posx = 0;
posy = 2000;
posz = 0;
[mfradar,BeamWidth] = setRadar(fc,fs,posx,posy,posz);

Az = 45;
El = 60;
%[beamPosition]=generateBeamPosition(Az,El,BeamWidth);
[beamPosition]=getScanGrid(Az,El,BeamWidth);

jobq = generateJobQueue(beamPosition);
current_time = 0;

% pulNum = 10;
 dwellTime = 10 * 1/mfradar.Wav.PRF;
 revisitTime = dwellTime * numel(beamPosition)/2;
% current_job = struct('JobType','Search','BeamDirection',[-20;8]);

current_job=jobq.SearchQueue(1);
detection = [];

% xr = generateEcho(mfradar,env,current_job);
% xrsmf = step(mfradar.AGC,step(mfradar.MF,xr));
% sumchan = zeros(200,1);
% for i =1:200
%     for k = 1:2500
%         sumchan(i)=sumchan(i)+xrsmf(i,k);
%     end
% end
% sumchan = abs(sumchan);
% 
DetectionResult = [];
for i = 1:numel(beamPosition)/2
[tgtpos,tgtvel] = step(env.TargetMotion,dwellTime);
[current_job,jobq] = jobUpdate(jobq,DetectionResult);

xr = generateEcho(mfradar,env,current_job);
[detection,flag] = generateDetection(xr,mfradar,current_job);

if flag
DetectionResult = detection;
break
end

jobq.Flag = flag;
current_time = current_time + dwellTime;
end

