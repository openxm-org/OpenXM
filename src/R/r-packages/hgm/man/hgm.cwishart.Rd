% $OpenXM$
\name{hgm.cwishart}
\alias{hgm.cwishart}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.cwishart evaluates the cumulative distribution function
  of random wishart matrix.
}
\description{
    The function hgm.cwishart evaluates the cumulative distribution function
  of random wishart matrix of size m times m.
}
\usage{
hgm.cwishart(m,n,beta,x0,approxdeg,h,dp,x)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{m}{}
  \item{n}{}
  \item{beta}{
  }
  \item{approxdeg}{
  }
  \item{h}{
  }
  \item{dp}{
  }
  \item{x}{}
}
\details{
  It is evaluated by the Koev-Edelman algorithm near the origin and
  and by the HGM when x is far from the origin.
%  \code{\link[RCurl]{postForm}}.
}
\value{
The output is x, y[0], ...,  y[0] is the value of the cumulative distribution
function at x.  y[1],... are some derivatives.
}
\references{
HNTT, 
\url{http://arxiv.org/abs/??},
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
hgm.cwishart(m=3,n=5,beta=c(1,2,3),x=10)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Cumulative distribution function of random wishart matrix }
\keyword{ Holonomic gradient method }
\keyword{ HGM }

