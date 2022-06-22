function [ DetectionResult,jobq ] = buildDetectionResult( flag,detection,jobq)
if flag
DetectionResult = detection.detection;
else
    DetectionResult=[];
end
jobq.Flag = flag;
end

