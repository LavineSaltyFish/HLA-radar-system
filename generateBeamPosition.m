function [beamPosition] = generateBeamPosition(Az,El,beamWidth) % scan_grid(alpha,beta),beamPosition(Az;El)
%% �߽�
bw_alpha = round(sin(beamWidth*pi/180)*1000)/1000;
% theta=beamWidth,phi=0
% alpha = sin(theta)cos(phi)=sin(beamWidth)
% beta = sin(theta)sin(phi)=0

%plane El
thetaT = atan(-(cos(0*pi/180) - cos(El*pi/180))/(sin(0*pi/180) - sin(El*pi/180))*cos(Az*pi/180))*180/pi;
% Nt=����ƫ�Ʒ�λ��=0,T=�������
% alpha = cos(El)sin(Az)cos(Nt)-cos(El)cos(Az)sin(Nt) = 0
% beta = -cos(El)sin(Az)sin(Nt)sin(T)-cos(El)cos(Az)sin(T)=-sin(T)


% ��R El Az��:�״�������ϵ
%  (R theta phi):����ƽ��������ϵ
% ��alpha beta��:���ҿռ�����ϵ

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

% ������ż����������Ҫ�����������ȣ����չ�������ұ�չ����
top = max(beta4);
topDown = min (beta4);

% figure
% subplot(2,2,1)
% plot(alpha1,beta1,'r');
% axis([ left-0.1 right+0.05 bottom-0.05 top+0.1]);
% hold on; grid on;
% title('�״ﲨ�������ҿռ�ı���');xlabel('alpha(rad)');ylabel('beta(rad)');
% plot(alpha2,beta2,'r');
% plot(alpha3,beta3,'r');
% plot(alpha4,beta4,'r');



%% ��λ���У�����ż�м��㣬Բ���ڱ߽����ȥ��
beamCount = 0;
Mx = round((right-left)/bw_alpha);
My = round((top - bottom)/(0.866*bw_alpha));% �������У���������

%----------------------------------������-----------------------------------
y0Counter = 0;


for y0 = bottom+bw_alpha/2: 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0������
    x0Counter = 0;
    for x0 = 0:bw_alpha:right % �Ұ��
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001���� x0����һ������
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0����
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0����
        
        flag = 0;
        % alpha2,beta2:�ұ߽�
        [minValue minSite] = min(abs(beta2 - y0));% �뵱ǰy0������ұ߽��
        if alpha2(minSite)  >= x0 % x0���ұ߽��ڣ����������ҵ�
            flag = 1;
        end
        
        if y0 < bottomUp % ���±߽��ϵĵ�
            % alpha3,beta3:�±߽�
            [minValue minSite] = min(abs(alpha3 - x0)); % �뵱ǰx0������±߽��
            if beta3(minSite) > y0 % y0���±߽�����棨�⣩
                flag = 0;
            end
        end
        if y0 > topDown % ���ϱ߽��ϵĵ�
            % alpha4,beta4:�ϱ߽�
            [minValue minSite] = min(abs(alpha4 - x0));% �뵱ǰx0������ϱ߽��
            if beta4(minSite) + bw_alpha/2 <= y0 % y0���ϱ߽����棨�⣩
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %��������һ����λ
            x0Counter = x0Counter + 1; % x0������
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
    y0Counter = y0Counter + 1; % y0������
    x0Counter = 0;
    for x0 = -bw_alpha:-bw_alpha:left % ����
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001���� x0����һ������
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0����
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0����
        
        % alpha1,beta1:��߽�
        flag = 0;
        [minValue minSite] = min(abs(beta1 - y0)); % �뵱ǰy0�������߽��
        if alpha1(minSite)  <= x0 % x0����߽��Ҳࣨ�ڣ�������Ѱ�ҵ�
            flag = 1;
        end
        
        if y0 < bottomUp % ���±߽��ϵĵ�
            % alpha3,beta3:�±߽�
            [minValue minSite] = min(abs(alpha3 - x0)); % �뵱ǰx0������±߽��
            if beta3(minSite) > y0 % y0���±߽�����棨�⣩
                flag = 0;
            end
        end
        if y0 > topDown % ���ϱ߽��ϵĵ�
            % alpha4,beta4:�ϱ߽�
            [minValue minSite] = min(abs(alpha4 - x0));% �뵱ǰx0������ϱ߽��
            if beta4(minSite) + bw_alpha/2 <= y0 % y0���ϱ߽����棨�⣩
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %��������һ����λ
            x0Counter = x0Counter + 1; % x0������
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



