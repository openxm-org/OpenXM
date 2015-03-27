%% $OpenXM: OpenXM/src/R/r-packages/hgm_fb/man/hgm_fb-package.Rd,v 1.1 2015/03/26 06:45:11 takayama Exp $
\name{hgmFB-package}
\alias{hgm_fb-package}
\alias{HGM_FB}
\alias{hgm_fb}
\docType{package}
\title{
HGM
}
\description{
  This package evaluates the normalizing constant for the Fisher-Bingham distributions and solves MLE problems by utilizing the holonomic gradient
method.
}
\details{
\tabular{ll}{
Package: \tab hgm_fb\cr
Type: \tab Package\cr
License: \tab GPL-2\cr
LazyLoad: \tab yes\cr
}
  This package evaluates the normalizing constant for the Fisher-Bingham distributions and solves MLE problems by utilizing the holonomic gradient
method.
The HGM and HGD are proposed in the paper below.
This method based on the fact that a broad class of normalizing constants
of unnormalized probability distributions belongs to the class of
holonomic functions, which are solutions of holonomic systems of linear
partial differential equations.
}
\note{
%  (see \code{\link[gsl]{gsl-package}}).
%  When you use the package gsl, it is recommeded to unload the shared libraries
%  of the package hgm by \code{library.dynam.unload("hgm")}<--error, todo.
%  (see \code{\link[base]{library.dynam.unload}}).
}
\references{
\itemize{
\item  [N3OST2]  Hiromasa Nakayama, Kenta Nishiyama, Masayuki Noro, Katsuyoshi Ohara,
Tomonari Sei, Nobuki Takayama, Akimichi Takemura,
Holonomic Gradient Descent  and its Application to Fisher-Bingham Integral,
Advances in Applied Mathematics 47 (2011), 639--658, 
\url{http://dx.doi.org/10.1016/j.aam.2011.03.001}
\item [dojo] Edited by T.Hibi,  Groebner Bases: Statistics and Software Systems, Springer, 2013,
\url{http://dx.doi.org/10.1007/978-4-431-54574-3}
\item  \url{http://www.openxm.org}
}
}
\keyword{ package }
\keyword{ holonomic gradient method}
\keyword{ holonomic gradient descent}
\keyword{ HGM }
\keyword{ HGD }
\keyword{Fisher-Bingham distribution} 
\seealso{
\code{\link{hgm.z.mleFBByOptim}},
}
\examples{
\dontrun{
}
}
