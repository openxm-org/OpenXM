rem $OpenXM$
rem
rem <1> Start ox_sm1 server with the command sm1_start_windows(0);
rem     from asirgui
rem
rem Set the following values
rem
set HOME=c:\cygwin\home\nobuki
set OpenXM_WIN_ROOT=c:\cygwin\home\nobuki\OpenXM\misc\packages\Windows\OpenXM-win
set OpenXM_HOME=%OpenXM_WIN_ROOT%
set OpenXM_HOME_WIN=%OpenXM_WIN_ROOT%
set OPENXMHOMEWIN=%OpenXM_HOME_WIN%
set OpenXM_START_EXE=%OpenXM_WIN_ROOT%\bin-windows\start.exe
set WIN_ASIR_ROOT=%OpenXM_WIN_ROOT%\asir
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
rem
rem  start the asirgui.exe
rem
%WIN_ASIR_ROOT%\bin\asirgui


