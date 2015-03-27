% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.so3nc.Rd,v 1.5 2015/03/22 00:27:51 takayama Exp $
\name{hgm.ncso3}
\alias{hgm.ncso3}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.ncso3 evaluates the normalization constant for the Fisher
  distribution on SO(3).
}
\description{
    The function hgm.ncso3 evaluates the normalization constant for the Fisher
  distribution on SO(3).
}
\usage{
hgm.ncso3(a,b,c,t0=0.0,q=1,deg=0)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{a}{See the description of c.}
  \item{b}{See the description of c.}
  \item{c}{
     This function evaluates the normalization constant for the parameter
     Theta=diag(theta_{ii}) of the Fisher distribution on SO(3).
     The variables a,b,c stand for the parameters
     theta_{11}, theta_{22}, theta_{33} respectively.
  }
  \item{t0}{
     It is the initial point to evaluate the series. If it is set to 0.0,
     a default value is used.
  }
  \item{q}{
     If it is 1, then the program works in a quiet mode.
  }
  \item{deg}{
     It gives the approximation degree of the power series approximation
     of the normalization constant near the origin.
     If it is 0, a default value is used.
  }
}
\details{
  The normalization constant c(Theta) 
  of the Fisher distribution on SO(3) is defined  by
  integral( exp(trace( transpose(Theta) X)) )
  where X is the integration variable and runs over S0(3) and
  Theta is a 3 x 3 matrix parameter.
  A general HGM algorithm to evaluate the normalization constant 
  is given in the reference below.
  We use the Corollary 1 and the series expansion in 3.2 for the evaluation.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is c(Theta).
}
\references{
Tomonari Sei, Hiroki Shibata, Akimichi Takemura, Katsuyoshi Ohara, Nobuki Takayama,
Properties and applications of Fisher distribution on the rotation group,
Journal of Multivariate Analysis, 116 (2013), 440--455,
\url{http://dx.doi.org/10.1016/j.jmva.2013.01.010}
}
\author{
Nobuki Takayama
}
%\note{
%%%  ~~further notes~~
%}

%% ~Make other sections like Warning with \section{Warning }{....} ~

%\seealso{
%%%\code{\link{oxm.matrix_r2tfb}}
%}
\examples{
## =====================================================
## Example 1. Computing normalization constant of the Fisher distribution on SO(3)
## =====================================================
hgm.ncso3(1,2,3)

## =====================================================
## Example 2. Asteroid data in the paper
## =====================================================
hgm.ncso3(19.6,0.831,-0.671)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }
\keyword{ HGM }
\keyword{ Fisher distribution on SO(3)}

