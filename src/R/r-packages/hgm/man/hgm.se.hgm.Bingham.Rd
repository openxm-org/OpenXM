% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.se.hgm.Bingham.Rd,v 1.5 2015/03/26 11:54:13 takayama Exp $
\name{hgm.ncBingham}
\alias{hgm.ncBingham}
%\alias{hgm.se.hgm.Bingham}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.ncBingham performs the holonomic gradient method (HGM)
   for Bingham distributions.
}
\description{
   The function hgm.ncBingham performs the holonomic gradient method (HGM)
   for Bingham distributions with the deSolve package in R.
}
\usage{
 hgm.ncBingham(th, d=rep(1,length(th)+1), logarithm=FALSE,
               ini.method="power", times=NULL, withvol=FALSE, ...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{th}{ A (p-1)-dimensional vector which specifies the first (p-1) components of the parameter vector of the Bingham distribution on the (p-1)-dim sphere. The p-th parameter is assumed to be zero.}
  \item{d}{
     A p-dimensional vector which specifies the multiplicity of the parameter. The default is all-one vector.
  }
  \item{logarithm}{
     If 'logarithm' is TRUE, then the result is log of the normalizing constant.
  }
  \item{ini.method}{
     The method for computing the initial value. Only "power" is implemented now.
  }
  \item{times}{
     a vector; times in [0,1] at which explicit estimates for G are desired.
     If time = NULL, the set {0,1} is used, and only the final value is returned.
  }
  \item{withvol}{
     If 'withvol' is TRUE, then the normalizing constant with volume of sphere is returned.
     Otherwise that without volume is returned.
     Therefore, if 'withvol' is FALSE and the parameter is zero, then the normalizing constant becomes 1.
  }
  \item{...}{
     Additional parameters for computing initial values. Details are omitted.
  }
}
\details{
  The function hgm.ncBingham computes the normalizing constant
  of the Bingham distribution and its derivatives at any specified point.
  The initial value is computed by the power series expansion.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is p-dimensional vector G.
The first element of G is the normalizing constant
and the following (p-1)-elements are partial derivative
of the normalizing constant with respect to the first
(p-1) components of the parameter 'th'.
}
\references{
\url{http://www.math.kobe-u.ac.jp/OpenXM/Math/hgm/ref-hgm.html}
}
\author{
Tomonari Sei
}
%\note{
%%%  ~~further notes~~
%}

%% ~Make other sections like Warning with \section{Warning }{....} ~

%\seealso{
%%%\code{\link{oxm.matrix_r2tfb}}
%}
\examples{
# Example 1.
hgm.ncBingham(c(1,3,5))
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }
\keyword{ HGM }


