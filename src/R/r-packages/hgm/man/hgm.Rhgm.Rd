% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.Rhgm.Rd,v 1.1 2014/03/25 02:25:26 takayama Exp $
\name{hgm.Rhgm}
\alias{hgm.Rhgm}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.Rhgm performs the holonomic gradient method (HGM)
   for a given Pfaffian system and an initial value vector.
}
\description{
   The function hgm.Rhgm performs the holonomic gradient method (HGM)
   for a given Pfaffian system and an initial value vector
   with the deSolve package in R.
}
\usage{
 hgm.Rhgm(th0, G0, th1, dG.fun, times=NULL, fn.params=NULL)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{th0}{ A d-dimensional vector which is an initial point of the parameter vector th (theta).}
  \item{G0}{
    A r-dimensional vector which is the initial value of the vector G
    of the normalizing constant and its derivatives.
  }
  \item{th1}{
    A d-dimensional vector which is the target point of th.
  }
  \item{dG.fun}{
     dG.fun is the ``right hand sides'' of the Pfaffian system. 
     It is a d*r-dimensional array.
  }
  \item{times}{a vector; times in [0,1] at which explicit estimates for G are desired.
         If time = NULL, the set {0,1} is used, and only the final value is returned.
  }
  \item{fn.params}{
  fn.params: a list of parameters passed to the function dG.fun.
         If fn.params = NULL, no parameter is passed to dG.fun.
  }
}
\details{
  The function hgm.Rhgm computes the value of a holonomic function
  at a given point, using HGM.
  This is a ``Step 3'' function (see the reference below),
  which can be used for an arbitrary input, in the HGM framework.
  Efficient ``Step 3'' functions are given for some distributions
  in this package.

  The Pfaffian system assumed is
    d G_j / d th_i = (dG.fun(th, G))_{i,j}

  The inputs of hgm.Rhgm are the initial point th0, initial value G0, final point th1,
  and Pfaffian system dG.fun. The output is the final value G1.

  If the argument `times' is specified, the function returns a matrix,
  where the first column denotes time, the following d-vector denotes th,
  and the remaining r-vector denotes G.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is the value of G at th1. The first element of G is the normalizing
constant.
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
# A demo program; von Mises--Fisher on S^{3-1}

G.exact = function(th){  # exact value by built-in function
  c( sinh(th[1])/th[1], cosh(th[1])/th[1] - sinh(th[1])/th[1]^2 )
}

dG.fun = function(th, G, fn.params=NULL){  # Pfaffian
  dG = array(0, c(1, 2))
  sh = G[1] * th[1]
  ch = G[2] * th[1] + G[1]
  dG[1,1] = G[2] # Pfaffian eq's
  dG[1,2] = sh/th[1] - 2*ch/th[1]^2 + 2*sh/th[1]^3
  dG
}

th0 = 0.5
th1 = 15

G0 = G.exact(th0)
G0

G1 = hgm.Rhgm(th0, G0, th1, dG.fun)  # HGM
G1

G1.exact = G.exact(th1)
G1.exact

#
# Example 2.
#
hgm.Rhgm.demo1()

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }
\keyword{ HGM }


