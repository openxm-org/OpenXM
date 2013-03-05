# $OpenXM: OpenXM/src/R/r-packages/hgm/R/hgm.cwishart.R,v 1.1 2013/02/23 07:00:21 takayama Exp $
"hgm.cwishart" <-
function(m=3,n=5,beta=c(1,2,3),x0=0.2,approxdeg=-1,h=0.01,dp=20,x=10,
         mode=c(1,1,0)) { 
  if (!is.loaded("hgm")) library.dynam("hgm",package="hgm",lib.loc=NULL);
  if (m<1) m=1;
  rank <- 2^m;
  rsize <- rank+1;
  if (mode[3] > 0) rsize <- rsize+mode[3]; 
  if (approxdeg < 0) approxdeg <- max(m*2,6);
  .C("Rmh_cwishart_gen",as.integer(m),as.integer(n),as.double(beta),as.double(x0),
     as.integer(approxdeg),
     as.double(h),
     as.integer(dp),as.double(x),
     as.integer(mode),as.integer(rank),
     retv=double(rsize),PACKAGE="hgm")$retv
}
