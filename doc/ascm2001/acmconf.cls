%%$OpenXM$
%% WARNING!!  This file is temporary put at our CVS server, but it is only
%% for committer's PRIVATE use. It is not allowed to redistributed 
%% this file from
%% our cvs server to anonymous public.
%% This file will be deleted after we finish our paper.

%%
%% This is file `acmconf.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% acmconf.dtx  (with options: `class')
%% 
%% IMPORTANT NOTICE:
%% 
%% For the copyright see the source file.
%% 
%% You are *not* allowed to modify this file.
%% 
%% You are *not* allowed to distribute this file.
%% For distribution of the original source see the terms
%% for copying and modification in the file acmconf.dtx.
%% 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% `acmconf' class to use with LaTeX2e.
%%
%% This class is used to typeset articles to be published in the proceedings
%% of the ACM (Association for Computing Machinery) conferences and workshops.
%%
%% Copyright (C) 1999, Dr. Juergen Vollmer
%%                     Viktoriastrasse 15, D-76133 Karlsruhe, Germany
%%                     Juergen.Vollmer@acm.org
%% License:
%%   This program can be redistributed and/or modified under the terms
%%   of the LaTeX Project Public License Distributed from CTAN
%%   archives in directory macros/latex/base/lppl.txt; either
%%   version 1 of the License, or any later version.
%%
%% If you find this software useful, please send me a postcard.
%%
%% $Id$
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{acmconf}[1999/06/16 v1.1 ACM Conference Papers]
\newif\if@AcmConfPrepare@
\newif\if@AcmConfSubmit@
\newif\if@AcmConfAccept@
\newif\if@AcmConfPublish@
\newif\if@AcmConfPrint@
\DeclareOption{prepare}{
  \@AcmConfPrepare@true
  \@AcmConfSubmit@false
  \@AcmConfAccept@false
  \@AcmConfPublish@false
  \@AcmConfPrint@false
}
\DeclareOption{submit}{
  \@AcmConfPrepare@false
  \@AcmConfSubmit@true
  \@AcmConfAccept@false
  \@AcmConfPublish@false
  \@AcmConfPrint@false
}
\DeclareOption{accept}{
  \@AcmConfPrepare@false
  \@AcmConfSubmit@false
  \@AcmConfAccept@true
  \@AcmConfPublish@false
  \@AcmConfPrint@false
}
\DeclareOption{publish}{
  \@AcmConfPrepare@false
  \@AcmConfSubmit@false
  \@AcmConfAccept@false
  \@AcmConfPublish@true
  \@AcmConfPrint@false
}
\DeclareOption{print}{
  \@AcmConfPrepare@false
  \@AcmConfSubmit@false
  \@AcmConfAccept@false
  \@AcmConfPublish@false
  \@AcmConfPrint@true
}
\DeclareOption{10pt}{
  \ClassWarningNoLine{acmconf}{%
    The `10pt' option is not allowed in the `acmconf' class}
  \OptionNotUsed
}
\DeclareOption{11pt}{
  \ClassWarningNoLine{acmconf}{%
    The `11pt' option is not allowed in the `acmconf' class}
  \OptionNotUsed
}
\DeclareOption{12pt}{
  \ClassWarningNoLine{acmconf}{%
    The `12pt' option is not allowed in the `acmconf' class}
  \OptionNotUsed
}
\DeclareOption{landscape}{
  \ClassWarningNoLine{acmconf}{%
    The `landscape' option is not allowed in the `acmconf' class}
  \OptionNotUsed
}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ExecuteOptions{prepare}
\ProcessOptions\relax
\LoadClass[twocolumn]{article}
\RequirePackage{flushend}
\newcommand{\IfPrepare}[2]{\if@AcmConfPrepare@#1\else#2\fi}
\newcommand{\IfSubmit}[2]{\if@AcmConfSubmit@#1\else#2\fi}
\newcommand{\IfAccept}[2]{\if@AcmConfAccept@#1\else#2\fi}
\newcommand{\IfPublish}[2]{\if@AcmConfPublish@#1\else#2\fi}
\newcommand{\IfPrint}[2]{\if@AcmConfPrint@#1\else#2\fi}
\def\@AcmConfConferenceName{}
\def\@AcmConfConferenceShortName{}
\def\@AcmConfPublishedPageFrom{}
\def\@AcmConfPublishedPageTo{}
\newcommand{\ConferenceName}[1]{
  \def\@AcmConfConferenceName{#1}
}
\newcommand{\TheConferenceName}{%
  \@AcmConfConferenceName
}
\newcommand{\ConferenceShortName}[1]{
  \def\@AcmConfConferenceShortName{#1}
}
\newcommand{\TheConferenceShortName}{
  \@AcmConfConferenceShortName
}
\newcommand{\PublishedPageFrom}[1]{
  \def\@AcmConfPublishedPageFrom{#1}
}
\newcommand{\PublishedPageTo}[1]{
  \def\@AcmConfPublishedPageTo{#1}
}
\def\@AcmConfPrepareText{
  Intended for submission to the \emph{\@AcmConfConferenceName}
}
\def\@AcmConfSubmitText{
  Submitted to the \emph{\@AcmConfConferenceName}
}
\def\@AcmConfAcceptText{
  Accepted for the \emph{\@AcmConfConferenceName}
}
\def\@AcmConfPrintText{
  Published in the Proceedings of the \emph{\@AcmConfConferenceName},
  pages \@AcmConfPublishedPageFrom--\@AcmConfPublishedPageTo
}
\def\@AcmConfCopyrightText{}
\newcommand{\PrepareText}[1]{\def\@AcmConfPrepareText{#1}}
\newcommand{\SubmitText}[1]{\def\@AcmConfSubmitText{#1}}
\newcommand{\AcceptText}[1]{\def\@AcmConfAcceptText{#1}}
\newcommand{\PrintText}[1]{\def\@AcmConfPrintText{#1}}
\newcommand{\CopyrightText}[1]{\def\@AcmConfCopyrightText{#1}}
\newcommand{\@AcmConfCopyrightSpace}{
  \def\ftype@AcmConfCopyrightBox{8}
  \@float{AcmConfCopyrightBox}[b]
  \framebox[\columnwidth]{
    \parbox[t][1.5in][t]{\columnwidth}{%
      \IfPrepare{\ifx\@AcmConfCopyrightText\empty
                     \copyright-Notice
                 \else
                     \@AcmConfCopyrightText
                 \fi}{}
      \IfSubmit{\ifx\@AcmConfCopyrightText\empty
                    \copyright-Notice
                 \else
                    \@AcmConfCopyrightText
                 \fi}{}
      \IfAccept{\@AcmConfCopyrightText}{}
      \IfPublish{\@AcmConfCopyrightText}{}
      \IfPrint{\@AcmConfCopyrightText}{}
    }
  }
  \end@float
}
\def\@AcmConfDate{\today}
\let\@AcmConfDateCmd\date
\renewcommand{\date}[1]{
    \@AcmConfDateCmd{#1}
    \def\@AcmConfDate{#1}
}
\def\@AcmConfFirstAuthor{}
\let\@AcmConfMaketitle\maketitle
\renewcommand{\maketitle}{
  \@AcmConfMaketitle
  \@AcmConfCopyrightSpace
}
\renewcommand{\@maketitle}{
  \newcommand{\Author}[1]{%
    \LARGE\sffamily ##1%
    \ifx\@AcmConfFirstAuthor\empty
        \gdef\@AcmConfFirstAuthor{##1}
    \fi
    }
  \newcommand{\Address}[1]{\large\sffamily ##1}
  \newcommand{\Phone}[1]{\large\sffamily   ##1}
  \newcommand{\Email}[1]{\LARGE\sffamily   ##1}
  \def\and{
      \end{tabular}
      \hskip 1em \@plus.17fil
      \begin{tabular}[t]{@{}c@{}}
  }
  \newpage
  \null
  \IfPublish{}{
    \hfill
    \parbox[t][0mm][t]{0.9\textwidth}{
      \vspace*{-10mm}
      \IfPrepare{\@AcmConfPrepareText}{}
      \IfSubmit{\@AcmConfSubmitText}{}
      \IfAccept{\@AcmConfAcceptText}{}
      \IfPrint{\@AcmConfPrintText}{}
      \vspace{1mm}\hrule
      }
    \hfill
    \vspace*{-5mm}
  }
  \parbox[t][14pc][t]{\textwidth}{
    \vskip 2em                   % Vertical space above title.
    \begin{center}
      {\sffamily\bfseries\Huge \@title \par}
      \vskip 1.5em               % Vertical space after title.
      {\lineskip .5em            % tabular environment
       \noindent
       \begin{tabular}[t]{@{}c@{}}\@author
       \end{tabular}\par
      }
      \vskip 1.5em                % Vertical space after author.
    \end{center}
    \vfill
  }
}
\IfPrepare{}{
  \renewcommand{\tableofcontents}{
     \ClassError{acmconf}{%
      \protect\tableofcontents\space is not
      allowed in the `acmconf' class except with option `prepare'}{}
  }
  \renewcommand{\listoffigures}{
     \ClassError{acmconf}{%
      \protect\listoffigures\space is not
      allowed in the `acmconf' class except with option `prepare'}{}
  }
  \renewcommand{\listoftables}{
     \ClassError{acmconf}{%
     \protect\listoftables\space is not
     allowed in the `acmconf' class except with option `prepare'}{}
  }
  \renewcommand{\pagestyle}[1]{
     \ClassError{acmconf}{%
      \protect\pagestyle\space is not
      allowed in the `acmconf' class except with option `prepare'}{}
  }
}
\setlength{\voffset}{-1in}
\setlength{\hoffset}{-1in}
\setlength{\textwidth}{7in}
\setlength{\textheight}{9.25in}
\setlength{\topmargin}{1in}
\setlength{\topskip}{9pt}
\setlength{\oddsidemargin}{0.75in}
\setlength{\evensidemargin}{0.75in}
\setlength{\footskip}{.35in}
\setlength{\columnsep}{.83cm}
\setlength{\columnseprule}{0pt}
\setlength{\headheight}{9pt}
\setlength{\headsep}{20pt}
\setlength{\marginparwidth}{0in}
\setlength{\marginparsep}{0in}

\def\normalsize{
  \@setfontsize\normalsize{9}{10.8}
  \abovedisplayskip 8pt plus2pt minus4pt
  \belowdisplayskip\abovedisplayskip
  \abovedisplayshortskip \z@ plus3pt
  \belowdisplayshortskip 5pt plus3pt minus3pt
  \let\@listi\@listI
}
\def\small{
  \@setfontsize\small{8}{9.6}
  \abovedisplayskip 7.5pt plus 3pt minus 4pt
  \belowdisplayskip\abovedisplayskip
  \abovedisplayshortskip \z@ plus2pt
  \belowdisplayshortskip 4pt plus2pt minus 2pt
  \def\@listi{
    \leftmargin\leftmargini
    \topsep 3pt plus 2pt minus 2pt
    \parsep 2pt plus 1pt minus 1pt
    \itemsep \parsep
    }
}
\def\footnotesize{\@setfontsize\normalsize{9}{10.8}} % \normalsize
\def\scriptsize{\@setfontsize\scriptsize{6}{7.2}}
\def\tiny{\@setfontsize\tiny{5}{6}}
\def\large{\@setfontsize\large{10}{12}}
\def\Large{\@setfontsize\Large{11}{13.2}}
\def\LARGE{\@setfontsize\LARGE{12}{14.4}}
\def\huge{\@setfontsize\huge{14}{16.8}}
\def\Huge{\@setfontsize\Huge{20.4}{24.48}}

\newenvironment{keywords}{\section*{KEYWORDS}}{}

\setcounter{secnumdepth}{5}
\setcounter{tocdepth}{5}
\renewcommand{\abstractname}{ABSTRACT}
\renewcommand{\refname}{REFERENCES}

\renewcommand{\section}{
  \@startsection{section}{1}{\z@}%
  {-3.5ex \@plus -1ex \@minus -.2ex}%
  {2.3ex \@plus.2ex}%
  {\normalfont\LARGE\bfseries\uppercase}%
}
\renewcommand{\subsection}{
  \@startsection{subsection}{2}{\z@}%
  {-3.25ex\@plus -1ex \@minus -.2ex}%
  {1.5ex \@plus .2ex}%
  {\normalfont\LARGE\bfseries}%
}
\renewcommand{\subsubsection}{
  \@startsection{subsubsection}{3}{\z@}%
  {-3.25ex\@plus -1ex \@minus -.2ex}%
  {1.5ex \@plus .2ex}%
  {\normalfont\Large\itshape}%
}
\newcommand{\subsubsubsection}{
  \@startsection{subsubsubsection}{4}{\z@}%
  {-3.25ex\@plus -1ex \@minus -.2ex}%
  {1.5ex \@plus .2ex}%
  {\normalfont\Large\itshape}%
}
\newcommand{\subsubsubsubsection}{
  \@startsection{subsubsubsubsection}{5}{\z@}%
  {-3.25ex\@plus -1ex \@minus -.2ex}%
  {1.5ex \@plus .2ex}%
  {\normalfont\Large\itshape}%
}
\renewcommand{\paragraph}{
  \@startsection{paragraph}{6}{\z@}%
  {3.25ex \@plus1ex \@minus.2ex}%
  {-1em}%
  {\normalfont\normalsize\bfseries}%
}
\renewcommand{\subparagraph}{
  \@startsection{subparagraph}{7}{%
  \parindent}%
  {3.25ex \@plus1ex \@minus .2ex}%
  {-1em}%
  {\normalfont\normalsize\sffamily\bfseries}%
}
\newcommand{\subsubsubsectionmark}[1]{}
\newcommand{\subsubsubsubsectionmark}[1]{}
\newcounter{subsubsubsection}[subsubsection]
\newcounter{subsubsubsubsection}[subsubsubsection]
\renewcommand{\thesubsubsubsection}{%
  \thesubsubsection .\@arabic\c@subsubsubsection%
}
\renewcommand{\thesubsubsubsubsection}{%
  \thesubsubsubsection .\@arabic\c@subsubsubsubsection%
}
\newcommand{\l@subsubsubsection}{\@dottedtocline{4}{3.8em}{3.8em}}
\newcommand{\l@subsubsubsubsection}{\@dottedtocline{5}{3.8em}{4.2em}}
\renewcommand{\l@paragraph}{\@dottedtocline{6}{7.0em}{4.1em}}
\renewcommand{\l@subparagraph}{\@dottedtocline{7}{10em}{5em}}
\IfPrepare{\typeout{**** Paper in preparation ****}}{}
\IfSubmit{\typeout{**** Sumbitted paper ****}}{}
\IfAccept{\typeout{**** Accepted paper ****}}{}
\IfPublish{\typeout{**** Published in proceedings ****}}{}
\IfPrint{\typeout{****  Published paper printed outside
                        proceedings ****}}{}
\AtBeginDocument{
  \normalsize
  \ifx\@AcmConfConferenceShortName\empty
    \ClassError{acmconf}{%
      You have not specified a conference short name.
      \MessageBreak
      Use \protect\ConferenceShortName\space in the preamble}{}
  \fi
  \ifx\@AcmConfConferenceName\empty
    \ClassError{acmconf}{%
      You have not specified a conference name.
      \MessageBreak
      Use \protect\ConferenceName\space in the preamble}{}
  \fi
  \IfSubmit{
    \def\@oddfoot{\parbox{0.3\textwidth}{\@AcmConfFirstAuthor,
                                         \@AcmConfDate}
                  \hfill
                  \thepage
                  \hfill
                  \parbox{0.3\textwidth}{\hfill
                                         \@AcmConfConferenceShortName}
    }
  }{}
  \IfAccept{
    \def\@oddfoot{\hfill\thepage\hfill}
    \ifx\@AcmConfCopyrightText\empty
      \ClassError{acmconf}{%
        You have not specified the copyright notice
        \MessageBreak
        Use \protect\CopyrightText\space in the preamble}{}
    \fi
  }{}
  \IfPublish{
    \let\ps@plain\ps@empty
    \let\ps@headings\ps@empty
    \let\ps@myheadings\ps@empty
    \def\@oddfoot{}
    \ifx\@AcmConfCopyrightText\empty
      \ClassError{acmconf}{%
        You have not specified the copyright notice
        \MessageBreak
        Use \protect\CopyrightText\space in the preamble}{}
    \fi
  }{}
  \IfPrint{
    \def\@oddfoot{\hfill\thepage\hfill}
    \ifx\@AcmConfCopyrightText\empty
      \ClassError{acmconf}{%
        You have not specified the copyright notice
        \MessageBreak
        Use \protect\CopyrightText\space in the preamble}{}
    \fi
    \ifx\@AcmConfPublishedPageFrom\empty
      \ClassError{acmconf}{%
        You have not specified the start page of the publication
        \MessageBreak
        Use \protect\PublishedPageFrom\space in the preamble}{}
    \fi
    \ifx\@AcmConfPublishedPageTo\empty
      \ClassError{acmconf}{%
        You have not specified the end page of the publication
        \MessageBreak
        Use \protect\PublishedPageTo\space in the preamble}{}
    \fi
  }{}
}
\AtEndDocument{
  \IfSubmit{
     \ifx\@AcmConfFirstAuthor\empty
     \ClassError{acmconf}{%
       You have not specified the name of the (first) author.
       \MessageBreak
       Use \protect\Author to typeset the author(s) of the paper.
       }{}
     \fi
  }{}
}
\endinput
%%
%% End of file `acmconf.cls'.
