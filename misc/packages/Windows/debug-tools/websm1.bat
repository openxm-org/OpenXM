rem $OpenXM: OpenXM/misc/packages/Windows/debug-tools/websm1.bat,v 1.2 2002/10/01 13:00:34 takayama Exp $
rem Moved from OpenXM/src/kan96xx/win/websm1.bat,v 
rem            1.2 2001/08/10 08:33:02 takayama Exp 
rem
rem Set the following values
rem
set OXWEB_POST=1
set HOME=c:\cygwin\home\nobuki
set OpenXM_WIN_ROOT=c:\cygwin\home\nobuki\OpenXM\misc\packages\Windows\OpenXM-win
set OPENXMHOMEWIN=%OpenXM_WIN_ROOT%
set OpenXM_HOME=%OPENXMHOMEWIN%
set WIN_ASIR_ROOT=%OpenXM_WIN_ROOT%\asir
rem 
rem Do not touch below
rem 
set PATH=%OpenXM_HOME%\bin;%PATH%;c:\windows\command
set ASIR_LIBDIR=%WIN_ASIR_ROOT%\lib
set ASIRLOADPATH=;%WIN_ASIR_ROOT%\lib;%OpenXM_HOME%\src\asir-contrib\packages\src;%OpenXM_HOME%\src\asir-contrib\packages\sample;%OpenXM_HOME%\lib\asir-contrib;.
set ASIR_CONFIG=%OpenXM_HOME%\rc\asirrc
set ASIR_RSH='ssh -f -X -A '
set LOAD_SM1_PATH=%OpenXM_HOME%\lib\sm1
set LOAD_K_PATH=%OpenXM_HOME%\lib\k097
cd %OpenXM_HOME_WIN%\bin
sm1 -s "[(parse) (httpd-sm1.sm1) pushfile] extension ; websm1 ; "


