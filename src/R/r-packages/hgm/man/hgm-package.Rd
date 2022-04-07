%% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm-package.Rd,v 1.16 2020/02/06 05:58:17 takayama Exp $
\name{hgm-package}
\alias{hgm-package}
\alias{HGM}
\alias{hgm}
\docType{package}
\title{
HGM
}
\description{
The holonomic gradient method (HGM, hgm) gives a way to evaluate normalizing
  constants of unnormalized probability distributions by utilizing holonomic 
  systems of differential or difference equations. 
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
This method based on the fact that a broad class of normalizing constants
of unnormalized probability distributions belongs to the class of
holonomic functions, which are solutions of holonomic systems of linear
partial differential equations.
}
\note{
  This package includes a small subset of the Gnu scientific library codes
  (\url{http://www.gnu.org/software/gsl/}).
  Then, it might cause a conflict with the package gsl.
%  (see \code{\link[gsl]{gsl-package}}).
%  When you use the package gsl, it is recommeded to unload the shared libraries
%  of the package hgm by \code{library.dynam.unload("hgm")}<--error, todo.
%  (see \code{\link[base]{library.dynam.unload}}).
}
\references{
\itemize{
\item  (N3OST2)  Hiromasa Nakayama, Kenta Nishiyama, Masayuki Noro, Katsuyoshi Ohara,
Tomonari Sei, Nobuki Takayama, Akimichi Takemura,
Holonomic Gradient Descent  and its Application to Fisher-Bingham Integral,
Advances in Applied Mathematics 47 (2011), 639--658, 
\doi{10.1016/j.aam.2011.03.001}
\item (dojo) Edited by T.Hibi,  Groebner Bases: Statistics and Software Systems, Springer, 2013,
\doi{10.1007/978-4-431-54574-3}
\item  \url{http://www.openxm.org}
}
}
\keyword{ package }
\keyword{ holonomic gradient method}
\keyword{ holonomic gradient descent}
\keyword{ HGM }
\keyword{ HGD }
\seealso{
\code{\link{hgm.ncBingham}},
\code{\link{hgm.ncorthant}},
\code{\link{hgm.ncso3}},
\code{\link{hgm.pwishart}},
\code{\link{hgm.Rhgm}}
\code{\link{hgm.p2wishart}},
}
\examples{
\dontrun{
example(hgm.ncBingham)
example(hgm.ncorthant)
example(hgm.ncso3)
example(hgm.pwishart)
example(hgm.Rhgm)
example(hgm.p2wishart)
}
}
