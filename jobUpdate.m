function [currentjob,jobq] = jobUpdate(jobq,detection)

searchq   = jobq.SearchQueue;
trackq    = jobq.TrackQueue;
searchidx = jobq.SearchIndex;
trackidx = jobq.TrackIndex;
flag = jobq.Flag;

if flag == 0
    if (mod((searchidx-trackidx),10)==0 && (trackidx~=0))
        currentjob = trackq;    
        searchidx = searchidx+1;
    else
    searchidx_roll = mod(searchidx-1,numel(searchq))+1;
    currentjob = searchq(searchidx_roll);
    searchidx = searchidx+1;
    end
else
         if isempty(trackq.BeamDirection)
            trackq.JobType = 'Track';
            trackq.BeamDirection = detection(1:2);
            trackq.Range = detection(3);
            trackidx = searchidx-1;
         end
    searchidx_roll = mod(searchidx-1,numel(searchq))+1;
    currentjob = searchq(searchidx_roll);
    searchidx = searchidx+1;
    flag = false;
end


jobq.SearchQueue  = searchq;
jobq.SearchIndex  = searchidx;
jobq.TrackQueue   = trackq;
jobq.TrackIndex = trackidx;
jobq.Flag = flag;
end
