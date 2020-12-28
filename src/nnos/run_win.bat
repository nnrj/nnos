if "%PROCESSOR_ARCHITECTURE%"=="x86" goto x86
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto x64
exit
:x64
	call run_win_win64.bat
	exit	
:x86
	call run_win_win32.bat
	exit