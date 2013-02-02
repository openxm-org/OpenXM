% $OpenXM$
\name{oxm.matrix_r2tfb}
\alias{oxm.matrix_r2tfb}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
  The function oxm.matrix_r2tfb translates a matrix in R into the tfb format.
}
\description{
  The matrix in R is translated into a string 
  in the tfb (tiny formula book) format.
}
\usage{
oxm.matrix_r2tfb(mat,s="[",s.end="]")
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{mat}{
     A matrix
  }
  \item{s}{
     A string before the body.
  }
  \item{s.end}{
     A string after the body.
  }
}
\details{
  The matrix in R is translated into a string 
  in the tfb (tiny formula book) format.
  The tfb format is used in OpenXM to exchange data in a string.
}
\value{
A string in the tfb format.
}
\references{
 \url{http://www.openxm.org}
}
\author{
%%  ~~who you are~~
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{oxm.markov}}
}
\examples{
mat<-matrix(c(1,0,0,0, 1,0,1,0, 1,0,0,1,
              1,1,0,0, 1,1,1,0, 1,1,0,1),nrow=4,ncol=6)
oxm.matrix_r2tfb(mat)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ tfb }
\keyword{ OpenXM }
