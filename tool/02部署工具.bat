@echo 
cd /d %~dp0
echo 'd'|xcopy .\make\*.* D:\nnos34d\make /s
@echo "make������ϡ�"
echo 'd'|xcopy .\nask\*.* D:\nnos34d\nask /s
@echo "nask������ϡ�"
echo 'd'|xcopy .\nasm\*.* D:\nnos34d\nasm /s
@echo "nasm������ϡ�"
echo 'd'|xcopy .\qemu\*.* D:\nnos34d\qemu /s
@echo "qemu������ϡ�"
echo 'd'|xcopy .\rawrite32-1.0.6.0\*.* D:\nnos34d\rawrite32-1.0.6.0 /s
@echo "rawrite������ϡ�"
echo 'd'|xcopy .\rawwritewin\*.* D:\nnos34d\rawwritewin /s
@echo "rawwrite������ϡ�"
@echo "���й��߲�����ϣ���������𻷾���"
pause