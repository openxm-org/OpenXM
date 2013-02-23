% $OpenXM: OpenXM/src/R/r-client/Ropenxm/man/oxm.markov.Rd,v 1.2 2013/02/02 05:16:44 takayama Exp $
\name{oxm.markov}
\alias{oxm.markov}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
  The function oxm.markov obtains the Markov basis for a given set of points by 4ti2 through an OpenXM server.
}
\description{
  The function oxm.markov obtains the Markov basis for a given set of points by 4ti2 through an OpenXM server.
  It is provided also for a sample implementation to call OpenXM servers from R.
}
\usage{
oxm.markov(mat,url="http://asir.math.kobe-u.ac.jp/cgi-bin/cgi-asir-r-markov.sh")
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{mat}{
     A matrix which represents a set of integer points. 
     Each column of the matrix is each point of the set.
  }
  \item{url}{
    This argument specifies the URL of an OpenXM server.  
  The default is an anonymous server in public set by the openxm.org. 
  Note that all inputs are recorded in the server 
  and the privacy policy of the public server declares 
  that the record is used only for debugging and for security tasks.  
  You can specify your own server.
  }
}
\details{
  A general algorithm to obtain the Markov basis for a set of points
  was given in
  "P.Diaconis and B.Sturmfels, Algebraic algorithms for sampling from
   conditional distributions, Annals of Statistics, 26, (1998) 363--397".
  4ti2 is an efficient implementation of this algorithm.
  The function oxm.markov calls the 4ti2 through an OpenXM server.
  In order to set up your own server, 
  please refer to \url{http://www.openxm.org.}
  The function utilizes \code{\link{oxm.matrix_r2tfb}} and 
  \code{\link[RCurl]{postForm}}.
}
\value{
The output is given by a list with the following named entries.
  \item{markov}{Markov basis in a list.}
  \item{text}{The string returned from the server. It is used to get errors.}
}
\references{
  \url{http://www.4ti2.de}

  P.Diaconis and B.Sturmfels (1998), Algebraic algorithms for sampling from
  conditional distributions, Annals of Statistics, 26, 363--397.
}
\author{
%%
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{oxm.matrix_r2tfb}}
}
\examples{
## =====================================================
## Example 1. Markov basis of 2 x 3 contingency table.
## =====================================================
mat<-matrix(c(1,0,0,0, 1,0,1,0, 1,0,0,1,
              1,1,0,0, 1,1,1,0, 1,1,0,1),nrow=4,ncol=6)
mb<-oxm.markov(mat)
mb$markov
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Markov basis }
\keyword{ 4ti2 }
\keyword{ OpenXM }
