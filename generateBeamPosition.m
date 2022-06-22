function [beamPosition] = generateBeamPosition(Az,El,beamWidth) % scan_grid(alpha,beta),beamPosition(Az;El)
%% 边界
bw_alpha = round(sin(beamWidth*pi/180)*1000)/1000;
% theta=beamWidth,phi=0
% alpha = sin(theta)cos(phi)=sin(beamWidth)
% beta = sin(theta)sin(phi)=0

%plane El
thetaT = atan(-(cos(0*pi/180) - cos(El*pi/180))/(sin(0*pi/180) - sin(El*pi/180))*cos(Az*pi/180))*180/pi;
% Nt=天线偏移方位角=0,T=天线倾角
% alpha = cos(El)sin(Az)cos(Nt)-cos(El)cos(Az)sin(Nt) = 0
% beta = -cos(El)sin(Az)sin(Nt)sin(T)-cos(El)cos(Az)sin(T)=-sin(T)


% （R El Az）:雷达球坐标系
%  (R theta phi):阵列平面球坐标系
% （alpha beta）:正弦空间坐标系

%left
az = -1*Az;
el = 0:El;
alpha1 = cos(el*pi/180)*sin(az*pi/180);
beta1 = sin(el*pi/180)*cos(thetaT*pi/180)-cos(el*pi/180)*cos(az*pi/180)*sin(thetaT*pi/180);
%right
az = Az;
el = 0:El;
alpha2 = cos(el*pi/180)*sin(az*pi/180);
beta2 = sin(el*pi/180)*cos(thetaT*pi/180)-cos(el*pi/180)*cos(az*pi/180)*sin(thetaT*pi/180);
%bottom
az = -1*Az:Az;
el = 0;
alpha3 = cos(el*pi/180)*sin(az*pi/180);
beta3 = sin(el*pi/180)*cos(thetaT*pi/180)-cos(el*pi/180)*cos(az*pi/180)*sin(thetaT*pi/180);
%top
az = -1*Az:Az;
el = El;
alpha4 = cos(el*pi/180)*sin(az*pi/180);
beta4 = sin(el*pi/180)*cos(thetaT*pi/180)-cos(el*pi/180)*cos(az*pi/180)*sin(thetaT*pi/180);

bottom = min(beta3);
bottomUp = max(beta3);


right = max(alpha3);
left= min(alpha3);

% right = max(alpha3) + bw_alpha/2;
% left= min(alpha3) - bw_alpha/2;

% 交错波束偶数行奇数行要错开半个波束宽度，左边展宽半个，右边展宽半个
top = max(beta4);
topDown = min (beta4);

% figure
% subplot(2,2,1)
% plot(alpha1,beta1,'r');
% axis([ left-0.1 right+0.05 bottom-0.05 top+0.1]);
% hold on; grid on;
% title('雷达波束在正弦空间的编排');xlabel('alpha(rad)');ylabel('beta(rad)');
% plot(alpha2,beta2,'r');
% plot(alpha3,beta3,'r');
% plot(alpha4,beta4,'r');



%% 波位排列，分奇偶行计算，圆心在边界外的去掉
beamCount = 0;
Mx = round((right-left)/bw_alpha);
My = round((top - bottom)/(0.866*bw_alpha));% 交错排列，正三角形

%----------------------------------奇数行-----------------------------------
y0Counter = 0;


for y0 = bottom+bw_alpha/2: 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0计数器
    x0Counter = 0;
    for x0 = 0:bw_alpha:right % 右半边
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001步长 x0处的一个波束
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0向上
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0向下
        
        flag = 0;
        % alpha2,beta2:右边界
        [minValue minSite] = min(abs(beta2 - y0));% 离当前y0最近的右边界点
        if alpha2(minSite)  >= x0 % x0在右边界内，继续向右找点
            flag = 1;
        end
        
        if y0 < bottomUp % 在下边界上的点
            % alpha3,beta3:下边界
            [minValue minSite] = min(abs(alpha3 - x0)); % 离当前x0最近的下边界点
            if beta3(minSite) > y0 % y0在下边界点下面（外）
                flag = 0;
            end
        end
        if y0 > topDown % 在上边界上的点
            % alpha4,beta4:上边界
            [minValue minSite] = min(abs(alpha4 - x0));% 离当前x0最近的上边界点
            if beta4(minSite) + bw_alpha/2 <= y0 % y0在上边界上面（外）
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %该行增加一个波位
            x0Counter = x0Counter + 1; % x0计数器
            beamCount = beamCount+1;
