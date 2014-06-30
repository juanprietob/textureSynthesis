load cities

stdr = std(ratings);
sr = ratings./repmat(stdr,329,1);

%boxplot(sr,'orientation','horizontal','labels',categories);

covmat = cov(sr);
[eigenvect, eigenval] = eig(covmat);

[coefs,scores,variances,t2] = princomp(sr);

c3 = coefs(:,1:3);