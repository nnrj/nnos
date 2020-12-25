# NNOS

<font size=2 color=red>【注意】如果您的IP在中国大陆，由于众所周知的原因，可能导致本页图片加载失败。</font>     

*更新日期：2020-09-15*

## 基本x86架构的迷你操作系统

> Email:tianhehechu@qq.com  
>
> <span id="NNOS开发者交流群">NNOS开发者交流群：<a target="_blank" href="//shang.qq.com/wpa/qunwpa?idkey=a0e8dd73153e233040a0cb4ea45172596f6e2237a629aa174741de79a631a456"><img border="0" src="//pub.idqqimg.com/wpa/images/group.png" alt="NNOS开发者交流群" title="NNOS开发者交流群"></a></span>（757581301）

- 本项目旨在开发一个完备的计算机操作系统。 
- 本系统基于川合秀实先生的《**30天自制操作系统**》设计和开发。
- 本系统尚未正式命名，临时命名为“年年操作系统”。  
- **发行版下载**： [NNOS_0.34d_beta.](https://raw.githubusercontent.com/nnrj/nnos/master/beta/NNOS_0.34d_beta.zip) 
- 目前已经完成了基本的内存管理、进程管理、I/O管理和文件管理。并提供了简陋的图形界面。 
- 其中文件管理系统仅完成了文件查看、可执行文件运行。急需加入文件写入等功能。 
- 目前本系统仅能运行在1.44MB标准软盘中，借助GNU GRUB可完成U盘启动。

 ![Image text](https://github.com/nnrj/nnos/blob/master/doc/img/NNOS_0.34b.gif) 

## 系统层次架构

![Image text](https://github.com/nnrj/nnos/blob/master/doc/img/NNOS_Framework.jpg)  

### 项目计划

*孕育阶段* 

- 实现钟表和日历；
- 完善文件管理系统，实现系统内应用程序编译、安装；
- 改进进程管理，使用链表替换数组；
- 完善图形界面，支持任务栏程序图标显示；
- 完善图形界面，支持桌面背景（将图片查看器应用移植嵌入系统内部）； 
- 修改底层汇编代码，脱离软盘，支持硬盘启动；
- 将文件系统由FAT12修改为FAT32；
- 增加任务管理器应用程序和文件浏览器应用程序；
- 实现C标准库`<stdio.h>`。

## 开发文档

- 完整的项目开发文档、参考手册和问题列表已发布，可在`doc`目录中找到。

- 如果您是系统开发者，请参考《**NNOS参考手册**》和《**NNOS_API文档**》；

- 如果您是应用开发者，只需要参考《**NNOS_API文档**》的**系统调用**部分。

## 开发团队

**欢迎加入我们的开发团队，QQ群号码【757581301】**。 

团队成员：天河何处 张继军

***

 NNOS开发团队

2020.09.15 18:55| *2019.05.25 00:11* |*2019.04.26 21:57*

***

Email:tianhehechu@qq.com  

<span id="NNOS开发者交流群">NNOS开发者交流群：<a target="_blank" href="//shang.qq.com/wpa/qunwpa?idkey=a0e8dd73153e233040a0cb4ea45172596f6e2237a629aa174741de79a631a456"><img border="0" src="//pub.idqqimg.com/wpa/images/group.png" alt="NNOS开发者交流群" title="NNOS开发者交流群"></a></span>（757581301）

