# $OpenXM$
"hgm.cwishart" <-
function(m=3,n=5,beta=c(1,2,3),x0=0.3,approxdeg=6,h=0.01,dp=20,x=10) { 
  if (!is.loaded("hgm")) library.dynam("hgm",package="hgm",lib.loc=NULL);
  if (m<1) m=1;
  mode <- 1;
  rank <- 2^m;
  .C("Rmh_cwishart_gen",as.integer(m),as.integer(n),as.double(beta),as.double(x0),
     as.integer(approxdeg),
     as.double(h),
     as.integer(dp),as.double(x),
     as.integer(mode),as.integer(rank),
     retv=double(rank+1),PACKAGE="hgm")$retv
}
