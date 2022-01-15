xcopy /y ini\Makefile_win.mak Makefile
xcopy /y ipl.asm ipl.nas
xcopy /y ipl32.asm ipl32.nas
xcopy /y syshead.asm syshead.nas
xcopy /y osfun.asm osfun.nas
xcopy /y .\app\api.asm .\app\api.nas
::.\ini\getnas.exe
echo syshead.nas ;[INSTRSET [INSTRSET|.\ini\getnas.exe
echo osfun.nas ;[ [|.\ini\getnas.exe
echo syshead.nas ALIGN ALIGNB|.\ini\getnas.exe
::echo make run | powershell
make run
powershell