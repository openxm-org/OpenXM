% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.ncorthant.Rd,v 1.5 2015/03/27 02:36:30 takayama Exp $
\name{hgm.ncorthant}
\alias{hgm.ncorthant}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.ncorthant evaluates the orthant probability.
}
\description{
    The function hgm.ncorthant evaluates the orthant probability,
  which is the normalization constant of the multivariate normal distribution
  restrcted to the first orthant.
}
\usage{
hgm.ncorthant(x,y,rk_step_size=1e-3)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{See the description of y.}
  \item{y}{
     This function evaluates the orthant probability for the m dimensional 
     multivariate
     normal distribution whose m by m covariance matrix and the mean vector 
     of size m are x and    y respectively. 
  }
  \item{rk_step_size}{
     The step size for the Runge-Kutta method to apply the HGM.
  }
}
\details{
    The function hgm.ncorthant evaluates the orthant probability,
  which is the normalization constant of the m-dimensional 
  multivariate normal distribution
  restrcted to the first orthant.
  It uses the holonomic gradient method (HGM) to evalute it.
  The rank of the system of differential equations for the HGM is
  2^m.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is the orthant probalibity.
}
\references{
Tamio Koyama, Akimichi Takemura, 
Calculation of orthant probabilities
by the holonomic gradient method,
\url{https://arxiv.org/abs/1211.6822}.
}
\author{
Tamio Koyama
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
## Example 1. Computing the orthant probability
## =====================================================
x <- matrix(c(15,26,23,19,
	      26,47,46,35,
	      23,46,65,38,
	      19,35,38,33), nrow =4) 
y <- c(1,2,3,4)
hgm.ncorthant(x,y)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }
\keyword{ HGM }
\keyword{ Orthant probability}

