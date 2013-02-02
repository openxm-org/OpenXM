oxm.markov <-
function(mat=matrix(c(1,2,3),nrow=1,ncol=3),
   url="http://polymake.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-markov.sh") { 
  cmd<-oxm.matrix_r2asir(mat,s="r_markov([",s.end="])");
  ans<-postForm(url,oxMessageBody=cmd,style="POST");
  mar<-eval(parse(text=ans));
  return(list(markov=mar,text=ans));
}

oxm.matrix_r2asir<-
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
