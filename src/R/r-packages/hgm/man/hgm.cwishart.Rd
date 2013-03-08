% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.cwishart.Rd,v 1.2 2013/03/01 05:27:08 takayama Exp $
\name{hgm.cwishart}
\alias{hgm.cwishart}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.cwishart evaluates the cumulative distribution function
  of random wishart matrix.
}
\description{
    The function hgm.cwishart evaluates the cumulative distribution function
  of random wishart matrix of size m times m.
}
\usage{
hgm.cwishart(m,n,beta,x0,approxdeg,h,dp,x,mode,method,err)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{m}{The dimension of the Wishart matrix.}
  \item{n}{The degree of freedome (a parameter of the Wishart distribution)}
  \item{beta}{The eigenvalues of the inverse of the covariant matrix /2
(a parameter of the Wishart distribution).
    The beta is equal to inverse(sigma)/2.
  }
  \item{x0}{The point to evaluate the matrix hypergeometric series. x0>0}
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
  \item{x}{
    The second value y[0] of this function is the Prob(L1 < x)
    where L1 is the first eigenvalue of the Wishart matrix.
  }
  \item{mode}{
    When mode=c(1,0,0), it returns the evaluation 
    of the matrix hypergeometric series and its derivatives at x0.
    When mode=c(1,1,(m^2+1)*p), intermediate values of P(L1 < x) with respect to
    p-steps of x are also returned.  Sampling interval is controled by dp.
  }
  \item{method}{
    rk4 is the default value. 
    When method="a-rk4", the adaptive Runge-Kutta method is used.
    Steps are automatically adjusted by err.
  }
  \item{err}{
    When err=c(e1,e2), e1 is the absolute error and e2 is the relative error.
    As long as NaN is not returned, it is recommended to set to
    err=c(0.0, 1e-10), because initial values are usually very small. 
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
Holonomic gradient method for the distribution function of the largest root of a Wishart matrix
\url{http://arxiv.org/abs/1201.0472},
}
\author{
Nobuki Takayama
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%%\code{\link{oxm.matrix_r2tfb}}
}
\examples{
## =====================================================
## Example 1. 
## =====================================================
hgm.cwishart(m=3,n=5,beta=c(1,2,3),x=10)
## =====================================================
## Example 2. 
## =====================================================
b<-hgm.cwishart(m=4,n=10,beta=c(1,2,3,4),x0=1,x=10,approxdeg=20,mode=c(1,1,(16+1)*100));
c<-matrix(b,ncol=16+1,byrow=1);
#plot(c)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Cumulative distribution function of random wishart matrix }
\keyword{ Holonomic gradient method }
\keyword{ HGM }

