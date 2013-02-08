# $OpenXM: OpenXM/src/R/r-packages/hgm/R/hgm.so3nc.R,v 1.1 2013/02/07 07:38:23 takayama Exp $
"hgm.so3nc" <-
function(x=1,y=2,z=3,t0=0.0,q=0,deg=0) { 
#  library.dynam("mypkg",package="mypkg",lib.loc="./");
  if (!is.loaded("hgm")) library.dynam("hgm",package="hgm",lib.loc=NULL);
  .C("so3_main",as.double(x),as.double(y),as.double(z),
     as.double(t0),
     as.integer(q),as.integer(deg),
     retv=as.double(0),PACKAGE="hgm")$retv
}
