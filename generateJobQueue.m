function jobq = generateJobQueue(beamPosition)
searchQ = struct('JobType','Search','BeamDirection',num2cell(beamPosition,1));
searchIdx = 1;
trackQ = struct('JobType','Track','BeamDirection',[],'Range',[]);
TrackIdx = 0;
tgtFound = false;
%
% % ================================job queue struct========================
jobq.SearchQueue  = searchQ;
jobq.SearchIndex  = searchIdx;
jobq.TrackQueue   = trackQ;
jobq.TrackIndex = TrackIdx;
jobq.Flag = tgtFound;
end