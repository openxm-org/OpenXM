% $OpenXM$
\name{hgm.z.mleFBByOptim}
\alias{hgm.z.mleFBByOptim}
%%Todo, write documents for hgm.z.mleDemo, hgm.ssFB
%\alias{hgm.ncso3}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
   MLE of Fisher-Bingham distribution by optim and HGM.
}
\description{
  It makes the maximal likelihood estimate (MLE) for the Fisher-Bingham
  distribution on S^d.
}
\usage{
 hgm.z.mleFBByOptim(d=0,ss=NULL,data=NULL,start=NULL,lb=NULL,ub=NULL,bigValue=10000)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{d}{The dimension of the sphere}
  \item{ss}{Sufficient statistics}
  \item{data}{
     The argument data is a set of data on the d-dimensional sphere.
     Its format is an n by (d+1) matrix where n is the number of data.
     When data is given, ss must be NULL
    and ss is calculated from data by hgm.ssFB(data).
  }
  \item{start}{
     Starting point for the function optim. The default value is a random
     vector.
  }
  \item{lb}{
     An array of length sslen = (d+1)*(d+2)/2 + (d+1), each of which
     is the lower bound of the parameter. The default value is -100.
  }
  \item{ub}{
     An array of length sslen = (d+1)*(d+2)/2 + (d+1), each of which
     is the upper bound of the parameter. The default value is 100.
  }
  \item{bigValue}{
     It is used as a value wall to avoid that the evaluation point is out of
     the search domain defined by lb and ub. 
  }
}
\details{
   It solves the MLE for the Fisher-Bingham distribution.
   The normalizing constant is evaluated by hgm_ko_ncfb (external program,
   which should in the path).
   The function 
%  \code{\link[RCurl]{postForm}}
  \code{\link{optim}}
   is used for the optimization.
   The output is used as a starting point for the holonomic gradient method.
   See nk_fb_gen_c.rr of \url{http://www.math.kobe-u.ac.jp/Asir}. 
   This function generates temporary work files whose names start with tmp.
   \code{data <- read.table(fileName,sep=",")} can be used to read CSV data
   from a file. 
}
\value{
The output format is that of the function optim().
}
\references{
T. Koyama, H. Nakayama, K. Nishiyama, N. Takayama,
Holonomic Gradient Descent for the Fisher-Bingham Distribution on the d-dimensional Sphere,
Computational Statistics (2013)
\url{http://dx.doi.org/10.1007/s00180-013-0456-z}
}
\author{
T.Koyama, H.Nakayama, K.Nishiyama, N.Takayama.
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{optim}}
}
\examples{
## =====================================================
## Example 1. Asteroid data in [N3OST2]
## =====================================================
\dontrun{
  d <- 2
  ss <- c(0.3119,0.0292,0.0707,
                 0.3605,0.0462,
	                    0.3276,
            0.0063,0.0054,0.0762)
  start <- c(0.1,0.1,1,1,1,-1,-1,-1,-1)
  hgm.z.mleFBByOptim(d=d,ss=ss,start=start)
}
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Holonomic gradient method }
\keyword{ HGM }
\keyword{ Fisher-Bingham distribution on S^d}
\keyword{ MLE }