%             plot(x,real(y1));
%             plot(x,real(y2));
            
            pause(0.01)
            scan_grid(beamCount,1) = x0;
            scan_grid(beamCount,2) = y0;
            
            beamNumGrid(2*y0Counter-1,1)= x0Counter;
            
        end
        
    end
    
end

y0Counter = 0;

for y0 = bottom+bw_alpha/2: 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0计数器
    x0Counter = 0;
    for x0 = -bw_alpha:-bw_alpha:left % 左半边
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001步长 x0处的一个波束
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0向上
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0向下
        
        % alpha1,beta1:左边界
        flag = 0;
        [minValue minSite] = min(abs(beta1 - y0)); % 离当前y0最近的左边界点
        if alpha1(minSite)  <= x0 % x0在左边界右侧（内），继续寻找点
            flag = 1;
        end
        
        if y0 < bottomUp % 在下边界上的点
            % alpha3,beta3:下边界
            [minValue minSite] = min(abs(alpha3 - x0)); % 离当前x0最近的下边界点
            if beta3(minSite) > y0 % y0在下边界点下面（外）
                flag = 0;
            end
        end
        if y0 > topDown % 在上边界上的点
            % alpha4,beta4:上边界
            [minValue minSite] = min(abs(alpha4 - x0));% 离当前x0最近的上边界点
            if beta4(minSite) + bw_alpha/2 <= y0 % y0在上边界上面（外）
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %该行增加一个波位
            x0Counter = x0Counter + 1; % x0计数器
            beamCount = beamCount+1;
%             plot(x,real(y1));
%             plot(x,real(y2));
            
            pause(0.01)
            scan_grid(beamCount,1) = x0;
            scan_grid(beamCount,2) = y0;
            beamNumGrid(2*y0Counter-1,1)= beamNumGrid(2*y0Counter-1,1)+1;
        end
        
    end
    
end



%-----------------------------------偶数行----------------------------------
y0Counter = 0;

for y0 = bottom+bw_alpha/2+0.866*bw_alpha : 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0计数器
    x0Counter = 0;
    for x0 = bw_alpha/2:bw_alpha:right % 右半边
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001步长 x0处的一个波束
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0向上
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0向下
        
        flag = 0;
        % alpha2,beta2:右边界
        [minValue minSite] = min(abs(beta2 - y0));% 离当前y0最近的右边界点
        if alpha2(minSite)  >= x0 % x0在右边界内，继续向右找点
            flag = 1;
        end
        
        if y0 < bottomUp % 在下边界上的点
            % alpha3,beta3:下边界
            [minValue minSite] = min(abs(alpha3 - x0)); % 离当前x0最近的下边界点
            if beta3(minSite) > y0 % y0在下边界点下面（外）
                flag = 0;
            end
        end
        if y0 > topDown % 在上边界上的点
            % alpha4,beta4:上边界
            [minValue minSite] = min(abs(alpha4 - x0));% 离当前x0最近的上边界点
            if beta4(minSite) + bw_alpha/2 <= y0 % y0在上边界上面（外）
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %该行增加一个波位
            x0Counter = x0Counter + 1; % x0计数器
            beamCount = beamCount+1;
%             plot(x,real(y1));
%             plot(x,real(y2));
            
            pause(0.01)
            scan_grid(beamCount,1) = x0;
            scan_grid(beamCount,2) = y0;
%             
             beamNumGrid(2*y0Counter,1)= x0Counter;
        end
    end
    
end

