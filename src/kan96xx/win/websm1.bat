rem $OpenXM$
rem
rem Set the following values
rem
set HOME=c:\cygwin\home\nobuki
set OpenXM_HOME=c:\cygwin\home\nobuki\OpenXM
rem 
rem Do not touch below
rem 
set PATH=%OpenXM_HOME%\bin;%PATH%
set ASIR_LIBDIR=%OpenXM_HOME%\lib\asir;%WIN_ASIR_ROOT%\lib
set ASIRLOADPATH=;%WIN_ASIR_ROOT%\lib;%OpenXM_HOME%\src\asir-contrib\packages\src;%OpenXM_HOME%\src\asir-contrib\packages\sample;%OpenXM_HOME%\lib\asir-contrib;.
set ASIR_CONFIG=%OpenXM_HOME%\rc\asirrc
set ASIR_RSH='ssh -f -X -A '
set LOAD_SM1_PATH=%OpenXM_HOME%\lib\sm1
set LOAD_K_PATH=%OpenXM_HOME%\lib\k097
%OpenXM_HOME%\src\kan96xx\Kan\sm1 -s "[(parse) (httpd-sm1.sm1) pushfile] extension ; websm1 ; "


