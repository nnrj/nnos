# NNOS 说明文件

版本：*TestCode05.0.0_2022011901*

> 欢迎加入我们的QQ群：【757581301】

## 编译运行方法

- Windows：
  - 方式一：直接双击运行`run_win.bat`。
  - 方式二：打开`cmd`或`powershell`，输入`./run_win.bat`后按回车。
  - 方式三：打开`cmd`或`powershell`，执行`make img`命令编译并生成系统镜像。然后输入`make run`运行之，或直接双击运行`bochsrc.bxrc`。
- Linux：
  - 方式一：打开终端，输入`./run_linux.sh`后按回车。
  - 方式二：打开终端，执行`make img`命令以编译并生成系统镜像。然后输入`make run`运行之。

> 注意：首次在Windows或Linux上运行时，请先至少执行一次`run_win.bat`/`run_linux.sh`，以便更新相应系统的Makefile文件。

## 命令大全

- `make blank.img`：生成一个大小为2GB的空白虚拟磁盘镜像。
- `make mbr.bin`：编译mbr.s，以生成mbr.bin。
- `make fat32.bin`：编译fat32.s，以生成fat32.bin。
- `make boot.o`：编译boot.s，以生成boot.o。
- `make naskfun.o`：编译naskfun.s，以生成naskfun.o。
- `make bootmain.o`：编译bootmain.c，以生成bootmain.o。
- `make freeldr.sys`：链接生成freeldr.sys文件。
- `make kernel.elf`：编译kernel.c生成kernel.elf文件。
- `make kernel.img`：生成系统镜像。
- `make run`：在虚拟机中运行os。（默认使用bochs）。
- `make run_b`：在bochs中运行os。
- `make run_q`：在qemu中运行os。
- `make run_db`：以debug模式在bochs中运行nnos以进行调试。
- `make me`：更新Makefile。
- `make clean`：清理中间产物（\*.lst，\*.bin，\*.elf，\*.obj，\*.o，\*.map，\*.sys）。
- `make clean`_log：清理日志（.ini/\*.log）。
- `make src_only`：清理所有产物（除执行`make clean`和`make clean_log`外，还清除了`blank.img`和`kernel.img`。
- `make`：执行 `make img`命令。

> 注意：
>
> - 停止bochs运行的方法：输入exit或Ctrl+C。
> - 停止qemu运行的方法：Ctrl+Alt+Q。



*NNOS开发团队*

*2022年01月19日*