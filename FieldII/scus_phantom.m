function [positions, amp] = scus_phantom()
load raw_res.dat

z_start = 30/1000;

x = raw_res(:,1)/1000;
y = raw_res(:,2)/1000;
z = raw_res(:,3)/1000 + z_start;
amp = raw_res(:,4);

positions = [x y z];