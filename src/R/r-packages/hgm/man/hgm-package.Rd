%% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm-package.Rd,v 1.3 2013/02/08 02:59:42 takayama Exp $
\name{hgm-package}
\alias{hgm-package}
\alias{HGM}
\alias{hgm}
\docType{package}
\title{
HGM
}
\description{
The holonomic gradient method (HGM, hgm) gives a way to evaluate normalization
  constants of unnormalized probability distributions by utilizing holonomic 
  systems of differential equations. 
  The holonomic gradient descent (HGD, hgd) gives a method
  to find maximal likelihood estimates by utilizing the HGM.
}
\details{
\tabular{ll}{
Package: \tab hgm\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2013-02-07\cr
License: \tab GPL-2\cr
LazyLoad: \tab yes\cr
}
The HGM and HGD are proposed in the paper below.
This method based on the fact that a broad class of normalization constants
of unnormalized probability distributions belongs to the class of
holonomic functions, which are solutions of holonomic systems of linear
partial differential equations.
}
\author{
\itemize{
\item Nobuki Takayama (takayama@math.kobe-u.ac.jp)
}
}
\note{
  This package includes a small subset of the Gnu scientific library codes
  (\url{http://www.gnu.org/software/gsl/}).
  Then, it might cause a conflict with the package gsl
  (see \code{\link[gsl]{gsl-package}}).
%  When you use the package gsl, it is recommeded to unload the shared libraries
%  of the package hgm by \code{library.dynam.unload("hgm")}<--error, todo.
%  (see \code{\link[base]{library.dynam.unload}}).
}
\references{
\itemize{
\item  Hiromasa Nakayama, Kenta Nishiyama, Masayuki Noro, Katsuyoshi Ohara,
  Tomonari Sei, Nobuki Takayama, Akimichi Takemura,
Holonomic Gradient Descent  and its Application to Fisher-Bingham Integral,
Advances in Applied Mathematics 47 (2011), 639--658, 
\url{http://dx.doi.org/10.1016/j.aam.2011.03.001}

\item  \url{http://www.openxm.org}
}
}
\keyword{ package }
\keyword{ holonomic gradient method}
\keyword{ holonomic gradient descent}
\keyword{ HGM }
\keyword{ HGD }
\seealso{
\code{\link{hgm.so3nc}}
\code{\link{hgm.cwishart}}
}
\examples{
\dontrun{
example(hgm.so3nc)
example(hgm.cwishart)
}
}
