#$OpenXM: OpenXM/src/R/r-client/Ropenxm/R/oxm.markov.R,v 1.4 2013/02/02 05:16:44 takayama Exp $
oxm.markov <-
function(mat=matrix(c(1,2,3),nrow=1,ncol=3),
   url="http://polymake.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-markov.sh") { 
  cmd<-oxm.matrix_r2tfb(mat,s="r_markov([",s.end="])");
  ans<-postForm(url,oxMessageBody=cmd,style="POST");
  pos<-pmatch("list(",c(ans),nomatch=0);

  # security check against that a fake server sends an illegal R command.
  len<-length(grep("[A-Zab]",ans));
  len<-len+length(grep("[d-h]",ans));
  len<-len+length(grep("[jk]",ans));
  len<-len+length(grep("[m-r]",ans));
  len<-len+length(grep("[u-z]",ans));

  if ((pos !=0) && (len == 0)) {mar<-eval(parse(text=ans));
  }else {mar<-NA;}
  return(list(markov=mar,text=ans));
}

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
