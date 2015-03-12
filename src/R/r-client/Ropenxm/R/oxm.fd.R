# $OpenXM: OpenXM/src/R/r-client/Ropenxm/R/oxm.fd.R,v 1.1 2014/11/12 01:15:54 takayama Exp $
library("RCurl")

oxm.matrix_r2tfb<-
function(mat=matrix(c(1,2,3,4),nrow=2,ncol=2),s="[",s.end="]") {
  m<-dim(mat)[1];n<-dim(mat)[2];
  for (i in seq(1:m)) {
   s <- paste(s,"[");
   for (j in seq(1,n)) {
     if (j != n) s<-paste(s,mat[i,j],",");
   }
   s<-paste(s,mat[i,n],"]");
   if (i != m) s<-paste(s,",");
  }
  s<-paste(s,s.end);
  return(s);
}

# import("names.rr");import("oh_number.rr"); 
# oh_number.rats(deval(1/17));  --> error, truncation loss
# y should be a matrix of rational numbers. 
# Rational number should be given by a string.
# oxm.ahvec(-20,c(-30,-40,-50),5,matrix(c(1,"1/2","1/3","1/4",1,1,1,1),nrow=2,ncol=4))

oxm.ahvec <-
function(a=-1,b=c(-10,-20),c=5,
   y=matrix(c(1,"1/2","1/3",1,1,1),nrow=2,ncol=3),
   url="http://asir2.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-fd2.sh",
   prec=20) { 
  cmd<-"r_ahvec(";
  cmd<-paste(cmd,a,",",oxm.vector_r2tfb(b),",",c,",",
                                   oxm.matrix_r2tfb(y),"|prec=",prec,")");
#  return(cmd);
  ans<-postForm(url,oxMessageBody=cmd,style="POST");
  pos<-pmatch("c(",c(ans),nomatch=0);

  # security check against that a fake server sends an illegal R command.
  len<-length(grep("[A-Zab]",ans));
  len<-len+length(grep("[dh]",ans));
  len<-len+length(grep("[jk]",ans));
  len<-len+length(grep("[m-r]",ans));
  len<-len+length(grep("[u-z]",ans));

  if ((pos !=0) && (len == 0)) {mar<-eval(parse(text=ans));
  }else {mar<-NA;}
  return(list(zvalue=mar,text=ans));
}

oxm.vector_r2tfb<-
function(vec=c(1,2,3,4)) {
  mm=matrix(vec,nrow=1);
  s = oxm.matrix_r2tfb(mat=mm,s="",s.end="");
  return(s);
}

# 2015.02.28
# $zvalue is the log(Z)
# $zmat is (d_{11} log(Z), d_{12} log(Z), ..., d_{21} log(Z), d_{22} log(Z), ....)
# Note that you need a transpose.
oxm.log_ahmat <-
function(a=-1,b=c(-10,-20),c=5,
   y=matrix(c(1,"1/2","1/3",1,1,1),nrow=2,ncol=3),
   url="http://asir2.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-fd2.sh",
   prec=20) { 
  cmd<-"r_log_ahmat(";
  cmd<-paste(cmd,a,",",oxm.vector_r2tfb(b),",",c,",",
                                   oxm.matrix_r2tfb(y),"|prec=",prec,")");
  ans<-postForm(url,oxMessageBody=cmd,style="POST");
  pos<-pmatch("c(",c(ans),nomatch=0);

  # security check against that a fake server sends an illegal R command.
  len<-length(grep("[A-Zab]",ans));
  len<-len+length(grep("[dh]",ans));
  len<-len+length(grep("[jk]",ans));
  len<-len+length(grep("[m-r]",ans));
  len<-len+length(grep("[u-z]",ans));

  if ((pos !=0) && (len == 0)) {mar<-eval(parse(text=ans));
  }else {mar<-NA;}
  return(list(zvalue=mar[1],zmat=mar[-1],text=ans));
}
