load raw_res.dat
z_start = 30/1000;

x = raw_res(:,1)/1000;
y = raw_res(:,2)/1000;
z = raw_res(:,3)/1000;
amp = raw_res(:,4);

zoneWidth = 50e-3;
zoneHeight = 60e-3;

param = getparam("LA530");
param.fs = 20*param.fc;

xf = 0;
zf = 300e-2;

txdel = txdelay(xf,zf,param);

RF = simus(x,y,z,amp,txdel,param);

IQ = rf2iq(RF,param.fs,param.fc);

xi = linspace(-zoneWidth/2,zoneWidth/2,512); % in m
zi = linspace(0,zoneHeight,512); % in m
[xi,zi] = meshgrid(xi,zi);

param.fnumber = [];
IQb = das(IQ,xi,zi,txdel,param); % beamformed I/Q

DR = 50;
B = bmode(IQb,DR); % log-compressed image
imagesc(xi(1,:)*1e2,zi(:,1)*1e2,B)
c = colorbar;
c.YTick = [0 255];
c.YTickLabel = {"-"+int2str(DR)+" dB",'0 dB'};
colormap gray
axis equal tight
xlabel('[cm]'), ylabel('[cm]')