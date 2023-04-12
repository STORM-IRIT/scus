MSEs = [];
NMSEs = [];

X = [1 2 3 4 5 6 7 8 9 10 15 20 27 64 125 343];

for i = X
    load("enveloppes/uniform/" + int2str(i));

    envcrop = env(1100:3000,2:48);
    envcropf = reshape(envcrop.',1,[]); 
    envcropf = (envcropf-min(envcropf))/(max(envcropf)-min(envcropf));

    h = histfit(envcropf,64,"rayleigh");
    histdata = get(h(1),'YData');      
    redcurve = get(h(2),'YData');

    xgrid64 = linspace(0,1,64);
    xgrid100 = linspace(0,1,100);
    
    
    %redcurve2 = redcurve;
    %redcurve2 = interp1(xgrid100,redcurve,xgrid64);
    redcurve2 = [mean(reshape(redcurve, 2, [])) zeros(1,14)];     % Mean Of Bin Edges Fit
    fit_err = histdata - redcurve2;                 % Error
    
    mse=norm(redcurve2-histdata,2)^2/length(redcurve2);
    sigEner=norm(redcurve2)^2;
    nmse=(mse/sigEner);
    
    NMSEs = [NMSEs nmse];
    MSEs = [MSEs mse];
    
end



