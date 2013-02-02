%% $OpenXM$
\name{Ropenxm-package}
\alias{Ropenxm-package}
\alias{Ropenxm}
\docType{package}
\title{
An OpenXM interface for R and its sample uses.
}
\description{
The OpenXM is an infrastructure for mathematical communication. 
The OpenXM proposes protocols for mathematical computation (OpenXM-RFC's). 
This package provides an interface to OpenXM servers and also some sample functions to call OpenXM servers.
}
\details{
\tabular{ll}{
Package: \tab Ropenxm\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2013-02-02\cr
License: \tab BSD\cr
LazyLoad: \tab yes\cr
}
OpenXM servers are called by utilizing the package RCurl through
OpenXM-RFC 104 (OoHP, OpenXM over HTTP POST, 2004).
Translation functions of the data format for OpenXM are implemented in the package.
The function \code{\link{oxm.matrix_r2tfb}} is a typical data translation function.
The function \code{\link{oxm.markov}} is a sample function to get a Markov basis
through an OpenXM server.
}
\author{
Nobuki Takayama (takayama@math.kobe-u.ac.jp)
}
\references{
 \url{http://www.openxm.org}
}
\keyword{ package }
\seealso{
\code{\link{oxm.markov}}
\code{\link{oxm.matrix_r2tfb}}
}
\examples{
\dontrun{
example(oxm.matrix_r2tfb)
example(oxm.markov)
}
}
