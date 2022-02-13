set MAKE_HOME=..\tools\make
xcopy /y ini\Makefile_win32.mak Makefile
%MAKE_HOME%make run
::powershell
cmd