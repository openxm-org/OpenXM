%% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm-package.Rd,v 1.7 2014/03/25 02:25:26 takayama Exp $
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
License: \tab GPL-2\cr
LazyLoad: \tab yes\cr
}
The HGM and HGD are proposed in the paper below.
This method based on the fact that a broad class of normalization constants
of unnormalized probability distributions belongs to the class of
holonomic functions, which are solutions of holonomic systems of linear
partial differential equations.
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
\code{\link{hgm.ncBingham}}
\code{\link{hgm.ncorthant}}
\code{\link{hgm.ncso3}}
\code{\link{hgm.pwishart}}
\code{\link{hgm.Rhgm}}
}
\examples{
\dontrun{
example(hgm.ncBingham)
example(hgm.ncorthant)
example(hgm.ncso3)
example(hgm.pwishart)
example(hgm.Rhgm)
}
}
