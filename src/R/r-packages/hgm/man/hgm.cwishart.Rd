% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.cwishart.Rd,v 1.9 2015/03/26 11:54:13 takayama Exp $
\name{hgm.pwishart}
\alias{hgm.pwishart}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.pwishart evaluates the cumulative distribution function
  of random wishart matrices.
}
\description{
    The function hgm.pwishart evaluates the cumulative distribution function
  of random wishart matrices of size m times m.
}
\usage{
hgm.pwishart(m,n,beta,q0,approxdeg,h,dp,q,mode,method,
            err,automatic,assigned_series_error,verbose)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{m}{The dimension of the Wishart matrix.}
  \item{n}{The degree of freedome (a parameter of the Wishart distribution)}
  \item{beta}{The eigenvalues of the inverse of the covariant matrix /2
(a parameter of the Wishart distribution).
    The beta is equal to inverse(sigma)/2.
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
  }
  \item{q}{
    The second value y[0] of this function is the Prob(L1 < q)
    where L1 is the first eigenvalue of the Wishart matrix.
  }
  \item{mode}{
    When mode=c(1,0,0), it returns the evaluation 
    of the matrix hypergeometric series and its derivatives at x0.
    When mode=c(1,1,(m^2+1)*p), intermediate values of P(L1 < x) with respect to
    p-steps of x are also returned.  Sampling interval is controled by dp.
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
}
\details{
  It is evaluated by the Koev-Edelman algorithm when x is near the origin and
  by the HGM when x is far from the origin.
  We can obtain more accurate result when the variables h is smaller,
  x0 is relevant value (not very big, not very small),
  and the approxdeg is more larger.
  A heuristic method to set parameters x0, h, approxdeg properly
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
H.Hashiguchi, Y.Numata, N.Takayama, A.Takemura,
Holonomic gradient method for the distribution function of the largest root of a Wishart matrix,
Journal of Multivariate Analysis, 117, (2013) 296-312, 
\url{http://dx.doi.org/10.1016/j.jmva.2013.03.011},
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
hgm.pwishart(m=3,n=5,beta=c(1,2,3),q=10)
## =====================================================
## Example 2. 
## =====================================================
b<-hgm.pwishart(m=4,n=10,beta=c(1,2,3,4),q0=1,q=10,approxdeg=20,mode=c(1,1,(16+1)*100));
c<-matrix(b,ncol=16+1,byrow=1);
#plot(c)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Cumulative distribution function of random wishart matrix }
\keyword{ Holonomic gradient method }
\keyword{ HGM }

