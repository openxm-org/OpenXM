rem $OpenXM$
rem
rem <1> You need OpenXM/bin, OpenXM/lib
rem <2> Copy OpenXM/bin/ox_sm1.exe with the name ox_sm1_forAsir.exe
rem     under OpenXM/lib/sm1/bin
rem <3> Start ox_sm1 server with the command sm1_start_windows(0);
rem     from asirgui
rem
rem Set the following values
rem
set HOME=c:\cygwin\home\nobuki
set OpenXM_HOME=c:\cygwin\home\nobuki\OpenXM
set OpenXM_HOME_WIN=c:\cygwin\home\nobuki\OpenXM
set OPENXMHOMEWIN=c:\cygwin\home\nobuki\OpenXM
set OpenXM_START_EXE=c:\windows\command\start
set WIN_ASIR_ROOT=c:\home\asir
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


