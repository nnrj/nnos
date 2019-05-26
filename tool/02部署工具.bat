@echo 
cd /d %~dp0
echo 'd'|xcopy .\make\*.* D:\nnos34d\make /s
@echo "make部署完毕。"
echo 'd'|xcopy .\nask\*.* D:\nnos34d\nask /s
@echo "nask部署完毕。"
echo 'd'|xcopy .\nasm\*.* D:\nnos34d\nasm /s
@echo "nasm部署完毕。"
echo 'd'|xcopy .\qemu\*.* D:\nnos34d\qemu /s
@echo "qemu部署完毕。"
echo 'd'|xcopy .\rawrite32-1.0.6.0\*.* D:\nnos34d\rawrite32-1.0.6.0 /s
@echo "rawrite部署完毕。"
echo 'd'|xcopy .\rawwritewin\*.* D:\nnos34d\rawwritewin /s
@echo "rawwrite部署完毕。"
@echo "所有工具部署完毕，请继续部署环境。"
pause