@echo off
ver|findstr /r /i " [�汾 5.1.*]" > NUL && goto WinXP||goto overWin7
:WinXP
@reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v path /t reg_expand_sz /d "%Path%;D:\nnos34d\make;D:\nnos34d\nask;D:\nnos34d\nasm;D:\nnos34d\qemu" /f
goto end1
:overWIN7
setx Path "%Path%;D:\nnos34d\make;D:\nnos34d\nask;D:\nnos34d\nasm;D:\nnos34d\qemu;D:\nnos34d\rawrite32-1.0.6.0;D:\nnos34d\rawwritewin"
goto end2
:end1
echo ��������������ϡ�Ϊȷ������������Ч�������������������
goto end
:end2
echo ��������������ϡ�
goto end
:end
