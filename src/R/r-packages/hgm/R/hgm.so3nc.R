# $OpenXM: OpenXM/src/R/r-packages/hgm/R/hgm.so3nc.R,v 1.2 2013/02/08 00:03:42 takayama Exp $
"hgm.so3nc" <-
function(a=1,b=2,c=3,t0=0.0,q=0,deg=0) { 
#  library.dynam("mypkg",package="mypkg",lib.loc="./");
  if (!is.loaded("hgm")) library.dynam("hgm",package="hgm",lib.loc=NULL);
  .C("so3_main",as.double(a),as.double(b),as.double(c),
     as.double(t0),
     as.integer(q),as.integer(deg),
     retv=as.double(0),PACKAGE="hgm")$retv
}
