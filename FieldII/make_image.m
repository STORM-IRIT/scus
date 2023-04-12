%  Compress the data to show 60 dB of
%  dynamic range for the cyst phantom image
%
%  version 1.3 by Joergen Arendt Jensen, April 1, 1998.
%  version 1.4 by Joergen Arendt Jensen, August 13, 2007.
%          Calibrated 60 dB display made

f0=3.0e6;                 %  Transducer center frequency [Hz]
fs=4*f0;                 %  Sampling frequency [Hz]
c=1540;                   %  Speed of sound [m/s]
no_lines=161;              %  Number of lines in image
image_width=50/1000;      %  Size of image sector
d_x=image_width/no_lines; %  Increment for image

%  Read the data and adjust it in time 


disp('Finding the envelope')
min_sample=0;
for i=1:no_lines

  %  Load the result

  cmd=['load rf_data/rf_ln',num2str(i),'.mat'];
  disp(cmd)
  eval(cmd)
  
  %  Find the envelope
  
  rf_env=abs(hilbert([zeros(round(tstart*fs-min_sample),1); rf_data]));
  env( 1:max(size(rf_env)) ,i)=rf_env;
  end

save("envtst.mat","env");

%  Do logarithmic compression

D=3;   %  Sampling frequency decimation factor

disp('Log Compression')
log_env=env(1:D:max(size(env)),:)/max(max(env));
log_env=20*log10(log_env);
log_env=127/60*(log_env+60);

%  Make an interpolated image

disp('Doing interpolation')
ID=20;
[n,m]=size(log_env);
new_env=zeros(n,m*ID);

for i=1:n
  new_env(i,:)=abs(interp(log_env(i,:),ID));
end

[n,m]=size(new_env);
  
fn=fs/D;
clf
a=((1:(ID*no_lines-1))*d_x/ID-no_lines*d_x/2)*1000;
b=((1:n)/fn+min_sample/fs)*c/2*1000;
image(a,b,new_env)
xlabel('Lateral distance [mm]')
ylabel('Axial distance [mm]')
colormap(gray(127))
axis('image')
axis([-image_width*500 image_width*500 35 90])

