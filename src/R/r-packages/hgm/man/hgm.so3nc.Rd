% $OpenXM$
\name{hgm.so3nc}
\alias{hgm.so3nc}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.so3nc evaluates the normalization constant for the Fisher
  distribution on SO(3).
}
\description{
    The function hgm.so3nc evaluates the normalization constant for the Fisher
  distribution on SO(3).
}
\usage{
hgm.so3nc(x,y,z,t0=0.0,q=0,deg=0)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{t0}{
     It is the initial point to evaluate the series. If it is set to 0.0,
     a default value is used.
  }
  \item{q}{
     If it is 1, then the program works in a quiet mode.
  }
  \item{deg}{
     It gives the approximation degree of the power series approximation
     of the normalization constant.
     If it is 0, a default value is used.
  }
}
\details{
  A general algorithm to obtain the normalization constant 
  is given in the reference below.
%  please refer to \url{http://www.openxm.org.}
%  The function utilizes \code{\link{oxm.matrix_r2tfb}} and 
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is double.
}
\references{
\url{http://arxiv.org/abs/1110.0721}

Tomonari Sei, Hiroki Shibata, Akimichi Takemura, Katsuyoshi Ohara, Nobuki Takayama,
Properties and applications of Fisher distribution on the rotation group,
arxiv:1110.0721
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
## Example 1. Computing normalization constant of the Fisher distribution on SO(3)
## =====================================================
hgm.so3nc(1,2,3)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }

