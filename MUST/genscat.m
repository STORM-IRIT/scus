function [x,y,z,a] = genscat(filename)
scats = importdata(filename);
%z_start = 30/1000;

x = scats(:,1)/1000;
y = scats(:,2)/1000;
z = scats(:,3)/1000;
a = scats(:,4);

end