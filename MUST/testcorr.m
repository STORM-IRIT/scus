corr = [];
uncorr = [];

for i = 1:3
    corr = [corr calcseqcorr(sprintf("corr%i/",i))];
end
corr = corr';

for i = 1:2
    uncorr = [uncorr calcseqcorr(sprintf("uncorr%i/",i))];
end
uncorr = uncorr';
