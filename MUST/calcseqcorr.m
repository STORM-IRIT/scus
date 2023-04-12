function correl = calcseqcorr(base) 

    load(base+"B_35.mat");
    ref=B;

    correl = zeros(71,1);
    
    for i = 0:70
       name = sprintf(base+"B_%d.mat",i);
       load(name);
       correl(i+1) = corr2(ref,B);
    end
end