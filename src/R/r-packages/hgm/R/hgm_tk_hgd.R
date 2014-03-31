# $OpenXM$
# This is a wrapper module for the holonomic gradient descent (HGD)
# hgm.mleFBByOptim, hgm.mleDemo
# hgm_nk_opt_fb.R and hgm_ko_ncfb are required.

# FisherBingham on S^d
# ss (sufficient statistics) or data on S^d are necessary.
hgm.mleFBByOptim<-function(d=0,ss=NULL,data=NULL,start=NULL,
                       lb=NULL,ub=NULL,bigValue=10000) {
  sslen <- (d+2)*(d+1)/2 + (d+1);
  if (!identical(data,NULL)) {
    if (!idental(ss,NULL)) return("error: data and ss are exclusive.");
    ss <- hgm.tk.ssFB(d,data)  # get the sufficient statistics.
  }
  print(c(d,ss,sslen));
  if (length(ss) != sslen) return("error: the dimension d and the format ss (sufficient statistics) are not compatible.");
  # Set default values
  # lb, ub define the search domain.
  if (identical(lb,NULL)) lb<-rep(-100,sslen);
  if (identical(ub,NULL)) ub<-rep(100,sslen);
  if (identical(start,NULL)) {
    print("Warning: default random choice of a starting point might lead to a search area where evaluation of the normalizing constant overflows (NaN), check also lb and ub to define the search area.");
#    start<-rep(0,sslen);
    start<-rnorm(sslen);
  }

  # define the cost function  
  # set global variables. Or, use assign and get.
  tk.global.ss <<-ss;
  tk.global.lb <<-lb;
  tk.global.ub <<-ub;
  tk.global.bigValue <<- bigValue;
  return(optim(start,hgm.tk.costFB));
}

hgm.tk.costFB<-function(x) {
     ss<-tk.global.ss;
     lb<-tk.global.lb;
     ub<-tk.global.ub;
     bigValue<-tk.global.bigValue;

     sslen<-length(ss);
     e<-exp(sum(-ss*x));
     v<-hgm.nk.call_tkin(x);  # normalizing constant by hgm_ko_ncfb
     val<-e*v[1];
     print(val); print(x);
     # print("global vars"); print(c(ss,lb,ub,bigValue));
     for (i in 1:length(x)) {
       if (lb[i] > x[i]) return(bigValue);
       if (ub[i] < x[i]) return(bigValue);
     }
     return(val);
}

hgm.tk.test1<-function() {
# Astro data.
  d <- 2;
  ss <- c(0.3119,0.0292,0.0707,
                 0.3605,0.0462,
	                    0.3276,
            0.0063,0.0054,0.0762);
  start <- c(0.1,0.1,1,1,1,-1,-1,-1,-1);
  return(hgm.mleFBByOptim(d=d,ss=ss,start=start));
}

hgm.mleDemo<-function(name) {
  switch(name,
   astro=hgm.nk.optim_astro_f(),
   mag=hgm.nk.optim_mag_f(),
   s3e1=hgm.nk.optim_s3_e1(),
   s3e2=hgm.nk.optim_s3_e2(),
   s3e3=hgm.nk.optim_s3_e3(),
   s4e1=hgm.nk.optim_s4_e1(),
   s4e2=hgm.nk.optim_s4_e2(),
   s4e3=hgm.nk.optim_s4_e3(),
   s5e1=hgm.nk.optim_s5_e1(),
   s5e2=hgm.nk.optim_s5_e2(),
   s5e3=hgm.nk.optim_s5_e3(),
   s6e1=hgm.nk.optim_s6_e1(),
   s6e2=hgm.nk.optim_s6_e2(),
   s6e3=hgm.nk.optim_s6_e3(),
   s7e1=hgm.nk.optim_s7_e1(),
   s7e2=hgm.nk.optim_s7_e2(),
   s7e3=hgm.nk.optim_s7_e3(),
   "Error: unknown example name."
  );
}

# Sufficient Statistics for Fisher-Bingham distribution on S^d
hgm.tk.ssFB<-function(d,data) {
  m <- d+1;
  if (m != length(data[1,])) {
    return("Error: data format error");
  }
  sx <- matrix(rep(0,(m)*(m)), nrow=m);
  sy <- rep(0,m);
  n = length(data[,1]); # sample size
  for (i in 1:m) {
    for (j in i:m) {
      x <- 0;
      for (k in 1:n)  x <- x + data[k,i]*data[k,j];
      x <- x/n;
      sx[i,j] <- x ;
    }
  }
  for (i in 1:m) {
    sy[i] <- sum(data[,i])/n;
  }
#  print(sx);
#  print(sy);
  ss <- rep(0,(m)*(d+2)/2 + (m));
  k <- 1;
  for (i in 1:m) {
    for (j in i:m) {
      ss[k] <- sx[i,j]; k <- k+1;
    }
  }
  for (i in 1:m) {
    ss[k] <- sy[i]; k <- k+1;
  }
  return(ss);
}

hgm.tk.s2data <- function(x1,x2) {
  return(c(x1,x2,sqrt(1-x1^2-x2^2)));
}
hgm.tk.test2 <- function(start=NULL) { 
# a <- read.table("d.txt",sep=",");
  a <- matrix(c(hgm.tk.s2data(0.1,0.2),
                hgm.tk.s2data(0.11,0.19),
                hgm.tk.s2data(0.1,0.21),
                hgm.tk.s2data(0.09,0.21)), nrow=4);
  print(a);
  ss <- hgm.tk.ssFB(2,a);
  print("ss="); print(ss);
  return(hgm.mleFBByOptim(d=2,ss=ss,start=start));
}