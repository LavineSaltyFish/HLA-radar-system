function [ env ] = setTarget( fs,fc,mode,rcs,tgt_Az,tgt_el,tgt_r,tgt_vx,tgt_vy,tgt_vz,tgt_ax,tgt_ay,tgt_az )

x = tgt_r*cosd(tgt_el)*sind(90-tgt_Az);
y = tgt_r*cosd(tgt_el)*cosd(90-tgt_Az);
z = tgt_r*sind(tgt_el);

tgt_pos = [x;y;z];
tgt_vel = [tgt_vx;tgt_vy;tgt_vz];
tgt_accelr = [tgt_ax;tgt_ay;tgt_az];
tgtrcs = rcs;

if mode == 1
    
    tgtmotion = phased.Platform('InitialPosition',tgt_pos,'Velocity',[0;0;0]);% phased.platform: positions+velocity+acceleration of objects in global coordinates
    target = phased.RadarTarget('MeanRCS',tgtrcs,'OperatingFrequency',fc);% generate the target's reflection coefficient sqrt£¨G£©
end

if mode == 2
    
    tgtmotion = phased.Platform('InitialPosition',tgt_pos,'Velocity',tgt_vel);
    target = phased.RadarTarget('MeanRCS',tgtrcs,'OperatingFrequency',fc);
end

if mode == 3
    tgtmotion = phased.Platform('MotionModel','Acceleration','InitialPosition',tgt_pos,'InitialVelocity',tgt_vel,'Acceleration',tgt_accelr);
    target = phased.RadarTarget('MeanRCS',tgtrcs,'OperatingFrequency',fc);
end

channel = phased.FreeSpace('SampleRate',fs,'TwoWayPropagation',true,'OperatingFrequency',fc);
env.Target       = target;
env.TargetMotion = tgtmotion;
env.Channel      = channel;
end

