% $OpenXM: OpenXM/src/R/r-packages/hgm/man/hgm.Rhgm.demo1.Rd,v 1.1 2015/03/24 07:49:06 takayama Exp $
\name{hgm.Rhgm.demo1}
\alias{hgm.Rhgm.demo1}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    The function hgm.Rhgm.demo1 performs a demonstration of the function
    hgm.Rhgm.
}
\description{
   The function hgm.Rhgm.demo1 performs a demonstration of the function
    hgm.Rhgm.
}
\usage{
 hgm.Rhgm.demo1()
}
%- maybe also 'usage' for other objects documented here.
%\arguments{
%}
\details{
  The function hgm.Rhgm.demo1 evaluates the normalizing constant
  of the Von-Mises distribution by the HGM.
}
\value{
The returned value is a dataframe.
The column exact of the dataframe is the exact value of the normalizing
constant. The column byHGM is the value obtained by the HGM.
The column start is the initial value for the HGM.
}
%\references{
%}
\author{
Tomonari Sei
}
%\note{
%%%  ~~further notes~~
%}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
\code{\link{hgm.Rhgm}}
}
%\examples{
%}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ Normalization constant }
\keyword{ Holonomic gradient method }
\keyword{ HGM }


