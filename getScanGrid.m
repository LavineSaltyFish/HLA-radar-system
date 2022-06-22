function [beamPosition]=getScanGrid(Az,El,BeamWidth)
x = floor(2*Az/BeamWidth);
gridx = [0:BeamWidth:x*BeamWidth]-x*BeamWidth/2;
y = floor(El/BeamWidth);
gridy = 0:BeamWidth:y*BeamWidth;
% beamNum = x*y;
beamPosition = [];
for i =1:y
    for k = 1:size(gridx,2)
        tempy(k) = gridy(i);
    end
    temp = [gridx;tempy];
    beamPosition = [beamPosition temp];
end
end

