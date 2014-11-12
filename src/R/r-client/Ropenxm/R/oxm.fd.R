# $OpenXM$
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
   url="http://asir2.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-fd.sh") { 
  cmd<-"r_ahvec(";
  cmd<-paste(cmd,a,",",oxm.vector_r2tfb(b),",",c,",",
                                   oxm.matrix_r2tfb(y),")");
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