%-----------------------------------ż����----------------------------------
y0Counter = 0;

for y0 = bottom+bw_alpha/2+0.866*bw_alpha : 2*0.866*bw_alpha : top
    y0Counter = y0Counter + 1; % y0������
    x0Counter = 0;
    for x0 = bw_alpha/2:bw_alpha:right % �Ұ��
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001���� x0����һ������
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0����
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0����
        
        flag = 0;
        % alpha2,beta2:�ұ߽�
        [minValue minSite] = min(abs(beta2 - y0));% �뵱ǰy0������ұ߽��
        if alpha2(minSite)  >= x0 % x0���ұ߽��ڣ����������ҵ�
            flag = 1;
        end
        
        if y0 < bottomUp % ���±߽��ϵĵ�
            % alpha3,beta3:�±߽�
            [minValue minSite] = min(abs(alpha3 - x0)); % �뵱ǰx0������±߽��
            if beta3(minSite) > y0 % y0���±߽�����棨�⣩
                flag = 0;
            end
        end
        if y0 > topDown % ���ϱ߽��ϵĵ�
            % alpha4,beta4:�ϱ߽�
            [minValue minSite] = min(abs(alpha4 - x0));% �뵱ǰx0������ϱ߽��
            if beta4(minSite) + bw_alpha/2 <= y0 % y0���ϱ߽����棨�⣩
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %��������һ����λ
            x0Counter = x0Counter + 1; % x0������
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
    y0Counter = y0Counter + 1; % y0������
    x0Counter = 0;
    for x0 = -bw_alpha/2:-bw_alpha:left % ����
        
        x = (-bw_alpha/2:0.001:bw_alpha/2) + x0; % 0.001���� x0����һ������
        y1 = sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0;  % y0����
        y2 = -sqrt((bw_alpha/2)^2 - (x - x0).^2) + y0; % y0����
        
        % alpha1,beta1:��߽�
        flag = 0;
        [minValue minSite] = min(abs(beta1 - y0)); % �뵱ǰy0�������߽��
        if alpha1(minSite)  <= x0 % x0����߽��Ҳࣨ�ڣ�������Ѱ�ҵ�
            flag = 1;
        end
        
        if y0 < bottomUp % ���±߽��ϵĵ�
            % alpha3,beta3:�±߽�
            [minValue minSite] = min(abs(alpha3 - x0)); % �뵱ǰx0������±߽��
            if beta3(minSite) > y0 % y0���±߽�����棨�⣩
                flag = 0;
            end
        end
        if y0 > topDown % ���ϱ߽��ϵĵ�
            % alpha4,beta4:�ϱ߽�
            [minValue minSite] = min(abs(alpha4 - x0));% �뵱ǰx0������ϱ߽��
            if beta4(minSite) + bw_alpha/2 <= y0 % y0���ϱ߽����棨�⣩
                flag = 0;
            end
        end
        
        if flag == 1
            %             beamCountRow = beamCountRow + 1; %��������һ����λ
            x0Counter = x0Counter + 1; % x0������
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
% title(['��λʾ��ͼ(��λ����',num2str(beamCount),')']);
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



%% �ӣ�alpha��theta������ϵת�أ�R��Az��El������ϵ

% El = asin(beta*cosT + sqrt(1-alpha^2-beta^2)*sinT)
% Az = atan((alpha*cos(Nt) - beta*sin(Nt)sin(T)...
% + sqrt(1-alpha^2-beta^2)*sin(Nt)cos(T))...
% /(-alpha*sin(Nt) - beta*cos(Nt)sin(T)...
% + sqrt(1-alpha^2-beta^2)*cos(Nt)cos(T)))


%�״�վ����ϵ�µĲ����ֲ�

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
% title(['��λʾ��ͼ(��λ����',num2str(beamCount),')']);
% xlabel('Az(��)');ylabel('El(��)');

beamPosition = beamPosition';

end

