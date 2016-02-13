% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.c2wishart.Rd,v 1.2 2016/02/13 07:12:52 takayama Exp $
\name{hgm.p2wishart}
\alias{hgm.p2wishart}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.p2wishart evaluates the cumulative distribution function
  of the largest eigenvalues of W1*inverse(W2).
}
\description{
    The function hgm.p2wishart evaluates the cumulative distribution function
  of the largest eigenvalues of W1*inverse(W2) where W1 and W2 are Wishart
  matrices of size m*m of the freedom n1 and n2 respectively.
}
\usage{
hgm.p2wishart(m,n1,n2,beta,q0,approxdeg,h,dp,q,mode,method,
            err,automatic,assigned_series_error,verbose,autoplot)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{m}{The dimension of the Wishart matrix.}
  \item{n1}{The degree of freedome of the Wishart distribution S1}
  \item{n2}{The degree of freedome of the Wishart distribution S2}
  \item{beta}{The eigenvalues of inverse(S2)*S1 where S1 and S2 are 
    covariant matrices of W1 and W2 respectively.
  }
  \item{q0}{The point to evaluate the matrix hypergeometric series. q0>0}
  \item{approxdeg}{
    Zonal polynomials upto the approxdeg are calculated to evaluate
   values near the origin. A zonal polynomial is determined by a given
   partition (k1,...,km). We call the sum k1+...+km the degree.
  }
  \item{h}{
   A (small) step size for the Runge-Kutta method. h>0.
  }
  \item{dp}{
    Sampling interval of solutions by the Runge-Kutta method.
    When autoplot=1, dp is automatically set.
  }
  \item{q}{
    The second value y[0] of this function is the Prob(L1 < q)
    where L1 is the first eigenvalue of the Wishart matrix.
  }
  \item{mode}{
    When mode=c(1,0,0), it returns the evaluation 
    of the matrix hypergeometric series and its derivatives at q0.
    When mode=c(1,1,(m^2+1)*p), intermediate values of P(L1 < x) with respect to
    p-steps of x are also returned.  Sampling interval is controled by dp.
    When autoplot=1, mode is automatically set.
  }
  \item{method}{
    a-rk4 is the default value. 
    When method="a-rk4", the adaptive Runge-Kutta method is used.
    Steps are automatically adjusted by err.
  }
  \item{err}{
    When err=c(e1,e2), e1 is the absolute error and e2 is the relative error.
    As long as NaN is not returned, it is recommended to set to
    err=c(0.0, 1e-10), because initial values are usually very small. 
  }  
  \item{automatic}{
    automatic=1 is the default value.
    If it is 1, the degree of the series approximation will be increased until 
    |(F(i)-F(i-1))/F(i-1)| < assigned_series_error where
    F(i) is the degree i approximation of the hypergeometric series
    with matrix argument.
    Step sizes for the Runge-Kutta method are also set automatically from
    the assigned_series_error if it is 1.
  }  
  \item{assigned_series_error}{
    assigned_series_error=0.00001 is the default value.
  }  
  \item{verbose}{
    verbose=0 is the default value.
    If it is 1, then steps of automatic degree updates and several parameters
    are output to stdout and stderr.
  }  
  \item{autoplot}{
    autoplot=0 is the default value.
    If it is 1, then it outputs an input for plot.
  }  
}
\details{
  It is evaluated by the Koev-Edelman algorithm when x is near the origin and
  by the HGM when x is far from the origin.
  We can obtain more accurate result when the variables h is smaller,
  q0 is relevant value (not very big, not very small),
  and the approxdeg is more larger.
  A heuristic method to set parameters q0, h, approxdeg properly
  is to make x larger and to check if the y[0] approaches to 1.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is x, y[0], ..., y[2^m] in the default mode,  
y[0] is the value of the cumulative distribution
function P(L1 < x) at x.  y[1],...,y[2^m] are some derivatives.
See the reference below.
}
\references{
H.Hashiguchi, N.Takayama, A.Takemura,
in preparation.
}
\author{
Nobuki Takayama
}
\note{
This function does not work well under the following cases:
1. The beta (the set of eigenvalues)
is degenerated or is almost degenerated.
2. The beta is very skew, in other words, there is a big eigenvalue
and there is also a small eigenvalue.
The error control is done by a heuristic method. 
The obtained value is not validated automatically.
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

%\seealso{
%%%\code{\link{oxm.matrix_r2tfb}}
%}
\examples{
## =====================================================
## Example 1. 
## =====================================================
hgm.p2wishart(m=3,n1=5,n2=10,beta=c(1,2,4),q=4)
## =====================================================
## Example 2. 
## =====================================================
b<-hgm.p2wishart(m=3,n1=5,n2=10,beta=c(1,2,4),q0=0.3,q=20,approxdeg=20,mode=c(1,1,(8+1)*1000));
c<-matrix(b,ncol=8+1,byrow=1);
#plot(c)
## =====================================================
## Example 3. 
## =====================================================
c<-hgm.p2wishart(m=3,n1=5,n2=10,beta=c(1,2,4),q0=0.3,q=20,approxdeg=20,autoplot=1);
#plot(c)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Cumulative distribution function of random two wishart matrices }
\keyword{ Holonomic gradient method }
\keyword{ HGM }

