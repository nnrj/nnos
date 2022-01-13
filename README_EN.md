# NNOS     

<font size=2 color=red>ATTENTION : If your IP from  Chinese mainland, the picture in this file may be load fail。</font>     

*Update date:2020-09-15*

## A Mini Operating System based on x86 Architecture

> Email:tianhehechu@qq.com  
>
> <span id="NNOS开发者交流群">QQ-Group for NNOS Developer: <a target="_blank" href="//shang.qq.com/wpa/qunwpa?idkey=a0e8dd73153e233040a0cb4ea45172596f6e2237a629aa174741de79a631a456"><img border="0" src="//pub.idqqimg.com/wpa/images/group.png" alt="NNOS开发者交流群" title="NNOS开发者交流群"></a></span>（757581301）

- The project aim to develop a complete computer operating system. 
- The OS is refer to `Osask`, a book write by 川合秀实 about develop OS. 
- The system has not yet been named, and temporarily named `NianOS`(shorter form `NNOS`).  
- **Release Download**： [NNOS_0.34d_beta.](https://raw.githubusercontent.com/nnrj/nnos/master/beta/NNOS_0.34d_beta.zip) 
- At present, we have completed the basic Memory Manager, Process Manager, I/O Manager and File Manager, and supply a crude GUI. 
- And File Manger just completed `file seek` and `run executable file`, so we need to add "file write". 
- At present, the OS rely on 1.44MB standard floppy disk, but it can run at USB flash disk help by GNU GRUB.  

 ![Image text](https://github.com/nnrj/nnos/blob/master/doc/img/NNOS_0.34b.gif) 

## System Structure

![Image text](https://github.com/nnrj/nnos/blob/master/doc/img/NNOS_Framework.jpg)  

## Project Plan

- Implement clock and calendar;
- Improve file system,implement computing app in OS;
- Improve process manager, replace array with linkedList;
- Improve GUI, support taskbar and icon;
- Improve GUI, support desktop background image; 
- Underlying code to throw off floppy and support hard disk;
- Modify file system format form FAT12 to FAT32 or other senior format;
- Add task manager application and file browser application;
- Implement `<stdio.h>`, an important standard library.

## Development Documentation

- The complete project development document, reference manual and question list have been released and can be found in the doc directory;

- If you are a system developer, please refer to `NNOS Reference Manual` and `NNOS_API Document`;

- If you are an application developer, you only need to refer to the system call part of `NNOS_API Document`.

## Development Team

**Welcome to join us!**

**QQ Group: 757581301**

Team members: 天河何处 张继军

***

the development team of NNOS

2020.09.15 18:55| *2019.05.25 00:11* |*2019.04.26 21:57*

***

Email:tianhehechu@qq.com  

<span id="NNOS开发者交流群">QQ-Group for NNOS Developer: <a target="_blank" href="//shang.qq.com/wpa/qunwpa?idkey=a0e8dd73153e233040a0cb4ea45172596f6e2237a629aa174741de79a631a456"><img border="0" src="//pub.idqqimg.com/wpa/images/group.png" alt="NNOS开发者交流群" title="NNOS开发者交流群"></a></span>（757581301）

