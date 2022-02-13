set MAKE_HOME = ..\tools\make\
xcopy /y ini\Makefile_win.mak Makefile
%MAKE_HOME%make run
powershell