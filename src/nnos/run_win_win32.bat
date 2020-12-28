xcopy /y ini\Makefile_win32 Makefile
xcopy /y ipl.asm ipl.nas
xcopy /y syshead.asm syshead.nas
xcopy /y osfun.asm osfun.nas
xcopy /y .\app\api.asm .\app\api.nas
::.\ini\getnas.exe
echo syshead.nas ;[INSTRSET [INSTRSET|.\ini\getnas_win32.exe
echo osfun.nas ;[ [|.\ini\getnas_win32.exe
echo syshead.nas ALIGN ALIGNB|.\ini\getnas_win32.exe
::echo make run | powershell
make run
::powershell
cmd