y0Counter = 0;
for y0 = bottom+bw_alpha/2+0.866*bw_alpha : 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0计数器
    x0Counter = 0;
    for x0 = -bw_alpha/2:-bw_alpha:left % 左半边
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001步长 x0处的一个波束
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0向上
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0向下
        
        % alpha1,beta1:左边界
        flag = 0;
        [minValue minSite] = min(abs(beta1 - y0)); % 离当前y0最近的左边界点
        if alpha1(minSite)  <= x0 % x0在左边界右侧（内），继续寻找点
            flag = 1;
        end
        
        if y0 < bottomUp % 在下边界上的点
            % alpha3,beta3:下边界
            [minValue minSite] = min(abs(alpha3 - x0)); % 离当前x0最近的下边界点
            if beta3(minSite) > y0 % y0在下边界点下面（外）
                flag = 0;
            end
        end
        if y0 > topDown % 在上边界上的点
            % alpha4,beta4:上边界
            [minValue minSite] = min(abs(alpha4 - x0));% 离当前x0最近的上边界点
            if beta4(minSite) + bw_alpha/2 <= y0 % y0在上边界上面（外）
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %该行增加一个波位
            x0Counter = x0Counter + 1; % x0计数器
            beamCount = beamCount+1;
%             plot(x,real(y1));
%             plot(x,real(y2));
            
            pause(0.01)
            scan_grid(beamCount,1) = x0;
            scan_grid(beamCount,2) = y0;
            
             beamNumGrid(2*y0Counter,1)= beamNumGrid(2*y0Counter,1)+1;
            
        end
    end
    
end


% subplot(2,2,3)
 row = size(scan_grid,1);
% 
% for n = 1:row
%     plot(scan_grid(n,1),scan_grid(n,2),'o','LineWidth',1,'MarkerEdgeColor','y','MarkerFaceColor','g','MarkerSize',5);
%     axis([left right bottom top]);
%     hold on;
% end
% title(['波位示意图(波位数：',num2str(beamCount),')']);
% xlabel('alpha(rad)');ylabel('beta(rad)');

[scan_grid,I] = sortrows(scan_grid,2);


startIdx = 1;
for n = 1 : row - 1
    if scan_grid(n + 1, 2) > scan_grid(n, 2)
        scan_grid(startIdx:n, :) = sortrows(scan_grid(startIdx:n, :), 1);
        startIdx = n + 1;
    end
end
scan_grid(startIdx:row, :) = sortrows(scan_grid(startIdx:row, :), 1);



%% 从（alpha，theta）坐标系转回（R，Az，El）坐标系

% El = asin(beta*cosT + sqrt(1-alpha^2-beta^2)*sinT)
% Az = atan((alpha*cos(Nt) - beta*sin(Nt)sin(T)...
% + sqrt(1-alpha^2-beta^2)*sin(Nt)cos(T))...
% /(-alpha*sin(Nt) - beta*cos(Nt)sin(T)...
% + sqrt(1-alpha^2-beta^2)*cos(Nt)cos(T)))


%雷达站坐标系下的波束分布

T = thetaT*pi/180;


for n = 1:row
    
    Tx = scan_grid(n,1); % alpha
    Ty = scan_grid(n,2); % beta
    temp = sqrt(1 - Tx^2 - Ty^2);
    
    beamPosition(n,2) = asin(Ty*cos(T) + temp*sin(T));
    beamPosition(n,1) = asin(Tx/(Ty*sin(T)+temp*cos(T)));
    
    
end

beamPosition = rad2deg(beamPosition);
beamPosition = real(beamPosition);



% for n = 1:row
%     subplot(2,2,4)
%     plot(beamPosition(n,1),beamPosition(n,2),'o','LineWidth',1,'MarkerEdgeColor','y','MarkerFaceColor','g','MarkerSize',5);
%     axis([-90 90 0 90]);
%     hold on;
%     
% end
% 
% title(['波位示意图(波位数：',num2str(beamCount),')']);
% xlabel('Az(°)');ylabel('El(°)');

beamPosition = beamPosition';

end

