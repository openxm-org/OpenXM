rem $OpenXM$
rem Batch file to set environmental variables properly on my laptop.
rem copy this file by the command
rem    cp setenv1.bat c:/
rem and execute it from the command prompt.
rem
rem <1> If your cd driver is f:, then cd f:
rem <2> If your OS is not windows 2000, then command /e:5000
rem
rem Set the following values
rem
set HOME=c:\home
set OpenXM_WIN_ROOT=c:\cygwin\home\nobuki\OpenXM\misc\packages\Windows\OpenXM-win
set OpenXM_HOME=%OpenXM_WIN_ROOT%
set OpenXM_HOME_WIN=%OpenXM_WIN_ROOT%
set OPENXMHOMEWIN=%OpenXM_HOME_WIN%
set OpenXM_START_EXE=start
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

cd c:\cygwin\home\nobuki\OpenXM\misc\packages\windows\OpenXM-win\bin



