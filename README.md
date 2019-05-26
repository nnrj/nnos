# nnos
基本x86架构的迷你操作系统。  
# A Mini Operating System based on x86 Architecture.  
Email:tianhehechu@qq.com  
本项目旨在开发一个完备的计算机操作系统。  
# The project aim to develop a complete computer operating system.  
本系统基于川合秀实先生的《30天自制操作系统》设计和开发。  
# The OS is refer to "Osask", a book write by 川合秀实 about develop OS.  
目前已经完成了基本的内存管理、进程管理、I/O管理和文件管理。并提供了简陋的图形界面。  
# At present, we have completed the basic Memery Manager, Process Manager, I/O Manager and File Manager, and supply a crude GUI.  
其中文件管理系统仅完成了文件查看、可执行文件运行。急需加入文件写入等功能。  
# And File Manger just completed "file seek" and "run executable file", so we need to add "file write".  
目前本系统仅能运行在1.44MB标准软盘中，借助GNU GRUB可完成U盘启动。  
# At present, the OS rely on 1.44MB standard floppy disk, but it can run at USB flash disk help by GNU GRUB.  
项目计划：  
# Project Plan:  
【孕育阶段】  
  ·实现钟表和日历；  # Implement clock and calendar.  
  ·完善文件管理系统，实现系统内应用程序编译、安装；  # Improve file system,implement computing app in OS.    
  ·改进进程管理，使用链表替换数组；  # Improve process manager, replace array with linkedList.  
  ·完善图形界面，支持任务栏程序图标显示；  # Improve GUI, support taskbar and icon.  
  ·完善图形界面，支持桌面背景（将图片查看器应用移植嵌入系统内部）；  # Improve GUI, support desktop background image.  
  ·修改底层汇编代码，脱离软盘，支持硬盘启动；  # Modify underlying code to throw off floppy and support hard disk.  
  ·将文件系统由FAT12修改为FAT32；  # Modify file system format form FAT12 to FAT32(or other senior format).  
  ·增加任务管理器应用程序和文件浏览器应用程序；  # Add task manager application and file browser application.  
  ·实现C标准库<stdio.h>。  # Implement "<stdio.h>", an important standard library.  
  
 ------------------------------------------------------------------------------  
 本系统尚未正式命名，临时命名为“年年操作系统”。  
 # The system has not yet been named, and temporarily named "NianOS"(shorter form “NNOS”).  
 相关开发文档和详细说明将在近期逐步完善。  
 # The developing document and other introduction about this project will be supplement in the near future(before 2020.5.25).  
 英文注释和英文帮助文档也将跟进。  
 # And the explanatory note in English as well as supplement in the same time.
  
  年年软件  # Nian Nian Software(NNRJ) 
  2019.5.25 00:11
  2019.4.26 21:57
