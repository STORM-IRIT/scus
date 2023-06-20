function [B, env] = generateImageFromScat(filename)

ImX = 600;
ImY = 500;

param = getparam("LA530");
% param = getparam("l11-5v");
L = param.pitch*(param.Nelements-1);

if param.Nelements==192, Ne = 32; else, Ne = 24; end
param.Nelements = Ne;
l = param.pitch*(Ne-1); % width of a sub-aperture
xsa = ((-L+l)/2:param.pitch:(L-l)/2); % center of the sub-apertures
Nsa = length(xsa); % number of sub-apertures

%-- Pseudorandom distribution of scatterers. The mean distance between two
%   scatterers is lambda_min. Their reflectivity coefficients are
%   calculated from the image I.




[xs,ys,zs,RC] = genscat(filename);
imwidth = max(xs,[],'all')*2;
imheight = max(zs,[],'all');

disp("Creating image")
disp(imwidth)
disp(imheight)

%-- Transmit delays for a focused trasnmit
xf = 0; zf = (imheight/2);
txdel = txdelay(xf,zf,param);
% txdel = zeros(1,Ne);

%-- Simulations of the RF signals with SIMUS 
RF = cell(Nsa,1); % this cell will contain the RF series
param.fs = 4*param.fc; % sampling frequency in Hz
%
option = [];
option.WaitBar = false; % remove the wait bar of SIMUS
option.ParPool = true;
% option.ElementSplitting = 1; % to make things faster
%
h = waitbar(100,"Computing RF");
for k = 1:Nsa
    
    waitbar(k/Nsa,h)
    %--- Scatterers in front of the sub-apertures
    idx = abs(xs-xsa(k))<(l/2);
    %---
    RF{k} = simus(xs(idx)-xsa(k),ys(idx),zs(idx),RC(idx),txdel,param,option);    
end
close(h);

%-- I/Q demodulation of the RF signals
IQ = cell(Nsa,1);  % this cell will contain the I/Q series
for k = 1:Nsa
    IQ{k} = rf2iq(RF{k},param.fs,param.fc);
end
n = min(cellfun(@(x) size(x,1),IQ)); % number of samples in an I/Q

%-- Coordinates of a B-mode scanline
x = zeros(ImX,1);
dz = imheight/ImX;
z = ((dz/2):dz:(imheight-dz/2))';

%-- DAS beamforming
IQb = zeros([ImX Nsa]);  % this array will contain the I/Q image
%
param.fnumber = [];
M = dasmtx(1i*[n Ne],x,z,txdel,param);
h = waitbar(100,"Beamforming");
for k = 1:Nsa
    tmp = IQ{k}(1:n,:);
    waitbar(k/Nsa,h)
    IQb(:,k) = M*tmp(:);    
end
close(h);

Xs = [];
Ys = [];
Zs = [];
RRC = [];
for i = 1:size(xs)
    if(mod(i,2)==0)
       Xs = [Xs xs(i)];  
       Ys = [Ys ys(i)];
       Zs = [Zs zs(i)];
    end
end
RRC = ones(size(Xs));

%-- TGC
RFtgc = simus(Xs,Ys,Zs,RRC,txdel,param,option);
IQtgc = rf2iq(RFtgc,param.fs,param.fc);
IQtgc = IQtgc(1:n,:);
IQbtgc = M*IQtgc(:);
TGC = smoothn(abs(IQbtgc),1e6);
TGC = TGC/max(TGC);

%-- Compound B-mode image
DR = 35; % dynamic range (in dB)
B = bmode(IQb./TGC,DR);
env = IQb./TGC;


%-- Resize the images
B = resize(B,[ImX ImY]);
save("B.mat","B");
imshow(B);
end
